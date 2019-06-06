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

#include "ungod/serialization/SerialRenderLayer.h"
#include "ungod/visual/RenderLayer.h"
#include "ungod/application/ScriptedGameState.h"
#include "ungod/base/World.h"

namespace ungod
{
    void SerialBehavior<RenderLayer, const sf::RenderTarget&>::serialize(const RenderLayer& data, MetaNode serializer, SerializationContext& context, const sf::RenderTarget&)
    {
        context.serializeProperty("d", data.getRenderDepth(), serializer);
        context.serializeProperty("n", data.getName(), serializer);
    }

    void DeserialBehavior<RenderLayer, const sf::RenderTarget&>::deserialize(RenderLayer& data, MetaNode deserializer, DeserializationContext& context, const sf::RenderTarget&)
    {
        auto attr = context.first(context.deserializeProperty([&data](float d) {data.setRenderDepth(d);}, 1.0f), "d", deserializer);
        context.next(context.deserializeProperty([&data](const std::string& s) {data.setName(s);}, std::string()), "n", deserializer, attr);
    }


    void SerialBehavior<RenderLayerContainer, const sf::RenderTarget&>::serialize(const RenderLayerContainer& data, MetaNode serializer,
                                                                                  SerializationContext& context, const sf::RenderTarget& target)
    {
        context.serializeObjectContainer<RenderLayer>("l", [&data] (std::size_t i) -> const RenderLayer& { return *data.getVector()[i].first.get(); }, data.getVector().size(), serializer, target);
    }

    void DeserialBehavior<RenderLayerContainer, const sf::RenderTarget&, ScriptedGameState&>::deserialize(RenderLayerContainer& data, MetaNode deserializer,
                                                                                      DeserializationContext& context, const sf::RenderTarget& target, ScriptedGameState& gamestate)
    {
        context.instantiate<World>([&data, &gamestate] ()
            {
                return gamestate.addWorld();
            }, target);

        context.first(context.deserializeInstantiationContainer<RenderLayer>(
            [&data] (std::size_t initsize) { data.mRenderLayers.reserve(initsize); },
            [&data] (RenderLayer* l, std::size_t i) { }),
            "l", deserializer);
    }
}
