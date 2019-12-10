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
			script::Usertype<Tile> tileType = state.registerUsertype<Tile>("Tile");
			tileType["getTileID"] = &Tile::getTileID;
			tileType["getMaterialID"] = & Tile::getMaterialID;

			script::Usertype<TileMap> tilemapType = state.registerUsertype<TileMap>("TileMap");
			tilemapType["reserveTileCount"] = &TileMap::reserveTileCount;
			tilemapType["loadTiles"] = [](TileMap& ground, const std::string& tileID, const std::string& metaID, unsigned cTileWidth, unsigned cTileHeight, script::Environment keymap)
				{
					ground.loadTiles(tileID, metaID, cTileWidth, cTileHeight, env2vec<std::string>(keymap));
				};
			tilemapType["getTileWidth"] = &TileMap::getTileWidth;
			tilemapType["getTileHeight"] = &TileMap::getTileHeight;
			tilemapType["getMapSizeX"] = &TileMap::getMapSizeX;
			tilemapType["getMapSizeY"] = &TileMap::getMapSizeY;
			tilemapType["getTiledata"] = [] (TileMap& ground, const sf::Vector2f& pos) { return ground.getTiledata(pos); };

			script::Usertype<TileMapRenderer> tmRendererType = state.registerUsertype<TileMapRenderer>("TileMapRenderer");
			tmRendererType["reserveTileCount"] = [](TileMapRenderer& vm, Entity e, std::size_t num, const unsigned mapSizeX, const unsigned mapSizeY) { vm.reserveTileCount(e, num, mapSizeX, mapSizeY); };
			tmRendererType["addTile"] = [](TileMapRenderer& vm, Entity e, int id, unsigned material, bool active, bool blocked) { vm.addTile(e, id, material, active, blocked); };
			tmRendererType["setTiles"] = sol::overload([](TileMapRenderer& vm, Entity e, script::Environment tiles, script::Environment materials, script::Environment active, script::Environment blocked, unsigned mapX, unsigned mapY)
					{ vm.setTiles(e, env2vec<int>(tiles), env2vec<unsigned>(materials), env2vec<bool>(active), env2vec<bool>(blocked), mapX, mapY); },
					[](TileMapRenderer& vm, Entity e, script::Environment tiles, script::Environment materials, script::Environment active, unsigned mapX, unsigned mapY)
					{ vm.setTiles(e, env2vec<int>(tiles), env2vec<unsigned>(materials), env2vec<bool>(active), mapX, mapY); });
			tmRendererType["loadTiles"] = [](TileMapRenderer& vm, Entity e, const std::string& tileID, const std::string& metaID, unsigned cTileWidth, unsigned cTileHeight, script::Environment keymap)
			{ vm.loadTiles(e, tileID, metaID, cTileWidth, cTileHeight, env2vec<std::string>(keymap)); };

			tmRendererType["reserveWaterTileCount"] = [](TileMapRenderer& vm, Entity e, std::size_t num, const unsigned mapSizeX, const unsigned mapSizeY) { vm.reserveWaterTileCount(e, num, mapSizeX, mapSizeY); };
			tmRendererType["addWaterTile"] = [](TileMapRenderer& vm, Entity e, int id, unsigned material, bool active, bool blocked) { vm.addWaterTile(e, id, material, active, blocked); };
			tmRendererType["setWaterTiles"] = sol::overload([](TileMapRenderer& vm, Entity e, script::Environment tiles, script::Environment materials, script::Environment active, script::Environment blocked, unsigned mapX, unsigned mapY)
				{ vm.setWaterTiles(e, env2vec<int>(tiles), env2vec<unsigned>(materials), env2vec<bool>(active), env2vec<bool>(blocked), mapX, mapY); },
				[](TileMapRenderer& vm, Entity e, script::Environment tiles, script::Environment materials, script::Environment active, unsigned mapX, unsigned mapY)
				{ vm.setWaterTiles(e, env2vec<int>(tiles), env2vec<unsigned>(materials), env2vec<bool>(active), mapX, mapY); });
			tmRendererType["loadWaterTiles"] = [](TileMapRenderer& vm, Entity e, const std::string& tileID, const std::string& metaID, unsigned cTileWidth, unsigned cTileHeight, script::Environment keymap)
				{ vm.loadWaterTiles(e, tileID, metaID, cTileWidth, cTileHeight, env2vec<std::string>(keymap)); };
			tmRendererType["loadWaterShaders"] = [&app](TileMapRenderer& vm, Entity e, const std::string& distortionMap, const std::string& fragmentShader, const std::string& vertexShader)
				{ vm.loadWaterShaders(e, distortionMap, fragmentShader, vertexShader, app.getWindow()); };
			tmRendererType["setWaterReflections"] = [](TileMapRenderer& vm, Entity e, bool flag) { vm.setWaterReflections(e, flag); };
			tmRendererType["setWaterShaders"] = [](TileMapRenderer& vm, Entity e, bool flag) { vm.setWaterShaders(e, flag); };
			tmRendererType["setWaterDistortionFactor"] = [](TileMapRenderer& vm, Entity e, float dist) { vm.setWaterDistortionFactor(e, dist); };
			tmRendererType["setWaterFlowFactor"] = [](TileMapRenderer& vm, Entity e, float flow) { vm.setWaterFlowFactor(e, flow); };
			tmRendererType["setWaterReflectionOpacity"] = [] (TileMapRenderer& vm, Entity e, float op) { vm.setWaterReflectionOpacity(e, op); };

			script::Usertype<TileMapComponent> tmCompType = state.registerUsertype<TileMapComponent>("TileMapComponent"); 
			tmCompType["getTileMap"] = & TileMapComponent::getTileMap;
			script::Usertype<WaterComponent> wCompType = state.registerUsertype<WaterComponent>("WaterComponent"); 
			wCompType["getWater"] = & WaterComponent::getWater;

            state.registerFunction("floodFill", &floodFill);
        }
    }
}


