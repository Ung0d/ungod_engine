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


#ifndef UNGOD_PS_DISTRIBUTIONS_H
#define UNGOD_PS_DISTRIBUTIONS_H

#include <SFML/Graphics.hpp>
#include "ungod/base/Utility.h"
#include "ungod/utility/FunctorHandling.h"
#include "ungod/serialization/SerialParticleSystem.h"

namespace ungod
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Data for the DirectionalForce affector. */
    struct FixedPosition : public BaseFunctorData<>
    {
        FixedPosition() = default;

        inline void init(const sf::Vector2f& p) { position = p; }

        sf::Vector2f position;

    private:
        FUNCTOR_DATA(FixedPosition)

    public:
        SERIALIZABLE_DERIVED
    };

    /** \brief Emits particles from a fixed position. */
    sf::Vector2f fixedPosition(FixedPosition& data);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Data for the DirectionalForce affector. */
    struct EllipseDist : public BaseFunctorData<>
    {
        EllipseDist() = default;

        inline void init(const sf::Vector2f& c, const sf::Vector2f& r) { center = c; radius = r; }

        sf::Vector2f center;
        sf::Vector2f radius;

    private:
        FUNCTOR_DATA(EllipseDist)

    public:
        SERIALIZABLE_DERIVED
    };

    /** \brief Emits particles from an ellipsiod field. */
    sf::Vector2f ellipseDistribution(EllipseDist& data);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Data for the DirectionalForce affector. */
    struct RectangleDist : public BaseFunctorData<>
    {
        RectangleDist() = default;

        inline void init(const sf::Vector2f& tl, const sf::Vector2f& s) { topleft = tl; sizes = s; }

        sf::Vector2f topleft;
        sf::Vector2f sizes;

    private:
        FUNCTOR_DATA(RectangleDist)

    public:
        SERIALIZABLE_DERIVED
    };

    /** \brief Emits particles from an rectangular field. */
    sf::Vector2f rectangleDistribution(RectangleDist& data);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Samples points from a line segment defined by 2 points. */
    struct LineSegmentDist : public BaseFunctorData<>
    {
        LineSegmentDist() = default;

        inline void init(const sf::Vector2f& p1, const sf::Vector2f& p2) { point1 = p1; point2 = p2; }

        sf::Vector2f point1;
        sf::Vector2f point2;

    private:
        FUNCTOR_DATA(LineSegmentDist)

    public:
        SERIALIZABLE_DERIVED
    };

    /** \brief Emits particles from an rectangular field. */
    sf::Vector2f lineSegmentDistribution(LineSegmentDist& data);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Data for the DirectionalForce affector. */
    struct FixedVelocity : public BaseFunctorData<>
    {
        FixedVelocity() = default;

        inline void init(const sf::Vector2f& v) { velocity = v; }

        sf::Vector2f velocity;

    private:
        FUNCTOR_DATA(FixedVelocity)

    public:
        SERIALIZABLE_DERIVED
    };

    /** \brief Emits particles with a fixed initial velocity. */
    sf::Vector2f fixedVelocity(FixedVelocity& data);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Data for the DirectionalForce affector. */
    struct ConeDist : public BaseFunctorData<>
    {
        ConeDist() = default;

        inline void init(const sf::Vector2f& d1, const sf::Vector2f& d2) { direction1 = d1; direction2 = d2; }

        sf::Vector2f direction1;
        sf::Vector2f direction2;

    private:
        FUNCTOR_DATA(ConeDist)

    public:
        SERIALIZABLE_DERIVED
    };


    /** \brief Emits particles within a cone along the given direction and for the given opening angle. */
    sf::Vector2f coneDistribution(ConeDist& data);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    /** \brief Data for the DirectionalForce affector. */
    struct IntervalTick : public BaseFunctorData<>
    {
        IntervalTick() = default;

        inline void init(float imsmin, float imsmax, int inumparticle)
        {
            msmin = imsmin;
            msmax = imsmax;
            numparticle = inumparticle;
            timer = 0.0f; timermax = NumberGenerator::getFloatRandBetw(msmin, msmax);
        }

        float msmin;
        float msmax;
        int numparticle;
        float timer;
        float timermax;

    private:
        FUNCTOR_DATA(IntervalTick)

    public:
        SERIALIZABLE_DERIVED
    };


    /** \brief Emits particles with a time tick within the given bounds. */
    int intervalTick(IntervalTick& data, float delta);



    /** \brief Data for the DirectionalForce affector. */
    struct OneShotTick : public BaseFunctorData<>
    {
        OneShotTick() = default;

        inline void init(int inumparticle)
        {
            numparticle = inumparticle;
            shot = false;
        }

        int numparticle;
        bool shot;

    private:
        FUNCTOR_DATA(OneShotTick)

    public:
        SERIALIZABLE_DERIVED
    };


    /** \brief Emits particles with a time tick within the given bounds. */
    int oneShotTick(OneShotTick& data, float delta);


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Data for the DirectionalForce affector. */
    struct IntervalLifetime : public BaseFunctorData<>
    {
        IntervalLifetime() = default;

        inline void init(float imsmin, float imsmax)
        {
            msmin = imsmin;
            msmax = imsmax;
        }

        float msmin;
        float msmax;

    private:
        FUNCTOR_DATA(IntervalLifetime)

    public:
        SERIALIZABLE_DERIVED
    };

    /** \brief Emits particles with a life time sampled from the given interval. */
    float intervalLifetime(IntervalLifetime& data);
}

#endif

