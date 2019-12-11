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

#include "ungod/physics/Collision.h"
#include "ungod/base/Logger.h"
#include "ungod/base/Transform.h"

namespace ungod
{
	std::pair<bool, sf::Vector2f> doCollide(const Collider& c1, const Collider& c2, const TransformComponent& t1, const TransformComponent& t2)
	{
		bool collision = false;
		float minMagn = std::numeric_limits<float>::max();
		sf::Vector2f mdvMin; 
		std::vector<sf::Vector2f> axis, pivots1, pivots2;
		axis.reserve(Collider::MAX_PARAM); //reserve enough space to prevent reallocation even in worst case
		pivots1.reserve((unsigned)(Collider::MAX_PARAM / 2));
		pivots2.reserve((unsigned)(Collider::MAX_PARAM / 2));
		for (unsigned i = 0; i < c1.getNumRuns(); i++)
			for (unsigned j = 0; j < c2.getNumRuns(); j++)
		{
			axis.clear();
			pivots1.clear();
			pivots2.clear();
			c1.getAxisAndPivots(t1, axis, pivots1, i);
			c2.getAxisAndPivots(t2, axis, pivots2, j);
			bool collisionHere;
			sf::Vector2f mdv;
			std::tie(collisionHere, mdv) = satAlgorithm(axis, pivots1, pivots2);
			if (dotProduct(t1.getTransform().transformPoint(c1.getCenter()) -
				t2.getTransform().transformPoint(c2.getCenter()), mdv) < 0)
			{
				mdv = -mdv;
			}
			float curMagn = sqMagnitude(mdv);
			if (curMagn < minMagn)
			{
				mdvMin = mdv;
				minMagn = curMagn;
			}
			collision = collision || collisionHere;
		}
		return { collision, mdvMin };
	}


	std::pair<bool, sf::Vector2f> satAlgorithm(const std::vector<sf::Vector2f>& axis, const std::vector<sf::Vector2f>& pivots1, const std::vector<sf::Vector2f>& pivots2)
	{
		float overlap;
		float smallestOverlap = std::numeric_limits<float>::max();
		sf::Vector2f offset;

		for (auto& a : axis)
		{
			float c1Left = std::numeric_limits<float>::infinity();
			float c1Right = -std::numeric_limits<float>::infinity();
			float c2Left = std::numeric_limits<float>::infinity();
			float c2Right = -std::numeric_limits<float>::infinity();

			float projection;

			for (auto& piv : pivots1)
			{
				projection = dotProduct(a, piv);
				c1Right = std::max(c1Right, projection);
				c1Left = std::min(c1Left, projection);
			}

			for (auto& piv : pivots2)
			{
				projection = dotProduct(a, piv);
				c2Right = std::max(c2Right, projection);
				c2Left = std::min(c2Left, projection);
			}

			overlap = std::min(c1Right, c2Right) - std::max(c1Left, c2Left);

			if (overlap < 0)
			{
				return { false, {0,0} };
			}
			if (overlap < smallestOverlap)
			{
				smallestOverlap = overlap;
				offset = a * overlap;
			}
		}

		return { true, offset };
	}


    bool containsPoint(const Collider& collider, const TransformComponent& transf, const sf::Vector2f& point)
    {
		switch (collider.getType())
		{
			case ColliderType::ROTATED_RECT:
				return rotatedRectContainsPoint(collider, transf, point);
			case ColliderType::CONVEX_POLYGON:
				return convexPolygonContainsPoint(collider, transf, point);
			/*case ColliderType::CIRCLE:
				return circleContainsPoint(collider, transf, point);*/
			default: //no support for edge chains
				return false;
		}
    }


	bool rotatedRectContainsPoint(const Collider& collider, const TransformComponent& transf, const sf::Vector2f& point)
	{
		RotatedRectConstAggregator rota{ collider };
		//translate the point into the collider "local" coordinates
		sf::Vector2f transfPoint = transf.getTransform().getInverse().transformPoint(point);
		if (rota.getRotation() != 0.0f)
		{
			//rotate the point in the coord.system of the collider
			sf::Transform inverseRotation;
			inverseRotation.rotate(-rota.getRotation(), rota.getCenter());
			transfPoint = inverseRotation.transformPoint(transfPoint);

		}
		return transfPoint.x >= rota.getUpLeftX() && transfPoint.y >= rota.getUpLeftY() &&
			transfPoint.x <= rota.getDownRightX() && transfPoint.y <= rota.getDownRightY();
	}


	bool convexPolygonContainsPoint(const Collider& collider, const TransformComponent& transf, const sf::Vector2f& point)
	{
		PointSetConstAggregator pa{ collider };
		//translate the point into the collider "local" coordinates
		sf::Vector2f transfPoint = transf.getTransform().getInverse().transformPoint(point);
		for (unsigned i = 0; i < pa.getNumberOfPoints(); i++)
		{
			unsigned inext = (i + 1) % pa.getNumberOfPoints();
			sf::Vector2f pcur{ pa.getPointX(i), pa.getPointY(i) };
			sf::Vector2f normal = sf::Vector2f{ pa.getPointX(inext), pa.getPointY(inext) } - pcur;
			float dp = dotProduct(normal, point- pcur);
			if (dp < 0.0f)
				return false;
		}
		return true;
	}


	/*bool circleContainsPoint(const Collider& collider, const TransformComponent& transf, const sf::Vector2f& point)
	{
		CircleConstAggregator ca{ collider };
		return distance({ ca.getCenterX(), ca.getCenterY() }, point) <= ca.getRadius();
	}*/
}
