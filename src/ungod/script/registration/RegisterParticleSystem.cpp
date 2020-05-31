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
		void ParticleSystemHandlerFrontEnd::clearAffectors()
		{
			mHandler.clearAffectors(mEntity);
		}

		void ParticleSystemHandlerFrontEnd::setMaxForce(float maxforce)
		{
			mHandler.setMaxForce(mEntity, maxforce);
		}

		void ParticleSystemHandlerFrontEnd::setMaxVelocity(float maxvel)
		{
			mHandler.setMaxVelocity(mEntity, maxvel);
		}

		void ParticleSystemHandlerFrontEnd::setParticleSpeed(float speed)
		{
			mHandler.setParticleSpeed(mEntity, speed);
		}

		void ParticleSystemHandlerFrontEnd::setUniversalEmitter()
		{
			mHandler.setEmitter<UniversalEmitter, UniversalEstimator>(mEntity, PS_UNIVERSAL_EMITTER, PS_UNIVERSAL_ESTIMATE);
		}

		void ParticleSystemHandlerFrontEnd::setFixedPosition(const sf::Vector2f& position)
		{
			mHandler.setPositionDist<FixedPosition>(mEntity, PS_FIXED_POSITION, position);
		}

		void ParticleSystemHandlerFrontEnd::setEllipseDist(const sf::Vector2f& center, const sf::Vector2f& radius)
		{
			mHandler.setPositionDist<EllipseDist>(mEntity, PS_ELLIPSE_DIST, center, radius);
		}

		void ParticleSystemHandlerFrontEnd::setRectangleDist(const sf::Vector2f& topleft, const sf::Vector2f& sizes)
		{
			mHandler.setPositionDist<RectangleDist>(mEntity, PS_RECTANGLE_DIST, topleft, sizes);
		}

		void ParticleSystemHandlerFrontEnd::setLineSegmentDist(const sf::Vector2f& point1, const sf::Vector2f& point2)
		{
			mHandler.setPositionDist<LineSegmentDist>(mEntity, PS_LINE_SEGMENT_DIST, point1, point2);
		}

		void ParticleSystemHandlerFrontEnd::setFixedVelocity(const sf::Vector2f& vel)
		{
			mHandler.setVelocityDist<FixedVelocity>(mEntity, PS_FIXED_VELOCITY, vel);
		}

		void ParticleSystemHandlerFrontEnd::setConeDist(const sf::Vector2f& d1, const sf::Vector2f& d2)
		{
			mHandler.setVelocityDist<ConeDist>(mEntity, PS_CONE_DIST, d1, d2);
		}

		void ParticleSystemHandlerFrontEnd::setIntervalTick(float imsmin, float imsmax, int inumparticle)
		{
			mHandler.setSpawnInterval<IntervalTick>(mEntity, PS_INTERVAL_TICK, imsmin, imsmax, inumparticle);
		}

		void ParticleSystemHandlerFrontEnd::setOneShotTick(int inumparticle)
		{
			mHandler.setSpawnInterval<OneShotTick>(mEntity, PS_ONE_SHOT_TICK, inumparticle);
		}

		void ParticleSystemHandlerFrontEnd::setLifetimeInterval(float ltmin, float ltmax)
		{
			mHandler.setLifetimeDist<IntervalLifetime>(mEntity, PS_INTERVAL_LIFETIME, ltmin, ltmax);
		}

		void ParticleSystemHandlerFrontEnd::addDirectionalForce(const sf::Vector2f& force)
		{
			mHandler.addAffector<DirectionalForce>(mEntity, PS_DIRECTIONAL_FORCE, force);
		}

		void ParticleSystemHandlerFrontEnd::addDisplaceForce(float speed, float circle, float angle)
		{
			mHandler.addAffector<DisplaceForce>(mEntity, PS_DISPLACE_FORCE, speed, circle, angle);
		}

		void ParticleSystemHandlerFrontEnd::addAnimatedParticles(const std::string& metaID, const std::string& key, std::size_t numAnim)
		{
			mHandler.addAffector<AnimatedParticles>(mEntity, PS_ANIMATED_PARTICLES, metaID, key, numAnim);
		}

		void ParticleSystemHandlerFrontEnd::addParticleFadeOut()
		{
			mHandler.addAffector<FadeOut>(mEntity, PS_FADE_OUT);
		}

		void ParticleSystemHandlerFrontEnd::addColorShift(const sf::Color& colorBegin, const sf::Color& colorEnd)
		{
			mHandler.addAffector<ColorShift>(mEntity, PS_COLOR_SHIFT, colorBegin, colorEnd);
		}

		void ParticleSystemHandlerFrontEnd::addParticleRotation(float speed)
		{
			mHandler.addAffector<RotateParticle>(mEntity, PS_ROTATE_PARTICLE, speed);
		}

		void ParticleSystemHandlerFrontEnd::addParticleScale(const sf::Vector2f& scalesBegin, const sf::Vector2f& scalesEnd)
		{
			mHandler.addAffector<ScaleParticle>(mEntity, PS_SCALE_PARTICLE, scalesBegin, scalesEnd);
		}

		void ParticleSystemHandlerFrontEnd::addVelocityBasedRotation()
		{
			mHandler.addAffector<VelocityBasedRotation>(mEntity, PS_ROTATE_PARTICLE);
		}

		void ParticleSystemHandlerFrontEnd::addExplicitTexrectInitalizer(const sf::IntRect& rect)
		{
			mHandler.setTexrectInitializer<ExplicitTexrect>(mEntity, PS_EXPLICIT_TEXRECT, rect);
		}

		void ParticleSystemHandlerFrontEnd::addKeyTexrectInitalizer(const std::string& metaID, const std::string& key)
		{
			mHandler.setTexrectInitializer<TexrectByKey>(mEntity, PS_TEXRECT_BY_KEY, metaID, key);
		}

		void ParticleSystemHandlerFrontEnd::addMultipleKeyTexrectInitalizer(const std::string& metaID, script::Environment keys)
		{
			mHandler.setTexrectInitializer<MultipleTexrectsByKey>(mEntity, PS_MULTIPLE_TEXRECTS_BY_KEY, metaID, env2vec<std::string>(keys));
		}


        void registerParticleSystem(ScriptStateBase& state)
        {
			script::Usertype< ParticleSystemHandlerFrontEnd> psmType = state.registerUsertype<ParticleSystemHandlerFrontEnd>("ParticleSystemHandlerFrontEnd");
			psmType["clearAffectors"] = &ParticleSystemHandlerFrontEnd::clearAffectors;
			psmType["setMaxForce"] = &ParticleSystemHandlerFrontEnd::setMaxForce;
			psmType["setMaxVelocity"] = &ParticleSystemHandlerFrontEnd::setMaxVelocity;
			psmType["setParticleSpeed"] = &ParticleSystemHandlerFrontEnd::setParticleSpeed;
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			psmType["setUniversalEmitter"] = &ParticleSystemHandlerFrontEnd::setUniversalEmitter;
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			psmType["setFixedPosition"] = &ParticleSystemHandlerFrontEnd::setFixedPosition;
			psmType["setEllipseDist"] = &ParticleSystemHandlerFrontEnd::setEllipseDist;
			psmType["setRectangleDist"] = &ParticleSystemHandlerFrontEnd::setRectangleDist;
			psmType["setLineSegmentDist"] = &ParticleSystemHandlerFrontEnd::setLineSegmentDist;
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			psmType["setFixedVelocity"] = &ParticleSystemHandlerFrontEnd::setFixedVelocity;
			psmType["setIntervalTick"] = &ParticleSystemHandlerFrontEnd::setIntervalTick;
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			psmType["setIntervalTick"] = &ParticleSystemHandlerFrontEnd::setIntervalTick;
			psmType["setOneShotTick"] = &ParticleSystemHandlerFrontEnd::setOneShotTick;
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			psmType["setLifetimeInterval"] = &ParticleSystemHandlerFrontEnd::setLifetimeInterval;
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			psmType["addDirectionalForce"] = &ParticleSystemHandlerFrontEnd::addDirectionalForce;
			psmType["addDisplaceForce"] = &ParticleSystemHandlerFrontEnd::addDisplaceForce;
			psmType["addAnimatedParticles"] = &ParticleSystemHandlerFrontEnd::addAnimatedParticles;
			psmType["addParticleFadeOut"] = &ParticleSystemHandlerFrontEnd::addParticleFadeOut;
			psmType["addColorShift"] = &ParticleSystemHandlerFrontEnd::addColorShift;
			psmType["addParticleRotation"] = &ParticleSystemHandlerFrontEnd::addParticleRotation;
			psmType["addParticleScale"] = &ParticleSystemHandlerFrontEnd::addParticleScale;
			psmType["addVelocityBasedRotation"] = &ParticleSystemHandlerFrontEnd::addVelocityBasedRotation;
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			psmType["addExplicitTexrectInitalizer"] = &ParticleSystemHandlerFrontEnd::addExplicitTexrectInitalizer;
			psmType["addKeyTexrectInitalizer"] = &ParticleSystemHandlerFrontEnd::addKeyTexrectInitalizer;
			psmType["addMultipleKeyTexrectInitalizer"] = &ParticleSystemHandlerFrontEnd::addMultipleKeyTexrectInitalizer;
        }
    }
}
