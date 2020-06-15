#ifndef UEDIT_TILEMAP_ACTIONS_H
#define UEDIT_TILEMAP_ACTIONS_H

#include "ungod/base/Entity.h"
#include "ungod/base/tilemap/TileMapHandler.h"
#include <unordered_map>

namespace uedit
{
    class ActionManager;

    class TilemapActions
    {
    public:
        TilemapActions(ActionManager& am) : mActionManager(am) {}

        bool setTiles(Entity e, TileData& tiles, unsigned mapSizeX, unsigned mapSizeY);

        bool setInactiveTiles(Entity e, unsigned mapSizeX, unsigned mapSizeY);

        void setTile(Entity e, int id, unsigned x, unsigned y);

        void setTile(Entity e, int id, const sf::Vector2f& position);

        void setTileDims(Entity e,
            unsigned tileWidth, unsigned tileHeight,
            const std::vector<std::string>& keymap = {});

        void addKey(Entity e, const std::string& key);

        void floodFillTileMap(Entity e, unsigned ix, unsigned iy, const std::vector<int>& replacementIDs);

    private:
        ActionManager& mActionManager;
    }
}

#endif