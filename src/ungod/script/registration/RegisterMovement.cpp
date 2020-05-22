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

#include "ungod/script/registration/RegisterMovement.h"

namespace ungod
{
    namespace scriptRegistration
    {
        const sf::Vector2f& MovementHandlerFrontEnd::getVelocity() const
        {
           return mEntity.get<MovementComponent>().getVelocity();
        }

        const sf::Vector2f& MovementHandlerFrontEnd::getAcceleration() const
        {
            return mEntity.get<MovementComponent>().getAcceleration();
        }

        MovementComponent::Direction MovementHandlerFrontEnd::getDirection() const
        {
            return mEntity.get<MovementComponent>().getDirection();
        }

        float MovementHandlerFrontEnd::getMaxVelocity() const
        {
            return mEntity.get<MovementComponent>().getMaxVelocity();
        }

        float MovementHandlerFrontEnd::getMaxForce() const
        {
            return mEntity.get<MovementComponent>().getMaxForce();
        }

        float MovementHandlerFrontEnd::getBaseSpeed() const
        {
            return mEntity.get<MovementComponent>().getBaseSpeed();
        }

        void MovementHandlerFrontEnd::setMaximumVelocity(float maxvel)
        {
            mHandler.setMaximumVelocity(mEntity, maxvel);
        }

        void MovementHandlerFrontEnd::setBaseSpeed(float basespeed)
        {
            mHandler.setBaseSpeed(mEntity, basespeed);
        }

        void MovementHandlerFrontEnd::setMaximumForce(float maxforce)
        {
            mHandler.setMaximumForce(mEntity, maxforce);
        }

        void MovementHandlerFrontEnd::accelerate(const sf::Vector2f& acceleration)
        {
            mHandler.accelerate(mEntity, acceleration);
        }

        void MovementHandlerFrontEnd::resetAcceleration()
        {
            mHandler.resetAcceleration(mEntity);
        }

        void MovementHandlerFrontEnd::flee(const sf::Vector2f& targetPos, float speed)
        {
            mHandler.flee(mEntity, targetPos, speed);
        }

        void MovementHandlerFrontEnd::arrival(const sf::Vector2f& targetPos, float speed, int arrivalRadius)
        {
            mHandler.arrival(mEntity, targetPos, speed, arrivalRadius);
        }

        void MovementHandlerFrontEnd::pursuit(Entity target, float speed, unsigned arrivalRadius, float damp)
        {
            mHandler.pursuit(mEntity, target, speed, arrivalRadius, damp);
        }

        void MovementHandlerFrontEnd::evade(Entity target, float speed, float circleDistance, float damp)
        {
            mHandler.evade(mEntity, target, speed, circleDistance, damp);
        }

        void MovementHandlerFrontEnd::stop()
        {
            mHandler.stop(mEntity);
        }

        void MovementHandlerFrontEnd::displace(float speed, float circleDistance, float angleRange)
        {
            mHandler.displace(mEntity, speed, circleDistance, angleRange);
        }

        void MovementHandlerFrontEnd::avoidObstacles(float avoidStrength, float maxSight)
        {
            mHandler.avoidObstacles(mEntity, avoidStrength, maxSight);
        }

        void MovementHandlerFrontEnd::directMovement(const sf::Vector2f& vec)
        {
            mHandler.directMovement(mEntity, vec);
        }

        void MovementHandlerFrontEnd::slowDown(float strength)
        {
            mHandler.slowDown(mEntity, strength);
        }


        void PathPlannerFrontEnd::setPath(script::Environment points)
        {
            mPathPlanner.setPath(mEntity, env2vec<sf::Vector2f>(points));
        }

        void PathPlannerFrontEnd::setPath(script::Environment points, PathFollowingPolicy policy, float speed, float radius)
        {
            mPathPlanner.setPath(mEntity, env2vec<sf::Vector2f>(points), policy, speed, radius);
        }


        void registerMovement(ScriptStateBase& state)
        {
            state.registerEnum< MovementComponent::Direction>("Direction",
				{ {"idle", MovementComponent::Direction::IDLE},
				{"left", MovementComponent::Direction::LEFT},
				{"right", MovementComponent::Direction::RIGHT},
				{"up", MovementComponent::Direction::UP},
				{"down", MovementComponent::Direction::DOWN}});

			script::Usertype<MovementHandlerFrontEnd> movementHandlerType = state.registerUsertype<MovementHandlerFrontEnd>("MovementHandlerFrontEnd");
            movementHandlerType["getVelocity"] = &MovementHandlerFrontEnd::getVelocity;
            movementHandlerType["getAcceleration"] = &MovementHandlerFrontEnd::getAcceleration;
            movementHandlerType["getDirection"] = &MovementHandlerFrontEnd::getDirection;
            movementHandlerType["getMaxVelocity"] = &MovementHandlerFrontEnd::getMaxVelocity;
            movementHandlerType["getMaxForce"] = &MovementHandlerFrontEnd::getMaxForce;
            movementHandlerType["getBaseSpeed"] = &MovementHandlerFrontEnd::getBaseSpeed;
            movementHandlerType["setMaximumVelocity"] = &MovementHandlerFrontEnd::setMaximumVelocity;
            movementHandlerType["setBaseSpeed"] = &MovementHandlerFrontEnd::setBaseSpeed;
            movementHandlerType["setMaximumForce"] = &MovementHandlerFrontEnd::setMaximumForce;
            movementHandlerType["accelerate"] = &MovementHandlerFrontEnd::accelerate;
            movementHandlerType["resetAcceleration"] = &MovementHandlerFrontEnd::resetAcceleration;
            movementHandlerType["flee"] = &MovementHandlerFrontEnd::flee;
            movementHandlerType["arrival"] = &MovementHandlerFrontEnd::arrival;
            movementHandlerType["pursuit"] = &MovementHandlerFrontEnd::pursuit;
            movementHandlerType["evade"] = &MovementHandlerFrontEnd::evade;
            movementHandlerType["stop"] = &MovementHandlerFrontEnd::stop;
            movementHandlerType["displace"] = &MovementHandlerFrontEnd::displace;
            movementHandlerType["avoidObstacles"] = &MovementHandlerFrontEnd::avoidObstacles;
            movementHandlerType["directMovement"] = &MovementHandlerFrontEnd::directMovement;
            movementHandlerType["slowDown"] = &MovementHandlerFrontEnd::slowDown;

            state.registerUsertype<SteeringPattern<script::Environment>>("SteeringPattern");

            script::Usertype<SteeringHandlerFrontEnd<script::Environment>> steeringHandlerType = state.registerUsertype<SteeringHandlerFrontEnd<script::Environment>>("SteeringHandlerFrontEnd");
            steeringHandlerType["disconnectPattern"] = &SteeringHandlerFrontEnd<script::Environment>::disconnectPattern;
            steeringHandlerType["connectPattern"] = sol::overload([] (SteeringHandlerFrontEnd<script::Environment>& sh, const std::string& key, const script::Environment& param)
                    { sh.connectPattern(key, param); },
                    [] (SteeringHandlerFrontEnd<script::Environment>& sh, SteeringPattern<script::Environment>* s, const script::Environment& param)
                    { sh.connectPattern(s, param); });
            steeringHandlerType["setActive"] = &SteeringHandlerFrontEnd<script::Environment>::setActive;
            steeringHandlerType["isActive"] = &SteeringHandlerFrontEnd<script::Environment>::isActive;

            script::Usertype<SteeringManager<script::Environment>> steeringManagerType = state.registerUsertype<SteeringManager<script::Environment>>("SteeringManager");
            steeringManagerType["newPattern"] = &SteeringManager<script::Environment>::newPattern;
            steeringManagerType["getPattern"] = &SteeringManager<script::Environment>::getPattern;
            steeringManagerType["removePattern"] = sol::overload([](SteeringManager<script::Environment>& sm, const std::string& key) { sm.removePattern(key); },
                [](SteeringManager<script::Environment>& sm, SteeringPattern<script::Environment>* s) { sm.removePattern(s); });
            steeringManagerType["attachSteering"] = sol::overload([] (SteeringManager<script::Environment>& sm, const std::string& key, const SteeringFunc<script::Environment>& steering)
                                                                    { sm.attachSteering(key, steering); },
                                                                [] (SteeringManager<script::Environment>& sm, SteeringPattern<script::Environment>* s, const SteeringFunc<script::Environment>& steering) 
                                                                    { sm.attachSteering(s, steering); });


            state.registerEnum< PathFollowingPolicy>("PathFollowingPolicy",
				{ {"OneShot", PathFollowingPolicy::ONE_SHOT},
				{"Cycle", PathFollowingPolicy::CYCLE},
				{"Patrol", PathFollowingPolicy::PATROL} });

			script::Usertype<PathPlannerFrontEnd> pathplannerType = state.registerUsertype<PathPlannerFrontEnd>("PathPlannerFrontEnd");
			pathplannerType["setPath"] = sol::overload(
                                            [] (PathPlannerFrontEnd& pathplanner, Entity e, script::Environment points)
                                            { pathplanner.setPath(points); },
                                            [] (PathPlannerFrontEnd& pathplanner, Entity e, script::Environment points, PathFollowingPolicy policy, float speed, float radius)
                                            { pathplanner.setPath(points, policy, speed, radius); });


            //steerings are added as a fixed-parameter and a modificable-parameter version
            //if the user wants to modify the given parameters (for example a seek behavior where the target position is
            //constantly changing), he can provide a parameter-environment, which must specify all required parameters
            state.registerFunction("displace",
                                       [] () -> std::function< void(Entity, MovementHandler&, const script::Environment&) >
                                       {
                                            return [] (Entity e, MovementHandler& mvh, const script::Environment& parameters)
                                            {
                                                mvh.displace( e,
                                                            detail::unpackParameter<float>(parameters, "speed"),
                                                            detail::unpackParameter<float>(parameters, "circleDistance"),
                                                            detail::unpackParameter<float>(parameters, "angleRange"));
                                            };
                                       });

            state.registerFunction("evade",
                                       [] () -> std::function< void(Entity, MovementHandler&, const script::Environment&) >
                                       {
                                            return [] (Entity e, MovementHandler& mvh, const script::Environment& parameters)
                                            {
                                                mvh.evade( e,
                                                            detail::unpackParameter<Entity>(parameters, "target"),
                                                            detail::unpackParameter<float>(parameters, "speed"),
                                                            detail::unpackParameter<float>(parameters, "distance"),
                                                            detail::unpackParameter<float>(parameters, "damp"));
                                            };
                                       });

            state.registerFunction("pursuit",
                                    []() -> std::function< void(Entity, MovementHandler&, const script::Environment&) >
                                    {
                                        return [](Entity e, MovementHandler& mvh, const script::Environment& parameters)
                                        {
                                            mvh.pursuit(e,
                                                detail::unpackParameter<Entity>(parameters, "target"),
                                                detail::unpackParameter<float>(parameters, "speed"),
                                                detail::unpackParameter<float>(parameters, "arrivalRadius"),
                                                detail::unpackParameter<float>(parameters, "damp"));
                                        };
                                    });

            state.registerFunction("flee",
                                       [] () -> std::function< void(Entity, MovementHandler&, const script::Environment&) >
                                       {
                                            return [] (Entity e, MovementHandler& mvh, const script::Environment& parameters)
                                            {
                                                mvh.flee( e,
                                                            detail::unpackParameter<sf::Vector2f>(parameters, "position"),
                                                            detail::unpackParameter<float>(parameters, "speed"));
                                            };
                                       });


            state.registerFunction("arrival",
                                       [] () -> std::function< void(Entity, MovementHandler&, const script::Environment&) >
                                       {
                                            return [] (Entity e, MovementHandler& mvh, const script::Environment& parameters)
                                            {
                                                mvh.arrival( e,
                                                            detail::unpackParameter<sf::Vector2f>(parameters, "position"),
                                                            detail::unpackParameter<float>(parameters, "speed"),
                                                            detail::unpackParameter<float>(parameters, "radius"));
                                            };
                                       });


            state.registerFunction("seek",
                                       [] () -> std::function< void(Entity, MovementHandler&, const script::Environment&) >
                                       {
                                            return [] (Entity e, MovementHandler& mvh, const script::Environment& parameters)
                                            {
                                                mvh.seek( e, detail::unpackParameter<sf::Vector2f>(parameters, "position"), detail::unpackParameter<float>(parameters, "speed"));
                                            };
                                       });


            state.registerFunction("directMovement",
                                       [] () -> std::function< void(Entity, MovementHandler&, const script::Environment&) >
                                       {
                                            return [] (Entity e, MovementHandler& mvh, const script::Environment& parameters)
                                            {
                                                mvh.directMovement( e, detail::unpackParameter<sf::Vector2f>(parameters, "vec"));
                                            };
                                       });
        }
    }
}
