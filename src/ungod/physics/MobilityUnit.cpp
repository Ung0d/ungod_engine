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

#include "ungod/physics/MobilityUnit.h"

namespace ungod
{
    void accelerate(MobilityUnit& mu, const V2f& direction, float speed)
    {
        mu.acceleration += speed*direction;
    }

    void stop(MobilityUnit& mu)
    {
        mu.velocity = { 0, 0 };
        resetAcceleration(mu);
    }

    void seek(MobilityUnit& mu, const V2f& currentPosition, const V2f& targetPosition, float speed)
    {
        sf::Vector2f desiredVelocity(targetPosition - currentPosition);
        if (desiredVelocity.x != 0 || desiredVelocity.y != 0)
            normalize(desiredVelocity);
        accelerate(mu, desiredVelocity, speed);
    }

    void flee(MobilityUnit& mu, const V2f& currentPosition, const V2f& targetPosition, float speed)
    {
        sf::Vector2f desiredVelocity(currentPosition - targetPosition);
        if (desiredVelocity.x != 0 || desiredVelocity.y != 0)
            normalize(desiredVelocity);
        accelerate(mu, desiredVelocity, speed);
    }

    void arrival(MobilityUnit& mu, const V2f& currentPosition, const V2f& targetPosition, float speed, float radius)
    {
        sf::Vector2f desiredVelocity(targetPosition - currentPosition);
        const float desMagn = magnitude(desiredVelocity);
        if (desMagn != 0)
        {
            desiredVelocity = desiredVelocity * (1/desMagn);
            if(desMagn <= radius)
            {
                float slow = desMagn/radius;
                desiredVelocity = desiredVelocity * slow * slow;
            }
            accelerate(mu, desiredVelocity, speed);
        }
    }

    void pursuit(MobilityUnit& mu, MobilityUnit& prey, const V2f& currentPosition, const V2f& targetPosition, float targetMaxVel, float speed, float radius, float damp)
    {
        sf::Vector2f dist = targetPosition - currentPosition;
        const float distmagn = magnitude(dist);
        if (dist.x != 0 || dist.y != 0)
            dist = dist*(1/distmagn);
        sf::Vector2f estimatedPosition = targetPosition + damp*(distmagn / targetMaxVel) * prey.velocity;
        arrival(mu, currentPosition, estimatedPosition, speed, radius);
    }

    void evade(MobilityUnit& mu, MobilityUnit& avoid, const V2f& currentPosition, const V2f& targetPosition, float targetMaxVel, float speed, float evadeDistance, float damp)
    {
        sf::Vector2f dist = targetPosition - currentPosition;
        const float distMagn = magnitude(dist);
        if (distMagn == 0)
            return;
        dist = dist*(1/distMagn);
        sf::Vector2f estimatedPosition = targetPosition + damp*(distMagn / targetMaxVel) * avoid.velocity;
        if (distMagn <= evadeDistance)
        {
            flee(mu, currentPosition, estimatedPosition, speed);
        }
    }

    void displace(MobilityUnit& mu, float speed, float circleDistance, float angleRange)
    {
        sf::Vector2f dispCircle = mu.velocity;
        if (dispCircle.x == 0 && dispCircle.y == 0)
            dispCircle = {1,1};
        normalize(dispCircle);
        sf::Vector2f displacementForce = dispCircle;
        float angle = acos(displacementForce.x); //dot-product between dispCircle and unit-x-vector
        angle += NumberGenerator::getFloatRandBetw(-angleRange,angleRange);
        displacementForce.x = cos(angle);
        displacementForce.y = sin(angle);
        dispCircle = dispCircle * circleDistance + displacementForce;
        normalize(dispCircle);
        accelerate( mu, dispCircle, speed );
    }

    bool isMoving(MobilityUnit& mu)
    {
        return mu.velocity.x != 0 || mu.velocity.y != 0;
    }

    void mobilize(MobilityUnit& mu, float maxForce, float maxVel)
    {
        //truncate the acceleration vector
        sf::Vector2f acc = mu.acceleration;

        const float accmagn = magnitude(acc);
        const float acctrunc = maxForce/accmagn;
        if (acctrunc < 1)
            acc *= acctrunc;

        //apply the acceleration and truncate again
        mu.velocity += acc;

        const float velmagn = magnitude(mu.velocity);
        if (velmagn < sEpsilon)
        {
            mu.velocity = {0,0};
        }
        else
        {
            const float veltrunc = maxVel/velmagn;
            if (veltrunc < 1)
                mu.velocity *= veltrunc;
        }
    }

    void resetAcceleration(MobilityUnit& mu)
    {
        mu.acceleration = {0,0};
    }
}
