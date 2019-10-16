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

#include "ungod/content/particle_system/ParticleFunctorMaster.h"
#include "ungod/content/particle_system/ParticleSystem.h"

namespace ungod
{
        ParticleFunctorMaster::ParticleFunctorMaster()
        {
            //affectors
            mAffectors.addFunctor<DirectionalForce>(PS_DIRECTIONAL_FORCE, &directionalForce);
            mAffectors.addFunctor<DisplaceForce>(PS_DISPLACE_FORCE, &displaceForce);
            mAffectors.addFunctor<FadeOut>(PS_FADE_OUT, &fadeOut);
            mAffectors.addFunctor<AnimatedParticles>(PS_ANIMATED_PARTICLES, &animatedParticles);
            mAffectors.addFunctor<ColorShift>(PS_COLOR_SHIFT, &colorShift);
            mAffectors.addFunctor<RotateParticle>(PS_ROTATE_PARTICLE, &rotateParticle);
            mAffectors.addFunctor<ScaleParticle>(PS_SCALE_PARTICLE, &scaleParticle);
            mAffectors.addFunctor<VelocityBasedRotation>(PS_ROTATE_VELOCITY, &velocityBasedRotation);

            //texrect inits
            mTexrectInitializers.addFunctor<ExplicitTexrect>(PS_EXPLICIT_TEXRECT, &explicitTexrect);
            mTexrectInitializers.addFunctor<TexrectByKey>(PS_TEXRECT_BY_KEY, &texrectByKey);
            mTexrectInitializers.addFunctor<MultipleTexrectsByKey>(PS_MULTIPLE_TEXRECTS_BY_KEY, &multipleTexrectsByKey);

            //position dists
            mPositionDistributions.addFunctor<FixedPosition>(PS_FIXED_POSITION, &fixedPosition);
            mPositionDistributions.addFunctor<EllipseDist>(PS_ELLIPSE_DIST, &ellipseDistribution);
            mPositionDistributions.addFunctor<RectangleDist>(PS_RECTANGLE_DIST, &rectangleDistribution);
            mPositionDistributions.addFunctor<LineSegmentDist>(PS_LINE_SEGMENT_DIST, &lineSegmentDistribution);

            //velocity dists
            mVelocityDistributions.addFunctor<FixedVelocity>(PS_FIXED_VELOCITY, &fixedVelocity);
            mVelocityDistributions.addFunctor<ConeDist>(PS_CONE_DIST, &coneDistribution);

            //interval dists
            mIntervalDistributions.addFunctor<IntervalTick>(PS_INTERVAL_TICK, &intervalTick);
            mIntervalDistributions.addFunctor<OneShotTick>(PS_ONE_SHOT_TICK, &oneShotTick);

            //lifetime dists
            mLifetimeDistributions.addFunctor<IntervalLifetime>(PS_INTERVAL_LIFETIME, &intervalLifetime);

            //emitters
            mEmitters.addFunctor<UniversalEmitter>(PS_UNIVERSAL_EMITTER, &universalEmitter);

            //estimators
            mCountEstimators.addFunctor<UniversalEstimator>(PS_UNIVERSAL_ESTIMATE, &universalCountEstimate);
        }
}
