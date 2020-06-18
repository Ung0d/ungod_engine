#ifndef UEDIT_PARTICLE_SYSTEM_ACTIONS_H
#define UEDIT_PARTICLE_SYSTEM_ACTIONS_H

#include "ungod/base/Entity.h"
#include "ungod/content/particle_system/ParticleSystem.h"

namespace uedit
{
    class ActionManager;

    class ParticleSystemActions
    {
    public:
        ParticleSystemActions(ActionManager& am) : mActionManager(am) {}
        template<typename EMITTER_DATA, typename ESTIMATOR_DATA, typename ... PARAM>
        void setEmitter(ungod::Entity e, const std::string& emitterkey, const std::string& estimatorkey, PARAM&& ... param);

        template<typename DATA, typename ... PARAM>
        void setPositionDist(ungod::Entity e, const std::string& key, PARAM&& ... param);
        template<typename DATA, typename ... PARAM>
        void setVelocityDist(ungod::Entity e, const std::string& key, PARAM&& ... param);
        template<typename DATA, typename ... PARAM>
        void setSpawnInterval(ungod::Entity e, const std::string& key, PARAM&& ... param);
        template<typename DATA, typename ... PARAM>
        void setLifetimeDist(ungod::Entity e, const std::string& key, PARAM&& ... param);
        template<typename DATA, typename ... PARAM>
        void addAffector(ungod::Entity e, const std::string& key, PARAM&& ... param);
        template<typename DATA, typename ... PARAM>
        void setTexrectInitializer(ungod::Entity e, const std::string& key, PARAM&& ... param);
        void clearAffectors(ungod::Entity e);
        void setParticleMaxForce(ungod::Entity e, float maxforce);
        void setParticleMaxVelocity(ungod::Entity e, float maxvel);
        void setParticleSpeed(ungod::Entity e, float speed);

        template<typename DATA, typename F>
        void alterPositionDist(ungod::Entity e, const F& func);
        template<typename DATA, typename F>
        void alterVelocityDist(ungod::Entity e, const F& func);
        template<typename DATA, typename F>
        void alterSpawnInterval(ungod::Entity e, const F& func);
        template<typename DATA, typename F>
        void alterLifetimeDist(ungod::Entity e, const F& func);
        template<typename DATA, typename F>
        void alterTexrectInitializer(ungod::Entity e, const F& func);
        template<typename DATA, typename F>
        void alterAffector(ungod::Entity e, std::size_t i, const F& func);

    private:
        ActionManager& mActionManager;
    };
}

#endif