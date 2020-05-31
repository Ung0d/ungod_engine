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

#include "ungod/content/tilemap/TileMapHandler.h"
#include "ungod/content/tilemap/FloodFill.h"
#include "ungod/base/World.h"
#include "ungod/application/Application.h"

namespace ungod
{
    namespace detail
    {
        void TilemapRendererChangeNotificator::notifyTileChanged(int id, unsigned x, unsigned y) const
        {
            //todo
            //gets called every time a change is made to a tilemap through a brush object that was created by
            //the tilemap renderers "make" functions
        }
    }

    void TileMapHandler::init(World& world)
    {
        viewSizeChanged(world, world.getGraph().getCamera().getView().getSize());
        world.onComponentAdded<TileMapComponent>([&world](Entity e) 
            {
                e.modify<TileMapComponent>().mTileMap.setViewSize(world.getGraph().getCamera().getView().getSize());
            });
    }

    void TileMapHandler::update(const std::list<Entity>& entities, const World& world)
    {
        viewSizeChanged(world, world.getGraph().getCamera().getView().getSize());
        dom::Utility<Entity>::iterate<TransformComponent, TileMapComponent>(entities,
            [&world](Entity e, TransformComponent& transf, TileMapComponent& tmc)
            {
                auto windowPos = world.getState()->getApp().getWindow().mapPixelToCoords(sf::Vector2i{ 0, 0 }, world.getGraph().getCamera().getView());
                windowPos = world.getNode().mapToLocalPosition(windowPos);
                //windowPos = transf.getTransform().transformPoint(windowPos);
                tmc.mTileMap.update(windowPos);
            });
    }

    bool TileMapHandler::setTiles(Entity e, TileData& tiles, unsigned mapSizeX, unsigned mapSizeY)
    {
        bool b = e.modify<TileMapComponent>().mTileMap.setTiles(tiles, mapSizeX, mapSizeY);
        if (b)
            mContentsChangedSignal(e, e.modify<TileMapComponent>().mTileMap.getBounds());
        return b;
    }

    void TileMapHandler::setTile(Entity e, int id, unsigned x, unsigned y)
    {
        e.modify<TileMapComponent>().mTileMap.setTile(id, x, y);
    }

    void TileMapHandler::setTileDims(Entity e,
        unsigned tileWidth, unsigned tileHeight,
        const std::vector<std::string>& keymap)
    {
        const auto& metadata = e.get<SpriteMetadataComponent>();
        e.modify<TileMapComponent>().mTileMap.setMetaMap(metadata.getMetaMap());
        e.modify<TileMapComponent>().mTileMap.setTileDims(tileWidth, tileHeight, keymap);
    }

    void TileMapHandler::addKey(Entity e, const std::string& key)
    {
        const auto& metadata = e.get<SpriteMetadataComponent>();
        e.modify<TileMapComponent>().mTileMap.setMetaMap(metadata.getMetaMap());
        e.modify<TileMapComponent>().mTileMap.addKey(key);
    }

    void TileMapHandler::floodFillTileMap(TileMapComponent& tilemap, unsigned ix, unsigned iy, const std::vector<int>& replacementIDs)
    {
        floodFill(tilemap.mTileMap, ix, iy, replacementIDs);
    }

    TilemapBrush TileMapHandler::makeTilemapBrush(Entity e, const std::string& identifier)
    {
        TilemapBrush brush{ identifier, e.modify<TileMapComponent>().mTileMap };
        brush.setNotificator<detail::TilemapRendererChangeNotificator>();
        return brush;
    }

	void TileMapHandler::setTilemapPosition(Entity e, const sf::Vector2f& position)
	{
		e.modify<TileMapComponent>().mTileMap.setPosition(position); 
		mContentsChangedSignal(e, e.modify<TileMapComponent>().mTileMap.getBounds());
	}
	
	void TileMapHandler::moveTilemap(Entity e, const sf::Vector2f& vec)
	{
		if (e.has<TileMapComponent>())
			e.modify<TileMapComponent>().mTileMap.move(vec);
	}

    void TileMapHandler::tilemapCallback(Entity e, TileMapComponent& tmc, const std::function<void(TileMap&)>& callback)
    {
        callback(tmc.mTileMap);
        mContentsChangedSignal(e, tmc.mTileMap.getBounds());
    }

    void TileMapHandler::viewSizeChanged(const World& world, const sf::Vector2f& viewsize)
    {
        world.getUniverse().iterateOverComponents<TileMapComponent>([viewsize](TileMapComponent& tm)
            {
                tm.mTileMap.setViewSize(viewsize);
            });
    }
}



