/*
* This file is part of the ungod - framework.
* Copyright (C) 2016 Felix Becker - fb132550@uni-greifswald.de
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

#include "ungod/content/particle_system/ParticleComponent.h"

namespace ungod
{
    void ParticleSystemHandler::update(const std::list<Entity>& entities, float delta)
    {
        if (mAABBUpdate.getElapsedTime().asMilliseconds() >= mRectUpdateTimer)
        {
            mAABBUpdate.restart();
            dom::Utility<Entity>::iterate<TransformComponent, ParticleSystemComponent>(entities,
              [this, delta] (Entity e, TransformComponent&, ParticleSystemComponent& ps)
              {
                  ps.mParticleSystem->update(delta);
                  mContentsChangedSignal(e, e.modify<ParticleSystemComponent>().mParticleSystem->getBounds());
              });
        }
        else
        {
            dom::Utility<Entity>::iterate<ParticleSystemComponent>(entities,
              [this, delta] (Entity e, ParticleSystemComponent& ps)
              {
                  ps.mParticleSystem->update(delta);
              });
        }

    }

    void ParticleSystemHandler::setSystem(Entity e, const ParticleSystem& p) const
    {
        e.modify<ParticleSystemComponent>().mParticleSystem = p;
    }

    void ParticleSystemHandler::onContentsChanged(const std::function<void(Entity, const sf::FloatRect&)>& callback)
    {
        mContentsChangedSignal.connect(callback);
    }

    void ParticleSystemHandler::onEmitterChanged(const std::function<void(Entity, const std::string&, const PSData&)>& callback)
    {
        mEmitterChangedSignal.connect(callback);
    }

    void ParticleSystemHandler::onTexrectInitChanged(const std::function<void(Entity, const std::string&, const PSData&)>& callback)
    {
        mTexRectInitChangedSignal.connect(callback);
    }

    void ParticleSystemHandler::onAffectorsChanged(const std::function<void(Entity, const std::string&, const PSData&)>& callback)
    {
        mAffectorChangedSignal.connect(callback);
    }

    void ParticleSystemHandler::handleParticleSystemAdded(Entity e)
    {
        if (!e.modify<ParticleSystemComponent>().mParticleSystem)
            e.modify<ParticleSystemComponent>().mParticleSystem.emplace( sParticleFunctorMaster );
    }

    void ParticleSystemHandler::setRectUpdateTimer(int rectupd)
    {
        mRectUpdateTimer = rectupd;
        mAABBUpdate.restart();
    }

    sf::Vector2f ParticleSystemHandler::getLowerBound(Entity e) const
    {
        if (e.has<ParticleSystemComponent>())
        {
            auto b = e.modify<ParticleSystemComponent>().mParticleSystem->getBounds();
            return sf::Vector2f {b.left + b.width, b.top + b.height};
        }
        else
        {
            return sf::Vector2f{0,0};
        }
    }

    sf::Vector2f ParticleSystemHandler::getUpperBound(Entity e) const
    {
        if (e.has<ParticleSystemComponent>())
        {
            auto b = e.modify<ParticleSystemComponent>().mParticleSystem->getBounds();
            return sf::Vector2f {b.left, b.top};
        }
        else
        {
            return sf::Vector2f{0,0};
        }
    }

    std::string ParticleSystemHandler::getEmitterKey(Entity e) const
    {
        return e.get<ParticleSystemComponent>().mParticleSystem->getEmitterKey();
    }

    std::string ParticleSystemHandler::getPositionDistKey(Entity e) const
    {
        return e.get<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getPositionDistKey();
    }

    std::string ParticleSystemHandler::getVelocityDistKey(Entity e) const
    {
        return e.get<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getVelocityDistKey();
    }

    std::string ParticleSystemHandler::getSpawnIntervalKey(Entity e) const
    {
        return e.get<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getSpawnIntervalKey();
    }

    std::string ParticleSystemHandler::getLifetimeDistKey(Entity e) const
    {
        return e.get<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getLifetimeDistKey();
    }

    std::string ParticleSystemHandler::getAffectorKey(Entity e, std::size_t i) const
    {
        return e.get<ParticleSystemComponent>().mParticleSystem->getAffectorKey(i);
    }

    std::string ParticleSystemHandler::getTexrectInitializerKey(Entity e) const
    {
        return e.get<ParticleSystemComponent>().mParticleSystem->getTexrectInitializerKey();
    }
}
