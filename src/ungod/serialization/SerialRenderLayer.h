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

#ifndef SERIAL_RENDER_LAYER_H
#define SERIAL_RENDER_LAYER_H

#include "ungod/serialization/Serializable.h"
#include <SFML/Graphics.hpp>

namespace ungod
{
    class RenderLayer;
    class RenderLayerContainer;
    class ScriptedGameState;
    class Entity;


    template <>
    struct SerialIdentifier<RenderLayer>
    {
        static std::string get()  { return "RenderLayer"; }
    };

    template <>
    struct SerialBehavior<RenderLayer>
    {
        static void serialize(const RenderLayer& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<RenderLayer, std::forward_list<Entity>&>
    {
        static void deserialize(RenderLayer& data, MetaNode deserializer, DeserializationContext& context, std::forward_list<Entity>& scriptEntities);
    };


    template <>
    struct SerialIdentifier<RenderLayerContainer>
    {
        static std::string get()  { return "RenderLayerContainer"; }
    };

    template <>
    struct SerialBehavior<RenderLayerContainer>
    {
        static void serialize(const RenderLayerContainer& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<RenderLayerContainer, std::forward_list<Entity>&>
    {
        static void deserialize(RenderLayerContainer& data, MetaNode deserializer, DeserializationContext& context, std::forward_list<Entity>& scriptEntities);
    };
}

#endif // SERIAL_RENDER_LAYER_H
