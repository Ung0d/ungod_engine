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
        mMapSizeY(0),
        mAltered(true),
        mLastMetaX(0),
        mLastMetaY(0),
        mOverextendX(0),
        mOverextendY(0)
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
            int metaX = (int)(std::round(windowPosition.x - getPosition().x) / (getScale().x * mTileWidth)) - (int)mOverextendX;
            int metaY = (int)(std::round(windowPosition.y - getPosition().y) / (getScale().y * mTileHeight)) - (int)mOverextendY;

            if (!mAltered && 
                (unsigned)std::abs(mLastMetaX - metaX) < mOverextendX  && 
                (unsigned)std::abs(mLastMetaY - metaY) < mOverextendY)
                return true;

            mLastMetaX = metaX;
            mLastMetaY = metaY;
            mAltered = false;

            unsigned cutMetaX = (unsigned)std::max(0, metaX);
            unsigned cutMetaY = (unsigned)std::max(0, metaY);

            for (unsigned x = cutMetaX; x < cutMetaX + mHorizontalSize && x < mMapSizeX; x++)
                for (unsigned y = cutMetaY; y < cutMetaY + mVerticalSize && y < mMapSizeY; y++)
                {
                    unsigned j = x * mMapSizeY + y;
                    sf::Vertex* quad = &mVertices[(y - cutMetaY + (x - cutMetaX) * mVerticalSize) * 4];

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


    bool TileMap::setTiles(const TileData& tiles, unsigned mapSizeX, unsigned mapSizeY)
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
        mAltered = true;
        return true;
    }


    void TileMap::setTile(int id, unsigned x, unsigned y)
    {
        unsigned estimatedPos = x * mMapSizeY + y;
        if (!mTiles.ids || estimatedPos < 0 || estimatedPos >= mTiles.ids->size())
            return;
        (*mTiles.ids)[estimatedPos] = id;
        mAltered = true;
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
        mAltered = true;
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
        mAltered = true;
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

    sf::Vector2i TileMap::getTileIndices(const sf::Vector2f& position) const
    {
        sf::Vector2i indices{ (int)(std::floor((position.x - getPosition().x) / getTileWidth())), (int)(std::floor((position.y - getPosition().y) / getTileHeight())) };
        indices.x = std::max(indices.x, -1);
        indices.y = std::max(indices.y, -1);
        indices.x = std::min(indices.x, (int)mMapSizeX-1);
        indices.y = std::min(indices.y, (int)mMapSizeY-1);
        return indices;
    }

    sf::Vector2f TileMap::getTilePosition(unsigned x, unsigned y) const
    {
        sf::Vector2f worldpos;
        worldpos.x = getPosition().x + x*getScale().x*mTileWidth;
        worldpos.y = getPosition().y + y*getScale().y*mTileHeight;
        return worldpos;
    }

    void TileMap::setViewSize(const sf::Vector2f& targetsize)
    {
        unsigned hs = (unsigned)ceil(targetsize.x / (getScale().x*mTileWidth) ) + 1;
        unsigned vs = (unsigned)ceil(targetsize.y / (getScale().y*mTileHeight) ) + 1;
        unsigned ox = std::min(100, (int)std::floor(hs / 5));
        unsigned oy = std::min(80, (int)std::floor(vs / 4));
        if (hs + 2*ox == mHorizontalSize && 
            vs + 2*oy == mVerticalSize)
            return;
        mOverextendX = ox;
        mOverextendY = oy;
        mHorizontalSize = hs + 2*ox;
        mVerticalSize = vs + 2*oy;
        mVertices.resize(mHorizontalSize*mVerticalSize*4);
        mAltered = true;
    }


    sf::FloatRect TileMap::getBounds() const
    {
        return getTransform().transformRect( {sf::Vector2f{0,0}, sf::Vector2f{(float)mTileWidth*mMapSizeX,(float)mTileHeight*mMapSizeY}} );
    }


    void TileMap::extend(unsigned leftExtend, unsigned topExtend, unsigned rightExtend, unsigned bottomExtend, int id)
    {
        if (!mTiles.ids) return;
        //y requires insertions at every mapSizeY-th position...
        std::vector<int> top, bottom;
        top.resize(topExtend, id);
        bottom.resize(bottomExtend, id);
        for (unsigned x = 0; x < mMapSizeX; x++)
        {
            mTiles.ids->insert(mTiles.ids->begin() + x * (mMapSizeY + topExtend + bottomExtend), 
                top.begin(), top.end());
            mTiles.ids->insert(mTiles.ids->begin() + ((x+1) * (mMapSizeY + topExtend + bottomExtend) - bottomExtend),
                bottom.begin(), bottom.end());
        }
        mMapSizeY += topExtend + bottomExtend;
        //...whereras x extensions are easy
        std::vector<int> left, right;
        left.resize(leftExtend * mMapSizeY, -1);
        right.resize(rightExtend * mMapSizeY, -1);
        mTiles.ids->insert(mTiles.ids->begin(), left.begin(), left.end());
        mTiles.ids->insert(mTiles.ids->end(), right.begin(), right.end());
        mMapSizeX += leftExtend + rightExtend;
        mAltered = true;
    }
}
