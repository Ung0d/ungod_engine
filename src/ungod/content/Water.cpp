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

#include "ungod/content/Water.h"
#include "ungod/base/Entity.h"
#include "ungod/base/World.h"

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

    Water::Water(World& world) : mShowReflections(true), mShowShaders(false),
                                               mDistortionFactor(DEFAULT_DISTORTION), mFlowFactor(DEFAULT_FLOW), mReflectionOpacity(DEFAULT_REFLECTION_OPACITY)
    {
    }

    Water::Water(const Water& other)
    {
        *this = other;
    }

    Water& Water::operator=(const Water& other)
    {
        if (this != &other)
        {
            if (other.mGround.getImage().isLoaded() && other.mGround.getMetaMap().isLoaded())
            {
                loadTiles(other.mGround.getImage().getFilePath(), other.mGround.getMetaMap().getFilePath(),
                          other.mGround.getTileWidth(), other.mGround.getTileHeight(), other.mGround.getKeyMap());
            }
            if (other.mDistortionTexID != "" && other.mVertexShaderID != "" && other.mFragmentShaderID != "")
            {
                loadShaders(other.mDistortionTexID, other.mFragmentShaderID, other.mVertexShaderID, other.mRenderTex);
            }
            mShowReflections = other.mShowReflections;
            mShowShaders = other.mShowShaders;
            mDistortionFactor = other.mDistortionFactor;
            mFlowFactor = other.mFlowFactor;
            mReflectionOpacity = other.mReflectionOpacity;
        }
        return *this;
    }

    Water::Water(World& world,
                 const std::string& distortionMap,
                 const std::string& texFilepath, const std::string& metaFilepath,
                 const std::string& fragmentShader, const std::string& vertexShader,
                 std::size_t tileWidth, std::size_t tileHeight,
                 const sf::RenderTarget& target,
                 const std::vector<std::string>& keymap) :
                     mShowReflections(true), mShowShaders(false),
                     mDistortionFactor(DEFAULT_DISTORTION), mFlowFactor(DEFAULT_FLOW), mReflectionOpacity(DEFAULT_REFLECTION_OPACITY)
    {
        loadShaders(distortionMap, fragmentShader, vertexShader, target);
        loadTiles(texFilepath, metaFilepath, tileWidth, tileHeight, keymap);
    }

    void Water::targetsizeChanged(const sf::Vector2u& targetsize)
    {
        if (targetsize.x != mRenderTex.getSize().x || targetsize.y != mRenderTex.getSize().y)
        {
            if (mShowShaders)
                buildDistortionTexture(targetsize);

            if (!mRenderTex.create(targetsize.x, targetsize.y))
            {
                mShowShaders = false;
                mShowReflections = false;
            }
        }
    }

    void Water::loadTiles(const std::string& texFilepath, const std::string& metaFilepath,
                     std::size_t tileWidth, std::size_t tileHeight,
                     const std::vector<std::string>& keymap)
    {
        mGround.loadTiles(texFilepath, metaFilepath, tileWidth, tileHeight,keymap);
    }

    void Water::loadShaders(const std::string& distortionMap,
                     const std::string& fragmentShader, const std::string& vertexShader,
                     const sf::RenderTarget& target)
    {
        if (!sf::Shader::isAvailable())
        {
            ungod::Logger::warning("Shaders are not available on the operating system.");
            ungod::Logger::endl();
            ungod::Logger::warning("Water is rendered with disabled shaders");
            ungod::Logger::endl();
            mShowShaders = false;
        }
        else
        {
            if (!mWaterShader.loadFromFile(vertexShader, fragmentShader))
            {
                Logger::warning("Can't load water shaders.");
                Logger::endl();
                mShowShaders = false;
            }
            else
            {
                mVertexShaderID = vertexShader;
                mFragmentShaderID = fragmentShader;
                mDistortionTexID = distortionMap;
                mShowShaders = true;
            }
        }

        targetsizeChanged(target.getSize());

        mOffset = target.getView().getCenter();
    }

    void Water::setReflections(bool flag)
    {
        mShowReflections = flag;
    }

    void Water::setShaders(bool flag)
    {
        mShowShaders = flag;
        targetsizeChanged(mRenderTex.getSize());
    }

    bool Water::render(sf::RenderTarget& target, sf::RenderStates states, World& world)
    {
        targetsizeChanged(target.getSize());
        mRenderTex.clear(sf::Color::Transparent);
        states.transform *= getTransform();
        //sf::RenderStates inverse;
        //inverse.transform.translate(-renderpos);
        mRenderTex.setView(target.getView());
        if (!mGround.render(mRenderTex, states))
            return false;  //nothing is drawn, no water tile is visible on the screen

        sf::Vector2f windowPosition = target.mapPixelToCoords(sf::Vector2i(0,0));

        //render reflections
        if (mShowReflections)
        {
            quad::PullResult<Entity> pull;
            world.getQuadTree().retrieve(pull, {windowPosition.x,
                                                  windowPosition.y - (BOUNDING_BOX_SCALING-1)*target.getView().getSize().y,
                                                  target.getView().getSize().x,
                                                  BOUNDING_BOX_SCALING*target.getView().getSize().y} );

            dom::Utility<Entity>::iterate<TransformComponent, VisualsComponent>(pull.getList(),
              [this, &states, &world] (Entity e, TransformComponent& transf, VisualsComponent& vis)
              {
                  //get the bounds of the visual contents of the entity
                  //we need to take the untransformed bounds here
                  //this is kind of a "hack" to avoid that rotation will mess up visuals
                  sf::Vector2f lowerBounds = world.getVisualsManager().getUntransformedLowerBound(e);
                  sf::Vector2f upperBounds = world.getVisualsManager().getUntransformedUpperBound(e);

                  float curOpacity = vis.getOpacity();
                  VisualsManager::setOpacity(e, curOpacity*mReflectionOpacity);
                  Renderer::renderEntity(e, transf, vis, mRenderTex, states, true, BOUNDS_OVERLAP * (-2*lowerBounds.y + upperBounds.y));
                  VisualsManager::setOpacity(e, curOpacity);
              });
        }



        //drawing to the rendertex done, attach it to a sprite
        mRenderTex.display();
        sf::Sprite renderbody(mRenderTex.getTexture());

        sf::RenderStates waterstates;

        //apply the shader and draw the rendertex to the target
        if (mShowShaders)
        {
            mWaterShader.setUniform("time", mDistortionTimer.getElapsedTime().asSeconds());
            mWaterShader.setUniform("screenSize", sf::Vector2f{(float)mRenderTex.getSize().x, (float)mRenderTex.getSize().y});
            mWaterShader.setUniform("offset", mRenderTex.getView().getCenter() - mOffset);
            waterstates.shader = &mWaterShader;
        }

        mOffset = mRenderTex.getView().getCenter();

        sf::View storview = target.getView();
        sf::View defview;
        defview.setSize((float)target.getSize().x, (float)target.getSize().y);
        defview.setCenter((float)target.getSize().x/2, (float)target.getSize().y/2);
        target.setView(defview);
        target.draw(renderbody, waterstates);
        target.setView(storview);

        //mGround.render(target, states);

        return true;  //todo meaningful return value
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
        if (!source.loadFromFile(mDistortionTexID))
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
            Logger::endl();
            mShowShaders = false;
        }
    }

    sf::FloatRect Water::getBounds() const
    {
        return mGround.getBounds();
    }
}
