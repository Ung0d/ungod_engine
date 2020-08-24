#ifndef UEDIT_LIGHT_ACTIONS_H
#define UEDIT_LIGHT_ACTIONS_H

#include "ungod/base/Entity.h"
#include "ungod/visual/Light.h"

namespace uedit
{
    class ActionManager;

    class LightActions
    {
    public:
        LightActions(ActionManager& am) : mActionManager(am) {}

        inline void setLightPosition(ungod::Entity e, const sf::Vector2f& position)
        {
            setLightPosition(e, e.modify<ungod::LightEmitterComponent>(), position);
        }
        inline void setMultiLightPosition(ungod::Entity e, const sf::Vector2f& position, std::size_t i)
        {
            setLightPosition(e, e.modify<ungod::MultiLightEmitter>().getComponent(i), position);
        }
        void setLightPosition(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Vector2f& position);

        inline void setLightScale(ungod::Entity e, const sf::Vector2f& scale)
        {
            setLightScale(e, e.modify<ungod::LightEmitterComponent>(), scale);
        }
        inline void setMultiLightScale(ungod::Entity e, const sf::Vector2f& scale, std::size_t i)
        {
            setLightScale(e, e.modify<ungod::MultiLightEmitter>().getComponent(i), scale);
        }
        void setLightScale(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Vector2f& scale);


        inline void setLightOrigin(ungod::Entity e, const sf::Vector2f& origin)
        {
            setLightOrigin(e, e.modify<ungod::LightEmitterComponent>(), origin);
        }
        inline void setMultiLightOrigin(ungod::Entity e, const sf::Vector2f& origin, std::size_t i)
        {
            setLightOrigin(e, e.modify<ungod::MultiLightEmitter>().getComponent(i), origin);
        }
        void setLightOrigin(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Vector2f& origin);

        inline void setLightColor(ungod::Entity e, const sf::Color& color)
        {
            setLightColor(e, e.modify<ungod::LightEmitterComponent>(), color);
        }
        inline void setMultiLightColor(ungod::Entity e, const sf::Color& color, std::size_t i)
        {
            setLightColor(e, e.modify<ungod::MultiLightEmitter>().getComponent(i), color);
        }
        void setLightColor(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Color& color);


        inline void setLightRadius(ungod::Entity e, float radius)
        {
            setLightRadius(e, e.modify<ungod::LightEmitterComponent>(), radius);
        }
        inline void setMultiLightRadius(ungod::Entity e, float radius, std::size_t i)
        {
            setLightRadius(e, e.modify<ungod::MultiLightEmitter>().getComponent(i), radius);
        }
        void setLightRadius(ungod::Entity e, ungod::LightEmitterComponent& le, float radius);

        inline void setShadowExtendMultiplier(ungod::Entity e, float multiplier)
        {
            setShadowExtendMultiplier(e, e.modify<ungod::LightEmitterComponent>(), multiplier);
        }
        inline void setMultiShadowExtendMultiplier(ungod::Entity e, float multiplier, std::size_t i)
        {
            setShadowExtendMultiplier(e, e.modify<ungod::MultiLightEmitter>().getComponent(i), multiplier);
        }
        void setShadowExtendMultiplier(ungod::Entity e, ungod::LightEmitterComponent& le, float multiplier);

        inline void setLightActive(ungod::Entity e, bool active)
        {
            setLightActive(e, e.modify<ungod::LightEmitterComponent>(), active);
        }
        inline void setMultiLightActive(ungod::Entity e, bool active, std::size_t i)
        {
            setLightActive(e, e.modify<ungod::MultiLightEmitter>().getComponent(i), active);
        }
        void setLightActive(ungod::Entity e, ungod::LightEmitterComponent& le, bool active);

        inline void loadLightTexture(ungod::Entity e, const std::string& textureID)
        {
            loadLightTexture(e, e.modify<ungod::LightEmitterComponent>(), textureID);
        }
        inline void loadMultiLightTexture(ungod::Entity e, const std::string& textureID, std::size_t i)
        {
            loadLightTexture(e, e.modify<ungod::MultiLightEmitter>().getComponent(i), textureID);
        }
        void loadLightTexture(ungod::Entity e, ungod::LightEmitterComponent& le, const std::string& textureID);

        void setPoint(ungod::Entity e, const sf::Vector2f& point, std::size_t pointIndex)
        {
            setPoint(e, e.modify<ungod::ShadowEmitterComponent>(), point, pointIndex);
        }
        void setMultiPoint(ungod::Entity e, const sf::Vector2f& point, std::size_t pointIndex, std::size_t multiIndex)
        {
            setPoint(e, e.modify<ungod::MultiShadowEmitter>().getComponent(multiIndex), point, pointIndex);
        }
        void setPoint(ungod::Entity e, ungod::ShadowEmitterComponent& shadow, const sf::Vector2f& point, std::size_t pointIndex);

        void setPointCount(ungod::Entity e, std::size_t num)
        {
            setPointCount(e, e.modify<ungod::ShadowEmitterComponent>(), num);
        }
        void setMultiPointCount(ungod::Entity e, std::size_t num, std::size_t multiIndex)
        {
            setPointCount(e, e.modify<ungod::MultiShadowEmitter>().getComponent(multiIndex), num);
        }
        void setPointCount(ungod::Entity e, ungod::ShadowEmitterComponent& shadow, std::size_t num);

        void setPoints(ungod::Entity e, const std::vector<sf::Vector2f>& points);
        void setMultiPoints(ungod::Entity e, const std::vector<sf::Vector2f>& points, std::size_t multiIndex);

        void setLightOverShape(ungod::Entity e, bool lightOverShape)
        {
            setLightOverShape(e, e.modify<ungod::ShadowEmitterComponent>(), lightOverShape);
        }
        void setMultiLightOverShape(ungod::Entity e, bool lightOverShape, std::size_t multiIndex)
        {
            setLightOverShape(e, e.modify<ungod::MultiShadowEmitter>().getComponent(multiIndex), lightOverShape);
        }
        void setLightOverShape(ungod::Entity e, ungod::ShadowEmitterComponent& shadow, bool lightOverShape);

        inline void setAffectorCallback(ungod::Entity e, const std::function<void(float, ungod::LightEmitterComponent&)>& callback)
        {
            setAffectorCallback(e, e.modify<ungod::LightAffectorComponent>(), callback);
        }
        inline void setMultiAffectorCallback(ungod::Entity e, const std::function<void(float, ungod::LightEmitterComponent&)>& callback, std::size_t affectorIndex)
        {
            setAffectorCallback(e, e.modify<ungod::MultiLightAffector>().getComponent(affectorIndex),  callback);
        }
        void setAffectorCallback(ungod::Entity e, ungod::LightAffectorComponent& affector, const std::function<void(float, ungod::LightEmitterComponent&)>& callback);

    private:
        ActionManager& mActionManager;
    };
}

#endif