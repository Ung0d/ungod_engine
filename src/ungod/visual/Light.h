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

#ifndef LIGHT_H
#define LIGHT_H

#include <SFML/Graphics.hpp>
#include "owls/Signal.h"
#include "quadtree/QuadTree.h"
#include "ungod/visual/Image.h"
#include "ungod/base/Transform.h"
#include "ungod/base/MultiComponent.h"

namespace ungod
{
    struct Penumbra;

    /** \brief A base class for lights and light-colliders. Provides basic functionality for
    * disable and enable the derived device. */
    class BaseLight
    {
    public:
        BaseLight();

        /** \brief Sets the active status. */
        void setActive(bool active);

        /** \brief Returns true if the device is currently active. */
        bool isActive() const;

        /** \brief Toggles the active status (flips the bool). */
        void toggleActive();

        /** \brief Renders penumbras to the texture. */
        void unmaskWithPenumbras(sf::RenderTexture& renderTexture,
                                 sf::RenderStates states,
                                 sf::Shader& unshadowShader,
                                 const std::vector<Penumbra>& penumbras,
                                 float shadowExtension) const;

    private:
        bool mActive; ///<states whether the object is currently active, that means it performs its underlying actions
    };

    /** \brief A collider for lights. Will cause the casting of shadows.
    * Light colliders are assumed to be convex polygons. */
    class LightCollider : public BaseLight
    {
    friend class LightSystem;
    public:
        LightCollider();
        LightCollider(std::size_t numPoints);

        sf::FloatRect getBoundingBox() const;

        void setPointCount(std::size_t numPoints);

        std::size_t getPointCount() const;

        void setPoint(std::size_t index, const sf::Vector2f& point);

        sf::Vector2f getPoint(std::size_t index) const;

        const sf::Transform& getTransform() const;

        bool getLightOverShape() const;

        void setLightOverShape(bool los);

        void render(sf::RenderTarget& target, sf::RenderStates states);

        void setColor(const sf::Color& color);

        const sf::ConvexShape& getShape() const;

    private:
        sf::ConvexShape mShape;
        bool mLightOverShape;
    };


    /** \brief A lightsource that emits light from a source point for certain radius.
    * The rendered light will break on LightColliders withins the lights radius and will cast
    * shadows with natural penumbras/antumbra. */
    class PointLight : public BaseLight
    {
    friend class LightSystem;
    friend class LightFlickering;
    friend class RandomizedFlickering;
    public:
        PointLight(const std::string& texturePath = DEFAULT_TEXTURE_PATH);

        sf::FloatRect getBoundingBox() const;

        void render(const sf::View& view,
                    sf::RenderTexture& lightTexture,
                    sf::RenderTexture& emissionTexture,
                    sf::RenderTexture& antumbraTexture,
					sf::RenderStates states,
                    const std::vector< std::pair<LightCollider*, TransformComponent*> >& colliders,
                    sf::Shader& unshadowShader,
                    sf::Shader& lightOverShapeShader,
                    const TransformComponent& transf) const;

        /** \brief Loads a texture for the light source. Replaces the default texture. */
        void loadTexture(const std::string& path = DEFAULT_TEXTURE_PATH);

        /** \brief Returns the current color of the light. */
        sf::Color getColor() const;

        /** \brief Sets the color of the light. */
        void setColor(const sf::Color& color);

        /** \brief Sets the size of the light. */
        sf::Vector2f getScale() const;

        /** \brief Gets the current local position of the light. */
        sf::Vector2f getPosition() const;

        /** \brief Sets the source point (where the lights origin is) in local coordinates. */
        void setSourcePoint(const sf::Vector2f& source);

        /** \brief Returns the source point (where the lights origin is) in local coordinates. */
        sf::Vector2f getSourcePoint() const;

        /** \brief Returns the correctly transformed source point of the light.
        * This is the center position of the underlying sprite with all
        * transformations applied. */
        sf::Vector2f getCastCenter() const;

        /** \brief Returns a reference to the texture of the light. */
        const Image& getImage() const;

        /** \brief Returns the radius of the light. */
        float getRadius() const;

        /** \brief Sets the radius of the light. */
        void setRadius(float radius);

        /** \brief Returns the shadow extend multiplier, that expresses how much the light extends
        * over the edges of light colliders. */
        float getShadowExtendMultiplier() const;

        /** \brief Sets the shadow extend multiplier, that expresses how much the light extends
        * over the edges of light colliders. */
        void setShadowExtendMultiplier(float multiplier);

        /** \brief Computes the coordinates of the penumbras in 2d space. */
        void getPenumbrasPoint(std::vector<Penumbra>& penumbras,
                               std::vector<int>& innerBoundaryIndices,
                               std::vector<sf::Vector2f>& innerBoundaryVectors,
                               std::vector<int>& outerBoundaryIndices,
                               std::vector<sf::Vector2f>& outerBoundaryVectors,
                               const LightCollider& collider,
                               const TransformComponent& colliderTransform,
                               const TransformComponent& lightTransform) const;

    private:
        sf::Sprite mSprite;
        sf::Vector2f mSourcePoint;
        float mRadius;
        float mShadowOverExtendMultiplier;
        Image mTexture;

    public:
        static const std::string DEFAULT_TEXTURE_PATH;
        static constexpr float DEFAULT_RADIUS = 10.0f;
        static constexpr float DEFAULT_SHADOW_EXTEND_MULTIPLIER = 1.4f;
    };

    /** \brief A struct modelling a penumbra (border reagion of a shadow). */
    struct Penumbra
    {
        sf::Vector2f source;
        sf::Vector2f lightEdge;
        sf::Vector2f darkEdge;
        float lightBrightness;
        float darkBrightness;
        float distance;
    };


    /**
    * \ingroup Components
    * \brief A component for entities that should have the ability to block light and
    * cast shadows. */
    class ShadowEmitterComponent : public Serializable<ShadowEmitterComponent>
    {
    friend class LightSystem;
     friend struct SerialBehavior<ShadowEmitterComponent, Entity, const World&, const Application&>;
    private:
        LightCollider mLightCollider;

    public:
        const LightCollider& getCollider() const { return mLightCollider; }
    };

    /**
    * \ingroup Components
    * \brief Same as shadow emitter but can hold multiple colliders (for a small overhead). Use only if collider
    * count will be greater then 1. */
    using MultiShadowEmitter = MultiComponent<ShadowEmitterComponent>;


    /**
    * \ingroup Components
    * \brief A component for entities that should have the ability to hold exactly one point light. */
    class LightEmitterComponent : public Serializable<LightEmitterComponent>
    {
    friend class LightSystem;
    friend class LightFlickering;
    friend class RandomizedFlickering;
     friend struct SerialBehavior<LightEmitterComponent, Entity, const World&, const Application&>;
    private:
        PointLight mLight;
    public:
        LightEmitterComponent() = default;
        LightEmitterComponent(const std::string& texture) : mLight(texture) {}

        PointLight& getLight() { return mLight; }
        const PointLight& getLight() const { return mLight; }
    };

    /**
    * \ingroup Components
    * \brief A component for entities that should have the ability to hold multiple point lights. */
    using MultiLightEmitter = MultiComponent<LightEmitterComponent>;


    /**
    * \ingroup Components
    * \brief A component that can be attached to an entity in addition to a LightEmitter. This component
    * will apply effects to the light that are defined in a callback function, that are updated each frame.
    * For example flickering. This file also provides templates for those callback functions. */
    class LightAffectorComponent : public Serializable<LightAffectorComponent>
    {
    friend class LightSystem;
    private:
        std::function<void(float, LightEmitterComponent&)> mCallback;
        LightEmitterComponent* mEmitter;
        bool mActive;

    public:
        LightAffectorComponent();

        void setActive(bool active);

        bool isActive() const;

        const std::function<void(float, LightEmitterComponent&)>& getCallback() const { return mCallback;  }
    };

    /**
    * \ingroup Components
    * \brief A LightAffector that can affect multiple lights. Only meaningful, if used in combination with a
    * MultiLightEmitter-component. */
    using MultiLightAffector = MultiComponent<LightAffectorComponent>;


    /** \brief A callable object, that can be used as a callback for LightAffector.
    * LightFlickering will make a light object flicker, randomly or continously.
    * Note that you can also use this template in your own callbacks.
    * Also note that this light will modify the scale of the lights-sprite. However the bounding rect of the light
    * is promised to be never bigger then in the original state and the quad-tree doesnt have to be updated when flickering. */
    class LightFlickering
    {
    private:
        bool mDirection;
        const float mPeriod;
        const float mStrength;
        sf::Clock mTimer;

    public:
        LightFlickering(float period, float strength);

        void operator() (float delta, LightEmitterComponent& light);
    };
    class RandomizedFlickering
    {
    private:
        bool mDirection;
        float mPeriod;
        const float mBasePeriod;
        const float mStrength;
        sf::Clock mTimer;
        float mSizeMemorizer;

    public:
        RandomizedFlickering(float basePeriod, float strength);

        void operator() (float delta, LightEmitterComponent& light);
    };


    /** \brief The bring-it-all-together class. Handles all Lights and LightColliders and is responsible for
    * rendering them. */
    class LightSystem : sf::NonCopyable
    {
    public:
        LightSystem();

        /** \brief Instantiates the light system will a pointer to the world-quadtree and filepaths to the required shaders.
        * Also requires a path to the penumbra-texture. */
        void init(Application& app, quad::QuadTree<Entity>* quadtree,
                  const sf::Vector2u &imageSize,
                  const std::string& unshadowVertex,
                  const std::string& unshadowFragment,
                  const std::string& lightOverShapeVertex,
                  const std::string& lightOverShapeFragment,
                  const std::string& penumbraTexture);

        /** \brief Updates the size of the underlying render-textures (e.g. if the window was resized). */
        void setImageSize(const sf::Vector2u &imageSize);

        /** \brief Renders lights and lightcolliders of a list of entities. */
        void render(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states, bool drawShadows = true);

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
        void onContentsChanged(const std::function<void(Entity, const sf::IntRect&)>& callback);

        /** \brief Returns the lower bound of the bounding rect around all contents of the given entity. */
        sf::Vector2f getLowerBound(Entity e);

        /** \brief Returns the upper bound of the bounding rect around all contents of the given entity. */
        sf::Vector2f getUpperBound(Entity e);

        /** \brief Methods that move all lights/colliders attached to the given entity. Are usually only
        * used internally by the transform-manager. */
        void moveLights(Entity e, const sf::Vector2f& vec);
        void moveLightColliders(Entity e, const sf::Vector2f& vec);

        ~LightSystem();


    private:
        sf::RenderTexture mLightTexture, mEmissionTexture, mAntumbraTexture, mCompositionTexture;
        sf::Shader mUnshadowShader, mLightOverShapeShader;
        quad::QuadTree<Entity>* mQuadTree;
        ungod::Image mPenumbraTexture;
        sf::Color mAmbientColor;
        sf::Sprite mDisplaySprite;
        sf::Vector3f mColorShift;
        owls::Signal<Entity, const sf::IntRect&> mContentsChangedSignal;
        owls::SignalLink<void, const sf::Vector2u&> mAppSignalLink;

    private:
        void renderLight(sf::RenderTarget& target, sf::RenderStates states, Entity e, TransformComponent& lightTransf, LightEmitterComponent& light, bool drawShadows);
    };
}

#endif //LIGHT_H
