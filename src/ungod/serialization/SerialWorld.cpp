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

#include "ungod/serialization/SerialWorld.h"
#include "ungod/serialization/EntitySerial.h"
#include "ungod/base/World.h"
#include "ungod/serialization/DeserialInit.h"

namespace ungod
{
    void SerialBehavior<World, const sf::RenderTarget&>::serialize(const World& data, MetaNode serializer, SerializationContext& context, const sf::RenderTarget& target)
    {
        SerialBehavior<RenderLayer, const sf::RenderTarget&>::serialize(data, serializer, context, target);

        quad::Bounds bounds = data.mQuadTree.getBoundary();
        context.serializeProperty("left_bound", bounds.position.x, serializer);
        context.serializeProperty("upper_bound", bounds.position.y, serializer);
        context.serializeProperty("world_size_x", bounds.size.x, serializer);
        context.serializeProperty("world_size_y", bounds.size.y, serializer);

        quad::PullResult<Entity> contentPull;
        data.mQuadTree.getContent(contentPull);

        //sort entities by instantiation-type
        std::unordered_map<std::string, std::vector<Entity>> sorted;
        while (!contentPull.done())
        {
            Entity e = contentPull.poll();
            auto v = sorted.emplace(e.getInstantiation()->getIdentifier(), std::vector<Entity>{});
            v.first->second.emplace_back(e);
        }

        for (const auto& v : sorted)
            context.serializeObjectContainer(v.first, v.first, v.second, serializer, data, static_cast<const Application&>(*data.mMaster->getApp()));

        if (!data.mTileMapRenderer.getTileMapEntities().empty())
            context.serializeObjectContainer(data.mTileMapRenderer.getTileMapEntities().back().getInstantiation()->getIdentifier(),
                                             data.mTileMapRenderer.getTileMapEntities().back().getInstantiation()->getIdentifier(),
                                             data.mTileMapRenderer.getTileMapEntities(), serializer, data, static_cast<const Application&>(*data.mMaster->getApp()));

        if (!data.mTileMapRenderer.getWaterEntities().empty())
            context.serializeObjectContainer(data.mTileMapRenderer.getWaterEntities().back().getInstantiation()->getIdentifier(),
                                             data.mTileMapRenderer.getWaterEntities().back().getInstantiation()->getIdentifier(),
                                             data.mTileMapRenderer.getWaterEntities(), serializer, data, static_cast<const Application&>(*data.mMaster->getApp()));

        MetaNode nameMapNode = context.appendSubnode(serializer, "name_map");
        for (const auto& nameEntityPair : data.mEntityNames)
        {
            if (nameEntityPair.get<World::EntityTag>())//also clears out invalid entities
                context.serializeWeak(nameEntityPair.get<World::NameTag>(), &nameEntityPair.get<World::EntityTag>(), nameMapNode);
        }
    }

    void DeserialBehavior<World, const sf::RenderTarget&>::deserialize(World& data, MetaNode deserializer, DeserializationContext& context, const sf::RenderTarget& target)
    {
        DeserialBehavior<RenderLayer, const sf::RenderTarget&>::deserialize(data, deserializer, context, target);

        initDeserial(context, data);

        //get the most basic parameters of the worlds
        auto result = deserializer.getAttributes<float, float, float, float>
                            ( {"left_bound", 0.0f}, {"upper_bound", 0.0f}, {"world_size_x", 0.0f}, {"world_size_y", 0.0f} );
        data.initSpace(std::get<0>(result), std::get<1>(result), std::get<2>(result), std::get<3>(result));

        //retrieve all entities and add them to the quadtree
        for (const auto& instantiation : data.mDeserialMap)
            instantiation.second(context, deserializer);


        //hacky solution, todo?
        //the weak deserial semantics imply that a assigner might be stored for later, when
        //the referenced object is deserialized. Generally, code like above that depends on local
        //state is broken code.
        //However, we assume here that every entitiy derialization is finished at this point
        //If for some reason (e.g. because of a broken file) an entity can be deserialized in the name map,
        //but this entitiy is not deserialized along with the instantiations above, then the assigner will be
        //invalid indeed, however, it will never be called and dies with the context object
        //therefore, this code is safe IF AND ONLY IF THE ASSUMPTION IS FULFILLED
        MetaNode nameMapNode = deserializer.firstNode("name_map");
        std::vector<Entity> entities;
        std::vector<std::string> names;
        forEachAttribute(nameMapNode, [&names] (MetaAttribute attr)
         {
            names.emplace_back(attr.name());
         });
        if (names.size() > 0)
        {
            entities.resize(names.size());
            MetaAttribute attrIter = context.first( context.deserializeWeak<Entity>([&entities](Entity& e) mutable { entities[0] = e; }), names[0], nameMapNode );
            for (unsigned i = 1; i < names.size(); i++)
                attrIter = context.next( context.deserializeWeak<Entity>([&entities, i](Entity& e) mutable { entities[i] = e; }), names[i], nameMapNode, attrIter);
            for (unsigned i = 0; i < entities.size(); i++)
            {
                if (entities[i])
                    data.tagWithName(entities[i], names[i]);
            }
        }
    }
}
