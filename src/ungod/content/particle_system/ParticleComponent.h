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


#ifndef UNGOD_PARTICLE_COMPONENT_H
#define UNGOD_PARTICLE_COMPONENT_H

#include <optional>
#include "ungod/content/particle_system/ParticleSystem.h"
#include "ungod/serialization/SerialParticleSystem.h"
#include "ungod/utility/ScopedAccessor.h"

namespace ungod
{
    /**
    * \ingroup Components
    * \brief A component that wraps a particle system in order to attach it to an entity.
    */
    class ParticleSystemComponent : public Serializable<ParticleSystemComponent>
    {
        friend class ParticleSystemHandler;
        friend class Renderer;
         friend struct SerialBehavior<ParticleSystemComponent, Entity>;
        friend struct DeserialBehavior<ParticleSystemComponent, Entity, DeserialMemory&>;

    public:
        ParticleSystemComponent() = default;

        const ParticleSystem& getSystem() const { return mParticleSystem.value(); }

    private:
        std::optional<ParticleSystem> mParticleSystem;
    };

    using PSData = detail::FunctorDataDeepBase;

    /** \brief A manager class that updates and renders entities with particle systems. */
    class ParticleSystemHandler
    {
    public:
        ParticleSystemHandler() : mRectUpdateTimer(200) {}

        void update(const std::list<Entity>& entities, float delta);

        /** \brief Explicitly sets the particle system of an entity. (e.g. by a copy from another entity) */
        void setSystem(Entity e, const ParticleSystem& p) const;

        /** \brief Sets an emitter for the particle system of the given entity. */
        template<typename EMITTER, typename ESTIMATOR, typename ... PARAM>
        inline void setEmitter(Entity e, const std::string& emitterkey, const std::string& estimatorkey, PARAM&& ... param);

        /** \brief Returns a scoped accessor to the attached emitter data. */
        template<typename EMITTER>
        detail::ScopedInitializer<EMITTER> getEmitter(Entity e);

        /** \brief Returns the attached emitter data by const reference. */
        template<typename EMITTER>
        const EMITTER& getEmitter(Entity e) const;

        /** \brief Sets a new distribution for the emitter position. This is only valid if the attached emitter is of UniversalEmitter type (which is the default) */
        template<typename DATA, typename ... PARAM>
        inline void setPositionDist(Entity e, const std::string& key, PARAM&& ... param);

        /** \brief Returns a scoped accessor to the attached position distribution. */
        template<typename DATA>
        detail::ScopedInitializer<DATA> getPositionDist(Entity e);

        /** \brief Returns the attached position distribution by const reference. */
        template<typename DATA>
        const DATA& getPositionDist(Entity e) const;

        /** \brief Sets a new distribution for the initial velocity. This is only valid if the attached emitter is of UniversalEmitter type (which is the default) */
        template<typename DATA, typename ... PARAM>
        inline void setVelocityDist(Entity e, const std::string& key, PARAM&& ... param);

        /** \brief Returns a scoped accessor to the attached velocity distribution. */
        template<typename DATA>
        detail::ScopedInitializer<DATA> getVelocityDist(Entity e);

        /** \brief Returns the attached velocity distribution by const reference. */
        template<typename DATA>
        const DATA& getVelocityDist(Entity e) const;

        /** \brief Sets a new distribution for the spawn rate. This is only valid if the attached emitter is of UniversalEmitter type (which is the default) */
        template<typename DATA, typename ... PARAM>
        inline void setSpawnInterval(Entity e, const std::string& key, PARAM&& ... param);

        /** \brief Returns a scoped accessor to the attached spawn interval. */
        template<typename DATA>
        detail::ScopedInitializer<DATA> getSpawnInterval(Entity e);

        /** \brief Returns the attached spawn interval by const reference. */
        template<typename DATA>
        const DATA& getSpawnInterval(Entity e) const;

        /** \brief Sets a new distribution for the lifetime of particles. This is only valid if the attached emitter is of UniversalEmitter type (which is the default) */
        template<typename DATA, typename ... PARAM>
        inline void setLifetimeDist(Entity e, const std::string& key, PARAM&& ... param);

        /** \brief Returns a scoped accessor to the attached lifetime distribution. */
        template<typename DATA>
        detail::ScopedInitializer<DATA> getLifetimeDist(Entity e);

        /** \brief Returns the attached lifetime distribution by const reference. */
        template<typename DATA>
        const DATA& getLifetimeDist(Entity e) const;

        /** \brief Adds an affector for the particle system of the given entity. */
        template<typename DATA, typename ... PARAM>
        inline void addAffector(Entity e, const std::string& key, PARAM&& ... param);

        /** \brief Returns a scoped accessor to the ith particle affector. */
        template<typename DATA>
        detail::ScopedInitializer<DATA> getAffector(Entity e, std::size_t i);

        /** \brief Returns the ith particle affector by const reference. */
        template<typename DATA>
        const DATA& getAffector(Entity e, std::size_t i) const;

        /** \brief Adds a texrect initializer for the particle system of the given entity. */
        template<typename DATA, typename ... PARAM>
        inline void setTexrectInitializer(Entity e, const std::string& key, PARAM&& ... param);

        /** \brief Returns a scoped accessor to the attached texrect initializer. */
        template<typename DATA>
        detail::ScopedInitializer<DATA> getTexrectInitializer(Entity e);

        /** \brief Returns the attached texrect initializer by const reference. */
        template<typename DATA>
        const DATA& getTexrectInitializer(Entity e) const;

        /** \brief Removes all affectors from the particle system of the given entity. */
        inline void clearAffectors(Entity e);

        /** \brief Sets a value for the maximum force, that can affect particles. */
        inline void setMaxForce(Entity e, float maxforce);

        /** \brief Sets a value for the maximum velocity, that particles can have. */
        inline void setMaxVelocity(Entity e, float maxvel);

        /** \brief Sets a value for the speed of the particles. */
        inline void setParticleSpeed(Entity e, float speed);

        /** \brief Registers new callback for the ContentsChanged signal. */
        void onContentsChanged(const std::function<void(Entity, const sf::FloatRect&)>& callback);

        /** \brief Registers new callback for the emitter changed signal. This signal if emitted if the emitter is changed or
        * one of the distributions of the emitter is altered in any way. The signal provides the key of the altered content, along with
        * the appropriate data. */
        void onEmitterChanged(const std::function<void(Entity, const std::string&, const PSData&)>& callback);

        /** \brief Registers new callback for the texrect init changed signal. The signal provides the key of the altered texrect initializer, along with
        * the appropriate data. */
        void onTexrectInitChanged(const std::function<void(Entity, const std::string&, const PSData&)>& callback);

        /** \brief Registers new callback for the affector changed signal. The signal provides the key of the altered affector, along with
        * the appropriate data. */
        void onAffectorsChanged(const std::function<void(Entity, const std::string&, const PSData&)>& callback);

        const ParticleFunctorMaster& getFunctorMaster() { return sParticleFunctorMaster; }

        /** \brief Returns the key of the attached emitter. */
        std::string getEmitterKey(Entity e) const;

        /** \brief Returns the key of the attached position distribution. */
        std::string getPositionDistKey(Entity e) const;

        /** \brief Returns the key of the attached velocity distribution. */
        std::string getVelocityDistKey(Entity e) const;

        /** \brief Returns the key of the attached spawn interval. */
        std::string getSpawnIntervalKey(Entity e) const;

        /** \brief Returns the key of the attached lifetime distribution. */
        std::string getLifetimeDistKey(Entity e) const;

        /** \brief Returns the key of the attached particle affector with index i. */
        std::string getAffectorKey(Entity e, std::size_t i) const;

        /** \brief Returns the key of the attached texrect initializer. */
        std::string getTexrectInitializerKey(Entity e) const;


        /** \brief Defines a update rate in milliseconds for the update of the bounds of all particle systems. */
        void setRectUpdateTimer(int rectupd);

        sf::Vector2f getLowerBound(Entity e) const;
        sf::Vector2f getUpperBound(Entity e) const;

    public:
        void handleParticleSystemAdded(Entity e);

    private:
        static const ParticleFunctorMaster sParticleFunctorMaster;
        sf::Clock mAABBUpdate;
        int mRectUpdateTimer;
        owls::Signal< Entity, const sf::FloatRect& > mContentsChangedSignal;
        owls::Signal< Entity, const std::string&, const PSData& > mEmitterChangedSignal;
        owls::Signal< Entity, const std::string&, const PSData& > mTexRectInitChangedSignal;
        owls::Signal< Entity, const std::string&, const PSData& > mAffectorChangedSignal;
    };



    template<typename EMITTER, typename ESTIMATOR, typename ... PARAM>
    inline void ParticleSystemHandler::setEmitter(Entity e, const std::string& emitterkey, const std::string& estimatorkey, PARAM&& ... param)
    {
        ParticleSystemComponent& psc = e.modify<ParticleSystemComponent>();
        psc.mParticleSystem->setEmitter<EMITTER, ESTIMATOR, PARAM...>(emitterkey, estimatorkey, std::forward<PARAM>(param)...);
        mEmitterChangedSignal(e, emitterkey, psc.mParticleSystem->getEmitter< PSData >());
    }


    template<typename EMITTER>
    detail::ScopedInitializer<EMITTER> ParticleSystemHandler::getEmitter(Entity e)
    {
        ParticleSystemComponent& psc = e.modify<ParticleSystemComponent>();
        return detail::ScopedInitializer<EMITTER>{ psc.mParticleSystem->getEmitter<EMITTER>(),
                                        [this, e] () { mEmitterChangedSignal(e,
                                                                          psc.mParticleSystem->getEmitterKey(),
                                                                          psc.mParticleSystem->getEmitter< PSData >()); } };
    }


    template<typename EMITTER>
    const EMITTER& ParticleSystemHandler::getEmitter(Entity e) const
    {
        return e.modify<ParticleSystemComponent>().mParticleSystem->getEmitter<EMITTER>();
    }


    template<typename DATA, typename ... PARAM>
    inline void ParticleSystemHandler::setPositionDist(Entity e, const std::string& key, PARAM&& ... param)
    {
        ParticleSystemComponent& psc = e.modify<ParticleSystemComponent>();
        psc.mParticleSystem->getEmitter<UniversalEmitter>().setPositionDist<DATA>(key, std::forward<PARAM>(param)...);
        mEmitterChangedSignal(e, key, psc.mParticleSystem->getEmitter<UniversalEmitter>().getPositionDist< PSData >());
    }


    template<typename DATA>
    detail::ScopedInitializer<DATA> ParticleSystemHandler::getPositionDist(Entity e)
    {
        return detail::ScopedInitializer<DATA>{ e.modify<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getPositionDist<DATA>(),
                                        [this, e] () { mEmitterChangedSignal(e,
                                                                          e.modify<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getPositionDistKey(),
                                                                          e.modify<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getPositionDist< PSData >()); } };
    }


    template<typename DATA>
    const DATA& ParticleSystemHandler::getPositionDist(Entity e) const
    {
        return e.modify<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getPositionDist<DATA>();
    }


    template<typename DATA, typename ... PARAM>
    inline void ParticleSystemHandler::setVelocityDist(Entity e, const std::string& key, PARAM&& ... param)
    {
        ParticleSystemComponent& psc = e.modify<ParticleSystemComponent>();
        psc.mParticleSystem->getEmitter<UniversalEmitter>().setVelocityDist<DATA>(key, std::forward<PARAM>(param)...);
        mEmitterChangedSignal(e, key, psc.mParticleSystem->getEmitter<UniversalEmitter>().getVelocityDist< PSData >());
    }


    template<typename DATA>
    detail::ScopedInitializer<DATA> ParticleSystemHandler::getVelocityDist(Entity e)
    {
        return detail::ScopedInitializer<DATA>{ e.modify<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getVelocityDist<DATA>(),
                                        [this, e] () { mEmitterChangedSignal(e,
                                                                          e.modify<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getVelocityDistKey(),
                                                                          e.modify<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getVelocityDist< PSData >()); } };
    }


    template<typename DATA>
    const DATA& ParticleSystemHandler::getVelocityDist(Entity e) const
    {
        return e.modify<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getVelocityDist<DATA>();
    }


    template<typename DATA, typename ... PARAM>
    inline void ParticleSystemHandler::setSpawnInterval(Entity e, const std::string& key, PARAM&& ... param)
    {
        ParticleSystemComponent& psc = e.modify<ParticleSystemComponent>();
        psc.mParticleSystem->getEmitter<UniversalEmitter>().setSpawnInterval<DATA>(key, std::forward<PARAM>(param)...);
        mEmitterChangedSignal(e, key, psc.mParticleSystem->getEmitter<UniversalEmitter>().getSpawnInterval< PSData >());
    }


    template<typename DATA>
    detail::ScopedInitializer<DATA> ParticleSystemHandler::getSpawnInterval(Entity e)
    {
        return detail::ScopedInitializer<DATA>{ e.modify<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getSpawnInterval<DATA>(),
                                        [this, e] () { mEmitterChangedSignal(e,
                                                                          e.modify<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getSpawnIntervalKey(),
                                                                          e.modify<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getSpawnInterval< PSData >()); } };
    }


    template<typename DATA>
    const DATA& ParticleSystemHandler::getSpawnInterval(Entity e) const
    {
        return e.modify<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getSpawnInterval<DATA>();
    }


    template<typename DATA, typename ... PARAM>
    inline void ParticleSystemHandler::setLifetimeDist(Entity e, const std::string& key, PARAM&& ... param)
    {
        ParticleSystemComponent& psc = e.modify<ParticleSystemComponent>();
        psc.mParticleSystem->getEmitter<UniversalEmitter>().setLifetimeDist<DATA>(key, std::forward<PARAM>(param)...);
        mEmitterChangedSignal(e, key, psc.mParticleSystem->getEmitter<UniversalEmitter>().getLifetimeDist< PSData >());
    }


    template<typename DATA>
    detail::ScopedInitializer<DATA> ParticleSystemHandler::getLifetimeDist(Entity e)
    {
        return detail::ScopedInitializer<DATA>{ e.modify<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getLifetimeDist<DATA>(),
                                        [this, e] () { mEmitterChangedSignal(e,
                                                                          e.modify<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getLifetimeDistKey(),
                                                                          e.modify<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getLifetimeDist< PSData >()); } };
    }


    template<typename DATA>
    const DATA& ParticleSystemHandler::getLifetimeDist(Entity e) const
    {
        return e.modify<ParticleSystemComponent>().mParticleSystem->getEmitter<UniversalEmitter>().getLifetimeDist<DATA>();
    }


    template<typename DATA, typename ... PARAM>
    inline void ParticleSystemHandler::addAffector(Entity e, const std::string& key, PARAM&& ... param)
    {
        ParticleSystemComponent& psc = e.modify<ParticleSystemComponent>();
        psc.mParticleSystem->addAffector<DATA, PARAM...>(key, std::forward<PARAM>(param)...);
        mAffectorChangedSignal(e, key, psc.mParticleSystem->getAffector< PSData >( psc.mParticleSystem->getAffectorCount()-1 ));
    }


    template<typename DATA>
    detail::ScopedInitializer<DATA> ParticleSystemHandler::getAffector(Entity e, std::size_t i)
    {
        return detail::ScopedInitializer<DATA>{ e.modify<ParticleSystemComponent>().mParticleSystem->getAffector<DATA>(i),
                                        [this, e, i] () { mAffectorChangedSignal(e,
                                                                          e.modify<ParticleSystemComponent>().mParticleSystem->getAffectorKey(i),
                                                                          e.modify<ParticleSystemComponent>().mParticleSystem->getAffector< PSData >(i)); } };
    }


    template<typename DATA>
    const DATA& ParticleSystemHandler::getAffector(Entity e, std::size_t i) const
    {
        return e.modify<ParticleSystemComponent>().mParticleSystem->getAffector<DATA>(i);
    }


    template<typename DATA, typename ... PARAM>
    inline void ParticleSystemHandler::setTexrectInitializer(Entity e, const std::string& key, PARAM&& ... param)
    {
        ParticleSystemComponent& psc = e.modify<ParticleSystemComponent>();
        psc.mParticleSystem->setTexrectInitializer<DATA, PARAM...>(key, std::forward<PARAM>(param)...);
        mTexRectInitChangedSignal(e, key, psc.mParticleSystem->getTexrectInitializer< PSData >());
    }


    template<typename DATA>
    detail::ScopedInitializer<DATA> ParticleSystemHandler::getTexrectInitializer(Entity e)
    {
        return detail::ScopedInitializer<DATA>{ e.modify<ParticleSystemComponent>().mParticleSystem->getTexrectInitializer<DATA>(),
                                        [this, e] () { mTexRectInitChangedSignal(e,
                                                                          e.modify<ParticleSystemComponent>().mParticleSystem->getTexrectInitializerKey(),
                                                                          e.modify<ParticleSystemComponent>().mParticleSystem->getTexrectInitializer< PSData >()); } };
    }


    template<typename DATA>
    const DATA& ParticleSystemHandler::getTexrectInitializer(Entity e) const
    {
        return e.modify<ParticleSystemComponent>().mParticleSystem->getTexrectInitializer<DATA>();
    }

    inline void ParticleSystemHandler::clearAffectors(Entity e)
    {
        e.modify<ParticleSystemComponent>().mParticleSystem->clearAffectors();
    }

    inline void ParticleSystemHandler::setMaxForce(Entity e, float maxforce)
    {
        e.modify<ParticleSystemComponent>().mParticleSystem->setMaxForce(maxforce);
    }

    inline void ParticleSystemHandler::setMaxVelocity(Entity e, float maxvel)
    {
        e.modify<ParticleSystemComponent>().mParticleSystem->setMaxVelocity(maxvel);
    }

    inline void ParticleSystemHandler::setParticleSpeed(Entity e, float speed)
    {
        e.modify<ParticleSystemComponent>().mParticleSystem->setParticleSpeed(speed);
    }
}

#endif // UNOGD_PRTICLE_SYSTEM_H

