#include "TilemapActions.h"
#include "ActionManager.h"

namespace uedit
{
    void TilemapActions::setTiles(ungod::Entity e, const ungod::TileData& tiles, unsigned mapSizeX, unsigned mapSizeY)
    {
        const ungod::TileMap& tm = e.get<ungod::TileMapComponent>().getTileMap();
        ungod::TileData oldData = tm.getTileData();
        unsigned oldmapx = tm.getMapSizeX();
        unsigned oldmapy = tm.getMapSizeY();
        mActionManager.action(std::function([this, &tiles, mapSizeX, mapSizeY](ungod::Entity e)
            { 
                e.getWorld().getTileMapHandler().setTiles(e, tiles, mapSizeX, mapSizeY);
            }),
            std::function([this, oldData, oldmapx, oldmapy](ungod::Entity e)
                {
                    e.getWorld().getTileMapHandler().setTiles(e, oldData, oldmapx, oldmapy);
                }),
                e);
    }

    void TilemapActions::setInactiveTiles(ungod::Entity e, unsigned mapSizeX, unsigned mapSizeY)
    {
        ungod::TileData data;
        data.ids = std::make_shared<std::vector<int>>();
        data.ids->resize((std::size_t)mapSizeX * (std::size_t)mapSizeY, -1);
        setTiles(e, data, mapSizeX, mapSizeY);
    }

    void TilemapActions::setTile(ungod::Entity e, int id, unsigned x, unsigned y)
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

    void TilemapActions::setTile(ungod::Entity e, int id, const sf::Vector2f& position)
    {
        const ungod::TileMap& tm = e.get<ungod::TileMapComponent>().getTileMap();
        sf::Vector2i indices = tm.getTileIndices(position);
        if (indices.x > -1)
            setTile(e, id, (unsigned)indices.x, (unsigned)indices.y);
    }

    void TilemapActions::setTileDims(ungod::Entity e,
        unsigned tileWidth, unsigned tileHeight,
        const std::vector<std::string>& keymap)
    {
        const ungod::TileMap& tm = e.get<ungod::TileMapComponent>().getTileMap();
        unsigned oldTileWidth = tm.getTileWidth();
        unsigned oldTileHeight = tm.getTileHeight();
        std::vector<std::string> oldKeymap = tm.getKeyMap();

        mActionManager.action(std::function([this, tileWidth, tileHeight, keymap](ungod::Entity e)
            { e.getWorld().getTileMapHandler().setTileDims(e, tileWidth, tileHeight, keymap); }),
            std::function([this, oldTileWidth, oldTileHeight, oldKeymap](ungod::Entity e)
                { e.getWorld().getTileMapHandler().setTileDims(e, oldTileWidth, oldTileHeight, oldKeymap); }),
            e);
    }

    void TilemapActions::addKey(ungod::Entity e, const std::string& key)
    {
        e.getWorld().getTileMapHandler().addKey(e, key);
    }

    void TilemapActions::floodFillTileMap(ungod::Entity e, unsigned ix, unsigned iy, const std::vector<int>& replacementIDs)
    {
        const ungod::TileMap& tm = e.get<ungod::TileMapComponent>().getTileMap();
        ungod::TileData oldData;
        unsigned oldmapx = tm.getMapSizeX();
        unsigned oldmapy = tm.getMapSizeY();
        oldData.ids = std::make_shared<std::vector<int>>(*tm.getTileData().ids);
        mActionManager.action(std::function([this, ix, iy, replacementIDs](ungod::Entity e)
            { e.getWorld().getTileMapHandler().floodFillTileMap(e, ix, iy, replacementIDs); }),
            std::function([this, oldData, oldmapx, oldmapy](ungod::Entity e)
                { e.getWorld().getTileMapHandler().setTiles(e, oldData, oldmapx, oldmapy); }),
            e);
    }

    void TilemapActions::paintTile(ungod::TilemapBrush& brush, const sf::Vector2f& worldpos, bool connect)
    {
        if (!brush.getTilemap())
            return;
        sf::Vector2i indices = brush.getTilemap()->getTileIndices(worldpos);
        if (indices.x == -1)
            return;
        int tileID = brush.getTilemap()->getTileID(indices.x, indices.y);
        mActionManager.action(std::function([&brush, worldpos, connect]()
            { brush.paintTile(worldpos, connect); }),
            std::function([&brush, worldpos, tileID]()
                { brush.eraseTile(worldpos, tileID); }));
    }

    void TilemapActions::eraseTile(ungod::TilemapBrush& brush, const sf::Vector2f& worldpos, int erasingID)
    {
        mActionManager.action(std::function([&brush, worldpos, erasingID]()
            { brush.eraseTile(worldpos, erasingID); }),
            std::function([&brush, worldpos]()
                { brush.paintTile(worldpos); }));
    }

    void TilemapActions::extend(ungod::Entity e, unsigned leftExtend, unsigned topExtend, unsigned rightExtend, unsigned bottomExtend, int id)
    {
        const ungod::TileMap& tm = e.get<ungod::TileMapComponent>().getTileMap();
        ungod::TileData oldData;
        unsigned oldmapx = tm.getMapSizeX();
        unsigned oldmapy = tm.getMapSizeY();
        oldData.ids = std::make_shared<std::vector<int>>(*tm.getTileData().ids);
        mActionManager.action(std::function([this, leftExtend, topExtend, rightExtend, bottomExtend, id](ungod::Entity e)
            { e.getWorld().getTileMapHandler().extendTilemap(e, leftExtend, topExtend, rightExtend, bottomExtend, id); }),
            std::function([this, oldData, oldmapx, oldmapy](ungod::Entity e)
                { e.getWorld().getTileMapHandler().setTiles(e, oldData, oldmapx, oldmapy); }),
            e);
    }
}