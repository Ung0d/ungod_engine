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

#ifndef REGISTER_WORLD_H
#define REGISTER_WORLD_H

#include "ungod/script/Behavior.h"
#include "ungod/base/World.h"

namespace ungod
{
    namespace scriptRegistration
    {
        namespace detail
        {
            /** \brief Entity creation helper....*/
            template<typename BASE, typename OPTIONAL>
            std::vector<Entity> entityCreator(World& world, std::size_t num)
            {
               std::vector<Entity> entities;
               entities.reserve(num);
               world.create(BASE(), OPTIONAL(), num, [&entities] (Entity e) { entities.emplace_back(e); });
               return entities;
            }
        }

        /** \brief Registers world functionality for scripts. */
        void registerWorld(ScriptStateBase& state);
    }
}

#endif

