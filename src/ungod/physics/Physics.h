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

#ifndef PHYSICS_H
#define PHYSICS_H

#include <SFML/Graphics/Shape.hpp>

namespace ungod
{
    ///free functions

    bool floatCompare(float a, float b);
    bool floatCompare(const sf::Vector2f& a, const sf::Vector2f& b);

    void normalize(sf::Vector2f& vec);

    sf::Vector2f normalizeVector(const sf::Vector2f& vec);

    float magnitude(const sf::Vector2f& vec);

	float sqMagnitude(const sf::Vector2f& vec);

    float distance(const sf::Vector2f& point1, const sf::Vector2f& point2);

    float dotProduct(const sf::Vector2f& point1, const sf::Vector2f& point2);

    float crossProduct(const sf::Vector2f& point1, const sf::Vector2f& point2);

    sf::Vector2f perpendicularVector(const sf::Vector2f& vec);

    bool rayIntersect(const sf::Vector2f& as, const sf::Vector2f& ad, const sf::Vector2f& bs, const sf::Vector2f& bd, sf::Vector2f& intersection);

    sf::FloatRect triangleBounds(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& c);

    sf::Vector2f triangleCenterOfGravity(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& c);

    /** \brief Retuns 0, if p2 is on the line from p3 to p1, a positive result, if it is right of the line and a negative result if it is left of the line. */
    float sign(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3);

    bool triangleContains(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& c, const sf::Vector2f& p);

	sf::Vector2f centerPoint(const sf::Vector2f& a, const sf::Vector2f& b);
}

#endif // PHYSICS_H
