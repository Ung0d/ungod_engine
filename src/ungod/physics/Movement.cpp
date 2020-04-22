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

#include "ungod/physics/Movement.h"
#include "ungod/physics/Physics.h"
#include "ungod/base/Transform.h"
#include "ungod/base/Utility.h"
#include "ungod/physics/CollisionContexts.h"

namespace ungod
{
    MovementComponent::MovementComponent() :
        mMaxForce(MovementManager::sMaxForce),
        mMaxVelocity(MovementManager::sMaxVelocity),
        mBaseSpeed(MovementManager::sBaseSpeed) {}


    const sf::Vector2f& MovementComponent::getVelocity() const
    {
        return mMobilityUnit.velocity;
    }

    const sf::Vector2f& MovementComponent::getAcceleration() const
    {
        return mMobilityUnit.acceleration;
    }


    MovementComponent::Direction MovementComponent::getDirection() const
    {
        return mDirection;
    }

    float  MovementComponent::getMaxVelocity() const
    {
        return mMaxVelocity;
    }

    float  MovementComponent::getBaseSpeed() const
    {
        return mBaseSpeed;
    }

    float  MovementComponent::getMaxForce() const
    {
        return mMaxForce;
    }



    MovementManager::MovementManager(quad::QuadTree<Entity>& quadtree,
                                     TransformManager& transformer) :
                                         mQuadtree(&quadtree), mTransformer(&transformer),
                                         mBeginMovingSignal(), mEndMovingSignal() {}


    void MovementManager::update(const std::list<Entity>& entities, float delta)
    {
        dom::Utility<Entity>::iterate<TransformComponent, MovementComponent>(entities,
          [delta, this] (Entity e, TransformComponent& transf, MovementComponent& movement)
          {
                bool currentlyMoving = isMoving(movement.mMobilityUnit);

                mobilize(movement.mMobilityUnit, movement.getMaxForce(), movement.getMaxVelocity());

                bool nowMoving = isMoving(movement.mMobilityUnit);

                ungod::resetAcceleration(movement.mMobilityUnit);

                if (nowMoving)
                {
                    //move according to the velocity
                    mTransformer->move( e, delta * movement.getBaseSpeed() * movement.getVelocity() );
                }

                //set direction enum
                MovementComponent::Direction newDirection;

                if (nowMoving)
                {
                    if (-movement.getVelocity().y >= std::abs(movement.getVelocity().x) )
                        newDirection = MovementComponent::Direction::UP;
                    else if (movement.getVelocity().x >= std::abs(movement.getVelocity().y))
                        newDirection = MovementComponent::Direction::RIGHT;
                    else if (movement.getVelocity().y >= std::abs(movement.getVelocity().x) )
                        newDirection = MovementComponent::Direction::DOWN;
                    else
                        newDirection = MovementComponent::Direction::LEFT;
                }
                else
                {
                    newDirection = MovementComponent::Direction::IDLE;
                }

                MovementComponent::Direction oldDirection = movement.mDirection;
                movement.mDirection = newDirection;
                if (oldDirection != newDirection)
                {
                    mDirectionChangedSignal(e, oldDirection, newDirection);
                }

                //eventually send out signals
                if (!currentlyMoving && nowMoving)
                    mBeginMovingSignal(e, movement.getVelocity());
                if (currentlyMoving && !nowMoving)
                    mEndMovingSignal(e);
          });
    }


    void MovementManager::accelerate(Entity e, const sf::Vector2f& acceleration)
    {
        ungod::accelerate(e.modify<MovementComponent>().mMobilityUnit, acceleration, 1.0f);
    }


    void MovementManager::resetAcceleration(Entity e)
    {
        ungod::resetAcceleration(e.modify<MovementComponent>().mMobilityUnit);
    }


    void MovementManager::seek(Entity e, const sf::Vector2f& targetPos, float speed)
    {
        ungod::seek(e.modify<MovementComponent>().mMobilityUnit, e.get<TransformComponent>().getCenterPosition(), targetPos, speed);
    }


    void MovementManager::flee(Entity e, const sf::Vector2f& targetPos, float speed)
    {
        ungod::flee(e.modify<MovementComponent>().mMobilityUnit, e.get<TransformComponent>().getCenterPosition(), targetPos, speed);
    }


    void MovementManager::arrival(Entity e, const sf::Vector2f& targetPos, float speed, const int arrivalRadius)
    {
        ungod::arrival(e.modify<MovementComponent>().mMobilityUnit, e.get<TransformComponent>().getCenterPosition(), targetPos, speed, (float)arrivalRadius);
    }


    void MovementManager::pursuit(Entity e, Entity target,
                                 float speed, unsigned arrivalRadius, float damp)
    {
        ungod::pursuit(e.modify<MovementComponent>().mMobilityUnit, target.modify<MovementComponent>().mMobilityUnit,
                e.get<TransformComponent>().getCenterPosition(), target.get<TransformComponent>().getCenterPosition(),
                target.modify<MovementComponent>().getMaxVelocity(), speed, (float)arrivalRadius, damp);
    }


    void MovementManager::evade(Entity e, Entity target,
                               float speed, float evadeDistance, float damp)
    {
        ungod::evade(e.modify<MovementComponent>().mMobilityUnit, target.modify<MovementComponent>().mMobilityUnit,
             e.get<TransformComponent>().getCenterPosition(), target.get<TransformComponent>().getCenterPosition(),
             target.modify<MovementComponent>().getMaxVelocity(), speed, evadeDistance, damp);
    }


    void MovementManager::stop(Entity e)
    {
        ungod::stop(e.modify<MovementComponent>().mMobilityUnit);
    }


    void MovementManager::displace(Entity e, float speed, float circleDistance, float angleRange)
    {
        ungod::displace(e.modify<MovementComponent>().mMobilityUnit, speed, circleDistance, angleRange);
    }


    void MovementManager::avoidObstacles(Entity e, float avoidStrength, float maxSight)
    {
        /*sf::Vector2f sightAhead;
        sightAhead.x = transf.getPosition().x + currentVelocity.x * maxSight / movement.getMaximumVelocity();
        sightAhead.y = transf.getPosition().y + currentVelocity.y * maxSight / movement.getMaximumVelocity();

        quad::PullResult<Entity> result;
        mQuadtree.retrieve(result, transf.getPosition(), transf.getSize());
        mQuadtree.retrieve(result, sightAhead, transf.getSize());

        Entity obstacle = Entity::null();

        float distToCurrentObstacle = std::numeric_limits<float>::max();

        while(!result.done())
        {
            Entity entity = result.poll();
            if(&entity.get<Transform>() != &transf &&
               entity.isSolid() &&
               entity.isStatic() &&
               entity.get<collisionWith(, LineCollider(transf.getPosition(), sightAhead)))
            {
                float curDist = distance(entity->getCenterPosition(), transf.getPosition());
                 if(!_obstacle || _dist_to_current_obstacle > curDist)
                 {
                     _obstacle = entity;
                     _dist_to_current_obstacle = curDist;
                 }
            }
        }

        if(obstacle)
        {
            sf::Vector2f _avoidance_force;
            _avoidance_force.x = _sight_ahead.x - obstacle->getCenterPosition().x;
            _avoidance_force.y = _sight_ahead.y - obstacle->getCenterPosition().y;
            normalize(_avoidance_force);
            _avoidance_force = avoidStrength*_avoidance_force;
            steeringForce.x += _avoidance_force.x;
            steeringForce.y += _avoidance_force.y;
        }*/
    }

    void MovementManager::directMovement(Entity e, const sf::Vector2f& vec)
    {
        accelerate( e, vec );
    }

    void MovementManager::slowDown(Entity e, float strength)
    {

    }

    void MovementManager::onBeginMovement(const std::function<void(Entity, const sf::Vector2f&)>& callback)
    {
        mBeginMovingSignal.connect(callback);
    }

    void MovementManager::onEndMovement(const std::function<void(Entity)>& callback)
    {
        mEndMovingSignal.connect(callback);
    }

    void MovementManager::onDirectionChanged(const std::function<void(Entity, MovementComponent::Direction, MovementComponent::Direction)>& callback)
    {
        mDirectionChangedSignal.connect(callback);
    }

    void MovementManager::handleCollision(Entity e, Entity other, const sf::Vector2f& vec,
                                          const Collider&, const Collider&)
    {
        mTransformer->move(e, vec);
    }

    void MovementManager::setMaximumForce(Entity e, float maxForce)
    {
        e.modify<MovementComponent>().mMaxForce = maxForce;
    }

    void MovementManager::setMaximumVelocity(Entity e, float maxVelocity)
    {
        e.modify<MovementComponent>().mMaxVelocity = maxVelocity;
    }

    void MovementManager::setBaseSpeed(Entity e, float baseSpeed)
    {
        e.modify<MovementComponent>().mBaseSpeed = baseSpeed;
    }
}
