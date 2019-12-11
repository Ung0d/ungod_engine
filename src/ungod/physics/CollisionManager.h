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

#ifndef UNGOD_COLLISION_MANAGER_H
#define UNGOD_COLLISION_MANAGER_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <SFML/Graphics/ConvexShape.hpp>
#include "quadtree/QuadTree.h"
#include "owls/Signal.h"
#include "ungod/physics/Collision.h"
#include "ungod/base/Entity.h"
#include "ungod/serialization/CollisionSerial.h"
#include "ungod/base/MultiComponent.h"

namespace ungod
{
	static constexpr std::size_t MAX_CONTEXTS = 2;

	template<std::size_t CONTEXT>
	class RigidbodyManager;

	/**
	* \ingroup Components
	* \brief A component that models a rigidbody, that is a collection of
	* colliders. Template argument is the context of the collision. 
	*/
	template<std::size_t CONTEXT = 0>
	class RigidbodyComponent : public Serializable<RigidbodyComponent<CONTEXT>>
	{
		static_assert(CONTEXT <= MAX_CONTEXTS, "Attempt to create too much collision contexts.");
		friend class RigidbodyManager<CONTEXT>;
	private:
		Collider mCollider;
		bool mActive;

	public:
		RigidbodyComponent() : mActive(true) {}

		const Collider& getCollider() const { return mCollider; }

		bool isActive() const { return mActive; }
	};


	/**
	* \ingroup Components
	* \brief MultiRigidbody. */
	template<std::size_t CONTEXT = 0>
	using MultiRigidbodyComponent = MultiComponent<RigidbodyComponent<CONTEXT>>;

    /** \brief A manager for operations on rigidbodies. Emits content changed signals. */
	template <std::size_t CONTEXT = 0>
    class RigidbodyManager
    {
    template <std::size_t N> friend struct ContextIteration;

    public:
        /** \brief Adds a new collider by copying to the rigidbody of entity e. */
		MULTI_COMP_METHOD_1ARG(addCollider,RigidbodyComponent<CONTEXT>, const Collider&, collider)

		/** \brief Adds and constructs inplace a new rotated rect collider. */
		MULTI_COMP_METHOD_3ARG(addRotatedRect, RigidbodyComponent<CONTEXT>, const sf::Vector2f&, upleft, const sf::Vector2f&, downright, float, rotation)

		/** \brief Adds and constructs inplace a new convex polygon collider. */
		MULTI_COMP_METHOD_1ARG(addConvexPolygon, RigidbodyComponent<CONTEXT>, const std::vector<sf::Vector2f>&, points)

		/** \brief Adds and constructs inplace a new edge chain collider. */
		MULTI_COMP_METHOD_1ARG(addEdgeChain, RigidbodyComponent<CONTEXT>, const std::vector<sf::Vector2f>&, points)

		/** \brief Adds and constructs inplace a new circle collider. */
		//MULTI_COMP_METHOD_2ARG(addCircle, RigidbodyComponent<CONTEXT>, const sf::Vector2f&, center, float, radius)
		
        /** \brief Removes a collider. */
		MULTI_COMP_METHOD_0ARG(clearCollider, RigidbodyComponent<CONTEXT>)

        /** \brief Sets the down right coordinates of a rotated rect collider. */
		MULTI_COMP_METHOD_1ARG(setRectDownRight, RigidbodyComponent<CONTEXT>, const sf::Vector2f&, downright)

        /** \brief Sets the up left coordinates of a rotated rect collider. */
		MULTI_COMP_METHOD_1ARG(setRectUpLeft, RigidbodyComponent<CONTEXT>, const sf::Vector2f&, upleft)

        /** \brief Rotates a rect collider. */
		MULTI_COMP_METHOD_1ARG(rotateRect, RigidbodyComponent<CONTEXT>, float, rotation)

		/** \brief Sets the rotation of a rect collider. */
		MULTI_COMP_METHOD_1ARG(setRectRotation, RigidbodyComponent<CONTEXT>, float, rotation)

        /** \brief Sets the ith point of a convex polygon collider or a edge-chain collider. */
		MULTI_COMP_METHOD_2ARG(setPoint, RigidbodyComponent<CONTEXT>, unsigned, i, const sf::Vector2f&, point)

		/** \brief Sets the center point of a circle collider. */
		//MULTI_COMP_METHOD_1ARG(setCircleCenter, RigidbodyComponent<CONTEXT>, const sf::Vector2f&, center)

		/** \brief Sets the radius of a circle collider. */
		//MULTI_COMP_METHOD_1ARG(setRadius, RigidbodyComponent<CONTEXT>, float, radius)

        /** \brief Activates/deactivates collision handling for the given entity. */
		MULTI_COMP_METHOD_1ARG(setActive, RigidbodyComponent<CONTEXT>, bool, active)

        /** \brief Registers new callback for the ContentsChanged signal. */
        void onContentsChanged(const std::function<void(Entity, const sf::FloatRect&)>& callback);

        /** \brief Registers new callback for the ContentsRemoved signal. */
        void onContentRemoved(const std::function<void(Entity)>& callback);
 
        /** \brief Returns the lower bound of the bounding rect around all contents of the given entity. */
        static sf::Vector2f getLowerBound(Entity e);

        /** \brief Returns the upper bound of the bounding rect around all contents of the given entity. */
		static sf::Vector2f getUpperBound(Entity e);

        /** \brief A method that moves all colliders attached to the given entity. This method is usually only
        * used internally by the transform-manager. */
		static void moveColliders(Entity e, const sf::Vector2f& vec);

    private:
        owls::Signal<Entity, const sf::FloatRect&> mContentsChangedSignal;
        owls::Signal<Entity> mContentRemoved;

        void move(Entity e, const sf::Vector2f& vec);

        void getLowerBounds(Entity e, sf::Vector2f& vec) const;

        void getUpperBounds(Entity e, sf::Vector2f& vec) const;
    };


    /**
    * \brief A manager that modifies Rigidbody-components of entities and
    * checks collisions between entities.
    * Emits collision events.
    */
    template<std::size_t CONTEXT = 0>
    class CollisionManager
    {
    static_assert( CONTEXT <= MAX_CONTEXTS, "Attempt to create too much collision contexts." );
    public:
        CollisionManager(quad::QuadTree<Entity>& quadtree);

        /**
        * \brief Checks collisions between all entities in the given list.
        */
        void checkCollisions(const std::list<Entity>& entities);

		/**
		* \brief Checks collisions between the given entity and all other entities in the same world.
		*/
		void checkCollisions(Entity e, TransformComponent t, RigidbodyComponent<CONTEXT>& r);


        /** \brief Registers new callback for the Collision signal. */
        void onCollision(const std::function<void(Entity, Entity, const sf::Vector2f&, const Collider&, const Collider&)>& callback);

        /** \brief Registers new callback for the CollisionBegin signal. */
        void onBeginCollision(const std::function<void(Entity, Entity)>& callback);

        /** \brief Registers new callback for the CollisionEnd signal. */
        void onEndCollision(const std::function<void(Entity, Entity)>& callback);

        /** \brief Checks for collision between 2 entities. Emits the given signal each time 2 colliders overlap. */
        static void entityCollision(Entity e1, Entity e2,
                                    owls::Signal<Entity, Entity, const sf::Vector2f&, const Collider&, const Collider&>& signal);
        static void entityCollision(Entity e1, Entity e2,
                                    TransformComponent t1, TransformComponent t2,
                                    RigidbodyComponent<CONTEXT>& r1,  RigidbodyComponent<CONTEXT>& r2,
                                    owls::Signal<Entity, Entity, const sf::Vector2f&, const Collider&, const Collider&>& signal);

    private:
        using CollidingEntitiesMap = std::unordered_map<Entity, std::unordered_set<Entity> >;  //maps an moveable entities to all static entities it collides with

        quad::QuadTree<Entity>* mQuadtree;
        std::array< CollidingEntitiesMap, 2 > mDoubleBuffers;
        bool mBufferActive;
        owls::Signal<Entity, Entity> mCollisionBeginSignal;
        owls::Signal<Entity, Entity, const sf::Vector2f&, const Collider&, const Collider&> mCollisionSignal;
        owls::Signal<Entity, Entity> mCollisionEndSignal;
    };
}

#endif // UNGOD_COLLISION_MANAGER_H

