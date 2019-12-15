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

#include "ungod/physics/Physics.h"
#include <cmath>

namespace ungod
{
    bool floatCompare(float a, float b)
    {
        return std::fabs(a - b) < std::numeric_limits<float>::epsilon();
    }

    bool floatCompare(const sf::Vector2f& a, const sf::Vector2f& b)
    {
        return floatCompare(a.x, b.x) && floatCompare(a.y, b.y);
    }

    void normalize(sf::Vector2f& vec)
    {
        const float magn= magnitude(vec);
        vec *= 1/magn;
    }

    sf::Vector2f normalizeVector(const sf::Vector2f& vec)
    {
        sf::Vector2f normal = vec;
        normalize(normal);
        return normal;
    }

    float magnitude(const sf::Vector2f& vec)
    {
        return sqrt(sqMagnitude(vec));
    }

	float sqMagnitude(const sf::Vector2f& vec)
	{
		return vec.x * vec.x + vec.y * vec.y;
	}

    float distance(const sf::Vector2f& point1, const sf::Vector2f& point2)
    {
        const float diffx = point1.x - point2.x;
        const float diffy = point1.y - point2.y;
        return sqrt(diffy*diffy + diffx*diffx);
    }

    float dotProduct(const sf::Vector2f& point1, const sf::Vector2f& point2)
    {
        return point1.x * point2.x + point1.y * point2.y;
    }

    float crossProduct(const sf::Vector2f& point1, const sf::Vector2f& point2)
    {
        return point1.x * point2.y - point2.x * point1.y;
    }

    sf::Vector2f perpendicularVector(const sf::Vector2f& vec)
    {
        return { -vec.y, vec.x };
    }

    bool rayIntersect(const sf::Vector2f& as, const sf::Vector2f& ad, const sf::Vector2f& bs, const sf::Vector2f& bd, sf::Vector2f& intersection)
    {
        float dx = bs.x - as.x;
        float dy = bs.y - as.y;
        float det = bd.x * ad.y - bd.y * ad.x;
        if (det == 0.0f)
            return false;
        float u = (dy * bd.x - dx * bd.y) / det;
        if (u < 0.0f)
            return false;
        float v = (dy * ad.x - dx * ad.y) / det;
        if (v < 0.0f)
            return false;
        intersection = as + ad * u;
        return true;
    }

    sf::FloatRect triangleBounds(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& c)
    {
        sf::FloatRect bounds;
        bounds.left = std::min(a.x, std::min(b.x, c.x));
        bounds.top = std::min(a.y, std::min(b.y, c.y));
        bounds.width = std::max(a.x, std::max(b.x, c.x)) - bounds.left;
        bounds.height = std::max(a.y, std::max(b.y, c.y)) - bounds.top;
        return bounds;
    }

    sf::Vector2f triangleCenterOfGravity(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& c)
    {
        return { (a.x + b.x + c.x)/3, (a.y + b.y + c.y)/3 };
    }

    float sign(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3)
    {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    }

    bool triangleContains(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& c, const sf::Vector2f& p)
    {
        bool b1 = sign(p, a, b) < 0.0f;
        bool b2 = sign(p, b, c) < 0.0f;
        bool b3 = sign(p, c, a) < 0.0f;

        return ((b1 == b2) && (b2 == b3));
    }

	sf::Vector2f centerPoint(const sf::Vector2f& a, const sf::Vector2f& b)
	{
		return (a + b) * 0.5f;
	}

    float distanceToLineSegment(const sf::Vector2f& point, const sf::Vector2f& a, const sf::Vector2f& b)
    {
        const float l2 = sqMagnitude(b-a);  
        if (l2 == 0.0) return distance(point, a);   
        // Consider the line extending the segment, parameterized as v + t (w - v).
        // We find projection of point p onto the line. 
        // It falls where t = [(p-a) . (b-a)] / |b-a|^2
        // We clamp t from [0,1] to handle points outside the segment vw.
        const float t = std::max(0.0f, std::min(1.0f, dotProduct(point - a, b - a) / l2));
        const sf::Vector2f projection = a + t * (b - a);  // Projection falls on the segment
        return distance(point, projection);
    }
}
