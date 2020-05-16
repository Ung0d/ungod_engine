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

#ifndef UNGOD_TILEMAP_H
#define UNGOD_TILEMAP_H

#include <SFML/Graphics.hpp>
#include "ungod/serialization/Serializable.h"
#include <memory>

namespace ungod
{
    struct TileData
    {
        std::unique_ptr<std::vector<int>> ids;
    };

    /**
    * \brief Class which uses a vertex array to encapsulate a tiled texture.
    * The tilemap can consist of different tiles and renders only the part of
    * them, which intersects the screen. Base of the map is a 2D-vector containing
    * int values representing the number of the tile in the texture (line by line).
    */
    class TileMap : public sf::Transformable : public Serializable<TileMap>
    {
    friend struct SerialBehavior<TileMap>;
    friend struct DeserialBehavior<TileMap>;
    private:
        sf::VertexArray mVertices; ///<contains only as much vertices, as required to cover the screen area
        std::vector<sf::Vector2u> mTilePositions; ///<tex positions for each tile type
        std::vector<std::string> mKeymap; ///<keys for each tile type
        TileData mTiles; ///<stores tile data for the whole map

        unsigned mTileWidth;
        unsigned mTileHeight;
        unsigned mHorizontalSize;
        unsigned mVerticalSize;
        unsigned mMapSizeX;
        unsigned mMapSizeY;

        const MetaMap& mMeta;

        /** \brief Internal reset of the visible part of the ground. */
        void refineSize(unsigned width, unsigned height);

        void keylookup(const MetaMap& meta, const std::string& key);

    public:
        //constructors
        TileMap();

        /** \brief Links a meta map to the tile maps that provides texture positions given string keys. */
        void setMetaMap(const MetaMap& meta);

        /** \brief Returns true if at least one active tile was rendered. */
        bool render(sf::RenderTarget& target, const sf::Texture* tex, sf::RenderStates states);

        /** Initializes the map by setting the given tiles and dimensions. The product of dimensions has to match the number of tiles. 
        * The tilemap takes ownership of the tiledata. */
        bool setTiles(TileData& tiles, unsigned mapSizeX, unsigned mapSizeY);

        /** \brief Sets the id of the given tile. */
        void setTile(int id, unsigned x, unsigned y);

        /** \brief Sets the key data that connects tile ids to texture infos. */
        void setTileDims(unsigned tileWidth, unsigned tileHeight,
                       const std::vector<std::string>& keymap = {});

        /** \brief Adds a single new key to the end of the current keymap. */
        void addKey(const std::string& key);

        //getters
        float getTileWidth() const {return getScale().x*mTileWidth;}
        float getTileHeight() const {return getScale().y*mTileHeight;}
        unsigned getMapSizeX() const {return mMapSizeX;}
        unsigned getMapSizeY() const {return mMapSizeY;}

        /** \brief Maps a position to tiledata. */
        int getTileID(const sf::Vector2f& position) const;

        /** \brief Maps a position to tiledata. */
        int getTileID(unsigned x, unsigned y) const;

        /** \brief Maps a position to the corresponding tile-indices. Returns {0,0} if the position is not inside the tilemap. */
        sf::Vector2u getTileIndices(const sf::Vector2f& position) const;

        /** \brief Returns the position of the given tiles. Returns {0,0} if the tile does not exists. */
        sf::Vector2f getTilePosition(unsigned x, unsigned y) const;

        const std::vector<std::string>& getKeyMap() const { return mKeymap; }

        sf::FloatRect getBounds() const;
    };
}

#endif // UNGOD_TILEMAP_H
