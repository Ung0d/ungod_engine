/*
* This file is part of the ungod - framework.
* Copyright (C) 2016/2017 Felix Becker - fb132550@uni-greifswald.de
*
* This software is provided 'as-is', without any express or
* implied warranty. In no event will the authors be held
* liable for any damages arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute
* it freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented;
*    you must not claim that you wrote the original software.
*    If you use this software in a product, an acknowledgment
*    in the product documentation would be appreciated but
*    is not required.
*
* 2. Altered source versions must be plainly marked as such,
*    and must not be misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any
*    source distribution.
*/

#include "ungod/content/water/Water.h"
#include "ungod/base/Entity.h"
#include "ungod/base/World.h"
#include "ungod/content/tilemap/TileMap.h"

namespace ungod
{
    constexpr float Water::DEFAULT_DISTORTION;
    constexpr float Water::DEFAULT_FLOW;
    constexpr float Water::DEFAULT_REFLECTION_OPACITY;
    constexpr float Water::BOUNDING_BOX_SCALING;
    constexpr float Water::BOUNDS_OVERLAP;

    Water::Water() : mShowReflections(true), mShowShaders(false),
                                               mDistortionFactor(DEFAULT_DISTORTION), mFlowFactor(DEFAULT_FLOW), mReflectionOpacity(DEFAULT_REFLECTION_OPACITY)
    {
    }

    Water::Water(const Water& other)
    {
        mShowReflections = other.mShowReflections;
        mShowShaders = other.mShowShaders;
        mDistortionFactor = other.mDistortionFactor;
        mFlowFactor = other.mFlowFactor;
        mReflectionOpacity = other.mReflectionOpacity;
        init(other.mDistortionTextureFile, mFragmentShaderID, mVertexShaderID);
        targetsizeChanged(other.mTargetSize);
    }

    bool Water::render(sf::RenderTarget& target, sf::RenderTexture& rendertex, const TileMap& tilemap, const sf::Texture* tilemapTex, sf::RenderStates states, World& world) const
    {
        rendertex.clear(sf::Color::Transparent);

        //sf::RenderStates inverse;
        //inverse.transform.translate(-renderpos);
        rendertex.setView(target.getView());
        tilemap.render(rendertex, tilemapTex, states);

        sf::Vector2f windowPosition = target.mapPixelToCoords(sf::Vector2i(0, 0));
        windowPosition = states.transform.getInverse().transformPoint(windowPosition);

        //render reflections
        if (mShowReflections)
        {
            quad::PullResult<Entity> pull;
            world.getQuadTree().retrieve(pull, { windowPosition.x,
                                                  windowPosition.y - (BOUNDING_BOX_SCALING - 1) * target.getView().getSize().y,
                                                  target.getView().getSize().x,
                                                  BOUNDING_BOX_SCALING * target.getView().getSize().y });

            dom::Utility<Entity>::iterate<TransformComponent, VisualsComponent>(pull.getList(),
                [this, &states, &world, &rendertex, &tilemap](Entity e, TransformComponent& transf, VisualsComponent& vis)
                {
                    if (states.transform.getInverse().transformRect(tilemap.getBounds()).intersects(e.get<TransformComponent>().getBounds()))
                    {
                        //get the bounds of the visual contents of the entity
                        //we need to take the untransformed bounds here
                        //this is kind of a "hack" to avoid that rotation will mess up visuals
                        sf::Vector2f lowerBounds = world.getVisualsHandler().getUntransformedLowerBound(e);
                        sf::Vector2f upperBounds = world.getVisualsHandler().getUntransformedUpperBound(e);

                        float curOpacity = vis.getOpacity();
                        VisualsHandler::setOpacity(e, curOpacity * mReflectionOpacity);
                        if (!e.has<WaterComponent>())
                            world.getState()->getRenderer().renderEntity(e, transf, vis, rendertex, states, true, BOUNDS_OVERLAP * (-2 * lowerBounds.y + upperBounds.y));
                        VisualsHandler::setOpacity(e, curOpacity);
                    }
                });
        }

        states.transform *= tilemap.getTransform();

        //drawing to the rendertex done, attach it to a sprite
        rendertex.display();
        sf::Sprite renderbody(rendertex.getTexture());

        sf::RenderStates waterstates;

        //apply the shader and draw the rendertex to the target
        if (mShowShaders)
        {
            waterstates.shader = &mWaterShader;
        }

        sf::View storview = target.getView();
        sf::View defview;
        defview.setSize((float)target.getSize().x, (float)target.getSize().y);
        defview.setCenter((float)target.getSize().x / 2, (float)target.getSize().y / 2);
        target.setView(defview);
        target.draw(renderbody, waterstates);
        target.setView(storview);

        return true; 
    }

    void Water::update()
    {
        mWaterShader.setUniform("time", mDistortionTimer.getElapsedTime().asSeconds());
    }

    void Water::init(const std::string& distortionTex, const std::string& fragmentShader, const std::string& vertexShader)
    {
        mDistortionTextureFile = distortionTex;
        if (!sf::Shader::isAvailable())
        {
            ungod::Logger::warning("Shaders are not available on the operating system. \n Water is rendered with disabled shaders");
            mShowShaders = false;
        }
        else
        {
            if (!mWaterShader.loadFromFile(vertexShader, fragmentShader))
            {
                Logger::warning("Can't load water shaders.");
                mShowShaders = false;
            }
            else
            {
                mVertexShaderID = vertexShader;
                mFragmentShaderID = fragmentShader;
                mShowShaders = true;
                targetsizeChanged(mTargetSize);
            }
        }
    }

    void Water::targetsizeChanged(const sf::Vector2u& targetsize)
    {
        mTargetSize = targetsize;
        if (mShowShaders && targetsize.x > 0u && targetsize.y > 0u)
        {
            buildDistortionTexture(targetsize);
            mWaterShader.setUniform("screenSize", sf::Vector2f{ (float)mTargetSize.x, (float)mTargetSize.y });
        }
    }

    void Water::setReflections(bool flag)
    {
        mShowReflections = flag;
    }

    void Water::setShaders(bool flag)
    {
        mShowShaders = flag;
        targetsizeChanged(mTargetSize);
    }

    void Water::setDistortionFactor(float distortion)
    {
        mDistortionFactor = distortion;
        mWaterShader.setUniform("distortionFactor", mDistortionFactor);
    }

    void Water::setFlowFactor(float flow)
    {
        mFlowFactor = flow;
        mWaterShader.setUniform("flowFactor", mFlowFactor);
    }

    void Water::setReflectionOpacity(float opacity)
    {
        mReflectionOpacity = opacity;
    }

    void Water::buildDistortionTexture(const sf::Vector2u& texsize)
    {
        sf::Image source;
        if (!source.loadFromFile(mDistortionTextureFile))
            return;
        sf::Image target;
        target.create(texsize.x, texsize.y);
        for (unsigned x = 0; x < texsize.x; ++x)
            for (unsigned y = 0; y < texsize.y; ++y)
            {
                target.setPixel(x,y, source.getPixel(x%source.getSize().x, y%source.getSize().y) );
            }
        if (mDistortionTexture.loadFromImage(target))
        {
            mDistortionTexture.setRepeated(true);
            mDistortionTexture.setSmooth(true);
            mWaterShader.setUniform("distortionMapTexture", mDistortionTexture);
            mWaterShader.setUniform("distortionMapSize", sf::Vector2f{(float)mDistortionTexture.getSize().x, (float)mDistortionTexture.getSize().y});
            mWaterShader.setUniform("currentTexture", sf::Shader::CurrentTexture);
            mWaterShader.setUniform("distortionFactor", mDistortionFactor);
            mWaterShader.setUniform("flowFactor", mFlowFactor);
            mShowShaders = true;
        }
        else
        {
            Logger::warning("Can't load distortion map for water shader.");
            mShowShaders = false;
        }
    }
}
