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

#ifndef REGISTER_COLLISION_H
#define REGISTER_COLLISION_H

#include "ungod/script/Behavior.h"
#include "ungod/physics/CollisionManager.h"
#include "ungod/script/registration/RegistrationDetail.h"

namespace ungod
{
    namespace scriptRegistration
    {
        /** \brief Registers collision functionality for scripts. */
        template <std::size_t CONTEXT>
        void registerCollision(ScriptStateBase& state)
        {
            state.registerUsertype<RigidbodyComponent<CONTEXT>>(STRCAT("Rigidbody", NUM2STR(CONTEXT)));
            detail::registerMultiComponent<RigidbodyComponent<CONTEXT>>(state, STRCAT("MultiRigidbody", NUM2STR(CONTEXT)));

            state.registerUsertype<CollisionManager<CONTEXT>>(STRCAT("CollisionManager", NUM2STR(CONTEXT)));

            script::Usertype<RigidbodyManager<CONTEXT>> rmType = 
                    state.registerUsertype<RigidbodyManager<CONTEXT>>(STRCAT("RigidbodyManager", NUM2STR(CONTEXT)));

            rmType["addCollider"] = sol::overload(
                [](RigidbodyManager<CONTEXT>& rm, Entity e, const Collider& collider) { rm.addCollider(e, collider); },
                [](RigidbodyManager<CONTEXT>& rm, Entity e, unsigned i, const Collider& collider) { rm.addCollider(e, i, collider);  });
            rmType["clearCollider"] = sol::overload(
                [](RigidbodyManager<CONTEXT>& rm, Entity e) { rm.clearCollider(e); },
                [](RigidbodyManager<CONTEXT>& rm, Entity e, unsigned i) { rm.clearCollider(e, i);  });
            rmType["setRectDownRight"] = sol::overload(
                [](RigidbodyManager<CONTEXT>& rm, Entity e, const sf::Vector2f& p) { rm.setRectDownRight(e,p); },
                [](RigidbodyManager<CONTEXT>& rm, Entity e, unsigned i, const sf::Vector2f& p) { rm.setRectDownRight(e, i, p);  });
            rmType["setRectUpLeft"] = sol::overload(
                [](RigidbodyManager<CONTEXT>& rm, Entity e, const sf::Vector2f& p) { rm.setRectUpLeft(e, p); },
                [](RigidbodyManager<CONTEXT>& rm, Entity e, unsigned i, const sf::Vector2f& p) { rm.setRectUpLeft(e, i, p);  });
            rmType["setRectRotation"] = sol::overload(
                [](RigidbodyManager<CONTEXT>& rm, Entity e, float r) { rm.setRectRotation(e, r); },
                [](RigidbodyManager<CONTEXT>& rm, Entity e, unsigned i, float r) { rm.setRectRotation(e, i, r);  });
            rmType["setPoint"] = sol::overload(
                [](RigidbodyManager<CONTEXT>& rm, Entity e, unsigned pointIndex, const sf::Vector2f& point) { rm.setPoint(e, pointIndex, point); },
                [](RigidbodyManager<CONTEXT>& rm, Entity e, unsigned multiIndex, unsigned pointIndex, const sf::Vector2f& point) { rm.setPoint(e, multiIndex, pointIndex, point);  });
            rmType["setActive"] = sol::overload(
                [](RigidbodyManager<CONTEXT>& rm, Entity e, bool active) { rm.setActive(e, active); },
                [](RigidbodyManager<CONTEXT>& rm, Entity e, unsigned i,bool active) { rm.setActive(e, i, active);  });

        }

        /** \brief Registers rigidbody attachment and manipulation functionality for scripts. */
        void registerRigidbody(ScriptStateBase& state);
    }
}

#endif

