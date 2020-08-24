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

#ifndef UNGOD_LIGHT_H
#define UNGOD_LIGHT_H

#include <SFML/Graphics.hpp>
#include "ungod/base/Transform.h"
#include "ungod/base/MultiComponent.h"
#include "ungod/visual/Image.h"

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
    friend class LightHandler;
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
    friend class LightHandler;
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
    friend class LightHandler;
     friend struct SerialBehavior<ShadowEmitterComponent, Entity>;
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
    friend class LightHandler;
    friend class LightFlickering;
    friend class RandomizedFlickering;
     friend struct SerialBehavior<LightEmitterComponent, Entity>;
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
    friend class LightHandler;
    private:
        std::function<void(float, LightEmitterComponent&)> mCallback;
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
}

#endif //UNGOD_LIGHT_H
