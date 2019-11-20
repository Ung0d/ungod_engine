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


#ifndef UNGOD_PARTICLE_EMITTER_H
#define UNGOD_PARTICLE_EMITTER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include "owls/Signal.h"
#include "ungod/content/particle_system/ParticleFunctorMaster.h"
#include "ungod/serialization/SerialParticleSystem.h"

namespace ungod
{
    class ParticleSystem;
    class ParticleFunctorMaster;

    /** \brief Data for the DirectionalForce affector. */
    class UniversalEmitter : public BaseFunctorData<ParticleFunctorMaster*>
    {
     friend struct SerialBehavior<UniversalEmitter, ParticleFunctorMaster*>;
    friend class DeserialBehavior<UniversalEmitter, ParticleFunctorMaster*>;
    public:
        UniversalEmitter() = default;

        /** \brief Initializes the emitter. */
        void init(ParticleFunctorMaster* master);

        /** \brief Sets a new distribution for the emitter position. */
        template<typename DATA, typename ... PARAM>
        inline void setPositionDist(const std::string& key, PARAM&& ... param);

        /** \brief Returns the attached position distribution. */
        template<typename DATA>
        DATA& getPositionDist();

        /** \brief Returns the attached position distribution. */
        template<typename DATA>
        const DATA& getPositionDist() const;

        /** \brief Sets a new distribution for the initial velocity. */
        template<typename DATA, typename ... PARAM>
        inline void setVelocityDist(const std::string& key, PARAM&& ... param);

        /** \brief Returns the attached velocity distribution. */
        template<typename DATA>
        DATA& getVelocityDist();

        /** \brief Returns the attached velocity distribution. */
        template<typename DATA>
        const DATA& getVelocityDist() const;

        /** \brief Sets a new distribution for the spawn rate. */
        template<typename DATA, typename ... PARAM>
        inline void setSpawnInterval(const std::string& key, PARAM&& ... param);

        /** \brief Returns the attached spawn interval. */
        template<typename DATA>
        DATA& getSpawnInterval();

        /** \brief Returns the attached spawn interval. */
        template<typename DATA>
        const DATA& getSpawnInterval() const;

        /** \brief Sets a new distribution for the lifetime of particles. */
        template<typename DATA, typename ... PARAM>
        inline void setLifetimeDist(const std::string& key, PARAM&& ... param);

        /** \brief Returns the attached lifetime distribution. */
        template<typename DATA>
        DATA& getLifetimeDist();

        /** \brief Returns the attached lifetime distribution. */
        template<typename DATA>
        const DATA& getLifetimeDist() const;

        /** \brief Returns the key of the attached position distribution. */
        std::string getPositionDistKey() const;

        /** \brief Returns the key of the attached velocity distribution. */
        std::string getVelocityDistKey() const;

        /** \brief Returns the key of the attached spawn interval. */
        std::string getSpawnIntervalKey() const;

        /** \brief Returns the key of the attached lifetime distribution. */
        std::string getLifetimeDistKey() const;

         const FunctorHandle& getPositionDist() const { return mPosInit; }

         const FunctorHandle& getVelocityDist() const { return mVelInit; }

         const FunctorHandle& getSpawnInterval() const { return mSpawnInterval; }

         const FunctorHandle& getLifetimeDist() const { return mLifetimeInit; }

         ParticleFunctorMaster* getMaster() { return mMaster; }

    private:
        FunctorHandle mPosInit;
        FunctorHandle mVelInit;
        FunctorHandle mSpawnInterval;
        FunctorHandle mLifetimeInit;
        ParticleFunctorMaster* mMaster;

    private:
        FUNCTOR_DATA(UniversalEmitter)

    public:
        virtual void serialize(ungod::MetaNode serializer, ungod::SerializationContext& context, ParticleFunctorMaster*&& master) const override
        {
            deferredSerialize<UniversalEmitter, ParticleFunctorMaster*>(*this, serializer, context, std::move(master));
        }
        virtual std::string getSerialIdentifier() const override
        {
            return deferredGetIdentifier<UniversalEmitter>();
        }
    };



    template<typename DATA, typename ... PARAM>
    inline void UniversalEmitter::setPositionDist(const std::string& key, PARAM&& ... param)
    {
        mPosInit = mMaster->getPositionDistributions().makeHandle<DATA>(key, std::forward<PARAM>(param)... );
    }

    template<typename DATA, typename ... PARAM>
    inline void UniversalEmitter::setVelocityDist(const std::string& key, PARAM&& ... param)
    {
        mVelInit = mMaster->getVelocityDistributions().makeHandle<DATA>(key, std::forward<PARAM>(param)... );
    }

    template<typename DATA, typename ... PARAM>
    inline void UniversalEmitter::setSpawnInterval(const std::string& key, PARAM&& ... param)
    {
        mSpawnInterval = mMaster->getIntervalDistributions().makeHandle<DATA>(key, std::forward<PARAM>(param)... );
    }

    template<typename DATA, typename ... PARAM>
    inline void UniversalEmitter::setLifetimeDist(const std::string& key, PARAM&& ... param)
    {
        mLifetimeInit = mMaster->getLifetimeDistributions().makeHandle<DATA>(key, std::forward<PARAM>(param)... );
    }

    template<typename DATA>
    DATA& UniversalEmitter::getPositionDist()
    {
        return *mPosInit.data->as<DATA>();
    }


    template<typename DATA>
    const DATA& UniversalEmitter::getPositionDist() const
    {
        return *mPosInit.data->as<DATA>();
    }


    template<typename DATA>
    DATA& UniversalEmitter::getVelocityDist()
    {
        return *mVelInit.data->as<DATA>();
    }


    template<typename DATA>
    const DATA& UniversalEmitter::getVelocityDist() const
    {
        return *mVelInit.data->as<DATA>();
    }


    template<typename DATA>
    DATA& UniversalEmitter::getSpawnInterval()
    {
        return *mSpawnInterval.data->as<DATA>();
    }


    template<typename DATA>
    const DATA& UniversalEmitter::getSpawnInterval() const
    {
        return *mSpawnInterval.data->as<DATA>();
    }


    template<typename DATA>
    DATA& UniversalEmitter::getLifetimeDist()
    {
        return *mLifetimeInit.data->as<DATA>();
    }


    template<typename DATA>
    const DATA& UniversalEmitter::getLifetimeDist() const
    {
        return *mLifetimeInit.data->as<DATA>();
    }


    void universalEmitter(UniversalEmitter& data, ParticleSystem& particles, float delta);


    /** \brief Data for the DirectionalForce affector. */
    struct UniversalEstimator : public BaseFunctorData<>
    {
        UniversalEstimator() = default;

        void init() {}

    private:
        FUNCTOR_DATA(UniversalEstimator)

    public:
        SERIALIZABLE_DERIVED
    };


    std::size_t universalCountEstimate(UniversalEstimator&, const FunctorHandle& emitter);
}

#endif

