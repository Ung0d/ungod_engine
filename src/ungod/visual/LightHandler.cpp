/*
* This file is part of the ungod - framework.
* Copyright (C) 2016 Felix Becker - fb132550@uni-greifswald.de
*
* This is a modified version of the Let There Be Light 2 framework.
* See https://github.com/222464/LTBL2
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

#include "ungod/visual/LightHandler.h"
#include "ungod/physics/Physics.h"
#include "ungod/base/World.h"

namespace ungod
{
    LightHandler::LightHandler() : mAmbientColor(sf::Color::White), mColorShift(0,0,0) {}

    void LightHandler::init(LightManager& lightManager)
    {
        mLightManager = lightManager;
    }

    void LightHandler::render(const quad::PullResult<Entity>& pull, const quad::QuadTree<Entity>& quadtree, sf::RenderTarget& target, sf::RenderStates states, bool drawShadows)
    {
        mLightManager.getCompositionTexture().clear(mAmbientColor);
        mLightManager.getCompositionTexture().setView(mCompositionTexture.getDefaultView());

        //iterator over the one-light-components
        dom::Utility<Entity>::iterate<TransformComponent, LightEmitterComponent>(pull.getList(),
          [this, &target, &states, drawShadows] (Entity e, TransformComponent& lightTransf, LightEmitterComponent& light)
          {
              renderLight(target, states, quadtree, e, lightTransf, light, drawShadows);
          });

        //iterate over the multiple-light-components
        dom::Utility<Entity>::iterate<TransformComponent, MultiLightEmitter>(pull.getList(),
          [this, &target, &states, drawShadows] (Entity e, TransformComponent& lightTransf, MultiLightEmitter& light)
          {
              for (std::size_t i = 0; i < light.getComponentCount(); ++i)
              {
                renderLight(target, states, quadtree, e, lightTransf, light.getComponent(i), drawShadows);
              }
          });

        mLightManager.getCompositionTexture().display();

        sf::RenderStates lightstates{};
		lightstates.blendMode = sf::BlendMultiply;

		mDisplaySprite.setTexture(mLightManager.getCompositionTexture().getTexture(), true);
		sf::View view = target.getView();
        target.setView(sf::View{ sf::FloatRect{0.0f,0.0f,(float)target.getSize().x,(float)target.getSize().y} });
		target.draw(mDisplaySprite, lightstates);
		target.setView(view);
    }


    void LightHandler::renderLight(sf::RenderTarget& target, sf::RenderStates states, const quad::QuadTree<Entity>& quadtree, Entity e, TransformComponent& lightTransf, LightEmitterComponent& light, bool drawShadows)
    {
        //pull all entities near the light
        quad::PullResult<Entity> shadowsPull;
        sf::FloatRect bounds = light.mLight.getBoundingBox();
        sf::Vector2f transformedUpperBound = lightTransf.getTransform().transformPoint( {bounds.left, bounds.top} );
        quadtree.retrieve(shadowsPull, { transformedUpperBound.x, transformedUpperBound.y, bounds.width, bounds.height });

        std::vector< std::pair<LightCollider*, TransformComponent*> > colliders;

        if (drawShadows)
        {
            //find the entities with light-colliders that are on the screen
            dom::Utility<Entity>::iterate<TransformComponent, ShadowEmitterComponent>(shadowsPull.getList(),
            [this, &colliders, &light, &lightTransf] (Entity e, TransformComponent& colliderTransf, ShadowEmitterComponent& shadow)
            {
                sf::FloatRect colliderBounds = colliderTransf.getTransform().transformRect( shadow.mLightCollider.getBoundingBox() );
                sf::FloatRect lightBounds = lightTransf.getTransform().transformRect( light.mLight.getBoundingBox() );
                //test if the collider is "in range" of the light. Do not render penumbras otherwise
                if ( colliderBounds.intersects(lightBounds) )
                    colliders.emplace_back( &shadow.mLightCollider, &colliderTransf );
            });

            dom::Utility<Entity>::iterate<TransformComponent, MultiShadowEmitter>(shadowsPull.getList(),
            [this, &colliders, &light, &lightTransf] (Entity e, TransformComponent& colliderTransf, MultiShadowEmitter& shadow)
            {
                for (std::size_t i = 0; i < shadow.getComponentCount(); ++i)
                {
                    sf::FloatRect colliderBounds = colliderTransf.getTransform().transformRect( shadow.getComponent(i).mLightCollider.getBoundingBox() );
                    sf::FloatRect lightBounds = lightTransf.getTransform().transformRect( light.mLight.getBoundingBox() );
                    //test if the collider is "in range" of the light. Do not render penumbras otherwise
                    if ( colliderBounds.intersects(lightBounds) )
                        colliders.emplace_back( &shadow.getComponent(i).mLightCollider, &colliderTransf );
                }
            });
        }

        //render the light and the colliders, draw umbras, penumbras + antumbras
        light.mLight.render(target.getView(), 
                            mLightManager.getLightTexture(),
                            mLightManager.getEmissionTexture(),
                            mLightManager.getAntumbraTexture(),
                            states,
                            colliders, 
                            mUnshadowShader, 
                            mLightOverShapeShader, 
                            lightTransf);

        //draw the resulting texture in the application window
        sf::RenderStates compoRenderStates;
        compoRenderStates.blendMode = sf::BlendAdd;
        mDisplaySprite.setTexture(mLightTexture.getTexture(), true);
        mLightManager.getCompositionTexture().draw(mDisplaySprite, compoRenderStates);
    }

    void LightHandler::update(const std::list<Entity>& entities, float delta)
    {
        //iterate over LightAffectors
        dom::Utility<Entity>::iterate<LightAffectorComponent>(entities,
          [delta, this] (Entity e, LightAffectorComponent& affector)
          {
              if (affector.isActive() && affector.mCallback)
                affector.mCallback( delta, *affector.mEmitter );
          });
        //iterate over MultilightLightAffectors
        dom::Utility<Entity>::iterate<MultiLightAffector>(entities,
          [delta, this] (Entity e, MultiLightAffector& affector)
          {
              for (std::size_t i = 0; i < affector.getComponentCount(); ++i)
              {
                  if (affector.getComponent(i).isActive() && affector.getComponent(i).mCallback)
                    affector.getComponent(i).mCallback( delta, *affector.getComponent(i).mEmitter );
              }
          });
    }

    void LightHandler::setAmbientColor(const sf::Color& color)
    {
        mAmbientColor = color;
    }

    sf::Color LightHandler::getAmbientColor() const
    {
        return mAmbientColor;
    }

    void LightHandler::interpolateAmbientLight(const sf::Color& color, float strength)
    {
        //compute the new color through simple linear interpolation
        mColorShift.x += (float)(color.r - mAmbientColor.r)/strength;
        if(mColorShift.x > 1)
        {
            mColorShift.x --;
            mAmbientColor.r ++;
        }
        else if(mColorShift.x < -1)
        {
            mColorShift.x ++;
            mAmbientColor.r --;
        }
        mColorShift.y += (float)(color.g - mAmbientColor.g)/strength;
        if(mColorShift.y > 1)
        {
            mColorShift.y --;
            mAmbientColor.g ++;
        }
        else if(mColorShift.y < -1)
        {
            mColorShift.y ++;
            mAmbientColor.g --;
        }
        mColorShift.z += (float)(color.b - mAmbientColor.b)/strength;
        if(mColorShift.z > 1)
        {
            mColorShift.z --;
            mAmbientColor.b ++;
        }
        else if(mColorShift.z < -1)
        {
            mColorShift.z ++;
            mAmbientColor.b --;
        }
    }

    void LightHandler::setLocalLightPosition(LightEmitterComponent& le, Entity e, const sf::Vector2f& position)
    {
        le.mLight.mSprite.setPosition(position);
        mContentsChangedSignal(e, le.mLight.getBoundingBox());
    }

    void LightHandler::setLightScale(LightEmitterComponent& le, Entity e, const sf::Vector2f& scale)
    {
        le.mLight.mSprite.setScale(scale);
        mContentsChangedSignal(e, le.mLight.getBoundingBox());
    }

    void LightHandler::setLightOrigin(LightEmitterComponent& le, Entity e, const sf::Vector2f& origin)
    {
        le.mLight.setSourcePoint(origin);
        mContentsChangedSignal(e, le.mLight.getBoundingBox());
    }

     void LightHandler::setLightColor(LightEmitterComponent& le, const sf::Color& color)
    {
        le.mLight.mSprite.setColor(color);
    }

    void LightHandler::setPoint(ShadowEmitterComponent& se, Entity e, const sf::Vector2f& point, std::size_t i)
    {
        se.mLightCollider.setPoint(i, point);
        mContentsChangedSignal(e, se.mLightCollider.getBoundingBox());
    }

    void LightHandler::setPointCount(Entity e, ShadowEmitterComponent& se, std::size_t num)
    {
        se.mLightCollider.setPointCount(num);
        mContentsChangedSignal(e, se.mLightCollider.getBoundingBox());
    }

    void LightHandler::setPoints(Entity e, const std::vector<sf::Vector2f>& points)
    {
        ShadowEmitterComponent& shadow = e.modify<ShadowEmitterComponent>();
        shadow.mLightCollider.setPointCount(points.size());
        for (std::size_t i = 0; i < points.size(); ++i)
            shadow.mLightCollider.setPoint(i, points[i]);
        mContentsChangedSignal(e, shadow.mLightCollider.getBoundingBox());
    }

    void LightHandler::setPoints(Entity e, const std::vector<sf::Vector2f>& points, std::size_t colliderIndex)
    {
        MultiShadowEmitter& multi = e.modify<MultiShadowEmitter>();
        multi.getComponent(colliderIndex).mLightCollider.setPointCount(points.size());
        for (std::size_t i = 0; i < points.size(); ++i)
            multi.getComponent(colliderIndex).mLightCollider.setPoint(i, points[i]);
        mContentsChangedSignal(e, multi.getComponent(colliderIndex).mLightCollider.getBoundingBox());
    }

    void LightHandler::setLightOverShape(ShadowEmitterComponent& se, bool lightOverShape)
    {
        se.mLightCollider.setLightOverShape(lightOverShape);
    }

    void LightHandler::setLightRadius(LightEmitterComponent& le, float radius)
    {
        le.mLight.setRadius(radius);
    }

    void LightHandler::setShadowExtendMultiplier(LightEmitterComponent& le, float multiplier)
    {
        le.mLight.setShadowExtendMultiplier(multiplier);
    }

    void LightHandler::setAffectorCallback(Entity e, const std::function<void(float, LightEmitterComponent&)>& callback)
    {
        setAffectorCallback(callback, e.modify<LightAffectorComponent>(), e.modify<LightEmitterComponent>());
    }

    void LightHandler::setAffectorCallback(Entity e, std::size_t lightIndex, const std::function<void(float, LightEmitterComponent&)>& callback)
    {
        setAffectorCallback(callback, e.modify<LightAffectorComponent>(), e.modify<MultiLightEmitter>().getComponent(lightIndex));
    }

    void LightHandler::setAffectorCallback(Entity e, std::size_t lightIndex, std::size_t affectorIndex, const std::function<void(float, LightEmitterComponent&)>& callback)
    {
        setAffectorCallback(callback, e.modify<MultiLightAffector>().getComponent(affectorIndex), e.modify<MultiLightEmitter>().getComponent(lightIndex));
    }

    void LightHandler::setAffectorCallback(const std::function<void(float, LightEmitterComponent&)>& callback, LightAffectorComponent& affector, LightEmitterComponent& emitter)
    {
        affector.mCallback = callback;
        affector.mEmitter = &emitter;
    }

    void LightHandler::loadLightTexture(LightEmitterComponent& le, const std::string& textureID)
    {
        le.mLight.loadTexture(textureID);
    }

    void LightHandler::setLightActive(LightEmitterComponent& le, bool active)
    {
        le.mLight.setActive(active);
    }

    sf::Vector2f LightHandler::getLowerBound(Entity e)
    {
        sf::Vector2f lowerBounds(0,0);
        if (e.has<LightEmitterComponent>())
        {
            const LightEmitterComponent& emitter = e.get<LightEmitterComponent>();
            sf::FloatRect bbox = emitter.getLight().getBoundingBox();
            lowerBounds.x = std::max(bbox.left + bbox.width, lowerBounds.x);
            lowerBounds.y = std::max(bbox.top + bbox.height, lowerBounds.y);
        }
        if (e.has<MultiLightEmitter>())
        {
            const MultiLightEmitter& emitter = e.get<MultiLightEmitter>();
            for (unsigned i = 0; i <emitter.getComponentCount(); ++i)
            {
                sf::FloatRect bbox = emitter.getComponent(i).getLight().getBoundingBox();
                lowerBounds.x = std::max(bbox.left + bbox.width, lowerBounds.x);
                lowerBounds.y = std::max(bbox.top + bbox.height, lowerBounds.y);
            }
        }
        if (e.has<ShadowEmitterComponent>())
        {
            const ShadowEmitterComponent& emitter = e.get<ShadowEmitterComponent>();
            sf::FloatRect bbox = emitter.mLightCollider.getBoundingBox();
            lowerBounds.x = std::max(bbox.left + bbox.width, lowerBounds.x);
            lowerBounds.y = std::max(bbox.top + bbox.height, lowerBounds.y);
        }
        if (e.has<MultiShadowEmitter>())
        {
            const MultiShadowEmitter& emitter = e.get<MultiShadowEmitter>();
            for (unsigned i = 0; i <emitter.getComponentCount(); ++i)
            {
                sf::FloatRect bbox = emitter.getComponent(i).mLightCollider.getBoundingBox();
                lowerBounds.x = std::max(bbox.left + bbox.width, lowerBounds.x);
                lowerBounds.y = std::max(bbox.top + bbox.height, lowerBounds.y);
            }
        }
        return lowerBounds;
    }

    sf::Vector2f LightHandler::getUpperBound(Entity e)
    {
        sf::Vector2f upperBound(0,0);
        if (e.has<LightEmitterComponent>())
        {
            const LightEmitterComponent& emitter = e.get<LightEmitterComponent>();
            sf::FloatRect bbox = emitter.getLight().getBoundingBox();
            upperBound.x = std::min(bbox.left, upperBound.x);
            upperBound.y = std::min(bbox.top, upperBound.y);
        }
        if (e.has<MultiLightEmitter>())
        {
            const MultiLightEmitter& emitter = e.get<MultiLightEmitter>();
            for (unsigned i = 0; i <emitter.getComponentCount(); ++i)
            {
                sf::FloatRect bbox = emitter.getComponent(i).getLight().getBoundingBox();
                upperBound.x = std::min(bbox.left, upperBound.x);
                upperBound.y = std::min(bbox.top, upperBound.y);
            }
        }
        if (e.has<ShadowEmitterComponent>())
        {
            const ShadowEmitterComponent& emitter = e.get<ShadowEmitterComponent>();
            sf::FloatRect bbox = emitter.mLightCollider.getBoundingBox();
            upperBound.x = std::min(bbox.left, upperBound.x);
            upperBound.y = std::min(bbox.top, upperBound.y);
        }
        if (e.has<MultiShadowEmitter>())
        {
            const MultiShadowEmitter& emitter = e.get<MultiShadowEmitter>();
            for (unsigned i = 0; i <emitter.getComponentCount(); ++i)
            {
                sf::FloatRect bbox = emitter.getComponent(i).mLightCollider.getBoundingBox();
                upperBound.x = std::min(bbox.left, upperBound.x);
                upperBound.y = std::min(bbox.top, upperBound.y);
            }
        }
        return upperBound;
    }

    void LightHandler::moveLights(Entity e, const sf::Vector2f& vec)
    {
        if (e.has<LightEmitterComponent>())
        {
            e.modify<LightEmitterComponent>().mLight.mSprite.move(vec);
        }
        if (e.has<MultiLightEmitter>())
        {
            MultiLightEmitter& light = e.modify<MultiLightEmitter>();
            for (std::size_t i = 0; i < light.getComponentCount(); ++i)
            {
                light.getComponent(i).mLight.mSprite.move(vec);
            }
        }
    }

    void LightHandler::moveLightColliders(Entity e, const sf::Vector2f& vec)
    {
        if (e.has<ShadowEmitterComponent>())
        {
            e.modify<ShadowEmitterComponent>().mLightCollider.mShape.move(vec);
        }
        if (e.has<MultiShadowEmitter>())
        {
            MultiShadowEmitter& shadow = e.modify<MultiShadowEmitter>();
            for (std::size_t i = 0; i < shadow.getComponentCount(); ++i)
            {
                shadow.getComponent(i).mLightCollider.mShape.move(vec);
            }
        }
    }
}
