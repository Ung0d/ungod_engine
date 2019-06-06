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

#ifndef UNGOD_TMBRUSH_H
#define UNGOD_TMBRUSH_H

#include "ungod/content/TileMap.h"
#include <unordered_map>
#include <functional>

namespace ungod
{

    /**
    * \brief A brush for tilemaps that paints a type of tile on top of another. Fitting border tiles are automatically
    * set depending of adjacent tiles. Brushes require a specific naming of the brush tiles. Let X be the name of the
    * brush, then the following naming is expected:
    *
    *
    *
    * IMPORTANT: The metadata file should not contain any other items beginning with X. A foreign tile for a brush set is
    * defined as not starting with X.
    */
    class TilemapBrush
    {
        public:
            /** \brief default constructor for later initialization. */
            TilemapBrush() : mLast(false), mBasic(-1), mIso(-1), mCross(-1), mPathH(-1), mPathV(-1), mDiag1(-1), mDiag2(-1),
                             mPathEnd{-1,-1,-1,-1}, mCorner{-1,-1,-1,-1}, mInnerCorner{-1,-1,-1,-1}, mTShape{-1,-1,-1,-1},
                             mTrans2path{-1,-1,-1,-1}, mCorner2pathA{-1,-1,-1,-1}, mCorner2pathB{-1,-1,-1,-1}, mCorner2pathC{-1,-1,-1,-1},
                             mTrans{-1,-1,-1,-1}, mPathCurve{-1,-1,-1,-1} {}

            /** \brief Setups the brush based on meta information. */
            TilemapBrush(const std::string& identifier, TileMap& tm);

            /** \brief Setups the brush based on meta information. */
            void init(const std::string& identifier, TileMap& tm);

            /** \brief Paints to the given tile of the given tilemap. Automatically sets fitting border tiles for transitions
            * depending on adjacent tiles.
            * If connect is true, all connections to adjacent border tiles are automatically created.
            * If it is false, automatic connections to adjacent tiles are only connected to the previously set tile. */
            void paintTile(std::size_t ix, std::size_t iy, bool connect = true);

            /** \brief Erases the given tile using the given foreign tile. Adjacent tiles are automatically fixed. */
            void eraseTile(std::size_t ix, std::size_t iy, int erasingTileID);

            /** \brief Sets the memorizer for the last visited tile to null. */
            void resetMemory();

            /** \brief Returns the identifier string for the brush tileset. */
            const std::string& getIdentifier() const { return mIdentifier; }

        private:
            enum TileType : int {    FOREIGN = -1,
                                          BASIC = 0,
                                          ISO = 1,
                                          PATH_END = 2,
                                          CROSS = 3,
                                          T_SHAPE = 4,
                                          PATH_HORIZ = 5,
                                          PATH_VERT = 6,
                                          PATH_CURVE = 7,
                                          TRANS2PATH = 8,
                                          CORNER2PATHA = 9,
                                          CORNER2PATHB = 10,
                                          CORNER2PATHC = 11,
                                          TRANS = 12,
                                          CORNER = 13,
                                          INNERCORNER = 14,
                                          DIAG1 = 15,
                                          DIAG2 = 16,
                                          SIZE =  17};

            bool mLast;
            unsigned mLastX;
            unsigned mLastY;
            TileMap* mTilemap;
            std::string mIdentifier;

            std::vector<TileType> mSetTiles;

            int mBasic;
            int mIso;
            int mCross;
            int mPathH, mPathV;
            int mDiag1, mDiag2;
            std::array<int, 4> mPathEnd;
            std::array<int, 4> mCorner;
            std::array<int, 4> mInnerCorner;
            std::array<int, 4> mTShape;
            std::array<int, 4> mTrans2path;
            std::array<int, 4> mCorner2pathA;
            std::array<int, 4> mCorner2pathB;
            std::array<int, 4> mCorner2pathC;
            std::array<int, 4> mTrans;
            std::array<int, 4> mPathCurve;

            static constexpr std::array<int, 4> mPos = {1,5,7,3};
            static constexpr std::array<int, 4> mPosCorner = {0,2,6,8};

        private:
            //retrieve data for tile and all 8 adjacent tiles (nullptr if not existing)
            //indexing:
            // 012
            // 345
            // 678
            std::array<Tile*, 9> getAdjacents(std::size_t ix, std::size_t iy);

            enum ConnectType { CONN_PATH, CONN_LEFTBOUND, CONN_RIGHTBOUND, CONN_FULL, CONN_NONE };

            //these methods fix connections in each direction
            //they return the new fixed tile type
            int fixTile(const Tile* tile, ConnectType ct, int d) const;
            int fixTileDiagonal(const Tile* tile, bool close, int d) const;

            void eraseAdjacent(const std::array<Tile*, 9>& tiles, int erasingTileID) const;
            void paintNoAdjacent(const std::array<Tile*, 9>& tiles) const;
            void paintAdjacentSingle(const std::array<Tile*, 9>& tiles, int d) const;
            void paintAdjacentCorner(const std::array<Tile*, 9>& tiles, int d) const;
            void paintAdjacentOpposite(const std::array<Tile*, 9>& tiles, int d) const;
            void paintAdjacentOneOut(const std::array<Tile*, 9>& tiles, int d) const;
            void paintAdjacentAll(const std::array<Tile*, 9>& tiles) const;
            void paintConnected2Last(const std::array<Tile*, 9>& tiles, int d) const;

            //sets the given tile ID if it is != -1
            static void setTileID(Tile* tile, int id);
    };
}

#endif // UNGOD_TMBRUSH_H
