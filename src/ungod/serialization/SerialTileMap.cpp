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

#include "ungod/serialization/SerialTileMap.h"
#include "ungod/content/TileMap.h"

namespace ungod
{
    void SerialBehavior<TileMap>::serialize(const TileMap& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("tiles_id", data.mImage.getFilePath(), serializer);
        context.serializeProperty("meta_id", data.mTilemeta.getFilePath(), serializer);
        context.serializeProperty("tile_width", data.mTileWidth, serializer);
        context.serializeProperty("tile_height", data.mTileHeight, serializer);
        context.serializeProperty("map_width", data.mMapSizeX, serializer);
        context.serializeProperty("map_height", data.mMapSizeY, serializer);

        context.serializePropertyContainer("keys", data.mKeymap, serializer);
        context.serializePropertyContainer<int>("ids", [&data] (std::size_t i) { return data.mTiles[i].getTileID(); } , data.mTiles.size(), serializer);
        context.serializePropertyContainer<unsigned>("materials", [&data] (std::size_t i) { return data.mTiles[i].getMaterialID(); } , data.mTiles.size(), serializer);
        context.serializePropertyContainer<bool>("active", [&data] (std::size_t i) { return data.mTiles[i].isActive(); } , data.mTiles.size(), serializer);
        context.serializePropertyContainer<bool>("blocked", [&data] (std::size_t i) { return data.mTiles[i].isBlocked(); } , data.mTiles.size(), serializer);

        /*MetaNode keymapNode = context.appendSubnode(serializer, "keys");
        context.serializeProperty("count", data.mKeymap.size(), keymapNode);
        for (const auto& key : data.mKeymap)
        {
            context.serializeProperty("key", key, keymapNode);
        }

        MetaNode mapNode = context.appendSubnode(serializer, "map");
        context.serializeProperty("count", data.mTiles.size(), mapNode);
        for (const auto& tile : data.mTiles)
        {
            MetaNode tileNode = context.appendSubnode(mapNode, "t");
            context.serializeProperty("id", tile.getTileID(), tileNode);
            context.serializeProperty("material", tile.getMaterialID(), tileNode);
        }*/
    }

    void DeserialBehavior<TileMap>::deserialize(TileMap& data, MetaNode deserializer, DeserializationContext& context)
    {
        std::string tilesID, metaID;
        unsigned tileWidth, tileHeight, mapWidth, mapHeight;

        auto attr = context.first(context.deserializeProperty(tilesID), "tiles_id", deserializer);
        attr = context.next(context.deserializeProperty(metaID), "meta_id", deserializer, attr);
        attr = context.next(context.deserializeProperty(tileWidth, 0u), "tile_width", deserializer, attr);
        attr = context.next(context.deserializeProperty(tileHeight, 0u), "tile_height", deserializer, attr);
        attr = context.next(context.deserializeProperty(mapWidth, 0u), "map_width", deserializer, attr);
        attr = context.next(context.deserializeProperty(mapHeight, 0u), "map_height", deserializer, attr);

        std::vector<std::string> keymap;
        attr = context.next(
            context.deserializeContainer<std::string>([&keymap] (std::size_t num) { keymap.reserve(num); },
                                 [&keymap] (const std::string& key) { keymap.emplace_back(key); }),
                                 "keys", deserializer, attr );

        data.loadTiles( tilesID, metaID, tileWidth, tileHeight, keymap);

        std::vector<int> tiles;
        std::vector<unsigned> materials;
        std::vector<bool> active;
        std::vector<bool> blocked;
        attr = context.next(
            context.deserializeContainer<int>([&tiles] (std::size_t num) { tiles.reserve(num); },
                             [&tiles] (int id) { tiles.emplace_back(id); }),
                             "ids", deserializer, attr );
        attr = context.next(
            context.deserializeContainer<unsigned>([&materials] (std::size_t num) { materials.reserve(num); },
                             [&materials] (unsigned mat) { materials.emplace_back(mat); }),
                             "materials", deserializer, attr );
        attr = context.next(
            context.deserializeContainer<bool>([&active] (std::size_t num) { active.reserve(num); },
                             [&active] (bool a) { active.push_back(a); }),
                             "active", deserializer, attr );
        attr = context.next(
            context.deserializeContainer<bool>([&blocked] (std::size_t num) { blocked.reserve(num); },
                             [&blocked] (bool b) { blocked.push_back(b); }),
                             "blocked", deserializer, attr );

        data.setTiles(tiles, materials, active, blocked, mapWidth, mapHeight);

        //deserialize the keymap
        /*MetaNode keymapNode = deserializer.firstNode("keys");
        MetaAttribute keysAttr;
        unsigned keyCount = context.getProperty<unsigned>("count", keymapNode, keysAttr, 0u);
        std::vector<std::string> keymap;
        keymap.reserve(keyCount);
        std::string key = context.getProperty<std::string>("key", keymapNode, keysAttr, "");
        while (key != "")
        {
            keymap.push_back(key);
            key = context.getProperty<std::string>("key", keymapNode, keysAttr, "");
        }

        data.loadTiles( std::get<0>(baseresult), std::get<1>(baseresult), std::get<2>(baseresult),
                       std::get<3>(baseresult), target, keymap);

        //deserialize the map itself
        MetaNode mapNode = deserializer.firstNode("map");
        MetaAttribute mapAttr;
        keyCount = context.getProperty<unsigned>("count", mapNode, mapAttr, 0u);
        std::vector<int> tiles;
        std::vector<unsigned> materials;
        tiles.reserve(keyCount);
        materials.reserve(keyCount);
        forEachSubnode(mapNode, [&tiles, &materials](MetaNode sub)
        {
            auto result = sub.getAttributes<int, unsigned>( {"id",0}, {"material",0u} );
            tiles.emplace_back(std::get<0>(result));
            materials.emplace_back(std::get<1>(result));
        }, "t"); */

        //data.setTiles(tiles, materials, std::get<4>(baseresult), std::get<5>(baseresult));
    }


    void SerialBehavior<FootstepSoundProfile>::serialize
                (const FootstepSoundProfile& data,
                 MetaNode serializer,
                 SerializationContext& context)
    {
        context.serializeProperty("material_data", data.mMaterialData.getFilePath(), serializer);
    }

    void DeserialBehavior<FootstepSoundProfile, AudioManager&, TileMap&>::deserialize
                (FootstepSoundProfile& data,
                 MetaNode deserializer,
                 DeserializationContext& context,
                 AudioManager& mgr,
                 TileMap& ground)
    {
        auto material = deserializer.getAttribute<std::string>("material_data", "");
        data.loadMaterialData(material, mgr, ground);
    }
}
