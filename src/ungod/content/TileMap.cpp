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

#include "ungod/content/TileMap.h"
#include "ungod/audio/VolumeSettings.h"

namespace ungod
{
    TileMap::TileMap() :
        mTiles(),
        mTileWidth(0),
        mTileHeight(0),
        mMapSizeX(0),
        mMapSizeY(0)
    {
        mVertices.setPrimitiveType(sf::Quads);
        mHorizontalSize = 0;
        mVerticalSize = 0;
    }


    TileMap::TileMap(const std::string& tileID, const std::string& metaID,
                   unsigned tileWidth, unsigned tileHeight,
                   const std::vector<std::string>& keymap) :
        mImage(),
        mTileWidth(0),
        mTileHeight(0),
        mMapSizeX(0),
        mMapSizeY(0)
    {
        loadTiles(tileID, metaID, tileWidth, tileHeight, keymap);
    }


    bool TileMap::render(sf::RenderTarget& target, sf::RenderStates states)
    {
        refineSize((unsigned)target.getView().getSize().x, (unsigned)target.getView().getSize().y);

        sf::Vector2f windowPosition = target.mapPixelToCoords(sf::Vector2i(0,0));
		windowPosition = states.transform.getInverse().transformPoint(windowPosition);

        bool active = false;
        if (mVertices.getVertexCount() && mImage.get()->getSize().x > 0)
        {
            unsigned metaX = std::max(0, (int)(std::round(windowPosition.x - getPosition().x) / (getScale().x*mTileWidth)));
            unsigned metaY = std::max(0, (int)(std::round(windowPosition.y - getPosition().y) / (getScale().y*mTileHeight)));

            for (unsigned x = metaX; x < metaX + mHorizontalSize && x < mMapSizeX; x++)
                for (unsigned y = metaY; y < metaY + mVerticalSize && y < mMapSizeY; y++)
            {
                unsigned j = x*mMapSizeY + y;
                sf::Vertex* quad = &mVertices[(y - metaY + (x - metaX) * mVerticalSize) * 4];

                if (mTiles[j].isActive())
                {
                    // define the 4 corners
                    quad[0].position = sf::Vector2f((float)(x * mTileWidth), (float)(y * mTileHeight));
                    quad[1].position = sf::Vector2f((float)((x + 1) * mTileWidth), (float)(y * mTileHeight));
                    quad[2].position = sf::Vector2f((float)((x + 1) * mTileWidth), (float)((y + 1) * mTileHeight));
                    quad[3].position = sf::Vector2f((float)(x * mTileWidth), (float)((y + 1) * mTileHeight));

                    // define the 4 texture coordinates
                    quad[0].texCoords = sf::Vector2f((float)(mTilePositions[mTiles[j].getTileID()].x), 
														(float)(mTilePositions[mTiles[j].getTileID()].y));
                    quad[1].texCoords = sf::Vector2f((float)(mTilePositions[mTiles[j].getTileID()].x + mTileWidth), 
						(float)(mTilePositions[mTiles[j].getTileID()].y));
                    quad[2].texCoords = sf::Vector2f((float)(mTilePositions[mTiles[j].getTileID()].x + mTileWidth), 
						(float)(mTilePositions[mTiles[j].getTileID()].y + mTileHeight));
                    quad[3].texCoords = sf::Vector2f((float)(mTilePositions[mTiles[j].getTileID()].x), 
													(float)(mTilePositions[mTiles[j].getTileID()].y + mTileHeight));

                    quad[0].color = sf::Color::White;
                    quad[1].color = sf::Color::White;
                    quad[2].color = sf::Color::White;
                    quad[3].color = sf::Color::White;

                    active = true;
                }
                else  //do not draw this tile
                {
                    // define the 4 corners
                    quad[0].position = sf::Vector2f(0,0);
                    quad[1].position = sf::Vector2f(0,0);
                    quad[2].position = sf::Vector2f(0,0);
                    quad[3].position = sf::Vector2f(0,0);

                    // define the 4 texture coordinates
                    quad[0].texCoords = sf::Vector2f(0,0);
                    quad[1].texCoords = sf::Vector2f(0,0);
                    quad[2].texCoords = sf::Vector2f(0,0);
                    quad[3].texCoords = sf::Vector2f(0,0);

                    // define the 4 texture coordinates
                    quad[0].color = sf::Color::Transparent;
                    quad[1].color = sf::Color::Transparent;
                    quad[2].color = sf::Color::Transparent;
                    quad[3].color = sf::Color::Transparent;
                }
            }
        }

        states.transform *= getTransform();
        states.texture = mImage.get();
        target.draw(mVertices, states);

        return active;
    }

    bool TileMap::reserveTileCount(std::size_t num, const unsigned mapSizeX, const unsigned mapSizeY)
    {
        if (mapSizeX*mapSizeY != num)
        {
            Logger::warning("Map dimensions do not fit with the number of given tile ids.");
            Logger::endl();
            return false;
        }
        mTiles.reserve(num);
        mMapSizeX = mapSizeX;
        mMapSizeY = mapSizeY;
        return true;
    }

    void TileMap::addTile(int id, bool active)
    {
        mTiles.emplace_back(id, active);
    }

    bool TileMap::setTiles(std::vector<int> tiles, std::vector<bool> active, const unsigned mapSizeX, const unsigned mapSizeY)
    {
        if (tiles.size() != active.size())
        {
            Logger::warning("Can't set ground tiles. Tile-ids-vector and active-vector are not of the same size.");
            Logger::endl();
            return false;
        }

        if (!reserveTileCount(tiles.size(), mapSizeX, mapSizeY))
            return false;

        for (std::size_t i = 0; i < tiles.size(); ++i)
            addTile(tiles[i], active[i]);

        return true;
    }

    bool TileMap::setTiles(std::vector<int> tiles, const unsigned mapSizeX, const unsigned mapSizeY)
    {
        if (!reserveTileCount(tiles.size(), mapSizeX, mapSizeY))
            return false;

        for (std::size_t i = 0; i < tiles.size(); ++i)
            addTile(tiles[i], true);

        return true;
    }

    void TileMap::loadTiles(const std::string& tileID, const std::string& metaID,
                           unsigned tileWidth, unsigned tileHeight,
                           const std::vector<std::string>& keymap)
    {
        mImage.load(tileID, LoadPolicy::ASYNC);
        mTileWidth = tileWidth;
        mTileHeight = tileHeight;

        mKeymap = keymap;

        mTilemeta.load(metaID);

        mTilePositions.reserve(mKeymap.size());

        for (const auto& key : mKeymap)
        {
            keylookup(key);
        }
    }

    void TileMap::keylookup(const std::string& key)
    {
        MetaNode tilenode = mTilemeta.getNodeWithKey(key);
        if (tilenode)
        {
            auto data = tilenode.getAttributes<unsigned, unsigned>( {"pos_x", 0u}, {"pos_y", 0u} );
            mTilePositions.emplace_back( std::get<0>(data), std::get<1>(data) );
        }
        else
        {
            ungod::Logger::warning("Cant find a ground tile with key");
            ungod::Logger::warning(key);
            ungod::Logger::endl();
            mTilePositions.emplace_back(0,0); //need to insert a default position to ensure index-consistency
        }
    }

    void TileMap::addKey(const std::string& key)
    {
        mKeymap.push_back(key);
        keylookup(key);
    }


    Tile const* TileMap::getTiledata(const sf::Vector2f& position) const
    {
        auto ti = getTileIndices(position);
        return getTiledata(ti.x, ti.y);
    }

    Tile const* TileMap::getTiledata(unsigned x, unsigned y) const
    {
        unsigned estimatedPos = x*mMapSizeY + y;

        if (estimatedPos < 0 || estimatedPos >= mTiles.size())
            return nullptr;

       return &mTiles[estimatedPos];
    }

    Tile* TileMap::getTiledata(const sf::Vector2f& position)
    {
        return const_cast<Tile*>(static_cast<const TileMap*>(this)->getTiledata(position));
    }

    Tile* TileMap::getTiledata(unsigned x, unsigned y)
    {
        return const_cast<Tile*>(static_cast<const TileMap*>(this)->getTiledata(x, y));
    }

    sf::Vector2i TileMap::getTileIndices(const sf::Vector2f& position) const
    {
        return {(int)(std::floor((position.x - getPosition().x) / getTileWidth())), (int)(std::floor((position.y - getPosition().y) / getTileHeight()))};
    }

    sf::Vector2f TileMap::getTilePosition(unsigned x, unsigned y) const
    {
        sf::Vector2f worldpos;
        worldpos.x = getPosition().x + x*getScale().x*mTileWidth;
        worldpos.y = getPosition().y + y*getScale().y*mTileHeight;
        return worldpos;
    }

    void TileMap::refineSize(unsigned width, unsigned height)
    {
        mHorizontalSize = (unsigned)ceil( width / (getScale().x*mTileWidth) ) + 1;
        mVerticalSize = (unsigned)ceil( height / (getScale().y*mTileHeight) ) + 1;
        if (mVertices.getVertexCount() != mHorizontalSize*mVerticalSize*4)
            mVertices.resize(mHorizontalSize*mVerticalSize*4);
    }


    sf::FloatRect TileMap::getBounds() const
    {
        return getTransform().transformRect( {sf::Vector2f{0,0}, sf::Vector2f{(float)mTileWidth*mMapSizeX,(float)mTileHeight*mMapSizeY}} );
    }



    FootstepSoundProfile::FootstepSoundProfile() : mAudiomanager(nullptr), mGround(nullptr) {}

    FootstepSoundProfile::FootstepSoundProfile(const std::string& materialdata, ungod::AudioManager& audiomanager, ungod::TileMap& ground)
    {
        loadMaterialData(materialdata, audiomanager, ground);
    }

    void FootstepSoundProfile::loadMaterialData(const std::string& materialdata, ungod::AudioManager& audiomanager, ungod::TileMap& ground)
    {
        mAudiomanager = &audiomanager;
        mGround = &ground;
        mMaterialData.load("material_data.xml");
        for (unsigned i = 0; i < mMaterialData.getNodeCount(); ++i)
        {
            ungod::MetaNode material = mMaterialData.getNodeAt(i);
            mSoundProfiles.push_back( mAudiomanager->initSoundProfile(material.name()) );
            mModifyPitch.push_back(material.getAttribute<bool>("modify_pitch", false));
            std::vector<std::string> filepaths;
            ungod::forEachSubnode(material, [&filepaths] (ungod::MetaNode sound)
              {
                auto data = sound.getAttribute<std::string>("filepath", "");
                filepaths.push_back(data);
              }, "sound");
            mAudiomanager->initSounds(mSoundProfiles.back(), filepaths.size());
            for (unsigned j = 0; j < filepaths.size(); ++j)
            {
                mAudiomanager->loadSound(mSoundProfiles.back(), filepaths[j], j);
            }
        }
    }

    void FootstepSoundProfile::playSoundAt(const sf::Vector2f& position)
    {
       /* Tile const* tile = mGround->getTiledata(position);
        if (tile)
        {
            if (!(tile->getMaterialID() < mSoundProfiles.size()))
            {
                ungod::Logger::error("No corresponding sound-profile is loaded for material id: ");
                ungod::Logger::error(tile->getMaterialID());
                ungod::Logger::endl();
                return;
            }

            std::size_t index = 0;
            float pitch = 1.0f;

            if (mSoundProfiles[tile->getMaterialID()].getSoundCount() > 1) //play a random sound
            {
                index = NumberGenerator::getRandBetw(0, (int)mSoundProfiles[tile->getMaterialID()].getSoundCount()-1);
            }

            if (mModifyPitch[tile->getMaterialID()])
            {
                pitch = NumberGenerator::getFloatRandBetw(0.85f, 1.15f);
            }

            mAudiomanager->playSound(mSoundProfiles[tile->getMaterialID()], index, 1.0f, VolumeSettings::FOOTSTEPS, pitch);
        } */
    }

    FootstepSoundProfile::~FootstepSoundProfile()
    {
       /* for (const auto& profile : mSoundProfiles)
        {
            mAudiomanager->expireSounds(profile);
        } */
    }



    void floodFill(TileMap& tilemap, std::size_t ix, std::size_t iy, const std::vector<int>& replacementIDs, bool activate);
}
