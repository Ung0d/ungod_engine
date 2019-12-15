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

#ifndef UNGOD_COLLISION_H
#define UNGOD_COLLISION_H

#include "ungod/physics/Physics.h"
#include "ungod/physics/Collider.h"
#include "ungod/base/Transform.h"

namespace ungod
{
    /** \brief Checks collision between two polygons of arbitrary type. */
    std::pair<bool, sf::Vector2f> doCollide( const Collider& c1, const Collider& c2, const TransformComponent& t1, const TransformComponent& t2 );

	/** \brief Checks for overlap given axis to check and two point sets to check on each axis. */
	std::pair<bool, sf::Vector2f> satAlgorithm(const std::vector<sf::Vector2f>& axis, 
									const std::vector<sf::Vector2f>& pivots1, const std::vector<sf::Vector2f>& pivots2);

	/** \brief A free method that checks whether a given point is inside a collider of arbitrary type.*/
    bool containsPoint(const Collider& collider, const TransformComponent& transf, const sf::Vector2f& point);

	/** \brief A free method that checks whether a given point is inside a collider if it is already known that the
	* collider is a rotated rectangle.*/
	bool rotatedRectContainsPoint(const Collider& collider, const TransformComponent& transf, const sf::Vector2f& point);

	/** \brief A free method that checks whether a given point is inside a collider if it is already known that the
	* collider is a convex polygon.*/
	bool convexPolygonContainsPoint(const Collider& collider, const TransformComponent& transf, const sf::Vector2f& point);

	/** \brief A free method that checks whether a given point is inside a collider if it is already known that the
	* collider is a edge shape. In fact, this method checks whether the distance of a point is smaller than a hardcoded constant. */
	bool edgeChainContainsPoint(const Collider& collider, const TransformComponent& transf, const sf::Vector2f& point);

	/** \brief A free method that checks whether a given point is inside a collider if it is already known that the
	* collider is a circle. */
	//bool circleContainsPoint(const Collider& collider, const TransformComponent& transf, const sf::Vector2f& point);
}

#endif // UNGOD_COLLISION_H
