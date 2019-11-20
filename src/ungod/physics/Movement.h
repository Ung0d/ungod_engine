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

#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <SFML/System/Vector2.hpp>
#include "owls/Signal.h"
#include "ungod/base/Entity.h"
#include "ungod/physics/Path.h"
#include "ungod/physics/MobilityUnit.h"

namespace ungod
{
    class TransformComponent;
    class TransformManager;
    class Collider;

    /** \brief
    * \ingroup Components
    * A Component that models the ability to move, that is altering the Position
    * in the Transform component of an entity.
    */
    class MovementComponent : public ungod::Serializable<MovementComponent>
    {
    friend class MovementManager;
     friend struct SerialBehavior<MovementComponent, Entity, const World&, const Application&>;
    friend struct DeserialBehavior<MovementComponent, Entity, World&, const Application&>;
    public:
        enum class Direction { IDLE,
                               UP, DOWN, LEFT, RIGHT };
    public:

        MovementComponent();

        /** \brief Returns the current velocity vector. */
        const sf::Vector2f& getVelocity() const;

        /** \brief Returns the current acceleration. */
        const sf::Vector2f& getAcceleration() const;

        /** \brief Returns the current movement direction of the entity. */
        Direction getDirection() const;

        /** \brief Returns the maximum possible velocity the entity can move. */
        float getMaxVelocity() const;

        /** \brief Returns the base speed multiplier of the entity. */
        float getBaseSpeed() const;

        /** \brief Returns the maximum possible force the entity can be affected by. */
        float getMaxForce() const;

    private:
        MobilityUnit mMobilityUnit;
        Direction mDirection;
        float mMaxForce;
        float mMaxVelocity;
        float mBaseSpeed;
    };

    /** \brief Manager class for all movement actions. Emits signals if
    * entities start or end moving processes. */
    class MovementManager
    {
    public:
        MovementManager(quad::QuadTree<Entity>& quadtree,
                        TransformManager& transformer);

        /** \brief Performs movements for the given period of deltatime. */
        void update(const std::list<Entity>& entities, float delta);

        /** \brief Accelerates e. */
        void accelerate(Entity e, const sf::Vector2f& acceleration);

        /** \brief Resets the acceleration vector of e to {0,0}. */
        void resetAcceleration(Entity e);

        /** \brief The entity moves towards the target point. */
        void seek(Entity e, const sf::Vector2f& targetPos, float speed);

        /** \brief The entity flees away from the given target. */
        void flee(Entity e, const sf::Vector2f& targetPos, float speed);

        /** \brief The entity moves towards the target position and will slow down to 0 if it is near enough. */
        void arrival(Entity e, const sf::Vector2f& targetPos, float speed, const int arrivalRadius);

        /**
        * \brief The entity predicts the given target entity's position in the near future and seeks this
        * position with the aim to catch the entity.
        */
        void pursuit(Entity e, Entity target,
                     float speed, unsigned arrivalRadius, float damp);

        /** \brief The character trys to stay far from the given character bewaring a given distance. */
        void evade(Entity e, Entity target,
                   float speed, float evadeDistance, float damp);

        /** \brief Break movement. Wramp down velocity to 0.*/
        void stop(Entity e);

        /**
        * \brief Wander around randomly but with soft direction, changes. Adds small displacements to the characters
        * velocity every frame.
        */
        void displace(Entity e, float speed, float circleDistance, float angleRange);

        /** \brief Avoids collision with other entities.*/
        void avoidObstacles(Entity e, float avoidStrength, float maxSight);

        void directMovement(Entity e, const sf::Vector2f& vec);

        void slowDown(Entity e, float strength);


        /** \brief Registers new callback for the BeginMovement signal. */
        void onBeginMovement(const std::function<void(Entity, const sf::Vector2f&)>& callback);

        /** \brief Registers new callback for the EndMovement signal. */
        void onEndMovement(const std::function<void(Entity)>& callback);

        /** \brief Registers new callback for the DirectionChanged signal. */
        void onDirectionChanged(const std::function<void(Entity, MovementComponent::Direction, MovementComponent::Direction)>& callback);

        /** \brief Collision reaction callback. */
        void handleCollision(Entity e, Entity other, const sf::Vector2f& vec, const Collider&, const Collider&);

        /** \brief Sets the maximum force value. */
        void setMaximumForce(Entity e, float maxForce);

        /** \brief Sets the maximum velocity value for the given entity. */
        void setMaximumVelocity(Entity e, float maxVelocity);

        /** \brief Sets the base speed. */
        void setBaseSpeed(Entity e, float baseSpeed);

    private:
        quad::QuadTree<Entity>* mQuadtree;
        TransformManager* mTransformer;

        //signals
        owls::Signal<Entity, const sf::Vector2f&> mBeginMovingSignal;
        owls::Signal<Entity> mEndMovingSignal;
        owls::Signal<Entity, MovementComponent::Direction, MovementComponent::Direction> mDirectionChangedSignal;

    public:
        static constexpr float sBaseSpeed = 0.2f;
        static constexpr float sMaxForce = 1.0f;
        static constexpr float sMaxVelocity = 1.0f;
    };
}

#endif // MOVEMENT_H
