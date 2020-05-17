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
			script::Usertype< ParticleSystemHandler> psmType = state.registerUsertype<ParticleSystemHandler>("ParticleSystemManager");
			psmType["clearAffectors"] = & ParticleSystemHandler::clearAffectors;
			psmType["setMaxForce"] = & ParticleSystemHandler::setMaxForce;
			psmType["setMaxVelocity"] = &ParticleSystemHandler::setMaxVelocity;
			psmType["setParticleSpeed"] = & ParticleSystemHandler::setParticleSpeed;
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			psmType["setUniversalEmitter"] = [](ParticleSystemHandler& psm,
				Entity e)
			{
				psm.setEmitter<UniversalEmitter, UniversalEstimator>(e, PS_UNIVERSAL_EMITTER, PS_UNIVERSAL_ESTIMATE);
			};
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			psmType["setFixedPosition"] = [](ParticleSystemHandler& psm,
				Entity e, const sf::Vector2f& position)
			{
				psm.setPositionDist<FixedPosition>(e, PS_FIXED_POSITION, position);
			};
			psmType["setEllipseDist"] = [](ParticleSystemHandler& psm,
				Entity e, const sf::Vector2f& center, const sf::Vector2f& radius)
			{
				psm.setPositionDist<EllipseDist>(e, PS_ELLIPSE_DIST, center, radius);
			};
			psmType["setRectangleDist"] = [](ParticleSystemHandler& psm,
				Entity e, const sf::Vector2f& topleft, const sf::Vector2f& sizes)
			{
				psm.setPositionDist<RectangleDist>(e, PS_RECTANGLE_DIST, topleft, sizes);
			};
			psmType["setLineSegmentDist"] = [](ParticleSystemHandler& psm,
				Entity e, const sf::Vector2f& point1, const sf::Vector2f& point2)
			{
				psm.setPositionDist<LineSegmentDist>(e, PS_LINE_SEGMENT_DIST, point1, point2);
			};
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			psmType["setFixedVelocity"] = [](ParticleSystemHandler& psm,
				Entity e, const sf::Vector2f& vel)
			{
				psm.setVelocityDist<FixedVelocity>(e, PS_FIXED_VELOCITY, vel);
			};
			psmType["setConeDist"] = [](ParticleSystemHandler& psm,
				Entity e, const sf::Vector2f& d1, const sf::Vector2f& d2)
			{
				psm.setVelocityDist<ConeDist>(e, PS_CONE_DIST, d1, d2);
			};
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			psmType["setIntervalTick"] = [](ParticleSystemHandler& psm,
				Entity e, float imsmin, float imsmax, int inumparticle)
			{
				psm.setSpawnInterval<IntervalTick>(e, PS_INTERVAL_TICK, imsmin, imsmax, inumparticle);
			};
			psmType["setOneShotTick"] = [](ParticleSystemHandler& psm,
				Entity e, int inumparticle)
			{
				psm.setSpawnInterval<OneShotTick>(e, PS_ONE_SHOT_TICK, inumparticle);
			};
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			psmType["setLifetimeInterval"] = [](ParticleSystemHandler& psm,
				Entity e, float ltmin, float ltmax)
			{
				psm.setLifetimeDist<IntervalLifetime>(e, PS_INTERVAL_LIFETIME, ltmin, ltmax);
			};
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			psmType["addDirectionalForce"] = [](ParticleSystemHandler& psm, Entity e, const sf::Vector2f& force)
			{
				psm.addAffector<DirectionalForce>(e, PS_DIRECTIONAL_FORCE, force);
			};
			psmType["addDisplaceForce"] = [](ParticleSystemHandler& psm, Entity e, float speed, float circle, float angle)
			{
				psm.addAffector<DisplaceForce>(e, PS_DISPLACE_FORCE, speed, circle, angle);
			};
			psmType["addAnimatedParticles"] = [](ParticleSystemHandler& psm, Entity e, const std::string& metaID, const std::string& key, std::size_t numAnim)
			{
				psm.addAffector<AnimatedParticles>(e, PS_ANIMATED_PARTICLES, metaID, key, numAnim);
			};
			psmType["addParticleFadeOut"] = [](ParticleSystemHandler& psm, Entity e)
			{
				psm.addAffector<FadeOut>(e, PS_FADE_OUT);
			};
			psmType["addColorShift"] = [](ParticleSystemHandler& psm, Entity e, const sf::Color& colorBegin, const sf::Color& colorEnd)
			{
				psm.addAffector<ColorShift>(e, PS_COLOR_SHIFT, colorBegin, colorEnd);
			};
			psmType["addParticleRotation"] = [](ParticleSystemHandler& psm, Entity e, float speed)
			{
				psm.addAffector<RotateParticle>(e, PS_ROTATE_PARTICLE, speed);
			};
			psmType["addParticleScale"] = [](ParticleSystemHandler& psm, Entity e, const sf::Vector2f& scalesBegin, const sf::Vector2f& scalesEnd)
			{
				psm.addAffector<ScaleParticle>(e, PS_SCALE_PARTICLE, scalesBegin, scalesEnd);
			};
			psmType["addVelocityBasedRotation"] = [](ParticleSystemHandler& psm, Entity e)
			{
				psm.addAffector<VelocityBasedRotation>(e, PS_ROTATE_PARTICLE);
			};
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			psmType["addExplicitTexrectInitalizer"] = [](ParticleSystemHandler& psm, Entity e, const sf::IntRect& rect)
			{
				psm.setTexrectInitializer<ExplicitTexrect>(e, PS_EXPLICIT_TEXRECT, rect);
			};
			psmType["addKeyTexrectInitalizer"] = [](ParticleSystemHandler& psm, Entity e, const std::string& metaID, const std::string& key)
			{
				psm.setTexrectInitializer<TexrectByKey>(e, PS_TEXRECT_BY_KEY, metaID, key);
			};
			psmType["addMultipleKeyTexrectInitalizer"] = [](ParticleSystemHandler& psm, Entity e, const std::string& metaID, script::Environment keys)
			{
				psm.setTexrectInitializer<MultipleTexrectsByKey>(e, PS_MULTIPLE_TEXRECTS_BY_KEY, metaID, env2vec<std::string>(keys));
			};

            state.registerUsertype<ParticleSystemComponent>("ParticleSystemComponent");
        }
    }
}
