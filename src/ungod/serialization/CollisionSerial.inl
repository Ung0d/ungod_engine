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

#ifndef COLLISION_SERIAL_IMPL_H
#define COLLISION_SERIAL_IMPL_H

namespace ungod
{
    //define serialization behavior
    template <std::size_t CONTEXT>
    std::string SerialIdentifier<RigidbodyComponent<CONTEXT>>::get()  { return std::string("RB") + std::to_string(CONTEXT); }

    template <std::size_t CONTEXT>
    void SerialBehavior<RigidbodyComponent<CONTEXT>, Entity, const World&, const Application&>::serialize(const RigidbodyComponent<CONTEXT>& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
        for (const auto& collider : data.mColliders)
        {
            MetaNode colliderNode = context.appendSubnode(serializer, "c");
            context.serializeProperty("left", collider.getUpleft().x, colliderNode);
            context.serializeProperty("top", collider.getUpleft().y, colliderNode);
            context.serializeProperty("right", collider.getDownright().x, colliderNode);
            context.serializeProperty("bottom", collider.getDownright().y, colliderNode);
            context.serializeProperty("rotation", collider.getRotation(), colliderNode);
        }
    }

    template <std::size_t CONTEXT>
    void DeserialBehavior<RigidbodyComponent<CONTEXT>, Entity, World&, const Application&>::deserialize(RigidbodyComponent<CONTEXT>& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
        forEachSubnode(deserializer, [&world, e] (MetaNode colliderNode)
        {
            auto result = colliderNode.getAttributes<float, float, float, float, float>
                ({"left", 0.0f}, {"top", 0.0f}, {"right", 0.0f}, {"bottom", 0.0f}, {"rotation", 0.0f});
            world.getRigidbodyManager().addCollider(e, {std::get<0>(result), std::get<1>(result)}, {std::get<2>(result), std::get<3>(result)}, std::get<4>(result));
        }, "c");
    }
}

#endif // COLLISION_SERIAL_IMPL_H
