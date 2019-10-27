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

#include "ungod/visual/TileMapRenderer.h"
#include "ungod/visual/Camera.h"
#include "ungod/application/Application.h"
#include "ungod/content/FloodFill.h"

namespace ungod
{
    namespace detail
    {
        void TilemapRendererChangeNotificator::notifyTileChanged(Tile* tile, int id) const
        {
            //todo
            //gets called every time a change is made to a tilemap through a brush object that was created by
            //the tilemap renderers "make" fu7nctions
        }
    }

    TileMapRenderer::TileMapRenderer(Application& app)
    {
        mAppSignalLink = app.onTargetSizeChanged([this] (const sf::Vector2u& targetsize)
                                  {
                                      for (auto e = mWaterEntities.begin(); e != mWaterEntities.end(); e++)
                                      {
                                        e->modify<WaterComponent>().mWater.targetsizeChanged(targetsize);
                                      }
                                  });
    }

    void TileMapRenderer::render(sf::RenderTarget& target, sf::RenderStates states, World& world)
    {
        for (const auto& e : mWaterEntities)
        {
            renderWater(e, target, states, world);
        }
        for (const auto& e : mTileMapEntities)
        {
            renderTileMap(e, target, states);
        }
    }

    void TileMapRenderer::renderTileMap(Entity e, sf::RenderTarget& target, sf::RenderStates states)
    {
        e.modify<TileMapComponent>().mTileMap.render(target, states);
    }

    void TileMapRenderer::renderWater(Entity e, sf::RenderTarget& target, sf::RenderStates states, World& world)
    {
        e.modify<WaterComponent>().mWater.render(target, states, world);
    }

    bool TileMapRenderer::reserveTileCount(Entity e, std::size_t num, const unsigned mapSizeX, const unsigned mapSizeY)
    {
        bool b = e.modify<TileMapComponent>().mTileMap.reserveTileCount(num, mapSizeX, mapSizeY);
        return b;
    }

    void TileMapRenderer::addTile(Entity e, int id, unsigned material, bool active, bool blocked)
    {
        e.modify<TileMapComponent>().mTileMap.addTile(id, material, active, blocked);
    }

    void TileMapRenderer::addKey(Entity e, const std::string& key)
    {
        e.modify<TileMapComponent>().mTileMap.addKey(key);
    }

    Tile* TileMapRenderer::getTile(Entity e, const sf::Vector2f& position)
    {
        return e.modify<TileMapComponent>().mTileMap.getTiledata(position);
    }

    Tile* TileMapRenderer::getTile(Entity e, std::size_t x, std::size_t y)
    {
        return e.modify<TileMapComponent>().mTileMap.getTiledata(x, y);
    }

    bool TileMapRenderer::setTiles(Entity e, std::vector<int> tiles, std::vector<unsigned> materials,
                             std::vector<bool> active, std::vector<bool> blocked, const unsigned mapSizeX, const unsigned mapSizeY)
    {
        bool b = e.modify<TileMapComponent>().mTileMap.setTiles(tiles, materials, active, blocked, mapSizeX, mapSizeY);
        return b;
    }

    bool TileMapRenderer::setTiles(Entity e, std::vector<int> tiles, std::vector<unsigned> materials,
                         std::vector<bool> active, const unsigned mapSizeX, const unsigned mapSizeY)
    {
        bool b = e.modify<TileMapComponent>().mTileMap.setTiles(tiles, materials, active, mapSizeX, mapSizeY);
        return b;
    }

    bool TileMapRenderer::setTiles(Entity e, std::vector<int> tiles, std::vector<unsigned> materials,
                  const unsigned mapSizeX, const unsigned mapSizeY)
    {
        bool b = e.modify<TileMapComponent>().mTileMap.setTiles(tiles, materials, mapSizeX, mapSizeY);
        return b;
    }

    void TileMapRenderer::loadTiles(Entity e, const std::string& tileID, const std::string& metaID,
                       unsigned cTileWidth, unsigned cTileHeight,
                       const std::vector<std::string>& keymap)
   {
        e.modify<TileMapComponent>().mTileMap.loadTiles(tileID, metaID, cTileWidth, cTileHeight, keymap);
   }


   bool TileMapRenderer::reserveWaterTileCount(Entity e, std::size_t num, const unsigned mapSizeX, const unsigned mapSizeY)
    {
        bool b = e.modify<WaterComponent>().mWater.getTileMap().reserveTileCount(num, mapSizeX, mapSizeY);
        return b;
    }

    void TileMapRenderer::addWaterTile(Entity e, int id, unsigned material, bool active, bool blocked)
    {
        e.modify<WaterComponent>().mWater.getTileMap().addTile(id, material, active, blocked);
    }

    void TileMapRenderer::addWaterKey(Entity e, const std::string& key)
    {
        e.modify<WaterComponent>().mWater.getTileMap().addKey(key);
    }


    Tile* TileMapRenderer::getWaterTile(Entity e, const sf::Vector2f& position)
    {
        return e.modify<WaterComponent>().mWater.getTileMap().getTiledata(position);
    }

    Tile* TileMapRenderer::getWaterTile(Entity e, std::size_t x, std::size_t y)
    {
        return e.modify<WaterComponent>().mWater.getTileMap().getTiledata(x, y);
    }

    bool TileMapRenderer::setWaterTiles(Entity e, std::vector<int> tiles, std::vector<unsigned> materials,
                             std::vector<bool> active, std::vector<bool> blocked, const unsigned mapSizeX, const unsigned mapSizeY)
    {
        bool b = e.modify<WaterComponent>().mWater.getTileMap().setTiles(tiles, materials, active, blocked, mapSizeX, mapSizeY);
        return b;
    }

    bool TileMapRenderer::setWaterTiles(Entity e, std::vector<int> tiles, std::vector<unsigned> materials,
                         std::vector<bool> active, const unsigned mapSizeX, const unsigned mapSizeY)
    {
        bool b = e.modify<WaterComponent>().mWater.getTileMap().setTiles(tiles, materials, active, mapSizeX, mapSizeY);
        return b;
    }

    bool TileMapRenderer::setWaterTiles(Entity e, std::vector<int> tiles, std::vector<unsigned> materials,
                  const unsigned mapSizeX, const unsigned mapSizeY)
    {
        bool b = e.modify<WaterComponent>().mWater.getTileMap().setTiles(tiles, materials, mapSizeX, mapSizeY);
        return b;
    }

    void TileMapRenderer::loadWaterTiles(Entity e, const std::string& tileID, const std::string& metaID,
                       unsigned cTileWidth, unsigned cTileHeight,
                       const std::vector<std::string>& keymap)
   {
        e.modify<WaterComponent>().mWater.loadTiles(tileID, metaID, cTileWidth, cTileHeight, keymap);
   }

    void TileMapRenderer::loadWaterShaders(WaterComponent& water, const std::string& distortionMap,
                      const std::string& fragmentShader, const std::string& vertexShader,
                      const sf::RenderTarget& target)
   {
      water.mWater.loadShaders(distortionMap, fragmentShader, vertexShader, target);
   }

   void TileMapRenderer::setWaterReflections(WaterComponent& water, bool flag)
   {
        water.mWater.setReflections(flag);
   }

    void TileMapRenderer::setWaterShaders(WaterComponent& water, bool flag)
    {
        water.mWater.setShaders(flag);
    }

    void TileMapRenderer::setWaterDistortionFactor(WaterComponent& water, float distortion)
    {
        water.mWater.setDistortionFactor(distortion);
    }

    void TileMapRenderer::setWaterFlowFactor(WaterComponent& water, float flow)
    {
        water.mWater.setFlowFactor(flow);
    }

    void TileMapRenderer::setWaterReflectionOpacity(WaterComponent& water, float opacity)
    {
        water.mWater.setReflectionOpacity(opacity);
    }

    void TileMapRenderer::floodFillTileMap(TileMapComponent& tilemap, std::size_t ix, std::size_t iy, const std::vector<int>& replacementIDs, bool activate)
    {
        floodFill(tilemap.mTileMap, ix, iy, replacementIDs, activate);
    }

    void TileMapRenderer::floodFillWater(WaterComponent& water, std::size_t ix, std::size_t iy, const std::vector<int>& replacementIDs, bool activate)
    {
        floodFill(water.mWater.getTileMap(), ix, iy, replacementIDs, activate);
    }

    void TileMapRenderer::setTileMap(TileMapComponent& tilemapc, const TileMap& tilemap)
    {
        tilemapc.mTileMap = tilemap;
    }

    void TileMapRenderer::setWater(WaterComponent& waterc, const Water& water)
    {
        waterc.mWater = water;
    }

    void TileMapRenderer::handleTileMapAdded(Entity e)
    {
        mTileMapEntities.emplace_back(e);
    }

    void TileMapRenderer::handleTileMapRemoved(Entity e)
    {
        mTileMapEntities.erase( std::remove( mTileMapEntities.begin(), mTileMapEntities.end(), e ), mTileMapEntities.end() );
    }

    void TileMapRenderer::handleWaterAdded(Entity e)
    {
        mWaterEntities.emplace_back(e);
    }

    void TileMapRenderer::handleWaterRemoved(Entity e)
    {
        mWaterEntities.erase( std::remove( mWaterEntities.begin(), mWaterEntities.end(), e ), mWaterEntities.end() );
    }

    TilemapBrush TileMapRenderer::makeTilemapBrush(Entity e, const std::string& identifier)
    {
        TilemapBrush brush{ identifier, e.modify<TileMapComponent>().mTileMap };
        brush.setNotificator<detail::TilemapRendererChangeNotificator>();
        return brush;
    }

    TilemapBrush TileMapRenderer::makeWaterBrush(Entity e, const std::string& identifier)
    {
        TilemapBrush brush{ identifier, e.modify<WaterComponent>().mWater.getTileMap() };
        brush.setNotificator<detail::TilemapRendererChangeNotificator>();
        return brush;
    }

    TileMapRenderer::~TileMapRenderer()
    {
        mAppSignalLink.disconnect();
    }
}

