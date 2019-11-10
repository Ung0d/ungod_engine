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

#include "ungod/serialization/SerialGraph.h"

namespace ungod
{
    void SerialBehavior<graph::EdgeInstantiator>::serialize(const graph::EdgeInstantiator& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("u", data.u, serializer);
        context.serializeProperty("v", data.v, serializer);
    }

    void DeserialBehavior<graph::EdgeInstantiator>::deserialize(graph::EdgeInstantiator& data, MetaNode deserializer, DeserializationContext& context)
    {
        auto result = deserializer.getAttributes<std::size_t, std::size_t>( {"u", 0u}, {"v", 0u});
        data.u = std::get<0>(result);
        data.v = std::get<1>(result);
    }

    void SerialBehavior<graph::UndirectedAdjacencyLists>::serialize(const graph::UndirectedAdjacencyLists& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("v", data.getVertexCount(), serializer);
        std::vector< graph::EdgeInstantiator > edges;
        edges.reserve(data.getEdgeCount());
        for (std::size_t i = 0; i < data.getVertexCount(); ++i)
            for (const auto& e : data.getNeighbors(i))
                edges.emplace_back(i, e.destination);
        context.serializeObjectContainer("e", edges, serializer);
    }

    void DeserialBehavior<graph::UndirectedAdjacencyLists>::deserialize(graph::UndirectedAdjacencyLists& data, MetaNode deserializer, DeserializationContext& context)
    {
        std::size_t numvertex;
        auto attr = context.first(context.deserializeProperty<std::size_t>(numvertex, 0u), "v", deserializer);
		if (numvertex == 0) return;
        std::vector< graph::EdgeInstantiator > edges;
        context.next(context.deserializeObjectContainer<graph::EdgeInstantiator>(
                                                        [&edges] (std::size_t init) { edges.resize(init); },
                                                        [&edges] (std::size_t i) -> graph::EdgeInstantiator& { return edges[i]; }), "e", deserializer, attr);


		data.mAdjacencies.resize(numvertex);
		data.mNumEdges = edges.size()/2;
		for (const auto& e : edges)
			data.mAdjacencies[e.u].emplace_back(e.v);
    }
}
