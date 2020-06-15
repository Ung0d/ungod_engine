#include "TilemapActions.h"
#include "ActionManager.h"

namespace uedit
{
    bool TilemapActions::setTiles(Entity e, TileData& tiles, unsigned mapSizeX, unsigned mapSizeY)
    {
        const ungod::TileMap& tm = e.get<ungod::TileMapComponent>().getTileMap();
        TileData oldData = tm.getTileData();
        unsigned oldmapx = tm.getMapSizeX();
        unsigned oldmapy = tm.getMapSizeY();
        mActionManager.action(std::function([this, tiles, mapSizeX, mapSizeY](ungod::Entity e)
            { 
                e.getWorld().getTileMapHandler().setTiles(e, tiles, mapSizeX, mapSizeY);
            }),
            std::function([this, oldData, oldmapx, oldmapy](ungod::Entity e)
                {
                    e.getWorld().getTileMapHandler().setTiles(e, oldData, oldmapx, oldmapy);
                }),
                e);
    }

    bool TilemapActions::setInactiveTiles(Entity e, unsigned mapSizeX, unsigned mapSizeY)
    {
        TileData data;
        data.ids = std::make_shared<std::vector<int>>();
        data.ids->resize(mapX * mapY, -1);
        setTiles(e, data, mapSizeX, mapSizeY);
    }

    void TilemapActions::setTile(Entity e, int id, unsigned x, unsigned y)
    {
        const ungod::TileMap& tm = e.get<ungod::TileMapComponent>().getTileMap();
        int oldid = tm.getTileID(x, y);
        mActionManager.action<ungod::Entity, unsigned, unsigned>(std::function([this, id](ungod::Entity e, unsigned x, unsigned y)
                {
                    e.getWorld().getTileMapHandler().setTile(e, id, x, y);
                }),
            std::function([this, oldid](ungod::Entity e, unsigned x, unsigned y)
                {
                    e.getWorld().getTileMapHandler().setTile(e, oldid, x, y);
                }),
                e, x, y);
    }

    void TilemapActions::setTile(Entity e, int id, const sf::Vector2f& position)
    {
        const ungod::TileMap& tm = e.get<ungod::TileMapComponent>().getTileMap();
        sf::Vector2i indices = tm.getTileIndices(position);
        if (indices.x > -1)
            setTile(e, id, (unsigned)indices.x, (unsigned)indices.y);
    }

    void TilemapActions::setTileDims(Entity e,
        unsigned tileWidth, unsigned tileHeight,
        const std::vector<std::string>& keymap = {})
    {
        const ungod::TileMap& tm = e.get<ungod::TileMapComponent>().getTileMap();
        unsigned oldTileWidth = tm.getTileWidth();
        unsigned oldTileHeight = tm.getTileHeight();
        std::vector<std::string> oldKeymap = tm.getKeyMap();

        mActionManager.action(std::function([this, cTileWidth, cTileHeight, keymap](ungod::Entity e)
            { e.getWorld().getTileMapHandler().setTileDims(e, cTileWidth, cTileHeight, keymap); }),
            std::function([this, oldTileWidth, oldTileHeight, oldKeymap](ungod::Entity e)
                { e.getWorld().getTileMapHandler().setTileDims(e, oldTileWidth, oldTileHeight, oldKeymap); }),
            e);
    }

    void TilemapActions::addKey(Entity e, const std::string& key)
    {
        e.getWorld().getTileMapHandler().addKey(e, key);
    }

    void TilemapActions::floodFillTileMap(Entity e, unsigned ix, unsigned iy, const std::vector<int>& replacementIDs)
    {
        const ungod::TileMap& tm = e.get<ungod::TileMapComponent>().getTileMap();
        TileData oldData;
        unsigned oldmapx = tm.getMapSizeX();
        unsigned oldmapy = tm.getMapSizeY();
        oldData.ids = std::make_shared<std::vector<int>>(tm.getTileData());
        mActionManager.action(std::function([this, ix, iy, replacementIDs](ungod::Entity e)
            { e.getWorld().getTileMapHandler().floodFillTileMap(e, ix, iy, replacementIDs); }),
            std::function([this, oldData, oldmapx, oldmapy](ungod::Entity e)
                { e.getWorld().getTileMapHandler().setTiles(e, oldData, oldmapx, oldmapy); }),
            e);
    }
}