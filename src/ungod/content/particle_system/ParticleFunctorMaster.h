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


#ifndef UNGOD_PARTICLE_FUNCTOR_MASTER_H
#define UNGOD_PARTICLE_FUNCTOR_MASTER_H

#include <SFML/Graphics.hpp>
#include "ungod/utility/FunctorHandling.h"

namespace ungod
{
    class ParticleSystem;

    //keys to identify callbacks
    const std::string PS_DIRECTIONAL_FORCE = "directional_force";
    const std::string PS_DISPLACE_FORCE = "displace_force";
    const std::string PS_FADE_OUT = "fade_out";
    const std::string PS_ANIMATED_PARTICLES = "animated_particles";
    const std::string PS_COLOR_SHIFT = "color_shift";
    const std::string PS_ROTATE_PARTICLE = "rotate_particle";
    const std::string PS_SCALE_PARTICLE = "scale_particle";
    const std::string PS_ROTATE_VELOCITY = "rotate_based_on_velocity";
    const std::string PS_EXPLICIT_TEXRECT = "explicit_texrect";
    const std::string PS_TEXRECT_BY_KEY = "texrect_by_key";
    const std::string PS_ANIMATION_TEXRECT = "animation_texrect";
    const std::string PS_FIXED_POSITION = "fixed_position";
    const std::string PS_ELLIPSE_DIST = "ellipse_dist";
    const std::string PS_RECTANGLE_DIST = "rectangle_dist";
    const std::string PS_LINE_SEGMENT_DIST = "line_segment_dist";
    const std::string PS_FIXED_VELOCITY = "fixed_velocity";
    const std::string PS_CONE_DIST = "cone_dist";
    const std::string PS_INTERVAL_TICK = "interval_tick";
    const std::string PS_INTERVAL_LIFETIME = "interval_lifetime";
    const std::string PS_UNIVERSAL_EMITTER = "universal_emitter";
    const std::string PS_UNIVERSAL_ESTIMATE = "universal_estimate";

    /** \brief A master class for all sorts of behaviors and callbacks. Usually one onstance is created and passed to
    * created particle systems. */
    class ParticleFunctorMaster
    {
    public:
        ParticleFunctorMaster();

        inline const FunctorSet<sf::Vector2f>& getPositionDistributions() { return mPositionDistributions; }
        inline const FunctorSet<sf::Vector2f>& getVelocityDistributions() { return mVelocityDistributions; }
        inline const FunctorSet<float>& getLifetimeDistributions() { return mLifetimeDistributions; }
        inline const FunctorSet<int, float>& getIntervalDistributions() { return mIntervalDistributions; }
        inline const FunctorSet<void, ParticleSystem&, float>& getEmitters() { return mEmitters; }
        inline const FunctorSet<void, ParticleSystem&, float>& getAffectors() { return mAffectors; }
        inline const FunctorSet<std::size_t, const FunctorHandle&>& getCountEstimators() { return mCountEstimators; }
        inline const FunctorSet<sf::IntRect>& getTexrectInitializers() { return mTexrectInitializers; }

    private:
        FunctorSet<sf::Vector2f> mPositionDistributions;
        FunctorSet<sf::Vector2f> mVelocityDistributions;
        FunctorSet<float> mLifetimeDistributions;
        FunctorSet<int, float> mIntervalDistributions;
        FunctorSet<void, ParticleSystem&, float> mEmitters;
        FunctorSet<void, ParticleSystem&, float> mAffectors;
        FunctorSet<std::size_t, const FunctorHandle&> mCountEstimators;
        FunctorSet<sf::IntRect> mTexrectInitializers;
    };
}

#endif

