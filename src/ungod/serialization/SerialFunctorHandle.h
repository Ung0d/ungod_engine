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

#ifndef UNGOD_SERIAL_FUNCTOR_HANDLE_H
#define UNGOD_SERIAL_FUNCTOR_HANDLE_H

#include "ungod/serialization/Serializable.h"

namespace ungod
{
    class FunctorHandle;

    template<>
    struct SerialIdentifier<FunctorHandle>
    {
        static std::string get()  { return "FunctorHandle"; }
    };

    template<typename ... PARAM>
    struct SerialBehavior<FunctorHandle, PARAM...>
    {
        static void serialize(const FunctorHandle& data, MetaNode serializer, SerializationContext& context);
    };

    template<typename ... PARAM>
    struct DeserialBehavior<FunctorHandle, PARAM...>
    {
        static void deserialize(FunctorHandle& data, MetaNode deserializer, DeserializationContext& context);
    };
}

#endif // UNGOD_SERIAL_FUNCTOR_HANDLE_H
