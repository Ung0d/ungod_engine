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

#include "ungod/content/particle_system/Distributions.h"

namespace ungod
{
        sf::Vector2f fixedPosition(FixedPosition& data)
        {
            return data.position;
        }


        sf::Vector2f ellipseDistribution(EllipseDist& data)
        {
            float phi = NumberGenerator::getFloatRandBetw(0, 2*PI);
            float rho = NumberGenerator::getFloatRandBetw(0, 1);

            return sf::Vector2f{ (float)(data.center.x + data.radius.x * sqrt(phi) * cos(rho)), (float)(data.center.y + data.radius.y * sqrt(phi) * sin(rho))};
        }


        sf::Vector2f rectangleDistribution(RectangleDist& data)
        {
            float xdiff = NumberGenerator::getFloatRandBetw(0, data.sizes.x);
            float ydiff = NumberGenerator::getFloatRandBetw(0, data.sizes.y);

            return sf::Vector2f{ data.topleft.x + xdiff, data.topleft.y + ydiff };
        }


        sf::Vector2f lineSegmentDistribution(LineSegmentDist& data)
        {
            float lambda = NumberGenerator::getFloatRandBetw(0,1);

            return lambda * data.point1 + (1-lambda)* data.point2;
        }


        sf::Vector2f fixedVelocity(FixedVelocity& data)
        {
            return data.velocity;
        }


        sf::Vector2f coneDistribution(ConeDist& data)
        {
            float lambda = NumberGenerator::getFloatRandBetw(0, 1);

            return sf::Vector2f{ lambda*data.direction1 + (1-lambda)*data.direction2 };
        }


        int intervalTick(IntervalTick& data, float delta)
        {
            int num = 0;
            data.timer += delta;
            while (data.timer >= data.timermax)
            {
                data.timer -= data.timermax;
                data.timermax = NumberGenerator::getFloatRandBetw(data.msmin, data.msmax);
                num += data.numparticle;
            }
            return num;
        }


        int oneShotTick(OneShotTick& data, float delta)
        {
            if (!data.shot)
            {
                data.shot = true;
                return data.numparticle;
            }
            return 0;
        }


        float intervalLifetime(IntervalLifetime& data)
        {
            return NumberGenerator::getFloatRandBetw(data.msmin, data.msmax);
        }
}
