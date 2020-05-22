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

#ifndef REGISTER_TILE_MAP_H
#define REGISTER_TILE_MAP_H

#include "ungod/script/Behavior.h"
#include <SFML/System/Vector2.hpp>

namespace ungod
{
    class Application;
    class Entity;
    class TileMapHandler;

    namespace scriptRegistration
    {
        class TileMapHandlerFrontEnd
        {
        public:
            TileMapHandlerFrontEnd(Entity& e, TileMapHandler& h) : mEntity(e), mHandler(h) {}
            float getTileWidth() const;
            float getTileHeight() const;
            unsigned getMapWidth() const;
            unsigned getMapHeight() const;
            int getTileID(const sf::Vector2f& pos) const;
            void setTiles(script::Environment tiles, unsigned mapX, unsigned mapY);
            void setTile(int id, unsigned x, unsigned y);
            void setTileDims(unsigned tileWidth, unsigned tileHeight, script::Environment keymap);
            void addKey(const std::string& key);
            void floodFill(unsigned ix, unsigned iy, script::Environment replacementIDs);
            void setPosition(const sf::Vector2f& position);
        private:
            Entity& mEntity;
            TileMapHandler& mHandler;
        };

        /** \brief Registers ground functionality for scripts. */
        void registerTileMap(ScriptStateBase& state);
    }
}

#endif



