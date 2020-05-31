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

#include "ungod/content/particle_system/ParticleSystem.h"

namespace ungod
{
        ParticleSystem::ParticleSystem(const ParticleFunctorMaster& funcMaster) :
            mFuncMaster(funcMaster),
            mParticleCount(0),
            mEstimateTimer(0.0f),
            mMaxForce(MAX_FORCE_DEFAULT),
            mMaxVelocity(MAX_VELOCITY_DEFAULT),
            mSpeed(PARTICLE_SPEED_DEFAULT),
            mAnims(0u),
            mStateNum(1u)
        {
            mEmitter = mFuncMaster.getEmitters().makeHandle<UniversalEmitter>(PS_UNIVERSAL_EMITTER, &mFuncMaster);
            mCountEstimator = mFuncMaster.getCountEstimators().makeHandle<UniversalEstimator>(PS_UNIVERSAL_ESTIMATE);
        }

        ParticleSystem::ParticleSystem(const ParticleSystem& other) :
            mFuncMaster(other.mFuncMaster),
            mEmitter(other.mEmitter),
            mCountEstimator(other.mCountEstimator),
            mAffectors(other.mAffectors),
            mTexrectInit(other.mTexrectInit),
            mParticleCount(0),
            mEstimateTimer(0.0f),
            mMaxForce(other.mMaxForce),
            mMaxVelocity(other.mMaxVelocity),
            mSpeed(other.mSpeed),
            mAnims(other.mAnims),
            mStateNum(other.mStateNum)
        {
        }

        ParticleSystem& ParticleSystem::operator=(ParticleSystem other)
        {
            std::swap(*this, other);
            return *this;
        }


        void ParticleSystem::update(float delta)
        {
            //may spawn new particles
            mFuncMaster.getEmitters().invoke(mEmitter, *this, delta);

            //let the affectors affect the particles
            for (const auto& a : mAffectors)
            {
                mFuncMaster.getAffectors().invoke(a, *this, delta);
            }


            //determine particles that have exceeded their lifetime
            //move living particles
            for (std::size_t i = 0; i < mParticleCount; ++i)
            {
                mParticleData[i].lifetime -= delta;
                if (mParticleData[i].lifetime <= 0.0f)
                {
                    resetParticle(i);
                }
                else
                {
                    mobilize(mParticleData[i].mov, mMaxForce, mMaxVelocity);
                    resetAcceleration(mParticleData[i].mov);
                    mParticleData[i].position += delta * mSpeed * mParticleData[i].mov.velocity;

                    sf::Transform transf;
                    transf.translate( mParticleData[i].position );
                    transf.rotate( mParticleData[i].rotation );
                    transf.scale( mParticleData[i].scale );
                    for (std::size_t k = 0; k < 4; k++)
                    {
                        mVertices[4*i + k].position = transf.transformPoint(mVertices[4*i + k].texCoords - mVertices[4*i].texCoords);
                        mVertices[4*i + k].color = mParticleData[i].color;
                    }
                }
            }

            //may resize the vectors, according to the estimated particle count for the next second
            mEstimateTimer += delta;
            if (mEstimateTimer >= 1000.0f)
            {
                mEstimateTimer -= 1000.0f;
                estimateAndFit();
            }
        }


        void ParticleSystem::render(sf::Texture const* tex, sf::RenderTarget& target, sf::RenderStates states) const
        {
            states.texture = tex;
            target.draw(mVertices.data(), mParticleCount*4, sf::Quads, states);
        }


        void ParticleSystem::spawnParticle(const sf::Vector2f& position,
                                           const sf::Vector2f& velocity,
                                           float lifetime)
        {
            mParticleCount++;

            if (mParticleData.size() < mParticleCount)
            {
                mParticleData.resize(mParticleCount);
                mVertices.resize(mParticleCount*4);
            }

            std::size_t i = mParticleCount-1;

            mParticleData[i].lifetime = lifetime;
            mParticleData[i].maxlifetime = lifetime;
            mParticleData[i].mov.velocity = velocity;
            mParticleData[i].position = position;
            mParticleData[i].scale = {1.0f, 1.0f};
            mParticleData[i].rotation = 0.0f;
            mParticleData[i].color = sf::Color::White;
            if (mAnims > 0)
                mParticleData[i].animIndex = (std::size_t)NumberGenerator::getRandBetw(0u, (int)mAnims-1);

            sf::IntRect texturerect;
            if (mTexrectInit)
                texturerect = mFuncMaster.getTexrectInitializers().invoke(mTexrectInit);

            mVertices[i*4].texCoords = sf::Vector2f{(float)texturerect.left,(float)texturerect.top};
            mVertices[i*4+1].texCoords = sf::Vector2f{(float)texturerect.left+texturerect.width,(float)texturerect.top};
            mVertices[i*4+2].texCoords = sf::Vector2f{(float)texturerect.left+texturerect.width, (float)texturerect.top+texturerect.height};
            mVertices[i*4+3].texCoords = sf::Vector2f{(float)texturerect.left, (float)texturerect.top+texturerect.height};
        }


        void ParticleSystem::clearAffectors()
        {
            mAffectors.clear();
            mTexrectInit.reset();
            mAnims = 0u;
        }


        void ParticleSystem::resetParticle(std::size_t i)
        {
            mParticleCount--;
            std::swap( mParticleData[i], mParticleData[mParticleCount] );
            std::swap( mVertices[i*4], mVertices[mParticleCount*4] );
            std::swap( mVertices[i*4 +1], mVertices[mParticleCount*4 +1] );
            std::swap( mVertices[i*4 +2], mVertices[mParticleCount*4 +2] );
            std::swap( mVertices[i*4 +3], mVertices[mParticleCount*4 +3] );
        }


        void ParticleSystem::estimateAndFit()
        {
            std::size_t estimate = std::max(mParticleCount, mFuncMaster.getCountEstimators().invoke(mCountEstimator, mEmitter));
            mVertices.resize(estimate*4);
            mParticleData.resize(estimate);
        }


        sf::Vertex* ParticleSystem::getVertices(std::size_t i)
        {
            return &mVertices[4*i];
        }


        sf::FloatRect ParticleSystem::getBounds() const
        {
            if (mParticleCount == 0)
                return sf::FloatRect{};

            sf::Vector2f lower, upper;
            upper.x = std::numeric_limits<float>::max();
            upper.y = std::numeric_limits<float>::max();
            lower.x = std::numeric_limits<float>::min();
            lower.y = std::numeric_limits<float>::min();
            for (std::size_t i = 0; i < mParticleCount; i++)
            {
                for (std::size_t k = 0; k < 4; k++)
                {
                    //perform bounding check, we have to check each vertex of each quad since the rect could be rotated
                    upper.x = std::min(upper.x, mVertices[4*i + k].position.x);
                    upper.y = std::min(upper.y, mVertices[4*i + k].position.y);
                    lower.x = std::max(lower.x, mVertices[4*i + k].position.x);
                    lower.y = std::max(lower.y, mVertices[4*i + k].position.y);
                }
            }
            return sf::FloatRect{ upper, lower - upper };
        }

        std::string ParticleSystem::getEmitterKey() const
        {
            return mFuncMaster.getEmitters().getKey(mEmitter);
        }

        std::string ParticleSystem::getAffectorKey(std::size_t i) const
        {
            return mFuncMaster.getAffectors().getKey(mAffectors[i]);
        }

        std::string ParticleSystem::getTexrectInitializerKey() const
        {
            return mFuncMaster.getTexrectInitializers().getKey(mTexrectInit);
        }

        std::size_t ParticleSystem::getAffectorCount() const
        {
            return mAffectors.size();
        }
}
