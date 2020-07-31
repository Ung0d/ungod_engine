#ifndef UEDIT_TILEMAP_ACTIONS_H
#define UEDIT_TILEMAP_ACTIONS_H

#include "ungod/base/Entity.h"
#include "ungod/content/tilemap/TileMapHandler.h"

namespace uedit
{
    class ActionManager;

    class TilemapActions
    {
    public:
        TilemapActions(ActionManager& am) : mActionManager(am) {}

        void setTiles(ungod::Entity e, const ungod::TileData& tiles, unsigned mapSizeX, unsigned mapSizeY);

        void setInactiveTiles(ungod::Entity e, unsigned mapSizeX, unsigned mapSizeY);

        void setTile(ungod::Entity e, int id, unsigned x, unsigned y);

        void setTile(ungod::Entity e, int id, const sf::Vector2f& position);

        void setTileDims(ungod::Entity e,
            unsigned tileWidth, unsigned tileHeight,
            const std::vector<std::string>& keymap = {});

        void addKey(ungod::Entity e, const std::string& key);

        void floodFillTileMap(ungod::Entity e, unsigned ix, unsigned iy, const std::vector<int>& replacementIDs);

        void paintTile(ungod::TilemapBrush& brush, const sf::Vector2f& worldpos, bool connect);

        void eraseTile(ungod::TilemapBrush& brush, const sf::Vector2f& worldpos, int erasingID);

        void extend(ungod::Entity e, unsigned leftExtend, unsigned topExtend, unsigned rightExtend, unsigned bottomExtend, int id = -1);

    private:
        ActionManager& mActionManager;
    };
}

#endif