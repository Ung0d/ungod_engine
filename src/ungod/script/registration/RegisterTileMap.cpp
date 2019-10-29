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

#include "ungod/script/registration/RegisterTileMap.h"
#include "ungod/content/TileMap.h"
#include "ungod/content/FloodFill.h"
#include "ungod/application/Application.h"
#include "ungod/visual/TileMapRenderer.h"
#include "ungod/base/World.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerTileMap(ScriptStateBase& state, Application& app)
        {
            state.registerUsertype<Tile>("Tile", "getTileID", &Tile::getTileID, "getMaterialID", &Tile::getMaterialID);

            state.registerUsertype<TileMap>("TileMap",
                                           "setTiles", sol::overload(
                                           [] (TileMap& ground, script::Environment tiles, script::Environment materials, std::size_t mapX, std::size_t mapY)
                                           {
                                                ground.setTiles(env2vec<int>(tiles), env2vec<unsigned>(materials), mapX, mapY);
                                           },
                                           [] (TileMap& ground, script::Environment tiles, script::Environment materials, script::Environment active, std::size_t mapX, std::size_t mapY)
                                           {
                                                ground.setTiles(env2vec<int>(tiles), env2vec<unsigned>(materials), env2vec<bool>(active), mapX, mapY);
                                           }),
                                           "reserveTileCount", &TileMap::reserveTileCount,
                                           "addTile", sol::overload( [] (TileMap& ground, int id, unsigned material) { ground.addTile(id, material); },
                                                                     [] (TileMap& ground, int id, unsigned material, bool active) { ground.addTile(id, material, active); }),
                                           "loadTiles", [] (TileMap& ground, const std::string& tileID, const std::string& metaID, unsigned cTileWidth, unsigned cTileHeight, script::Environment keymap)
                                           {
                                                ground.loadTiles(tileID, metaID, cTileWidth, cTileHeight, env2vec<std::string>(keymap));
                                           },
                                           "getTileWidth", &TileMap::getTileWidth,
                                           "getTileHeight", &TileMap::getTileHeight,
                                           "getMapSizeX", &TileMap::getMapSizeX,
                                           "getMapSizeY", &TileMap::getMapSizeY,
                                           "getTiledata", [] (TileMap& ground, const sf::Vector2f& pos) { return ground.getTiledata(pos); });

            state.registerUsertype<TileMapRenderer>("TileMapRenderer",
                                                    "reserveTileCount", [] (TileMapRenderer& vm, Entity e, std::size_t num, const unsigned mapSizeX, const unsigned mapSizeY) { vm.reserveTileCount(e, num, mapSizeX, mapSizeY); },
                                                     "addTile", sol::overload(  [] (TileMapRenderer& vm, Entity e, int id, unsigned material, bool active, bool blocked) { vm.addTile(e, id, material, active, blocked); },
                                                                                [] (TileMapRenderer& vm, Entity e, int id, unsigned material, bool active) { vm.addTile(e, id, material, active); },
                                                                                [] (TileMapRenderer& vm, Entity e, int id, unsigned material) { vm.addTile(e, id, material); }),
                                                     "setTiles", sol::overload( [] (TileMapRenderer& vm, Entity e, script::Environment tiles, script::Environment materials, script::Environment active, script::Environment blocked, std::size_t mapX, std::size_t mapY)
                                                                               { vm.setTiles(e, env2vec<int>(tiles), env2vec<unsigned>(materials), env2vec<bool>(active), env2vec<bool>(blocked), mapX, mapY); },
                                                                               [] (TileMapRenderer& vm, Entity e, script::Environment tiles, script::Environment materials, script::Environment active, std::size_t mapX, std::size_t mapY)
                                                                               { vm.setTiles(e, env2vec<int>(tiles), env2vec<unsigned>(materials), env2vec<bool>(active), mapX, mapY); },
                                                                               [] (TileMapRenderer& vm, Entity e, script::Environment tiles, script::Environment materials,std::size_t mapX, std::size_t mapY)
                                                                               { vm.setTiles(e, env2vec<int>(tiles), env2vec<unsigned>(materials), mapX, mapY); } ),
                                                     "loadTiles", [] (TileMapRenderer& vm, Entity e, const std::string& tileID, const std::string& metaID, unsigned cTileWidth, unsigned cTileHeight, script::Environment keymap)
                                                        { vm.loadTiles(e, tileID, metaID, cTileWidth, cTileHeight, env2vec<std::string>(keymap)); },

                                                     "reserveWaterTileCount", [] (TileMapRenderer& vm, Entity e, std::size_t num, const unsigned mapSizeX, const unsigned mapSizeY) { vm.reserveWaterTileCount(e, num, mapSizeX, mapSizeY); },
                                                     "addWaterTile", sol::overload(  [] (TileMapRenderer& vm, Entity e, int id, unsigned material, bool active, bool blocked) { vm.addWaterTile(e, id, material, active, blocked); },
                                                                                [] (TileMapRenderer& vm, Entity e, int id, unsigned material, bool active) { vm.addWaterTile(e, id, material, active); },
                                                                                [] (TileMapRenderer& vm, Entity e, int id, unsigned material) { vm.addWaterTile(e, id, material); }),
                                                     "setWaterTiles", sol::overload( [] (TileMapRenderer& vm, Entity e, script::Environment tiles, script::Environment materials, script::Environment active, script::Environment blocked, std::size_t mapX, std::size_t mapY)
                                                                               { vm.setWaterTiles(e, env2vec<int>(tiles), env2vec<unsigned>(materials), env2vec<bool>(active), env2vec<bool>(blocked), mapX, mapY); },
                                                                               [] (TileMapRenderer& vm, Entity e, script::Environment tiles, script::Environment materials, script::Environment active, std::size_t mapX, std::size_t mapY)
                                                                               { vm.setWaterTiles(e, env2vec<int>(tiles), env2vec<unsigned>(materials), env2vec<bool>(active), mapX, mapY); },
                                                                               [] (TileMapRenderer& vm, Entity e, script::Environment tiles, script::Environment materials,std::size_t mapX, std::size_t mapY)
                                                                               { vm.setWaterTiles(e, env2vec<int>(tiles), env2vec<unsigned>(materials), mapX, mapY); } ),
                                                     "loadWaterTiles", [] (TileMapRenderer& vm, Entity e, const std::string& tileID, const std::string& metaID, unsigned cTileWidth, unsigned cTileHeight, script::Environment keymap)
                                                        { vm.loadWaterTiles(e, tileID, metaID, cTileWidth, cTileHeight, env2vec<std::string>(keymap)); },
                                                     "loadWaterShaders", [&app] (TileMapRenderer& vm, Entity e, const std::string& distortionMap, const std::string& fragmentShader, const std::string& vertexShader)
                                                        { vm.loadWaterShaders(e, distortionMap, fragmentShader, vertexShader, app.getWindow()); },
                                                     "setWaterReflections", [] (TileMapRenderer& vm, Entity e, bool flag) { vm.setWaterReflections(e, flag); },
                                                     "setWaterShaders", [] (TileMapRenderer& vm, Entity e, bool flag) { vm.setWaterShaders(e, flag); },
                                                     "setWaterDistortionFactor", [] (TileMapRenderer& vm, Entity e, float dist) { vm.setWaterDistortionFactor(e, dist); },
                                                     "setWaterFlowFactor", [] (TileMapRenderer& vm, Entity e, float flow) { vm.setWaterFlowFactor(e, flow); },
                                                     "setWaterReflectionOpacity", [] (TileMapRenderer& vm, Entity e, float op) { vm.setWaterReflectionOpacity(e, op); } );

            state.registerUsertype<TileMapComponent>("TileMapComponent", "getTileMap", &TileMapComponent::getTileMap);
            state.registerUsertype<WaterComponent>("WaterComponent", "getWater", &WaterComponent::getWater);

            state.registerFunction("floodFill", &floodFill);
        }
    }
}


