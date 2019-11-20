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


#ifndef UNGOD_PARTICLE_AFFECTOR_H
#define UNGOD_PARTICLE_AFFECTOR_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include "ungod/visual/Animation.h"
#include "ungod/serialization/MetaData.h"
#include "ungod/utility/FunctorHandling.h"
#include "ungod/serialization/SerialParticleSystem.h"

namespace ungod
{
    class ParticleSystem;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Data for the DirectionalForce affector. */
    struct DirectionalForce : public BaseFunctorData<>
    {
        DirectionalForce() = default;

        inline void init(const sf::Vector2f& f) { force = f; }

        sf::Vector2f force;

    private:
        FUNCTOR_DATA(DirectionalForce)

    public:
        SERIALIZABLE_DERIVED
    };

    /** \brief Moves particles along a fixed direction. */
    void directionalForce(DirectionalForce& data, ParticleSystem& particles, float delta);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Data for the DirectionalForce affector. */
    struct DisplaceForce : public BaseFunctorData<>
    {
        DisplaceForce() : speed(0), circle(0), angle(0) {}

        inline void init(float s, float c, float a) { speed = s; circle = c; angle = a; }

        float speed, circle, angle;

    private:
        FUNCTOR_DATA(DisplaceForce)

    public:
        SERIALIZABLE_DERIVED
    };

    /** \brief Moves particles along a fixed direction. */
    void displaceForce(DisplaceForce& data, ParticleSystem& particles, float delta);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Data for the fadeout affector. */
    struct FadeOut : public BaseFunctorData<>
    {
    FadeOut() = default;

    inline void init() { }

    private:
        FUNCTOR_DATA(FadeOut)

    public:
        SERIALIZABLE_DERIVED
    };

    /** \brief Reduces particles transparency with ongoing lifetime. */
    void fadeOut(FadeOut& data, ParticleSystem& particles, float delta);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Data for the animated particles affector. */
    struct AnimatedParticles : public BaseFunctorData<>
    {
        AnimatedParticles() = default;

        void init(const std::string& metaID, const std::string& keyInit, std::size_t numAnim);

        struct AnimComp
        {
            Animation animation;
            sf::Vertex vertices[4];
        };

        std::string key;
        std::vector<AnimComp> animations;
        MetaMap meta;

        private:
            FUNCTOR_DATA(AnimatedParticles)

        public:
            SERIALIZABLE_DERIVED
    };



    /** \brief Applies one or more animations to particles. */
    void animatedParticles(AnimatedParticles& data, ParticleSystem& particles, float delta);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Data for the color shift affector. */
    struct ColorShift : public BaseFunctorData<>
    {
    ColorShift() = default;

    inline void init(const sf::Color& colorB, const sf::Color& colorE) { colorBegin = colorB; colorEnd = colorE; }


    sf::Color colorBegin;
    sf::Color colorEnd;


    private:
        FUNCTOR_DATA(ColorShift)

    public:
        SERIALIZABLE_DERIVED
    };

    /** \brief Shifts particle color from colorBegin to colorEnd over the lifetime of a particle. */
    void colorShift(ColorShift& data, ParticleSystem& particles, float delta);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Data for the color shift affector. */
    struct RotateParticle : public BaseFunctorData<>
    {
    RotateParticle() : speed(0) {}

    inline void init(float s) { speed = s; }


    float speed;


    private:
        FUNCTOR_DATA(RotateParticle)

    public:
        SERIALIZABLE_DERIVED
    };

    /** \brief Shifts particle color from colorBegin to colorEnd over the lifetime of a particle. */
    void rotateParticle(RotateParticle& data, ParticleSystem& particles, float delta);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Data for the color shift affector. */
    struct ScaleParticle : public BaseFunctorData<>
    {
    ScaleParticle() = default;

    inline void init(const sf::Vector2f& sb, const sf::Vector2f& se) { scalesBegin = sb; scalesEnd = se; }


    sf::Vector2f scalesBegin;
    sf::Vector2f scalesEnd;


    private:
        FUNCTOR_DATA(ScaleParticle)

    public:
        SERIALIZABLE_DERIVED
    };

    /** \brief Shifts particle color from colorBegin to colorEnd over the lifetime of a particle. */
    void scaleParticle(ScaleParticle& data, ParticleSystem& particles, float delta);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Data for the rotate-according-to-velocity affector. */
    struct VelocityBasedRotation : public BaseFunctorData<>
    {
    VelocityBasedRotation() = default;

    inline void init() {  }

    private:
        FUNCTOR_DATA(VelocityBasedRotation)

    public:
        SERIALIZABLE_DERIVED
    };

    /** \brief Shifts particle color from colorBegin to colorEnd over the lifetime of a particle. */
    void velocityBasedRotation(VelocityBasedRotation& data, ParticleSystem& particles, float delta);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Data for the ExplicitTexrect affector. */
    struct ExplicitTexrect : public BaseFunctorData<>
    {
        ExplicitTexrect() = default;

        inline void init(const sf::IntRect& r) { rect = r; }

        sf::IntRect rect;

    private:
        FUNCTOR_DATA(ExplicitTexrect)

    public:
        SERIALIZABLE_DERIVED
    };

    /** \brief Initializes particles with an explicit texrect. */
    sf::IntRect explicitTexrect(ExplicitTexrect& data);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Data for the texrectByMetaNode affector. */
    struct TexrectByKey : public BaseFunctorData<>
    {
        TexrectByKey() = default;

        void init(const std::string& metaID, const std::string& k);

        std::string key;
        MetaMap meta;
        MetaNode node;

    private:
        FUNCTOR_DATA(TexrectByKey)

    public:
        SERIALIZABLE_DERIVED
    };


    /** \brief Initializes particles with a texrect that is determined by a metanode. */
    sf::IntRect texrectByKey(TexrectByKey& data);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Data for the multipleTexrectByMetaNode affector. */
    struct MultipleTexrectsByKey : public BaseFunctorData<>
    {
        MultipleTexrectsByKey() = default;

        void init(const std::string& metaID, const std::vector<std::string>& k);

        std::vector<std::string> keys;
        MetaMap meta;
        std::vector<MetaNode> nodes;

    private:
        FUNCTOR_DATA(MultipleTexrectsByKey)

    public:
        SERIALIZABLE_DERIVED
    };


    /** \brief Initializes particles with a texrect that is randomly chosen of a vector of possibilities. */
    sf::IntRect multipleTexrectsByKey(MultipleTexrectsByKey& data);
}

#endif

