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

#include "ungod/script/registration/RegisterParticleSystem.h"
#include "ungod/script/registration/RegistrationDetail.h"
#include "ungod/content/particle_system/ParticleComponent.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerParticleSystem(ScriptStateBase& state)
        {
            state.registerUsertype<ParticleSystemManager>("ParticleSystemManager",
               "clearAffectors", &ParticleSystemManager::clearAffectors,
               "setMaxForce", &ParticleSystemManager::setMaxForce,
               "setMaxVelocity", &ParticleSystemManager::setMaxVelocity,
               "setParticleSpeed", &ParticleSystemManager::setParticleSpeed,
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
               "setUniversalEmitter", [] (ParticleSystemManager& psm,
                                          Entity e)
                  {
                        psm.setEmitter<UniversalEmitter, UniversalEstimator>(e, PS_UNIVERSAL_EMITTER, PS_UNIVERSAL_ESTIMATE);
                  },
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                "setFixedPosition", [] (ParticleSystemManager& psm,
                                          Entity e, const sf::Vector2f& position)
                  {
                        psm.setPositionDist<FixedPosition>(e, PS_FIXED_POSITION, position);
                  },
                "setEllipseDist", [] (ParticleSystemManager& psm,
                                          Entity e, const sf::Vector2f& center, const sf::Vector2f& radius)
                  {
                        psm.setPositionDist<EllipseDist>(e, PS_ELLIPSE_DIST, center, radius);
                  },
                "setRectangleDist", [] (ParticleSystemManager& psm,
                                          Entity e, const sf::Vector2f& topleft, const sf::Vector2f& sizes)
                  {
                        psm.setPositionDist<RectangleDist>(e, PS_RECTANGLE_DIST, topleft, sizes);
                  },
                "setLineSegmentDist", [] (ParticleSystemManager& psm,
                                          Entity e, const sf::Vector2f& point1, const sf::Vector2f& point2)
                  {
                        psm.setPositionDist<LineSegmentDist>(e, PS_LINE_SEGMENT_DIST, point1, point2);
                  },
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                "setFixedVelocity", [] (ParticleSystemManager& psm,
                                          Entity e, const sf::Vector2f& vel)
                  {
                        psm.setVelocityDist<FixedVelocity>(e, PS_FIXED_VELOCITY, vel);
                  },
                "setConeDist", [] (ParticleSystemManager& psm,
                                          Entity e, const sf::Vector2f& d1, const sf::Vector2f& d2)
                  {
                        psm.setVelocityDist<ConeDist>(e, PS_CONE_DIST, d1, d2);
                  },
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                "setIntervalTick", [] (ParticleSystemManager& psm,
                                          Entity e, float imsmin, float imsmax, int inumparticle)
                  {
                        psm.setSpawnInterval<IntervalTick>(e, PS_INTERVAL_TICK, imsmin, imsmax, inumparticle);
                  },
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                "setLifetimeInterval", [] (ParticleSystemManager& psm,
                                          Entity e, float ltmin, float ltmax)
                  {
                        psm.setLifetimeDist<IntervalLifetime>(e, PS_INTERVAL_LIFETIME, ltmin, ltmax);
                  },
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
               "addDirectionalForce", [] (ParticleSystemManager& psm, Entity e, const sf::Vector2f& force)
                  {
                        psm.addAffector<DirectionalForce>(e, PS_DIRECTIONAL_FORCE, force);
                  },
               "addDisplaceForce", [] (ParticleSystemManager& psm, Entity e, float speed, float circle, float angle)
                  {
                        psm.addAffector<DisplaceForce>(e, PS_DISPLACE_FORCE, speed, circle, angle);
                  },
               "addAnimatedParticles", [] (ParticleSystemManager& psm, Entity e, const std::string& metaID, const std::string& key, std::size_t numAnim)
                  {
                        psm.addAffector<AnimatedParticles>(e, PS_ANIMATED_PARTICLES, metaID, key, numAnim);
                  },
               "addParticleFadeOut", [] (ParticleSystemManager& psm, Entity e)
                  {
                        psm.addAffector<FadeOut>(e, PS_FADE_OUT);
                  },
               "addColorShift", [] (ParticleSystemManager& psm, Entity e, const sf::Color& colorBegin, const sf::Color& colorEnd)
                  {
                        psm.addAffector<ColorShift>(e, PS_COLOR_SHIFT, colorBegin, colorEnd);
                  },
               "addParticleRotation", [] (ParticleSystemManager& psm, Entity e, float speed)
                  {
                        psm.addAffector<RotateParticle>(e, PS_ROTATE_PARTICLE, speed);
                  },
               "addParticleScale", [] (ParticleSystemManager& psm, Entity e, const sf::Vector2f& scalesBegin, const sf::Vector2f& scalesEnd)
                  {
                        psm.addAffector<ScaleParticle>(e, PS_SCALE_PARTICLE, scalesBegin, scalesEnd);
                  },
               "addVelocityBasedRotation", [] (ParticleSystemManager& psm, Entity e)
                  {
                        psm.addAffector<VelocityBasedRotation>(e, PS_ROTATE_PARTICLE);
                  },
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
               "addExplicitTexrectInitalizer", [] (ParticleSystemManager& psm, Entity e, const sf::IntRect& rect)
                  {
                        psm.setTexrectInitializer<ExplicitTexrect>(e, PS_EXPLICIT_TEXRECT, rect);
                  },
               "addKeyTexrectInitalizer", [] (ParticleSystemManager& psm, Entity e, const std::string& metaID, const std::string& key)
                  {
                        psm.setTexrectInitializer<TexrectByKey>(e, PS_TEXRECT_BY_KEY, metaID, key);
                  });

            state.registerUsertype<ParticleSystemComponent>("ParticleSystemComponent");
        }
    }
}
