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

#include "ungod/content/tilemap/TileMap.h"

namespace ungod
{
    TileMap::TileMap() :
        mMeta(nullptr),
        mTileWidth(0),
        mTileHeight(0),
        mMapSizeX(0),
        mMapSizeY(0)
    {
        mVertices.setPrimitiveType(sf::Quads);
        mHorizontalSize = 0;
        mVerticalSize = 0;
    }


    void TileMap::setMetaMap(const MetaMap& meta)
    {
        mMeta = &meta;
    }


    bool TileMap::update(const sf::Vector2f& windowPosition)
    {
        if (!mTiles.ids)
            return false;

        const auto& ids = *mTiles.ids;

        bool active = false;
        if (mVertices.getVertexCount())
        {
            unsigned metaX = std::max(0, (int)(std::round(windowPosition.x - getPosition().x) / (getScale().x * mTileWidth)));
            unsigned metaY = std::max(0, (int)(std::round(windowPosition.y - getPosition().y) / (getScale().y * mTileHeight)));

            for (unsigned x = metaX; x < metaX + mHorizontalSize && x < mMapSizeX; x++)
                for (unsigned y = metaY; y < metaY + mVerticalSize && y < mMapSizeY; y++)
                {
                    unsigned j = x * mMapSizeY + y;
                    sf::Vertex* quad = &mVertices[(y - metaY + (x - metaX) * mVerticalSize) * 4];

                    if (ids[j] >= 0)
                    {
                        // define the 4 corners
                        quad[0].position = sf::Vector2f((float)(x * mTileWidth), (float)(y * mTileHeight));
                        quad[1].position = sf::Vector2f((float)((x + 1) * mTileWidth), (float)(y * mTileHeight));
                        quad[2].position = sf::Vector2f((float)((x + 1) * mTileWidth), (float)((y + 1) * mTileHeight));
                        quad[3].position = sf::Vector2f((float)(x * mTileWidth), (float)((y + 1) * mTileHeight));

                        // define the 4 texture coordinates
                        quad[0].texCoords = sf::Vector2f((float)(mTilePositions[ids[j]].x),
                            (float)(mTilePositions[ids[j]].y));
                        quad[1].texCoords = sf::Vector2f((float)(mTilePositions[ids[j]].x + mTileWidth),
                            (float)(mTilePositions[ids[j]].y));
                        quad[2].texCoords = sf::Vector2f((float)(mTilePositions[ids[j]].x + mTileWidth),
                            (float)(mTilePositions[ids[j]].y + mTileHeight));
                        quad[3].texCoords = sf::Vector2f((float)(mTilePositions[ids[j]].x),
                            (float)(mTilePositions[ids[j]].y + mTileHeight));

                        quad[0].color = sf::Color::White;
                        quad[1].color = sf::Color::White;
                        quad[2].color = sf::Color::White;
                        quad[3].color = sf::Color::White;

                        active = true;
                    }
                    else  //do not draw this tile
                    {
                        // define the 4 corners
                        quad[0].position = sf::Vector2f(0, 0);
                        quad[1].position = sf::Vector2f(0, 0);
                        quad[2].position = sf::Vector2f(0, 0);
                        quad[3].position = sf::Vector2f(0, 0);

                        // define the 4 texture coordinates
                        quad[0].texCoords = sf::Vector2f(0, 0);
                        quad[1].texCoords = sf::Vector2f(0, 0);
                        quad[2].texCoords = sf::Vector2f(0, 0);
                        quad[3].texCoords = sf::Vector2f(0, 0);

                        // define the 4 texture coordinates
                        quad[0].color = sf::Color::Transparent;
                        quad[1].color = sf::Color::Transparent;
                        quad[2].color = sf::Color::Transparent;
                        quad[3].color = sf::Color::Transparent;
                    }
                }
        }

        return active;
    }


    void TileMap::render(sf::RenderTarget& target, const sf::Texture* tex, sf::RenderStates states) const
    {
        states.transform *= getTransform();
        states.texture = tex;
        target.draw(mVertices, states);
    }


    bool TileMap::setTiles(TileData& tiles, unsigned mapSizeX, unsigned mapSizeY)
    {
        if (!tiles.ids)
        {
            Logger::warning("Invalid tile data.");
            return false;
        }
        if (mapSizeX * mapSizeY != tiles.ids->size())
        {
            Logger::warning("Map dimensions do not fit with the number of given tile ids.");
            return false;
        }
        mTiles = tiles;
        mMapSizeX = mapSizeX;
        mMapSizeY = mapSizeY;
        return true;
    }


    void TileMap::setTile(int id, unsigned x, unsigned y)
    {
        unsigned estimatedPos = x * mMapSizeY + y;
        if (!mTiles.ids || estimatedPos < 0 || estimatedPos >= mTiles.ids->size())
            return;
        (*mTiles.ids)[estimatedPos] = id;
    }


    void TileMap::setTileDims(unsigned tileWidth, unsigned tileHeight,
                           const std::vector<std::string>& keymap)
    {
        mTileWidth = tileWidth;
        mTileHeight = tileHeight;
        mKeymap = keymap;
        mTilePositions.reserve(mKeymap.size());
        for (const auto& key : mKeymap)
            keylookup(key);
    }

    void TileMap::keylookup(const std::string& key)
    {
        if (!mMeta)
        {
            ungod::Logger::warning("No metamap set for the tilemap. Call setMetaMap before adding keys.");
            mTilePositions.emplace_back(0,0); //need to insert a default position to ensure index-consistency
            return;
        }
        MetaNode tilenode = mMeta->getNodeWithKey(key);
        if (tilenode)
        {
            auto data = tilenode.getAttributes<unsigned, unsigned>( {"pos_x", 0u}, {"pos_y", 0u} );
            mTilePositions.emplace_back( std::get<0>(data), std::get<1>(data) );
        }
        else
        {
            ungod::Logger::warning("Cant find a ground tile with key", key, ". \n Is a val id metamap linked?");
            mTilePositions.emplace_back(0, 0); //need to insert a default position to ensure index-consistency
        }
    }

    void TileMap::addKey(const std::string& key)
    {
        mKeymap.emplace_back(key);
        keylookup(key);
    }

    int TileMap::getTileID(const sf::Vector2f& position) const
    {
        auto ti = getTileIndices(position);
        return getTileID(ti.x, ti.y);
    }

    int TileMap::getTileID(unsigned x, unsigned y) const
    {
        unsigned estimatedPos = x * mMapSizeY + y;
        if (!mTiles.ids || estimatedPos < 0 || estimatedPos >= mTiles.ids->size())
            return -1;
        return mTiles.ids->at(estimatedPos);
    }

    sf::Vector2u TileMap::getTileIndices(const sf::Vector2f& position) const
    {
        return {(unsigned)(std::floor((position.x - getPosition().x) / getTileWidth())), (unsigned)(std::floor((position.y - getPosition().y) / getTileHeight()))};
    }

    sf::Vector2f TileMap::getTilePosition(unsigned x, unsigned y) const
    {
        sf::Vector2f worldpos;
        worldpos.x = getPosition().x + x*getScale().x*mTileWidth;
        worldpos.y = getPosition().y + y*getScale().y*mTileHeight;
        return worldpos;
    }

    void TileMap::setWindowSize(const sf::Vector2u& targetsize)
    {
        mHorizontalSize = (unsigned)ceil(targetsize.x / (getScale().x*mTileWidth) ) + 1;
        mVerticalSize = (unsigned)ceil(targetsize.y / (getScale().y*mTileHeight) ) + 1;
        mVertices.resize(mHorizontalSize*mVerticalSize*4);
    }


    sf::FloatRect TileMap::getBounds() const
    {
        return getTransform().transformRect( {sf::Vector2f{0,0}, sf::Vector2f{(float)mTileWidth*mMapSizeX,(float)mTileHeight*mMapSizeY}} );
    }
}
