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
#include "ungod/physics/Path.h"
#include "ungod/physics/Steering.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerMovement(ScriptStateBase& state)
        {
            state.registerEnum("Direction",
                                     "idle", MovementComponent::Direction::IDLE,
                                     "left", MovementComponent::Direction::LEFT,
                                     "right", MovementComponent::Direction::RIGHT,
                                     "up", MovementComponent::Direction::UP,
                                     "down", MovementComponent::Direction::DOWN);

            state.registerUsertype<MovementComponent>("Movement",
                                                   "getVelocity", &MovementComponent::getVelocity,
                                                   "getAcceleration", &MovementComponent::getAcceleration,
                                                   "getDirection", &MovementComponent::getDirection,
                                                   "getMaxVelocity", &MovementComponent::getMaxVelocity,
                                                   "getMaxForce", &MovementComponent::getMaxForce,
                                                   "getBaseSpeed", &MovementComponent::getBaseSpeed);

            state.registerUsertype<MovementManager>("MovementManager"/*,
                                                          "setMaximumVelocity", &MovementManager::setMaximumVelocity,
                                                          "setBaseSpeed", &MovementManager::setBaseSpeed,
                                                          "setMaximumForce", &MovementManager::setMaximumForce,
                                                          "accelerate", &MovementManager::accelerate,
                                                          "resetAcceleration", &MovementManager::resetAcceleration,
                                                          "flee", &MovementManager::flee,
                                                          "arrival", &MovementManager::arrival,
                                                          "pursuit", &MovementManager::pursuit,
                                                          "evade", &MovementManager::evade,
                                                          "stop", &MovementManager::stop,
                                                          "displace", &MovementManager::displace,
                                                          "avoidObstacles", &MovementManager::avoidObstacles,
                                                          "directMovement", &MovementManager::directMovement,
                                                          "slowDown", &MovementManager::slowDown,
                                                          "resetAcceleration", &MovementManager::resetAcceleration*/
                                                          );

            state.registerUsertype<SteeringComponent<script::Environment>>("Steering",
                                                   "isActive", &SteeringComponent<script::Environment>::isActive);

            state.registerUsertype<SteeringPattern<script::Environment>>("SteeringPattern");

            state.registerUsertype<SteeringManager<script::Environment>>("SteeringManager"/*,
                                                   "setActive", &SteeringManager<script::Environment>::setActive,
                                                   "newPattern", &SteeringManager<script::Environment>::newPattern,
                                                   "getPattern", &SteeringManager<script::Environment>::getPattern,
                                                   "disconnectPattern", &SteeringManager<script::Environment>::disconnectPattern,
                                                   "connectPattern", sol::overload([] (SteeringManager<script::Environment>& sm, Entity e, const std::string& key, const script::Environment& param)
                                                                                   { sm.connectPattern(e, key, param); },
                                                                                   [] (SteeringManager<script::Environment>& sm, Entity e, SteeringPattern<script::Environment>* s, const script::Environment& param)
                                                                                   { sm.connectPattern(e, s, param); }),
                                                   "removePattern", sol::overload([] (SteeringManager<script::Environment>& sm, const std::string& key) { sm.removePattern(key); },
                                                                                   [] (SteeringManager<script::Environment>& sm, SteeringPattern<script::Environment>* s) { sm.removePattern(s); }),
                                                   "attachSteering", sol::overload([] (SteeringManager<script::Environment>& sm, const std::string& key, const SteeringFunc<script::Environment>& steering) { sm.attachSteering(key, steering); },
                                                                                   [] (SteeringManager<script::Environment>& sm, SteeringPattern<script::Environment>* s, const SteeringFunc<script::Environment>& steering) { sm.attachSteering(s, steering); })*/);


            state.registerEnum("PathFollowingPolicy",
                               "OneShot", PathFollowingPolicy::ONE_SHOT,
                               "Cycle", PathFollowingPolicy::CYCLE,
                               "Patrol", PathFollowingPolicy::PATROL);

            state.registerUsertype<PathPlanner>("PathPlanner",
                                                "setPath", sol::overload(
                                                [] (PathPlanner& pathplanner, Entity e, script::Environment points)
                                                { pathplanner.setPath(e, env2vec<sf::Vector2f>(points)); },
                                                [] (PathPlanner& pathplanner, Entity e, script::Environment points, PathFollowingPolicy policy, float speed, float radius)
                                                { pathplanner.setPath(e, env2vec<sf::Vector2f>(points), policy, speed, radius); }));


            //steerings are added as a fixed-parameter and a modificable-parameter version
            //if the user wants to modify the given parameters (for example a seek behavior where the target position is
            //constantly changing), he can provide a parameter-environment, which must specify all required parameters
            state.registerFunction("displace",
                                       [] () -> std::function< void(Entity, MovementManager&, const script::Environment&) >
                                       {
                                            return [] (Entity e, MovementManager& mvm, const script::Environment& parameters)
                                            {
                                                mvm.displace( e,
                                                            detail::unpackParameter<float>(parameters, "speed"),
                                                            detail::unpackParameter<float>(parameters, "circleDistance"),
                                                            detail::unpackParameter<float>(parameters, "angleRange"));
                                            };
                                       });

            state.registerFunction("evade",
                                       [] () -> std::function< void(Entity, MovementManager&, const script::Environment&) >
                                       {
                                            return [] (Entity e, MovementManager& mvm, const script::Environment& parameters)
                                            {
                                                mvm.evade( e,
                                                            detail::unpackParameter<Entity>(parameters, "target"),
                                                            detail::unpackParameter<float>(parameters, "speed"),
                                                            detail::unpackParameter<float>(parameters, "distance"),
                                                            detail::unpackParameter<float>(parameters, "damp"));
                                            };
                                       });

            state.registerFunction("flee",
                                       [] () -> std::function< void(Entity, MovementManager&, const script::Environment&) >
                                       {
                                            return [] (Entity e, MovementManager& mvm, const script::Environment& parameters)
                                            {
                                                mvm.flee( e,
                                                            detail::unpackParameter<sf::Vector2f>(parameters, "position"),
                                                            detail::unpackParameter<float>(parameters, "speed"));
                                            };
                                       });


            state.registerFunction("arrival",
                                       [] () -> std::function< void(Entity, MovementManager&, const script::Environment&) >
                                       {
                                            return [] (Entity e, MovementManager& mvm, const script::Environment& parameters)
                                            {
                                                mvm.arrival( e,
                                                            detail::unpackParameter<sf::Vector2f>(parameters, "position"),
                                                            detail::unpackParameter<float>(parameters, "speed"),
                                                            detail::unpackParameter<float>(parameters, "radius"));
                                            };
                                       });


            state.registerFunction("seek",
                                       [] () -> std::function< void(Entity, MovementManager&, const script::Environment&) >
                                       {
                                            return [] (Entity e, MovementManager& mvm, const script::Environment& parameters)
                                            {
                                                mvm.seek( e, detail::unpackParameter<sf::Vector2f>(parameters, "position"), detail::unpackParameter<float>(parameters, "speed"));
                                            };
                                       });


            state.registerFunction("directMovement",
                                       [] () -> std::function< void(Entity, MovementManager&, const script::Environment&) >
                                       {
                                            return [] (Entity e, MovementManager& mvm, const script::Environment& parameters)
                                            {
                                                mvm.directMovement( e, detail::unpackParameter<sf::Vector2f>(parameters, "vec"));
                                            };
                                       });
        }
    }
}
