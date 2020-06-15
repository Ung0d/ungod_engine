#include "WorldActionWrapper.h"
#include "EditorFrame.h"
#include <wx/msgdlg.h>

namespace uedit
{
    WorldActionWrapper::WorldActionWrapper(EditorFrame* eframe) :
        mEFrame(eframe)
    {
    }



    void WorldActionWrapper::removeEntity(ungod::Entity e)
    {
        mEFrame->action(std::function([this](ungod::Entity e){ e.getWorld().getQuadTree().remove(e); }),
                        std::function([this](ungod::Entity e){ e.getWorld().getQuadTree().insert(e); }),
                        e);
        mRemovedEntities.emplace_back(e);
    }

    void WorldActionWrapper::deleteEntities()
    {
        for (const auto& e : mRemovedEntities)
        {
            e.getWorld().destroy(e);
        }
        mRemovedEntities.clear();
    }




    void WorldActionWrapper::setLightPosition(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Vector2f& position)
    {
        auto oldpos = le.getLight().getPosition();
        mEFrame->action(std::function([this, &le, position](ungod::Entity e){ e.getWorld().getLightSystem().setLocalLightPosition(le, e, position); }),
                        std::function([this, &le, oldpos](ungod::Entity e){ e.getWorld().getLightSystem().setLocalLightPosition(le, e, oldpos); }),
                        e);
    }

    void WorldActionWrapper::setLightScale(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Vector2f& scale)
    {
        auto oldscale = le.getLight().getScale();
        mEFrame->action(std::function([this, &le, scale](ungod::Entity e){ e.getWorld().getLightSystem().setLightScale(le, e, scale); }),
                        std::function([this,  &le, oldscale](ungod::Entity e){ e.getWorld().getLightSystem().setLightScale(le, e, oldscale); }),
                        e);
    }

    void WorldActionWrapper::setLightOrigin(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Vector2f& origin)
    {
        auto oldorigin = le.getLight().getSourcePoint();
        mEFrame->action(std::function([this, &le, origin](ungod::Entity e){ e.getWorld().getLightSystem().setLightOrigin(le, e, origin); }),
                        std::function([this, &le, oldorigin](ungod::Entity e){ e.getWorld().getLightSystem().setLightOrigin(le, e, oldorigin); }),
                        e);
    }

    void WorldActionWrapper::setLightColor(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Color& color)
    {
        auto oldcol = le.getLight().getColor();
        mEFrame->action(std::function([this, &le, color](ungod::Entity e){ e.getWorld().getLightSystem().setLightColor(le, color); }),
                        std::function([this, &le, oldcol](ungod::Entity e){ e.getWorld().getLightSystem().setLightColor(le, oldcol); }),
                        e);
    }

    void WorldActionWrapper::setLightRadius(ungod::Entity e, ungod::LightEmitterComponent& le, float radius)
    {
        auto oldrad = le.getLight().getRadius();
        mEFrame->action(std::function([this, &le, radius](ungod::Entity e){ e.getWorld().getLightSystem().setLightRadius(le, radius); }),
                        std::function([this, &le, oldrad](ungod::Entity e){ e.getWorld().getLightSystem().setLightRadius(le, oldrad); }),
                        e);
    }

    void WorldActionWrapper::setShadowExtendMultiplier(ungod::Entity e, ungod::LightEmitterComponent& le, float multiplier)
    {
        auto oldsem = le.getLight().getShadowExtendMultiplier();
        mEFrame->action(std::function([this, &le, multiplier](ungod::Entity e){ e.getWorld().getLightSystem().setShadowExtendMultiplier(le, multiplier); }),
                        std::function([this, &le, oldsem](ungod::Entity e){ e.getWorld().getLightSystem().setShadowExtendMultiplier(le, oldsem); }),
                        e);
    }

    void WorldActionWrapper::setLightActive(ungod::Entity e, ungod::LightEmitterComponent& le, bool active)
    {
        auto oldact = le.getLight().isActive();
        mEFrame->action(std::function([this, &le, active](ungod::Entity e){ e.getWorld().getLightSystem().setLightActive(le, active); }),
                        std::function([this, &le, oldact](ungod::Entity e){ e.getWorld().getLightSystem().setLightActive(le, oldact); }),
                        e);
    }

    void WorldActionWrapper::loadLightTexture(ungod::Entity e, ungod::LightEmitterComponent& le, const std::string& textureID)
    {
        auto oldfile = le.getLight().getImage().getFilePath();
        mEFrame->action(std::function([this, &le, textureID](ungod::Entity e){ e.getWorld().getLightSystem().loadLightTexture(le, textureID); }),
                        std::function([this, &le, oldfile](ungod::Entity e){ e.getWorld().getLightSystem().loadLightTexture(le, oldfile); }),
                        e);
    }

    void WorldActionWrapper::setPoint(ungod::Entity e, ungod::ShadowEmitterComponent& shadow, const sf::Vector2f& point, std::size_t pointIndex)
    {
        auto oldpoint = shadow.getCollider().getPoint(pointIndex);
        mEFrame->action(std::function([this, &shadow, point](ungod::Entity e, std::size_t pointIndex)
                                      { e.getWorld().getLightSystem().setPoint(shadow, e, point, pointIndex); }),
                        std::function([this, &shadow, oldpoint](ungod::Entity e, std::size_t pointIndex)
                                      { e.getWorld().getLightSystem().setPoint(shadow, e, oldpoint, pointIndex); }),
                        e, pointIndex);
    }

    void WorldActionWrapper::setPointCount(ungod::Entity e, ungod::ShadowEmitterComponent& shadow, std::size_t num)
    {
        auto oldnum = shadow.getCollider().getPointCount();
        mEFrame->action(std::function([this, &shadow, num](ungod::Entity e){ e.getWorld().getLightSystem().setPointCount(e, shadow, num); }),
                        std::function([this, &shadow, oldnum](ungod::Entity e){ e.getWorld().getLightSystem().setPointCount(e, shadow, oldnum); }),
                        e);
    }

    void WorldActionWrapper::setPoints(ungod::Entity e, const std::vector<sf::Vector2f>& points)
    {
        std::vector<sf::Vector2f> oldpoints;
        const ungod::LightCollider& lc = e.get<ungod::ShadowEmitterComponent>().getCollider();
        oldpoints.reserve(lc.getPointCount());
        for (std::size_t i = 0 ; i < lc.getPointCount(); ++i)
        {
            oldpoints.emplace_back(lc.getPoint(i));
        }
        mEFrame->action(std::function([this, points](ungod::Entity e){ e.getWorld().getLightSystem().setPoints(e, points); }),
                        std::function([this, oldpoints](ungod::Entity e){ e.getWorld().getLightSystem().setPoints(e, oldpoints); }),
                        e);
    }

    void WorldActionWrapper::setMultiPoints(ungod::Entity e, const std::vector<sf::Vector2f>& points, std::size_t multiIndex)
    {
        std::vector<sf::Vector2f> oldpoints;
        const ungod::LightCollider& lc = e.get<ungod::MultiShadowEmitter>().getComponent(multiIndex).getCollider();
        oldpoints.reserve(lc.getPointCount());
        for (std::size_t i = 0 ; i < lc.getPointCount(); ++i)
        {
            oldpoints.emplace_back(lc.getPoint(i));
        }
        mEFrame->action(std::function([this, points](ungod::Entity e, std::size_t multiIndex){ e.getWorld().getLightSystem().setPoints(e, points, multiIndex); }),
                        std::function([this, oldpoints](ungod::Entity e, std::size_t multiIndex){ e.getWorld().getLightSystem().setPoints(e, oldpoints, multiIndex); }),
                        e, multiIndex);
    }

    void WorldActionWrapper::setLightOverShape(ungod::Entity e, ungod::ShadowEmitterComponent& shadow, bool lightOverShape)
    {
        bool old = shadow.getCollider().getLightOverShape();
        mEFrame->action(std::function([this, &shadow, lightOverShape](ungod::Entity e){ e.getWorld().getLightSystem().setLightOverShape(shadow, lightOverShape); }),
                        std::function([this, &shadow, old](ungod::Entity e){ e.getWorld().getLightSystem().setLightOverShape(shadow, old); }),
                        e);
    }


    void WorldActionWrapper::setAffectorCallback(ungod::Entity e, ungod::LightAffectorComponent& affector, ungod::LightEmitterComponent& emitter, const std::function<void(float, ungod::LightEmitterComponent&)>& callback)
    {
        auto oldcallback = affector.getCallback();
        mEFrame->action(std::function([this, &affector, &emitter, callback](ungod::Entity e)
                                      { e.getWorld().getLightSystem().setAffectorCallback(callback, affector, emitter); }),
                        std::function([this, &affector, &emitter, oldcallback](ungod::Entity e)
                                      { e.getWorld().getLightSystem().setAffectorCallback(oldcallback, affector, emitter); }),
                        e);
    }

    void WorldActionWrapper::attachScript(ungod::Entity e, const std::string& name)
    {
        std::string oldscript = "";
        if (e.get<ungod::EntityBehaviorComponent>().valid())
            oldscript = e.get<ungod::EntityBehaviorComponent>().getScriptName();
        mEFrame->action(std::function([this, name](ungod::Entity e){ e.getWorld().getBehaviorManager().assignBehavior(e, name); }),
                        std::function([this, oldscript](ungod::Entity e)
                                     {
                                         if (oldscript != "")
                                            e.getWorld().getBehaviorManager().assignBehavior(e, oldscript);
                                    }),
                            e);
    }

    void WorldActionWrapper::clearAffectors(ungod::Entity e)
    {
        ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
        mEFrame->action(std::function([this] (ungod::Entity e)
                                      { e.getWorld().getParticleSystemManager().clearAffectors(e); }),
                        std::function([this, old] (ungod::Entity e)
                                      { e.getWorld().getParticleSystemManager().setSystem(e, old); } ),
                        e);
    }

    void WorldActionWrapper::setParticleMaxForce(ungod::Entity e, float maxforce)
    {
        float old = e.get<ungod::ParticleSystemComponent>().getSystem().getMaxForce();
        mEFrame->action(std::function([this, maxforce] (ungod::Entity e)
                                      { e.getWorld().getParticleSystemManager().setMaxForce(e, maxforce); }),
                        std::function([this, old] (ungod::Entity e)
                                      { e.getWorld().getParticleSystemManager().setMaxForce(e, old); } ),
                        e);
    }

    void WorldActionWrapper::setParticleMaxVelocity(ungod::Entity e, float maxvel)
    {
        float old = e.get<ungod::ParticleSystemComponent>().getSystem().getMaxVelocity();
        mEFrame->action(std::function([this, maxvel] (ungod::Entity e)
                                      { e.getWorld().getParticleSystemManager().setMaxVelocity(e, maxvel); }),
                        std::function([this, old] (ungod::Entity e)
                                      { e.getWorld().getParticleSystemManager().setMaxVelocity(e, old); } ),
                        e);
    }

    void WorldActionWrapper::setParticleSpeed(ungod::Entity e, float speed)
    {
        float old = e.get<ungod::ParticleSystemComponent>().getSystem().getParticleSpeed();
        mEFrame->action(std::function([this, speed] (ungod::Entity e)
                                      { e.getWorld().getParticleSystemManager().setParticleSpeed(e, speed); }),
                        std::function([this, old] (ungod::Entity e)
                                      { e.getWorld().getParticleSystemManager().setParticleSpeed(e, old); } ),
                        e);
    }



    void WorldActionWrapper::loadAudioEmitterTrack(ungod::Entity e, const std::string& file)
    {
        std::string old = e.get<ungod::MusicEmitterComponent>().getFilePath();
        mEFrame->action(std::function([this, file] (ungod::Entity e)
                                      { e.getWorld().getAudioManager().getMusicEmitterMixer().loadMusic(e, file); }),
                        std::function([this, old] (ungod::Entity e)
                                      { e.getWorld().getAudioManager().getMusicEmitterMixer().loadMusic(e, old); }),
                        e);
    }

    void WorldActionWrapper::setAudioEmitterDistanceCap(ungod::Entity e, float cap)
    {
        float old = e.get<ungod::MusicEmitterComponent>().getDistanceCap();
        mEFrame->action(std::function([this, cap] (ungod::Entity e)
                                      { e.getWorld().getAudioManager().getMusicEmitterMixer().setDistanceCap(e, cap); }),
                        std::function([this, old] (ungod::Entity e)
                                      { e.getWorld().getAudioManager().getMusicEmitterMixer().setDistanceCap(e, old); }),
                        e);
    }

    void WorldActionWrapper::setAudioEmitterActive(ungod::Entity e, bool active)
    {
        bool old = e.get<ungod::MusicEmitterComponent>().isActive();
        mEFrame->action(std::function([this, active] (ungod::Entity e)
                                      { e.getWorld().getAudioManager().getMusicEmitterMixer().setEmitterActive(e, active); }),
                        std::function([this, old] (ungod::Entity e)
                                      { e.getWorld().getAudioManager().getMusicEmitterMixer().setEmitterActive(e, old); }),
                        e);
    }

    void WorldActionWrapper::setAudioEmitterVolume(ungod::Entity e, float vol)
    {
        float old = e.get<ungod::MusicEmitterComponent>().isActive();
        mEFrame->action(std::function([this, vol] (ungod::Entity e)
                                      { e.getWorld().getAudioManager().getMusicEmitterMixer().setEmitterVolume(e, vol); }),
                        std::function([this, old] (ungod::Entity e)
                                      { e.getWorld().getAudioManager().getMusicEmitterMixer().setEmitterVolume(e, old); }),
                        e);
    }

    void WorldActionWrapper::paintTile(ungod::TilemapBrush& brush, const sf::Vector2f& worldpos, bool connect)
    {
        if (!brush.getTilemap())
            return;
        ungod::Tile const* tile = brush.getTilemap()->getTiledata(worldpos);
        if (!tile)
            return;
        int tileID = tile->getTileID();
        mEFrame->action(std::function([&brush, worldpos, connect] ()
                                      { brush.paintTile(worldpos, connect); }),
                        std::function([&brush, worldpos, tileID] ()
                                      { brush.eraseTile(worldpos, tileID); }));
    }

    void WorldActionWrapper::eraseTile(ungod::TilemapBrush& brush, const sf::Vector2f& worldpos, int erasingID)
    {
        mEFrame->action(std::function([&brush, worldpos, erasingID] ()
                                      { brush.eraseTile(worldpos, erasingID); }),
                        std::function([&brush, worldpos] ()
                                      { brush.paintTile(worldpos); }));
    }
}
