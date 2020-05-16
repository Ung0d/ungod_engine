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

#ifndef UNGOD_ENTITY_BEHAVIOR_H
#define UNGOD_ENTITY_BEHAVIOR_H

#include <memory>
#include <queue>
#include "ungod/serialization/Serializable.h"
#include "ungod/base/Entity.h"

namespace ungod
{
    typedef void(*InitCallback)(Entity, World&);

    /** \brief A compile time generated array that holds callbacks. */
    template<typename FUNC, FUNC... args>
    struct ArrayGen
    {
        static const FUNC data[sizeof...(args)];
        static const size_t size;
    };

    template<typename FUNC, FUNC... args>
    const FUNC ArrayGen<FUNC, args...>::data[sizeof...(args)] = { args... };

    template<typename FUNC, FUNC... args>
    const size_t ArrayGen<FUNC, args...>::size = sizeof...(args);
}

#endif // UNGOD_ENTITY_BEHAVIOR_H
