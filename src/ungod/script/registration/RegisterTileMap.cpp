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
#include "ungod/content/tilemap/TileMap.h"
#include "ungod/content/tilemap/FloodFill.h"
#include "ungod/content/tilemap/TileMapHandler.h"
#include "ungod/application/Application.h"
#include "ungod/base/World.h"

namespace ungod
{
    namespace scriptRegistration
    {
		float TileMapHandlerFrontEnd::getTileWidth() const
		{
			return mEntity.get<TileMapComponent>().getTileMap().getTileWidth();
		}

		float TileMapHandlerFrontEnd::getTileHeight() const
		{
			return mEntity.get<TileMapComponent>().getTileMap().getTileHeight();
		}

		unsigned TileMapHandlerFrontEnd::getMapWidth() const
		{
			return mEntity.get<TileMapComponent>().getTileMap().getMapSizeX();
		}

		unsigned TileMapHandlerFrontEnd::getMapHeight() const
		{
			return mEntity.get<TileMapComponent>().getTileMap().getMapSizeY();
		}

		int TileMapHandlerFrontEnd::getTileID(const sf::Vector2f& pos) const
		{
			sf::Vector2f posLocal = mEntity.get<TransformComponent>().getTransform().getInverse().transformPoint(pos);
			return mEntity.get<TileMapComponent>().getTileMap().getTileID(posLocal);
		}

		void TileMapHandlerFrontEnd::setTiles(script::Environment tiles, unsigned mapX, unsigned mapY)
		{
			TileData data;
			data.ids = std::make_shared<std::vector<int>>();
			env2vec<int>(tiles, *data.ids);
			mHandler.setTiles(mEntity, data, mapX, mapY);
		}

		void TileMapHandlerFrontEnd::setTile(int id, unsigned x, unsigned y)
		{
			mHandler.setTile(mEntity, id, x, y);
		}

		void TileMapHandlerFrontEnd::setTileDims(unsigned tileWidth, unsigned tileHeight, script::Environment keymap)
		{
			mHandler.setTileDims(mEntity, tileWidth, tileHeight, env2vec<std::string>(keymap));
		}

		void TileMapHandlerFrontEnd::addKey(const std::string& key)
		{
			mHandler.addKey(mEntity, key);
		}

		void TileMapHandlerFrontEnd::floodFill(unsigned ix, unsigned iy, script::Environment replacementIDs)
		{
			mHandler.floodFillTileMap(mEntity, ix, iy, env2vec<int>(replacementIDs));
		}

		void TileMapHandlerFrontEnd::setPosition(const sf::Vector2f& position)
		{
			mHandler.setTilemapPosition(mEntity, position);
		}


        void registerTileMap(ScriptStateBase& state)
        {
			script::Usertype<TileMapHandlerFrontEnd> tmHandlerType = state.registerUsertype<TileMapHandlerFrontEnd>("TileMapHandlerFrontEnd");
			tmHandlerType["getTileWidth"] = &TileMapHandlerFrontEnd::getTileWidth;
			tmHandlerType["getTileHeight"] = &TileMapHandlerFrontEnd::getTileHeight;
			tmHandlerType["getMapSizeX"] = &TileMapHandlerFrontEnd::getMapWidth;
			tmHandlerType["getMapSizeY"] = &TileMapHandlerFrontEnd::getMapHeight;
			tmHandlerType["getTileID"] = &TileMapHandlerFrontEnd::getTileID;
			tmHandlerType["setTiles"] = &TileMapHandlerFrontEnd::setTiles;
			tmHandlerType["setTile"] = &TileMapHandlerFrontEnd::setTile;
			tmHandlerType["addKey"] = &TileMapHandlerFrontEnd::addKey;
			tmHandlerType["setTileDims"] = &TileMapHandlerFrontEnd::setTileDims;
			tmHandlerType["floodfill"] = &TileMapHandlerFrontEnd::floodFill;
			tmHandlerType["setPosition"] = &TileMapHandlerFrontEnd::setPosition;
        }
    }
}


