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

#include "ungod/serialization/SerialBehavior.h"

namespace ungod
{
    /*std::string SerialIdentifier<Transform>::get() { return "Transform"; }

    void SerialIdentifier<Transform>::serialize(
            const Transform& en, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("pos_x", en.getPosition().x, serializer);
        context.serializeProperty("pos_y", en.getPosition().y, serializer);
        context.serializeProperty("plane", en.isPlane(), serializer);
        context.serializeProperty("scale", en.getScale(), serializer);
    }

    void SerialIdentifier<Transform,  void(Entity)>::deserialize(
            MetaNode deserializer, DeserializationContext& context,
            Entity e, TransformManager& manager)
    {
        MetaAttribute attr;
        auto attributes = deserializer.getAttributes<float, float, bool, float>(
                {"pos_x", 0.0f}, {"pos_y", 0.0f}, {"plane", false}, {"scale", 1.0f} );
        manager.setPosition(e, {std::get<0>(attributes), std::get<1>(attributes)});
        manager.setPlane(e, std::get<2>(attributes));
        manager.setScale(e, std::get<3>(attributes));
    }*/
}
