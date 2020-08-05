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

#ifndef UNGOD_LIGHT_HANDLER_H
#define UNGOD_LIGHT_HANDLER_H

#include "ungod/visual/LightManager.h"
#include "owls/Signal.h"

namespace ungod
{
    class World; 

    /** \brief Handles all Lights and LightColliders and is responsible for rendering them. */
    class LightHandler
    {
    public:
        LightHandler();

        void init(LightManager& lightManager);

        /** \brief Renders lights and lightcolliders of a list of entities. */
        void render(const quad::PullResult<Entity>& pull, const World& world, sf::RenderTarget& target, sf::RenderStates states, bool drawShadows = true);

        /** \brief Updates LightAffectors. */
        void update(const std::list<Entity>& entities, float delta);

        /** \brief Sets the color of the ambient light. */
        void setAmbientColor(const sf::Color& color);

        /** \brief Gets the color of the ambient light. */
        sf::Color getAmbientColor() const;

        /**
        * \brief Calling this function over a certain amount of time will result in
        * smoothly color transform to the given color. The strength value should somehow
        * depend on the applications delta value.
        */
        void interpolateAmbientLight(const sf::Color& color, float strength);

        /** \brief Sets the local position of the light of entity e if a LightEmitter
        * component is attached. */
        inline void setLocalLightPosition(Entity e, const sf::Vector2f& position) { setLocalLightPosition(e.modify<LightEmitterComponent>(), e, position); }
        void setLocalLightPosition(LightEmitterComponent& le, Entity e, const sf::Vector2f& position);

        /** \brief Sets the local position of the light of entity e if a MultiLightEmitter
        * component is attached. */
        inline void setLocalLightPosition(Entity e, const sf::Vector2f& position, std::size_t index) { setLocalLightPosition(e.modify<MultiLightEmitter>().getComponent(index), e, position); }

        /** \brief Sets the scale of the light of entity e. Requires a LightEmitter component. */
        inline void setLightScale(Entity e, const sf::Vector2f& scale) { setLightScale(e.modify<LightEmitterComponent>(), e, scale); }
        void setLightScale(LightEmitterComponent& le, Entity e, const sf::Vector2f& scale);

        /** \brief Sets the scale of the light with given index of entity e. Requires a MultiLightEmitter component. */
        inline void setLightScale(Entity e, const sf::Vector2f& scale, std::size_t index) { setLightScale(e.modify<MultiLightEmitter>().getComponent(index), e, scale); }

        /** \brief Sets the origin of the light of entity e. Requires a LightEmitter component. */
        inline void setLightOrigin(Entity e, const sf::Vector2f& origin) { setLightOrigin(e.modify<LightEmitterComponent>(), e, origin); }
        void setLightOrigin(LightEmitterComponent& le, Entity e, const sf::Vector2f& origin);

        /** \brief Sets the origin of the light with given index of entity e. Requires a MultiLightEmitter component. */
        inline void setLightOrigin(Entity e, const sf::Vector2f& origin, std::size_t index) { setLightOrigin(e.modify<MultiLightEmitter>().getComponent(index), e, origin); }


        /** \brief Sets the color of the light of entity e. Requires a LightEmitter component. */
        inline void setLightColor(Entity e, const sf::Color& color) { setLightColor(e.modify<LightEmitterComponent>(), color); }
        void setLightColor(LightEmitterComponent& le, const sf::Color& color);

        /** \brief Sets the color of the light with given index of entity e. Requires a MultiLightEmitter component. */
       inline void setLightColor(Entity e, const sf::Color& color, std::size_t multiIndex) { setLightColor(e.modify<MultiLightEmitter>().getComponent(multiIndex), color); }

        /** \brief Sets the coordinates of the ith point of the LightCollider. Requires ShadowEmitter component. */
        inline void setPoint(Entity e, const sf::Vector2f& point, std::size_t i) { setPoint(e.modify<ShadowEmitterComponent>(), e, point, i); }
        void setPoint(ShadowEmitterComponent& se, Entity e, const sf::Vector2f& point, std::size_t i);

        /** \brief Sets the coordinates of the ith point of the LightCollider with given index. Requires MultiShadowEmitter component. */
        inline void setPoint(Entity e, const sf::Vector2f& point, std::size_t pointIndex, std::size_t colliderIndex)  { setPoint(e.modify<MultiShadowEmitter>().getComponent(colliderIndex), e, point, pointIndex); }

        /** \brief Initializes a given number of edge-points. They have to be set via setPoint(..) */
        inline void setPointCount(Entity e, std::size_t num) { setPointCount(e, e.modify<ShadowEmitterComponent>(), num); }
        void setPointCount(Entity e, ShadowEmitterComponent& se, std::size_t num);

        /** \brief Initializes a given number of edge-points. They have to be set via setPoint(..) */
        inline void setPointCount(Entity e, std::size_t num, std::size_t colliderIndex) { setPointCount(e, e.modify<MultiShadowEmitter>().getComponent(colliderIndex), num); }

        /** \brief Sets the whole coordinate-set of the of the LightCollider. Requires ShadowEmitter component. */
        void setPoints(Entity e, const std::vector<sf::Vector2f>& points);

        /** \brief Sets the whole coordinate-set of the of the LightCollider with given index. Requires MultiShadowEmitter component. */
        void setPoints(Entity e, const std::vector<sf::Vector2f>& points, std::size_t colliderIndex);

        /** \brief Sets the light over shape flag. */
        void setLightOverShape(ShadowEmitterComponent& se, bool lightOverShape);
        inline void setLightOverShape(Entity e, bool lightOverShape) { setLightOverShape(e.modify<ShadowEmitterComponent>(), lightOverShape); }
        inline void setLightOverShape(Entity e, bool lightOverShape, std::size_t colliderIndex) { setLightOverShape(e.modify<MultiShadowEmitter>().getComponent(colliderIndex), lightOverShape); }

        /** \brief Sets the light radius of the light emitter. */
        void setLightRadius(LightEmitterComponent& le, float radius);
        inline void setLightRadius(Entity e, float radius) { setLightRadius(e.modify<LightEmitterComponent>(), radius); }
        inline void setLightRadius(Entity e, float radius, std::size_t colliderIndex) { setLightRadius(e.modify<MultiLightEmitter>().getComponent(colliderIndex), radius); }

        /** \brief Sets the shadow extend multiplier of the light emitter. */
        void setShadowExtendMultiplier(LightEmitterComponent& le, float multiplier);
        inline void setShadowExtendMultiplier(Entity e, float multiplier) { setShadowExtendMultiplier(e.modify<LightEmitterComponent>(), multiplier); }
        inline void setShadowExtendMultiplier(Entity e, float multiplier, std::size_t colliderIndex) { setShadowExtendMultiplier(e.modify<MultiLightEmitter>().getComponent(colliderIndex), multiplier); }

        /** \brief Defines the callback for the affector. Is mandatory to get the
        * affector to work. Requires LightEmitter-component and a LightEffector-component. */
        void setAffectorCallback(Entity e, const std::function<void(float, LightEmitterComponent&)>& callback);

        /** \brief Sets the affector for a MultiLightEmitter-component. */
        void setAffectorCallback(Entity e, std::size_t lightIndex, const std::function<void(float, LightEmitterComponent&)>& callback);

        /** \brief Sets the affector for a MultiLightEmitter-component and MultiLightEmitter-component. */
        void setAffectorCallback(Entity e, std::size_t lightIndex, std::size_t affectorIndex, const std::function<void(float, LightEmitterComponent&)>& callback);

        void setAffectorCallback(const std::function<void(float, LightEmitterComponent&)>& callback, LightAffectorComponent& affector, LightEmitterComponent& emitter);

        /** \brief Loads a custom texture for the light emitter of the entity. */
        inline void loadLightTexture(Entity e, const std::string& textureID) { loadLightTexture(e.modify<LightEmitterComponent>(), textureID); }
        inline void loadLightTexture(Entity e, const std::string& textureID, std::size_t multiIndex) { loadLightTexture(e.modify<MultiLightEmitter>().getComponent(multiIndex), textureID); }
        void loadLightTexture(LightEmitterComponent& le, const std::string& textureID);

        /** \brief Activates or deactivates the light emission. */
        inline void setLightActive(Entity e, bool active) { setLightActive(e.modify<LightEmitterComponent>(), active); }
        inline void setLightActive(Entity e, bool active, std::size_t multiIndex) { setLightActive(e.modify<MultiLightEmitter>().getComponent(multiIndex), active); }
        void setLightActive(LightEmitterComponent& le, bool active);


        /** \brief Registers new callback for the ContentsChanged signal. */
        decltype(auto) onContentsChanged(const std::function<void(Entity, const sf::FloatRect&)>& callback) 
        {
            return mContentsChangedSignal.connect(callback);
        }

        /** \brief Returns the lower bound of the bounding rect around all contents of the given entity. */
        sf::Vector2f getLowerBound(Entity e);

        /** \brief Returns the upper bound of the bounding rect around all contents of the given entity. */
        sf::Vector2f getUpperBound(Entity e);

        /** \brief Methods that move all lights/colliders attached to the given entity. Are usually only
        * used internally by the transform-manager. */
        void moveLights(Entity e, const sf::Vector2f& vec);
        void moveLightColliders(Entity e, const sf::Vector2f& vec);


    private:
        LightManager* mLightManager;
        sf::Color mAmbientColor;
        sf::Vector3f mColorShift;
        sf::Sprite mDisplaySprite;
        owls::Signal<Entity, const sf::FloatRect&> mContentsChangedSignal;

    private:
        void renderLight(sf::RenderTarget& target, sf::RenderStates states, const quad::QuadTree<Entity>& quadtree, Entity e, TransformComponent& lightTransf, LightEmitterComponent& light, bool drawShadows);
    };
}

#endif //UNGOD_LIGHT_HANDLER_H
