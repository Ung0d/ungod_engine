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

#ifndef COLLISION_SERIAL_H
#define COLLISION_SERIAL_H

#include "ungod/serialization/Serializable.h"

namespace ungod
{
    template <std::size_t CONTEXT> class RigidbodyComponent;
    class Entity;
    class World;

    //define serialization behavior
    template <std::size_t CONTEXT>
    struct SerialIdentifier<RigidbodyComponent<CONTEXT>>
    {
        static std::string get();
    };

    template <std::size_t CONTEXT>
    struct SerialBehavior<RigidbodyComponent<CONTEXT>, Entity, const World&, const Application&>
    {
        static void serialize(const RigidbodyComponent<CONTEXT>& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&);
    };

    template <std::size_t CONTEXT>
    struct DeserialBehavior<RigidbodyComponent<CONTEXT>, Entity, World&, const Application&>
    {
        static void deserialize(RigidbodyComponent<CONTEXT>& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&);
    };
}

#endif // COLLISION_SERIAL_H
