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

#ifndef SERIAL_WORLD_H
#define SERIAL_WORLD_H

#include "ungod/serialization/Serializable.h"
#include <SFML/Graphics.hpp>

namespace ungod
{
    class World;
    class Application;
    struct DeserialMemory;

    template <>
    struct SerialIdentifier<World>
    {
        static std::string get()  { return "World"; }
    };

    template <>
    struct SerialBehavior<World>
    {
        static void serialize(const World& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<World, DeserialMemory&>
    {
        static void deserialize(World& data, MetaNode deserializer, DeserializationContext& context, DeserialMemory& deserialMemory);
    };
}

#endif // SERIAL_WORLD_H
