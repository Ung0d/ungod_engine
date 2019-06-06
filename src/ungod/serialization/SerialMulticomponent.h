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

#ifndef SERIAL_MULTI_COMPONENT_H
#define SERIAL_MULTI_COMPONENT_H

#include "ungod/serialization/Serializable.h"
#include "ungod/base/MultiComponent.h"
#include "ungod/base/World.h"

namespace ungod
{
    template <typename C>
    struct SerialIdentifier<MultiComponent<C>>
    {
        static std::string get() { return std::string("M") + SerialIdentifier<C>::get(); }
    };

    template <typename C>
    struct SerialBehavior<MultiComponent<C>, Entity, const World&, const Application&>
    {
        static void serialize(const MultiComponent<C>& data, MetaNode serializer, SerializationContext& context, Entity e, const World& world, const Application& app)
        {
            context.serializeProperty("cc", data.getComponentCount(), serializer);
            for (unsigned i = 0; i < data.getComponentCount(); ++i)
            {
                MetaNode compNode = context.appendSubnode(serializer, "c");
                SerialBehavior<C, Entity, const World&, const Application&>::serialize(data.getComponent(i), compNode, context, e, world, app);
            }
        }
    };

    template <typename C>
    struct DeserialBehavior<MultiComponent<C>, Entity, World&, const Application&>
    {
        static void deserialize(MultiComponent<C>& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application& app)
        {
            std::size_t compCount = deserializer.getAttribute<std::size_t>("cc", 0u);
            data.init(compCount, world.getUniverse());
            std::size_t i = 0;
            forEachSubnode(deserializer, [&] (MetaNode sub)
            {
                DeserialBehavior<C, Entity, World&, const Application&>::deserialize(data.getComponent(i), sub, context, e, world, app);
                ++i;
            }, "c");
        }
    };
}

#endif // SERIAL_MULTI_COMPONENT_H

