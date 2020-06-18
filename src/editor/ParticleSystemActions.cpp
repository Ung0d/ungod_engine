#include "ParticleSystemActions.h"
#include "ActionManager.h"

namespace uedit
{
    void ParticleSystemActions::clearAffectors(ungod::Entity e)
    {
        ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
        mActionManager.action(std::function([this](ungod::Entity e)
            { e.getWorld().getParticleSystemHandler().clearAffectors(e); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getParticleSystemHandler().setSystem(e, old); }),
            e);
    }

    void ParticleSystemActions::setParticleMaxForce(ungod::Entity e, float maxforce)
    {
        float old = e.get<ungod::ParticleSystemComponent>().getSystem().getMaxForce();
        mActionManager.action(std::function([this, maxforce](ungod::Entity e)
            { e.getWorld().getParticleSystemHandler().setMaxForce(e, maxforce); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getParticleSystemHandler().setMaxForce(e, old); }),
            e);
    }

    void ParticleSystemActions::setParticleMaxVelocity(ungod::Entity e, float maxvel)
    {
        float old = e.get<ungod::ParticleSystemComponent>().getSystem().getMaxVelocity();
        mActionManager.action(std::function([this, maxvel](ungod::Entity e)
            { e.getWorld().getParticleSystemHandler().setMaxVelocity(e, maxvel); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getParticleSystemHandler().setMaxVelocity(e, old); }),
            e);
    }

    void ParticleSystemActions::setParticleSpeed(ungod::Entity e, float speed)
    {
        float old = e.get<ungod::ParticleSystemComponent>().getSystem().getParticleSpeed();
        mActionManager.action(std::function([this, speed](ungod::Entity e)
            { e.getWorld().getParticleSystemHandler().setParticleSpeed(e, speed); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getParticleSystemHandler().setParticleSpeed(e, old); }),
            e);
    }
}