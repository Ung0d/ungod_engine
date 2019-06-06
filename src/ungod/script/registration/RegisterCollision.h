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
#include "ungod/physics/Collision.h"

namespace ungod
{
    namespace scriptRegistration
    {
        /** \brief Registers collision functionality for scripts. */
        template <std::size_t CONTEXT>
        void registerCollision(ScriptStateBase& state)
        {
            state.registerUsertype<RigidbodyComponent<CONTEXT>>(STRCAT("Rigidbody", NUM2STR(CONTEXT)));

            state.registerUsertype<CollisionManager<CONTEXT>>(STRCAT("Rigidbody", NUM2STR(CONTEXT)));
        }

        /** \brief Registers rigidbody attachment and manipulation functionality for scripts. */
        void registerRigidbody(ScriptStateBase& state);
    }
}

#endif

