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
#include "ungod/content/tilemap/TileMap.h"

namespace ungod
{
    void SerialBehavior<TileMap>::serialize(const TileMap& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("tile_width", data.mTileWidth, serializer);
        context.serializeProperty("tile_height", data.mTileHeight, serializer);
        context.serializeProperty("map_width", data.mMapSizeX, serializer);
        context.serializeProperty("map_height", data.mMapSizeY, serializer);

        context.serializePropertyContainer("keys", data.mKeymap, serializer);

        if (data.mTiles.ids)
            context.serializePropertyContainer<int>("ids", [&data] (std::size_t i) { return data.mTiles.ids->at(i); } , data.mTiles.ids->size(), serializer);
    }

    void DeserialBehavior<TileMap>::deserialize(TileMap& data, MetaNode deserializer, DeserializationContext& context)
    {
        unsigned tileWidth, tileHeight, mapWidth, mapHeight;

        auto attr = context.first(context.deserializeProperty(tileWidth, 0u), "tile_width", deserializer);
        attr = context.next(context.deserializeProperty(tileHeight, 0u), "tile_height", deserializer, attr);
        attr = context.next(context.deserializeProperty(mapWidth, 0u), "map_width", deserializer, attr);
        attr = context.next(context.deserializeProperty(mapHeight, 0u), "map_height", deserializer, attr);

        std::vector<std::string> keymap;
        attr = context.next(
            context.deserializeContainer<std::string>([&keymap] (std::size_t num) { keymap.reserve(num); },
                                 [&keymap] (const std::string& key) { keymap.emplace_back(key); }),
                                 "keys", deserializer, attr );

        data.setTileDims( tileWidth, tileHeight, keymap);

        TileData tmdata;
        tmdata.ids = std::make_unique<std::vector<int>>();
        attr = context.next(
            context.deserializeContainer<int>([&tmdata] (std::size_t num) { tmdata.ids->reserve(num); },
                             [&tmdata] (int id) { tmdata.ids->emplace_back(id); }),
                             "ids", deserializer, attr );

        data.setTiles(tmdata, mapWidth, mapHeight);
    }
}
