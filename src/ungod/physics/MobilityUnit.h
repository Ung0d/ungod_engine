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

#ifndef UNGOD_MOBILITY_UNIT_H
#define UNGOD_MOBILITY_UNIT_H

#include "ungod/base/Entity.h"
#include "ungod/base/Utility.h"
#include "ungod/physics/Physics.h"

namespace ungod
{
    constexpr float sEpsilon = 0.1f;

    using V2f = sf::Vector2f;

    /** \brief Holds parameters of a single modile unit. */
    struct MobilityUnit
    {
        MobilityUnit() : velocity(0,0), acceleration(0,0) {}

        sf::Vector2f velocity;
        sf::Vector2f acceleration;
    };

    /** \brief Moves the mobility unit along the given (unit-)vector. */
    void accelerate(MobilityUnit& mu, const V2f& direction, float speed);

    /** \brief Stops all movement of the given mobility unit. */
    void stop(MobilityUnit& mu);

    /** \brief Moves the mobilitys unit towards the specified target point. */
    void seek(MobilityUnit& mu, const V2f& currentPosition, const V2f& targetPosition, float speed);

    /** \brief Moves the mobilitys unit away from the specified target point. */
    void flee(MobilityUnit& mu, const V2f& currentPosition, const V2f& targetPosition, float speed);

    /** \brief Moves the mobilitys unit towards the specified target point with smooth arrival inside the given radius around the target point. */
    void arrival(MobilityUnit& mu, const V2f& currentPosition, const V2f& targetPosition, float speed, float radius);

    /** \brief Pursuits another mobility unit by estimating its future position according to its velocity and moving
    * towards this position. */
    void pursuit(MobilityUnit& mu, MobilityUnit& prey, const V2f& currentPosition, const V2f& targetPosition, float targetMaxVel, float speed, float radius, float damp);

    /** \brief Evades the given mobility unit by estimating its future position and avoiding it. */
    void evade(MobilityUnit& mu, MobilityUnit& avoid, const V2f& currentPosition, const V2f& targetPosition, float targetMaxVel, float speed, float evadeDistance, float damp);

    /** \brief Wanders around randomly but with soft direction, changes. Adds small displacements to the units velocity every frame.*/
    void displace(MobilityUnit& mu, float speed, float circleDistance, float angleRange);

    /** \brief Returns true if and only if the mobility unit is currently in motion. */
    bool isMoving(MobilityUnit& mu);

    /** \brief Performs all final movement calculations for the unit. */
    void mobilize(MobilityUnit& mu, float maxForce, float maxVel);

    /** \brief Resets the acceleration to 0. */
    void resetAcceleration(MobilityUnit& mu);
}

#endif // UNGOD_MOBILITY_UNIT
