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

#ifndef COLLISION_H
#define COLLISION_H

#include <SFML/Graphics/ConvexShape.hpp>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "quadtree/QuadTree.h"
#include "owls/Signal.h"
#include "ungod/base/Entity.h"
#include "ungod/physics/Physics.h"
#include "ungod/base/Transform.h"
#include "ungod/base/Logger.h"
#include "ungod/serialization/CollisionSerial.h"

namespace ungod
{
    /**
    * \brief A Collider with 3 degrees of freedom, modelled by an oriented rectangle in 2d space.
    */
    class Collider
    {
    friend class RigidbodyManager;
    public:
        Collider();
        Collider(const sf::Vector2f& upleft, const sf::Vector2f& downright, float rotation = 0);

        const sf::Vector2f& getUpleft() const;
        const sf::Vector2f& getDownright() const;

        /** \brief Returns the center of the bounding box of the polygon. */
        sf::Vector2f getCenter() const;

        /** \brief Returns the bounding box in the "default" coordinate system of the collider. */
        sf::IntRect getBoundingBox(const TransformComponent& t) const;
        sf::IntRect getBoundingBox(sf::Transform t = sf::Transform()) const;

        /** \brief Returns a polygon that is translated according to the given transform. */
        Collider translate(const TransformComponent& t) const;

        /** \brief Returns the degree of rotation of the collider. */
        float getRotation() const;

        /** \brief Moves the internal position of the collider along the given vector. */
        void move(const sf::Vector2f& vec);

    private:
        sf::Vector2f mUpleft;
        sf::Vector2f mDownright;
        float mRotation;
    };

    /** \brief A free method implementing the SAT-algorithm for
    * fast collision checkup of convex polygons. */
    std::pair<bool, sf::Vector2f> satAlgorithm( const Collider& c1, const Collider& c2, const TransformComponent& t1, const TransformComponent& t2 );


    /** \brief A free method that checks whether a given point is inside a collider.*/
    bool containsPoint(const Collider& collider, const TransformComponent& transf, const sf::Vector2f& point);


    /** \brief A manager for operations on rigidbodies. Emits content changed signals. */
    class RigidbodyManager
    {
    template <std::size_t N> friend struct ContextIteration;
    public:
        static constexpr std::size_t MAX_CONTEXTS = 2;

    public:
        /** \brief Adds a new collider to the rigidbody of entity e. */
        template <std::size_t CONTEXT = 0>
        void addCollider(Entity e, const Collider& collider, int index = -1);
        template <std::size_t CONTEXT = 0>
        void addCollider(Entity e, const sf::Vector2f& upleft, const sf::Vector2f& downright, float rotation = 0, int index = -1);

        /** \brief Removes the collider with given index. */
        template <std::size_t CONTEXT = 0>
        void removeCollider(Entity e, std::size_t index);

        /** \brief Sets a new collider for entity e with given index. */
        template <std::size_t CONTEXT = 0>
        void modifyCollider(Entity e, std::size_t index, const Collider& collider);

        /** \brief Sets the down right coordinates of the collider with given index. */
        template <std::size_t CONTEXT = 0>
        void setColliderDownRight(Entity e, std::size_t index, const sf::Vector2f& downright);

        /** \brief Sets the up left coordinates of the collider with given index. */
        template <std::size_t CONTEXT = 0>
        void setColliderUpLeft(Entity e, std::size_t index, const sf::Vector2f& upleft);

        /** \brief Rotates the collider with given index. */
        template <std::size_t CONTEXT = 0>
        void rotateCollider(Entity e, std::size_t index, float rotation);

        /** \brief Rotates the collider with given index. */
        template <std::size_t CONTEXT = 0>
        void setColliderRotation(Entity e, std::size_t index, float rotation);

        /** \brief Activates/deactivates collision handling for the given entity. */
        template <std::size_t CONTEXT>
        void setActive(Entity e, bool active);

        /** \brief Registers new callback for the ContentsChanged signal. */
        void onContentsChanged(const std::function<void(Entity, const sf::IntRect&)>& callback);

        /** \brief Registers new callback for the ContentsRemoved signal. */
        void onContentRemoved(const std::function<void(Entity)>& callback);

        /** \brief Returns the lower bound of the bounding rect around all contents of the given entity. */
        sf::Vector2f getLowerBound(Entity e);

        /** \brief Returns the upper bound of the bounding rect around all contents of the given entity. */
        sf::Vector2f getUpperBound(Entity e);

        /** \brief A method that moves all colliders attached to the given entity. This method is usually only
        * used internally by the transform-manager. */
        void moveColliders(Entity e, const sf::Vector2f& vec);

    private:
        owls::Signal<Entity, const sf::IntRect&> mContentsChangedSignal;
        owls::Signal<Entity> mContentRemoved;

        template <std::size_t CONTEXT>
        void moveContext(Entity e, const sf::Vector2f& vec);

        template <std::size_t CONTEXT>
        void contextLowerBounds(Entity e, sf::Vector2f& vec) const;

        template <std::size_t CONTEXT>
        void contextUpperBounds(Entity e, sf::Vector2f& vec) const;
    };

    /**
    * \ingroup Components
    * \brief A component that models a rigidbody, that is a collection of
    * colliders. Template argument is the context of the collision. If you
    */
    template<std::size_t CONTEXT = 0>
    class RigidbodyComponent : public Serializable<RigidbodyComponent<CONTEXT>>
    {
    static_assert( CONTEXT <= RigidbodyManager::MAX_CONTEXTS, "Attempt to create too much collision contexts." );
    friend class RigidbodyManager;
     friend struct SerialBehavior<RigidbodyComponent<CONTEXT>, Entity, const World&, const Application&>;
    private:
        std::vector< Collider > mColliders;
        bool mActive;

    public:
        RigidbodyComponent() : mActive(true) {}

        const std::vector< Collider >& getColliders() const;

        bool isActive() const { return mActive; }
    };


    /**
    * \brief A manager that modifies Rigidbody-components of entities and
    * checks collisions between entities.
    * Emits collision events.
    */
    template<std::size_t CONTEXT = 0>
    class CollisionManager
    {
    static_assert( CONTEXT <= RigidbodyManager::MAX_CONTEXTS, "Attempt to create too much collision contexts." );
    public:
        CollisionManager(quad::QuadTree<Entity>& quadtree);

        /**
        * \brief Checks collisions in the given area of the world. That means,
        * it checks collisions for all entities in that area and emit
        * collision signals of detection.
        */
        void checkCollisions(const std::list<Entity>& entities);


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

#endif // COLLISION_H
