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

#include "ungod/content/particle_system/Emitter.h"
#include "ungod/content/particle_system/Distributions.h"
#include "ungod/content/particle_system/ParticleSystem.h"

namespace ungod
{
        void UniversalEmitter::init(const ParticleFunctorMaster* master)
        {
            mMaster = master;
            mPosInit = mMaster->getPositionDistributions().makeHandle<FixedPosition>(PS_FIXED_POSITION, sf::Vector2f{0,0});
            mVelInit = mMaster->getVelocityDistributions().makeHandle<FixedVelocity>(PS_FIXED_VELOCITY, sf::Vector2f{1,0});
            mSpawnInterval = mMaster->getIntervalDistributions().makeHandle<IntervalTick>(PS_INTERVAL_TICK, 200.0f, 200.0f, 1);
            mLifetimeInit = mMaster->getLifetimeDistributions().makeHandle<IntervalLifetime>(PS_INTERVAL_LIFETIME, 1000.0f, 3000.0f);
        }

        std::string UniversalEmitter::getPositionDistKey() const
        {
            if (mMaster)
                return mMaster->getPositionDistributions().getKey(mPosInit);
            else
                return {};
        }

        std::string UniversalEmitter::getVelocityDistKey() const
        {
            if (mMaster)
                return mMaster->getVelocityDistributions().getKey(mVelInit);
            else
                return {};
        }

        std::string UniversalEmitter::getSpawnIntervalKey() const
        {
            if (mMaster)
                return mMaster->getIntervalDistributions().getKey(mSpawnInterval);
            else
                return {};
        }

        std::string UniversalEmitter::getLifetimeDistKey() const
        {
            if (mMaster)
                return mMaster->getLifetimeDistributions().getKey(mLifetimeInit);
            else
                return {};
        }

        void universalEmitter(UniversalEmitter& data, ParticleSystem& particles, float delta)
        {
            int num = data.getMaster()->getIntervalDistributions().invoke(data.getSpawnInterval(), delta);
            for (int i = 0; i < num; i++)
            {
                particles.spawnParticle(data.getMaster()->getPositionDistributions().invoke(data.getPositionDist()),
                                        data.getMaster()->getVelocityDistributions().invoke(data.getVelocityDist()),
                                        data.getMaster()->getLifetimeDistributions().invoke(data.getLifetimeDist()));
            }
        }

        std::size_t universalCountEstimate(UniversalEstimator&, const FunctorHandle& emitter)
        {
            return (std::size_t)emitter.data->as<UniversalEmitter>()->getMaster()->getIntervalDistributions().invoke(emitter.data->as<UniversalEmitter>()->getSpawnInterval(), 1000.0f);
        }
}

