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

#include "ungod/script/registration/RegisterCollision.h"
#include "ungod/base/World.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerRigidbody(ScriptStateBase& state)
        {
            state.registerUsertype<Collider>("Collider",
                                                   sol::constructors<Collider(const sf::Vector2f& upleft, const sf::Vector2f& downright), Collider(const sf::Vector2f& upleft, const sf::Vector2f& downright, float rotation)>(),
                                                   "getUpleft", &Collider::getUpleft,
                                                   "getDownright", &Collider::getDownright,
                                                   "getCenter", &Collider::getCenter,
                                                   "getRotation", &Collider::getRotation);

            state.registerUsertype<RigidbodyManager>("RigidbodyManager",
                                                           "addMovementCollider", sol::overload(
                                                                            [] (RigidbodyManager& rm, Entity e, const Collider& collider) { rm.addCollider<MOVEMENT_COLLISION_CONTEXT>(e,collider); },
                                                                            [] (RigidbodyManager& rm, Entity e, const sf::Vector2f& upleft, const sf::Vector2f& downright) { rm.addCollider<MOVEMENT_COLLISION_CONTEXT>(e,upleft,downright); },
                                                                            [] (RigidbodyManager& rm, Entity e, const sf::Vector2f& upleft, const sf::Vector2f& downright, float rotation) { rm.addCollider<MOVEMENT_COLLISION_CONTEXT>(e,upleft,downright, rotation); }),
                                                           "modifyMovementCollider", &RigidbodyManager::modifyCollider<MOVEMENT_COLLISION_CONTEXT>,
                                                           "rotateMovementCollider", &RigidbodyManager::rotateCollider<MOVEMENT_COLLISION_CONTEXT>,
                                                           "setMovementColliderRotation", &RigidbodyManager::setColliderRotation<MOVEMENT_COLLISION_CONTEXT>,
                                                           "setMovementColliderDownRight", &RigidbodyManager::setColliderDownRight<MOVEMENT_COLLISION_CONTEXT>,
                                                           "setMovementColliderUpLeft", &RigidbodyManager::setColliderUpLeft<MOVEMENT_COLLISION_CONTEXT>,
                                                           "addSemanticsCollider", sol::overload(
                                                                            [] (RigidbodyManager& rm, Entity e, const Collider& collider) { rm.addCollider<SEMANTICS_COLLISION_CONTEXT>(e,collider); },
                                                                            [] (RigidbodyManager& rm, Entity e, const sf::Vector2f& upleft, const sf::Vector2f& downright) { rm.addCollider<SEMANTICS_COLLISION_CONTEXT>(e,upleft,downright); },
                                                                            [] (RigidbodyManager& rm, Entity e, const sf::Vector2f& upleft, const sf::Vector2f& downright, float rotation) { rm.addCollider<SEMANTICS_COLLISION_CONTEXT>(e,upleft,downright, rotation); }),
                                                           "modifySemanticsCollider", &RigidbodyManager::modifyCollider<SEMANTICS_COLLISION_CONTEXT>,
                                                           "rotateSemanticsCollider", &RigidbodyManager::rotateCollider<SEMANTICS_COLLISION_CONTEXT>,
                                                           "setSemanticsColliderRotation", &RigidbodyManager::setColliderRotation<SEMANTICS_COLLISION_CONTEXT>,
                                                           "setSemanticsColliderDownRight", &RigidbodyManager::setColliderDownRight<SEMANTICS_COLLISION_CONTEXT>,
                                                           "setSemanticsColliderUpLeft", &RigidbodyManager::setColliderUpLeft<SEMANTICS_COLLISION_CONTEXT>,
                                                           "getLowerBound", &RigidbodyManager::getLowerBound,
                                                           "getUpperBound", &RigidbodyManager::getUpperBound );
        }
    }
}
