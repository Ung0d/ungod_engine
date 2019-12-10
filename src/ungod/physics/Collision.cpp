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
		switch (c1.getType())
		{
		case ColliderType::ROTATED_RECT:
			return { false, {} };
		default:
			return { false, {} };
		}
	}


	std::pair<bool, sf::Vector2f> satAlgorithm(std::vector<sf::Vector2f>& axis,
		const std::vector<sf::Vector2f>& points1,
		const std::vector<sf::Vector2f>& points2)
	{
		float overlap;
		float smallestOverlap = std::numeric_limits<float>::max();
		sf::Vector2f offset;

		for (auto& a : axis)
		{
			normalize(a);
			sf::Vector2f perp = perpendicularVector(a);

			float c1Left = std::numeric_limits<float>::infinity();
			float c1Right = -std::numeric_limits<float>::infinity();
			float c2Left = std::numeric_limits<float>::infinity();
			float c2Right = -std::numeric_limits<float>::infinity();

			float projection;

			for (auto& point : points1)
			{
				projection = dotProduct(perp, point);
				c1Right = std::max(c1Right, projection);
				c1Left = std::min(c1Left, projection);
			}

			for (auto& point : points2)
			{
				projection = dotProduct(perp, point);
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
				offset = perp * overlap;
			}
		}

		return { true, offset };
	}


	std::pair<bool, sf::Vector2f> checkPolygonCollision(
		const Collider& c1, const Collider& c2, const TransformComponent& t1, const TransformComponent& t2)
	{
		std::vector<sf::Vector2f> axis;
		std::vector<sf::Vector2f> axis2;
		std::vector<sf::Vector2f> points1;
		std::vector<sf::Vector2f> points2;
		std::tie(points1, axis) = detail::getAxisAndPivots(c1, t1);
		std::tie(points2, axis2) = detail::getAxisAndPivots(c2, t2);
		axis.insert(axis.end(), axis2.begin(), axis2.end());
		return satAlgorithm(axis, points1, points2);
	}


	std::pair<bool, sf::Vector2f> checkPolygonWithLineChainCollision(
		const Collider& c1, const Collider& c2, const TransformComponent& t1, const TransformComponent& t2)
	{
		std::vector<sf::Vector2f> axis;
		std::vector<sf::Vector2f> polyPoints;
		std::vector<sf::Vector2f> linePoints{ 2 };
		std::tie(polyPoints, axis) = detail::getAxisAndPivots(c1, t1);
		axis.emplace_back(); //additional slot for the axis induced by each line segment
		PointSetConstAggregator psa{ c2 };
		bool collision = false;
		sf::Vector2f mdv;
		for (unsigned i = 0; i < psa.getNumberOfPoints(); i++)
		{
			bool collisionHere;
			sf::Vector2f mdvHere;
			unsigned inext = (i + 1) % psa.getNumberOfPoints();
			linePoints[0] = t2.getTransform().transformPoint({ psa.getPointX(i), psa.getPointY(i) });
			linePoints[1] = t2.getTransform().transformPoint({ psa.getPointX(inext), psa.getPointY(inext) }); //todo reuse from prev iteration
			axis.back() = linePoints[1] - linePoints[0];
			std::tie(collisionHere, mdv) = satAlgorithm(axis, polyPoints, linePoints);
			if (collisionHere)
			{
				collision = true;
				if (magnitude(mdvHere) < magnitude(mdv))
					mdv = mdvHere;
			}
		}
		return { collision, mdv };
	}


	std::pair<bool, sf::Vector2f> checkPolygonWithCircleCollision(
		const Collider& c1, const Collider& c2, const TransformComponent& t1, const TransformComponent& t2)
	{

		return { false, {} };
	}


	std::pair<bool, sf::Vector2f> checkSegmentChainWithCircleCollision(
		const Collider& c1, const Collider& c2, const TransformComponent& t1, const TransformComponent& t2)
	{

		return { false, {} };
	}


	std::pair<bool, sf::Vector2f> checkCircleCollision(
		const Collider& c1, const Collider& c2, const TransformComponent& t1, const TransformComponent& t2)
	{
		/*CircleAggregator ca1{ c1 }, ca2{ c2 };

		std::vector<sf::Vector2f> axis{ sf::Vector2f{ca1.getCenterX(), ca1.getCenterY()} };
		return satAlgorithm(axis, );*/
		return { false, {} };
	}


    bool containsPoint(const Collider& collider, const TransformComponent& transf, const sf::Vector2f& point)
    {
		switch (collider.getType())
		{
			case ColliderType::ROTATED_RECT:
				return rotatedRectContainsPoint(collider, transf, point);
			case ColliderType::CONVEX_POLYGON:
				return convexPolygonContainsPoint(collider, transf, point);
			case ColliderType::CIRCLE:
				return circleContainsPoint(collider, transf, point);
			default:
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


	bool circleContainsPoint(const Collider& collider, const TransformComponent& transf, const sf::Vector2f& point)
	{
		CircleConstAggregator ca{ collider };
		return distance({ ca.getCenterX(), ca.getCenterY() }, point) <= ca.getRadius();
	}

	namespace detail
	{
		std::tuple<std::vector<sf::Vector2f>, std::vector<sf::Vector2f>> 
			getAxisAndPivots(const Collider& collider, const TransformComponent& transf)
		{
			switch (collider.getType())
			{
			case ColliderType::ROTATED_RECT:
				return RotatedRectConstAggregator{ collider }.getAxisAndPivots(transf);
			case ColliderType::CONVEX_POLYGON:
				return PointSetConstAggregator{ collider }.getAxisAndPivots(transf);
			default:
				return { {},{} };
			}
		}
	}
}
