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

#ifndef GROUND_H
#define GROUND_H

#include <SFML/Graphics.hpp>
#include "ungod/visual/Image.h"
#include "ungod/audio/Audio.h"
#include "ungod/serialization/MetaData.h"
#include "ungod/serialization/Serializable.h"
#include "ungod/serialization/SerialTileMap.h"

namespace ungod
{
    /**
    * \brief A struct that holds information about a ground tile.
    */
    struct Tile
    {
    public:
        //constructors
        Tile() : tileID(0), materialID(0u), active(false), blocked(false) {}
        Tile(const int cTileID, const unsigned cMaterialID, const bool cActive, const bool cBlocked) : tileID(cTileID), materialID(cMaterialID), active(cActive), blocked(cBlocked) {}

    private:
        //members
        int tileID;
        unsigned materialID;
        bool active;
        bool blocked;

    public:
        int getTileID() const { return tileID; }
        void setTileID(int newTileID) { tileID = newTileID; }
        void setMaterialID(unsigned newMaterialID) { materialID = newMaterialID; }
        unsigned getMaterialID() const { return materialID; }
        bool isActive() const { return active; }
        void setActive(bool nactive) { active = nactive; }
        bool isBlocked() const { return blocked; }
        void setBlocked(bool nblocked) { blocked = nblocked; }
    };

    /**
    * \brief Class which uses a vertex array to encapsulate a tiled texture.
    * The tilemap can consist of different tiles and renders only the part of
    * them, which intersects the screen. Base of the map is a 2D-vector containing
    * int values representing the number of the tile in the texture (line by line).
    */
    class TileMap : public sf::Transformable, public Serializable<TileMap>
    {
     friend struct SerialBehavior<TileMap>;
    friend class DeserialBehavior<TileMap>;

    private:
        sf::VertexArray mVertices;
        ungod::Image mImage;
        std::vector<sf::Vector2u> mTilePositions;
        MetaMap mTilemeta;
        std::vector< Tile > mTiles;
        std::vector<std::string> mKeymap; //stored for serialization

        unsigned mTileWidth;
        unsigned mTileHeight;
        unsigned mHorizontalSize;
        unsigned mVerticalSize;
        unsigned mMapSizeX;
        unsigned mMapSizeY;

        /** \brief Internal reset of the visible part of the ground. */
        void refineSize(unsigned width, unsigned height);

        void keylookup(const std::string& key);

    public:
        //constructors
        TileMap();
        TileMap(const std::string& tileID, const std::string& metaID,
               unsigned cmTileWidth, unsigned cmTileHeight,
               const std::vector<std::string>& keymap = {});

        /** \brief Returns true if at least one active tile was rendered. */
        bool render(sf::RenderTarget& target, sf::RenderStates states);
        //setup
        bool reserveTileCount(std::size_t num, const unsigned mapSizeX, const unsigned mapSizeY);
        void addTile(int id, unsigned material, bool active = true, bool blocked = false);
        bool setTiles(std::vector<int> tiles, std::vector<unsigned> materials, std::vector<bool> active, std::vector<bool> blocked, const unsigned mapSizeX, const unsigned mapSizeY);
        bool setTiles(std::vector<int> tiles, std::vector<unsigned> materials, std::vector<bool> active, const unsigned mapSizeX, const unsigned mapSizeY);
        bool setTiles(std::vector<int> tiles, std::vector<unsigned> materials, const unsigned mapSizeX, const unsigned mapSizeY);
        void loadTiles(const std::string& tileID, const std::string& metaID,
                       unsigned cTileWidth, unsigned cTileHeight,
                       const std::vector<std::string>& keymap = {});

        //modifiers
        void addKey(const std::string& key);

        //getters
        const unsigned getTileWidth() const {return getScale().x*mTileWidth;}
        const unsigned getTileHeight() const {return getScale().y*mTileHeight;}
        const unsigned getMapSizeX() const {return mMapSizeX;}
        const unsigned getMapSizeY() const {return mMapSizeY;}

        /** \brief Maps a world position to tiledata. */
        Tile const* getTiledata(const sf::Vector2f& position) const;
        Tile const* getTiledata(std::size_t x, std::size_t y) const;
        Tile* getTiledata(const sf::Vector2f& position);
        Tile* getTiledata(std::size_t x, std::size_t y);

        /** \brief Maps a world position to the corresponding tile-indices. Returns {0,0} if the position is not inside the tilemap. */
        sf::Vector2i getTileIndices(const sf::Vector2f& position) const;

        /** \brief Returns the world position of the given tiles. Returns {0,0} if the tile does not exists. */
        sf::Vector2f getTilePosition(unsigned x, unsigned y) const;

        const Image& getImage() const { return mImage; }
        const MetaMap& getMetaMap() const { return mTilemeta; }

        const std::vector<std::string>& getKeyMap() const { return mKeymap; }

        sf::FloatRect getBounds() const;
    };

    /** \brief A utility class that helps to create sound profiles for footstep sounds
    * for different materials of ground tiles. */
    class FootstepSoundProfile
    {
     friend struct SerialBehavior<FootstepSoundProfile>;
    friend class DeserialBehavior<FootstepSoundProfile>;
    public:
        FootstepSoundProfile();
        FootstepSoundProfile(const std::string& materialdata, ungod::AudioManager& audiomanager, ungod::TileMap& ground);

        void loadMaterialData(const std::string& materialdata, ungod::AudioManager& audiomanager, ungod::TileMap& ground);

        void playSoundAt(const sf::Vector2f& position);
    private:
        ungod::MetaList mMaterialData;
        std::vector<ungod::ProfileHandle> mSoundProfiles;
        std::vector<bool> mModifyPitch;
        ungod::AudioManager* mAudiomanager;
        ungod::TileMap* mGround;

    public:
        ~FootstepSoundProfile();
    };
}

#endif // GROUND_H
