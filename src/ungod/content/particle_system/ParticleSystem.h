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


#ifndef UNGOD_PARTICLE_SYSTEM_H
#define UNGOD_PARTICLE_SYSTEM_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include "owls/Signal.h"
#include "ungod/base/Utility.h"
#include "ungod/base/Entity.h"
#include "ungod/physics/MobilityUnit.h"
#include "ungod/visual/Animation.h"
#include "ungod/visual/Visual.h"
#include "ungod/content/particle_system/ParticleFunctorMaster.h"
#include "ungod/content/particle_system/Affector.h"
#include "ungod/content/particle_system/Emitter.h"
#include "ungod/content/particle_system/Distributions.h"
#include "ungod/serialization/SerialParticleSystem.h"

namespace ungod
{
    class ParticleSystem;

    namespace detail
    {
        /** \brief Data for a single particle. */
        struct ParticleData
        {
            MobilityUnit mov;
            float lifetime;
            float maxlifetime;
            sf::Vector2f position;
            sf::Vector2f scale;
            float rotation;
            sf::Color color;
            std::size_t animIndex;
        };
    }

    /** \brief A particle-system that is desgined to emit and affect 2d quads with a texture
    * and some sort of affection over time (animation, tranform) in a highly efficient way.
    *
    * Particles are emitted by an emitter function objects, that initializes spawn position,
    * velocity and acceeleration of particles. A single particle system has exactly one emitter.
    *
    * They are affected by an affector function object, that modifies the particles over time.
    * (e.g. move them in a certain direction, make them follow a path, animate them and so on)
    * A single particle system can have multiple affectors. */
    class ParticleSystem : public Serializable<ParticleSystem>
    {
    friend class SerialBehavior<ParticleSystem>;
    friend class DeserialBehavior<ParticleSystem>;
    public:
        static constexpr float MAX_FORCE_DEFAULT = 1.0f;
        static constexpr float MAX_VELOCITY_DEFAULT = 2.0f;
        static constexpr float PARTICLE_SPEED_DEFAULT = 1.0f;
    public:
        ParticleSystem(ParticleFunctorMaster& funcMaster);

        ParticleSystem(const ParticleSystem& other);

        ParticleSystem& operator=(ParticleSystem other);

        /** \brief Updates the particles system, let affectors affect the particles.
        * Destroys particles with exceeded lifetime and respaws new particles. */
        void update(float delta);

        /** \brief Renders all currently emitted particles. */
        void render(sf::Texture const* tex, sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Spawns a new particle. */
        void spawnParticle(const sf::Vector2f& position,
                           const sf::Vector2f& velocity,
                           float lifetime);

        /** \brief Sets the emitter. Takes ownership. */
        template<typename EMITTER_DATA, typename ESTIMATOR_DATA, typename ... PARAM>
        inline void setEmitter(const std::string& emitterkey, const std::string& estimatorkey, PARAM&& ... param);

        /** \brief Returns the attached emitter data. */
        template<typename EMITTER_DATA>
        EMITTER_DATA& getEmitter();

        /** \brief Returns the attached emitter data. */
        template<typename EMITTER_DATA>
        const EMITTER_DATA& getEmitter() const;

        /** \brief Adds a new affector. Takes ownership. */
        template<typename DATA, typename ... PARAM>
        inline void addAffector(const std::string& key, PARAM&& ... param);

        /** \brief Returns the attached affector with index i. */
        template<typename AFFECTOR_DATA>
        AFFECTOR_DATA& getAffector(std::size_t i);

        /** \brief Returns the attached affector with index i. */
        template<typename AFFECTOR_DATA>
        const AFFECTOR_DATA& getAffector(std::size_t i) const;

        /** \brief Sets up a new texrect-initializer. Takes ownership. */
        template<typename DATA, typename ... PARAM>
        inline void setTexrectInitializer(const std::string& key, PARAM&& ... param);

        /** \brief Returns the attached texrect-initializer data. */
        template<typename DATA>
        DATA& getTexrectInitializer();

        /** \brief Returns the attached texrect-initializer data. */
        template<typename DATA>
        const DATA& getTexrectInitializer() const;

        /** \brief Clears all internal affectors. */
        void clearAffectors();

        /** \brief Accesses the underlying particle data. */
        detail::ParticleData& getParticleData(std::size_t i) { return mParticleData[i]; }

        /** \brief Sets a value for the maximum force, that can affect particles. */
        void setMaxForce(float maxforce) { mMaxForce = maxforce; }

        /** \brief Sets a value for the maximum velocity, that particles can have. */
        void setMaxVelocity(float maxvel) { mMaxVelocity = maxvel; }

        /** \brief Sets a value for the speed of the particles. */
        void setParticleSpeed(float speed) { mSpeed = speed; }

        /** \brief Sets number of states a particle is in. */
        void setStateNum(std::size_t statenum) { mStateNum = statenum; }

        /** \brief Returns the value for the maximum force, that can affect particles. */
        float getMaxForce() const { return mMaxForce; }

        /** \brief Returns the value for the maximum velocity, that particles can have. */
        float getMaxVelocity() const { return mMaxVelocity; }

        /** \brief Returns the value for the speed of the particles. */
        float getParticleSpeed() const { return mSpeed; }

        /** \brief Returns the number of states a particle is in. */
        float getStateNum() const { return mStateNum; }

        /** \brief Returns the current number of emitted particles. */
        std::size_t getParticleCount() const { return mParticleCount; }

        /** \brief Accesses the vertices of particle i. */
        sf::Vertex* getVertices(std::size_t i);

        /** \brief Returns the bounds of the particle system around all particles. */
        sf::FloatRect getBounds() const;

        /** \brief Returns the key of the attached emitter. */
        std::string getEmitterKey() const;

        /** \brief Returns the key of the attached particle affector with index i. */
        std::string getAffectorKey(std::size_t i) const;

        /** \brief Returns the key of the attached texrect initializer. */
        std::string getTexrectInitializerKey() const;

        /** \brief Returns the number of assigned affectors. */
        std::size_t getAffectorCount() const;

        ~ParticleSystem() = default;

    private:
        /** \brief Resets the particle with given index. */
        void resetParticle(std::size_t i);

        /** \brief Estimates the number of particles in the next second and fits datastructre sizes. */
        virtual void estimateAndFit();

    private:
        ParticleFunctorMaster& mFuncMaster;
        FunctorHandle mEmitter;
        FunctorHandle mCountEstimator;
        std::vector< FunctorHandle > mAffectors;
        FunctorHandle mTexrectInit;
        std::vector<sf::Vertex> mVertices;
        std::vector<detail::ParticleData> mParticleData;
        std::size_t mParticleCount;
        float mEstimateTimer;
        float mMaxForce;
        float mMaxVelocity;
        float mSpeed;
        std::size_t mAnims;
        std::size_t mStateNum;
    };



    template<typename EMITTER_DATA, typename ESTIMATOR_DATA, typename ... PARAM>
    inline void ParticleSystem::setEmitter(const std::string& emitterkey, const std::string& estimatorkey, PARAM&& ... param)
    {
        mEmitter = mFuncMaster.getEmitters().makeHandle<EMITTER_DATA>(emitterkey, &mFuncMaster, std::forward<PARAM>(param)...);
        mCountEstimator = mFuncMaster.getCountEstimators().makeHandle<ESTIMATOR_DATA>(estimatorkey);
        estimateAndFit();
    }

    template<typename EMITTER_DATA>
    EMITTER_DATA& ParticleSystem::getEmitter()
    {
        return *mEmitter.data->as<EMITTER_DATA>();
    }

    template<typename EMITTER_DATA>
    const EMITTER_DATA& ParticleSystem::getEmitter() const
    {
        return *mEmitter.data->as<EMITTER_DATA>();
    }


    template<typename DATA, typename ... PARAM>
    inline void ParticleSystem::addAffector(const std::string& key, PARAM&& ... param)
    {
        mAffectors.emplace_back( mFuncMaster.getAffectors().makeHandle<DATA>(key, std::forward<PARAM>(param)...) );
    }

    template<>
    inline void ParticleSystem::addAffector<AnimatedParticles, const std::string&, const std::string&, std::size_t&>(const std::string& key, const std::string& metaID, const std::string& k, std::size_t& numAnim)
    {
        mAffectors.emplace_back( mFuncMaster.getAffectors().makeHandle<AnimatedParticles>(key, metaID, k, numAnim ));
        mAnims = numAnim;
    }

    template<>
    inline void ParticleSystem::addAffector<AnimatedParticles, const std::string&, const std::string&, std::size_t>(const std::string& key, const std::string& metaID, const std::string& k, std::size_t&& numAnim)
    {
        mAffectors.emplace_back( mFuncMaster.getAffectors().makeHandle<AnimatedParticles>(key, metaID, k, numAnim ));
        mAnims = numAnim;
    }

    template<typename AFFECTOR_DATA>
    AFFECTOR_DATA& ParticleSystem::getAffector(std::size_t i)
    {
        return *mAffectors[i].data->as<AFFECTOR_DATA>();
    }

    template<typename AFFECTOR_DATA>
    const AFFECTOR_DATA& ParticleSystem::getAffector(std::size_t i) const
    {
        return *mAffectors[i].data->as<AFFECTOR_DATA>();
    }



    template<typename DATA, typename ... PARAM>
    inline void ParticleSystem::setTexrectInitializer(const std::string& key, PARAM&& ... param)
    {
        mTexrectInit = mFuncMaster.getTexrectInitializers().makeHandle<DATA>(key, std::forward<PARAM>(param)...);
    }

    template<typename DATA>
    DATA& ParticleSystem::getTexrectInitializer()
    {
        return *mTexrectInit.data->as<DATA>();
    }

    template<typename DATA>
    const DATA& ParticleSystem::getTexrectInitializer() const
    {
        return *mTexrectInit.data->as<DATA>();
    }
}

#endif

