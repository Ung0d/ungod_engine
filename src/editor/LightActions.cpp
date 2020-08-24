#include "LightActions.h"
#include "ActionManager.h"

namespace uedit
{
    void LightActions::setLightPosition(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Vector2f& position)
    {
        auto oldpos = le.getLight().getPosition();
        mActionManager.action(std::function([this, &le, position](ungod::Entity e) { e.getWorld().getLightHandler().setLocalLightPosition(le, e, position); }),
            std::function([this, &le, oldpos](ungod::Entity e) { e.getWorld().getLightHandler().setLocalLightPosition(le, e, oldpos); }),
            e);
    }

    void LightActions::setLightScale(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Vector2f& scale)
    {
        auto oldscale = le.getLight().getScale();
        mActionManager.action(std::function([this, &le, scale](ungod::Entity e) { e.getWorld().getLightHandler().setLightScale(le, e, scale); }),
            std::function([this, &le, oldscale](ungod::Entity e) { e.getWorld().getLightHandler().setLightScale(le, e, oldscale); }),
            e);
    }

    void LightActions::setLightOrigin(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Vector2f& origin)
    {
        auto oldorigin = le.getLight().getSourcePoint();
        mActionManager.action(std::function([this, &le, origin](ungod::Entity e) { e.getWorld().getLightHandler().setLightOrigin(le, e, origin); }),
            std::function([this, &le, oldorigin](ungod::Entity e) { e.getWorld().getLightHandler().setLightOrigin(le, e, oldorigin); }),
            e);
    }

    void LightActions::setLightColor(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Color& color)
    {
        auto oldcol = le.getLight().getColor();
        mActionManager.action(std::function([this, &le, color](ungod::Entity e) { e.getWorld().getLightHandler().setLightColor(le, color); }),
            std::function([this, &le, oldcol](ungod::Entity e) { e.getWorld().getLightHandler().setLightColor(le, oldcol); }),
            e);
    }

    void LightActions::setLightRadius(ungod::Entity e, ungod::LightEmitterComponent& le, float radius)
    {
        auto oldrad = le.getLight().getRadius();
        mActionManager.action(std::function([this, &le, radius](ungod::Entity e) { e.getWorld().getLightHandler().setLightRadius(le, radius); }),
            std::function([this, &le, oldrad](ungod::Entity e) { e.getWorld().getLightHandler().setLightRadius(le, oldrad); }),
            e);
    }

    void LightActions::setShadowExtendMultiplier(ungod::Entity e, ungod::LightEmitterComponent& le, float multiplier)
    {
        auto oldsem = le.getLight().getShadowExtendMultiplier();
        mActionManager.action(std::function([this, &le, multiplier](ungod::Entity e) { e.getWorld().getLightHandler().setShadowExtendMultiplier(le, multiplier); }),
            std::function([this, &le, oldsem](ungod::Entity e) { e.getWorld().getLightHandler().setShadowExtendMultiplier(le, oldsem); }),
            e);
    }

    void LightActions::setLightActive(ungod::Entity e, ungod::LightEmitterComponent& le, bool active)
    {
        auto oldact = le.getLight().isActive();
        mActionManager.action(std::function([this, &le, active](ungod::Entity e) { e.getWorld().getLightHandler().setLightActive(le, active); }),
            std::function([this, &le, oldact](ungod::Entity e) { e.getWorld().getLightHandler().setLightActive(le, oldact); }),
            e);
    }

    void LightActions::loadLightTexture(ungod::Entity e, ungod::LightEmitterComponent& le, const std::string& textureID)
    {
        auto oldfile = le.getLight().getImage().getFilePath();
        mActionManager.action(std::function([this, &le, textureID](ungod::Entity e) { e.getWorld().getLightHandler().loadLightTexture(le, textureID); }),
            std::function([this, &le, oldfile](ungod::Entity e) { e.getWorld().getLightHandler().loadLightTexture(le, oldfile); }),
            e);
    }

    void LightActions::setPoint(ungod::Entity e, ungod::ShadowEmitterComponent& shadow, const sf::Vector2f& point, std::size_t pointIndex)
    {
        auto oldpoint = shadow.getCollider().getPoint(pointIndex);
        mActionManager.action(std::function([this, &shadow, point](ungod::Entity e, std::size_t pointIndex)
            { e.getWorld().getLightHandler().setPoint(shadow, e, point, pointIndex); }),
            std::function([this, &shadow, oldpoint](ungod::Entity e, std::size_t pointIndex)
                { e.getWorld().getLightHandler().setPoint(shadow, e, oldpoint, pointIndex); }),
            e, pointIndex);
    }

    void LightActions::setPointCount(ungod::Entity e, ungod::ShadowEmitterComponent& shadow, std::size_t num)
    {
        auto oldnum = shadow.getCollider().getPointCount();
        mActionManager.action(std::function([this, &shadow, num](ungod::Entity e) { e.getWorld().getLightHandler().setPointCount(e, shadow, num); }),
            std::function([this, &shadow, oldnum](ungod::Entity e) { e.getWorld().getLightHandler().setPointCount(e, shadow, oldnum); }),
            e);
    }

    void LightActions::setPoints(ungod::Entity e, const std::vector<sf::Vector2f>& points)
    {
        std::vector<sf::Vector2f> oldpoints;
        const ungod::LightCollider& lc = e.get<ungod::ShadowEmitterComponent>().getCollider();
        oldpoints.reserve(lc.getPointCount());
        for (std::size_t i = 0; i < lc.getPointCount(); ++i)
        {
            oldpoints.emplace_back(lc.getPoint(i));
        }
        mActionManager.action(std::function([this, points](ungod::Entity e) { e.getWorld().getLightHandler().setPoints(e, points); }),
            std::function([this, oldpoints](ungod::Entity e) { e.getWorld().getLightHandler().setPoints(e, oldpoints); }),
            e);
    }

    void LightActions::setMultiPoints(ungod::Entity e, const std::vector<sf::Vector2f>& points, std::size_t multiIndex)
    {
        std::vector<sf::Vector2f> oldpoints;
        const ungod::LightCollider& lc = e.get<ungod::MultiShadowEmitter>().getComponent(multiIndex).getCollider();
        oldpoints.reserve(lc.getPointCount());
        for (std::size_t i = 0; i < lc.getPointCount(); ++i)
        {
            oldpoints.emplace_back(lc.getPoint(i));
        }
        mActionManager.action(std::function([this, points](ungod::Entity e, std::size_t multiIndex) { e.getWorld().getLightHandler().setPoints(e, points, multiIndex); }),
            std::function([this, oldpoints](ungod::Entity e, std::size_t multiIndex) { e.getWorld().getLightHandler().setPoints(e, oldpoints, multiIndex); }),
            e, multiIndex);
    }

    void LightActions::setLightOverShape(ungod::Entity e, ungod::ShadowEmitterComponent& shadow, bool lightOverShape)
    {
        bool old = shadow.getCollider().getLightOverShape();
        mActionManager.action(std::function([this, &shadow, lightOverShape](ungod::Entity e) { e.getWorld().getLightHandler().setLightOverShape(shadow, lightOverShape); }),
            std::function([this, &shadow, old](ungod::Entity e) { e.getWorld().getLightHandler().setLightOverShape(shadow, old); }),
            e);
    }


    void LightActions::setAffectorCallback(ungod::Entity e, ungod::LightAffectorComponent& affector, const std::function<void(float, ungod::LightEmitterComponent&)>& callback)
    {
        auto oldcallback = affector.getCallback();
        mActionManager.action(std::function([this, &affector, callback](ungod::Entity e)
            { e.getWorld().getLightHandler().setAffectorCallback(callback, affector); }),
            std::function([this, &affector, oldcallback](ungod::Entity e)
                { e.getWorld().getLightHandler().setAffectorCallback(oldcallback, affector); }),
            e);
    }
}