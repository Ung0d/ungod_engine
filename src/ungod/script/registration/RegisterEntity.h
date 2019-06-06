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

#ifndef REGISTER_ENTITY_H
#define REGISTER_ENTITY_H

#include "ungod/script/Behavior.h"
#include "ungod/base/Entity.h"

namespace ungod
{
    namespace scriptRegistration
    {
        struct Has
        {
            Has(Entity e) : entity(e) {}

            template<typename C>
            bool has() const { return entity.has<C>(); }

            Entity entity;
        };

        struct Get
        {
            Get(Entity e) : entity(e) {}

            template<typename C>
            const C& get() const { return entity.get<C>(); }

            Entity entity;
        };

        struct Add
        {
            Add(Entity e) : entity(e) {}

            template<typename C>
            void add(dom::ComponentInstantiator<C> c) const { entity.add<C>(c); }

            template<typename C>
            void add() const { entity.add<C>(); }

            Entity entity;
        };

        struct Rem
        {
            Rem(Entity e) : entity(e) {}

            template<typename C>
            void rem() const { entity.rem<C>(); }

            Entity entity;
        };

        /** \brief Registers entity functionality for scripts. */
        void registerEntity(ScriptStateBase& state);
    }
}

#endif
