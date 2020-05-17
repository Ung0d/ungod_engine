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

#include "ungod/content/tilemap/TilemapBrush.h"

namespace ungod
{
    TilemapBrush::TilemapBrush(const std::string& identifier, TileMap& tm) : mLast(false), mTilemap(nullptr), mBasic(-1), mIso(-1), mCross(-1), mPathH(-1), mPathV(-1), mDiag1(-1), mDiag2(-1),
                             mPathEnd{-1,-1,-1,-1}, mCorner{-1,-1,-1,-1}, mInnerCorner{-1,-1,-1,-1}, mTShape{-1,-1,-1,-1},
                             mTrans2path{-1,-1,-1,-1}, mCorner2pathA{-1,-1,-1,-1}, mCorner2pathB{-1,-1,-1,-1}, mCorner2pathC{-1,-1,-1,-1},
                             mTrans{-1,-1,-1,-1}, mPathCurve{-1,-1,-1,-1}, mChangeNotificator{new detail::TilemapChangeNotificator()}
    {
        init(identifier, tm);
    }


    void TilemapBrush::init(const std::string& identifier, TileMap& tm)
    {
        mIdentifier = identifier;
        mTilemap = &tm;
        mSetTiles.resize(mTilemap->getKeyMap().size(), FOREIGN);

        for (unsigned i = 0; i < mTilemap->getKeyMap().size(); i++)
        {
            if (mTilemap->getKeyMap()[i].find(identifier) == 0)
            {
                std::string rest = mTilemap->getKeyMap()[i].substr(identifier.size());
                if (rest == "_basic")
                {
                    mBasic = i;
                    mSetTiles[i] = BASIC;
                }
                else if (rest == "_iso")
                {
                    mIso = i;
                    mSetTiles[i] = ISO;
                }
                else if (rest == "_pathend_up")
                {
                    mPathEnd[0] = i;
                    mSetTiles[i] = PATH_END;
                }
                else if (rest == "_pathend_right")
                {
                    mPathEnd[1] = i;
                    mSetTiles[i] = PATH_END;
                }
                else if (rest == "_pathend_down")
                {
                    mPathEnd[2] = i;
                    mSetTiles[i] = PATH_END;
                }
                else if (rest == "_pathend_left")
                {
                    mPathEnd[3] = i;
                    mSetTiles[i] = PATH_END;
                }
                else if (rest == "_Tshape_up")
                {
                    mTShape[0] = i;
                    mSetTiles[i] = T_SHAPE;
                }
                else if (rest == "_Tshape_right")
                {
                    mTShape[1] = i;
                    mSetTiles[i] = T_SHAPE;
                }
                else if (rest == "_Tshape_down")
                {
                    mTShape[2] = i;
                    mSetTiles[i] = T_SHAPE;
                }
                else if (rest == "_Tshape_left")
                {
                    mTShape[3] = i;
                    mSetTiles[i] = T_SHAPE;
                }
                else if (rest == "_cross")
                {
                    mCross = i;
                    mSetTiles[i] = CROSS;
                }
                else if (rest == "_path_horiz")
                {
                    mPathH = i;
                    mSetTiles[i] = PATH_HORIZ;
                }
                else if (rest == "_path_vert")
                {
                    mPathV = i;
                    mSetTiles[i] = PATH_VERT;
                }
                else if (rest == "_pathcurve_1")
                {
                    mPathCurve[0] = i;
                    mSetTiles[i] = PATH_CURVE;
                }
                else if (rest == "_pathcurve_2")
                {
                    mPathCurve[1] = i;
                    mSetTiles[i] = PATH_CURVE;
                }
                else if (rest == "_pathcurve_3")
                {
                    mPathCurve[2] = i;
                    mSetTiles[i] = PATH_CURVE;
                }
                else if (rest == "_pathcurve_4")
                {
                    mPathCurve[3] = i;
                    mSetTiles[i] = PATH_CURVE;
                }
                else if (rest == "_trans2path_up")
                {
                    mTrans2path[0] = i;
                    mSetTiles[i] = TRANS2PATH;
                }
                else if (rest == "_trans2path_right")
                {
                    mTrans2path[1] = i;
                    mSetTiles[i] = TRANS2PATH;
                }
                else if (rest == "_trans2path_down")
                {
                    mTrans2path[2] = i;
                    mSetTiles[i] = TRANS2PATH;
                }
                else if (rest == "_trans2path_left")
                {
                    mTrans2path[3] = i;
                    mSetTiles[i] = TRANS2PATH;
                }
                else if (rest == "_corner2path_A_1")
                {
                    mCorner2pathA[0] = i;
                    mSetTiles[i] = CORNER2PATHA;
                }
                else if (rest == "_corner2path_A_2")
                {
                    mCorner2pathA[1] = i;
                    mSetTiles[i] = CORNER2PATHA;
                }
                else if (rest == "_corner2path_A_3")
                {
                    mCorner2pathA[2] = i;
                    mSetTiles[i] = CORNER2PATHA;
                }
                else if (rest == "_corner2path_A_4")
                {
                    mCorner2pathA[3] = i;
                    mSetTiles[i] = CORNER2PATHA;
                }
                else if (rest == "_corner2path_B_1")
                {
                    mCorner2pathB[0] = i;
                    mSetTiles[i] = CORNER2PATHB;
                }
                else if (rest == "_corner2path_B_2")
                {
                    mCorner2pathB[1] = i;
                    mSetTiles[i] = CORNER2PATHB;
                }
                else if (rest == "_corner2path_B_3")
                {
                    mCorner2pathB[2] = i;
                    mSetTiles[i] = CORNER2PATHB;
                }
                else if (rest == "_corner2path_B_4")
                {
                    mCorner2pathB[3] = i;
                    mSetTiles[i] = CORNER2PATHB;
                }
                else if (rest == "_corner2path_C_1")
                {
                    mCorner2pathC[0] = i;
                    mSetTiles[i] = CORNER2PATHC;
                }
                else if (rest == "_corner2path_C_2")
                {
                    mCorner2pathC[1] = i;
                    mSetTiles[i] = CORNER2PATHC;
                }
                else if (rest == "_corner2path_C_3")
                {
                    mCorner2pathC[2] = i;
                    mSetTiles[i] = CORNER2PATHC;
                }
                else if (rest == "_corner2path_C_4")
                {
                    mCorner2pathC[3] = i;
                    mSetTiles[i] = CORNER2PATHC;
                }
                else if (rest == "_trans_up")
                {
                    mTrans[0] = i;
                    mSetTiles[i] = TRANS;
                }
                else if (rest == "_trans_right")
                {
                    mTrans[1] = i;
                    mSetTiles[i] = TRANS;
                }
                else if (rest == "_trans_down")
                {
                    mTrans[2] = i;
                    mSetTiles[i] = TRANS;
                }
                else if (rest == "_trans_left")
                {
                    mTrans[3] = i;
                    mSetTiles[i] = TRANS;
                }
                else if (rest == "_corner_1")
                {
                    mCorner[0] = i;
                    mSetTiles[i] = CORNER;
                }
                else if (rest == "_corner_2")
                {
                    mCorner[1] = i;
                    mSetTiles[i] = CORNER;
                }
                else if (rest == "_corner_3")
                {
                    mCorner[2] = i;
                    mSetTiles[i] = CORNER;
                }
                else if (rest == "_corner_4")
                {
                    mCorner[3] = i;
                    mSetTiles[i] = CORNER;
                }
                else if (rest == "_innercorner_1")
                {
                    mInnerCorner[0] = i;
                    mSetTiles[i] = INNERCORNER;
                }
                else if (rest == "_innercorner_2")
                {
                    mInnerCorner[1] = i;
                    mSetTiles[i] = INNERCORNER;
                }
                else if (rest == "_innercorner_3")
                {
                    mInnerCorner[2] = i;
                    mSetTiles[i] = INNERCORNER;
                }
                else if (rest == "_innercorner_4")
                {
                    mInnerCorner[3] = i;
                    mSetTiles[i] = INNERCORNER;
                }
                else if (rest == "_diag_1")
                {
                    mDiag1 = i;
                    mSetTiles[i] = DIAG1;
                }
                else if (rest == "_diag_2")
                {
                    mDiag2 = i;
                    mSetTiles[i] = DIAG2;
                }
            }
        }
    }


    void TilemapBrush::paintTile(unsigned ix, unsigned iy, bool connect)
    {
        if (!mTilemap)
            return;
        //indexing:
        // 012
        // 345
        // 678
        std::array<TileData, 9> tiles = getAdjacents(ix, iy);

        //todo? currently, for the sake of simplicity, the brush only works for tiles, where
        //all 8 adjacent tiles exist
        for (std::size_t i = 0; i < 9; i++)
        {
            if (tiles[i].id == -1)
            {
                ungod::Logger::warning("At least one adjacent tile does not exist. Can't paint.");
                return;
            }
        }

        std::bitset<4> setTiles;
        std::array<int, 4> indices = {1,3,5,7};
        for (unsigned i = 0; i<4; i++)
        {
            if (mSetTiles[tiles[indices[i]]].id != FOREIGN)
                setTiles.set(i);
        }

        if (connect)
        {
            paint(tiles, setTiles);
        }
        else if (mLast)
        {
            int d = -1;
            if (ix == mLastX)
            {
                if (iy == mLastY+1)
                    d = 0;
                else if (mLastY>0 && iy == mLastY-1)
                    d = 2;
            }
            else if (iy == mLastY)
            {
                if (ix == mLastX+1)
                    d = 3;
                else if (mLastX>0 && ix == mLastX-1)
                    d = 1;
            }
            if (d != -1)
                paintConnected2Last(tiles, d);
            else
                paint(tiles, setTiles);
        }

        mLast = true;
        mLastX = ix;
        mLastY = iy;
    }


    void TilemapBrush::paintTile(const sf::Vector2f& pos, bool connect)
    {
        if (!mTilemap)
            return;
        auto ind = mTilemap->getTileIndices(pos);
        paintTile(ind.x, ind.y, connect);
    }


    void TilemapBrush::eraseTile(std::size_t ix, std::size_t iy, int erasingTileID)
    {
        if (!mTilemap)
            return;
        //indexing:
        // 012
        // 345
        // 678
        std::array<Tile*, 9> tiles = getAdjacents(ix, iy);

        //todo? currently, for the sake of simplicity, the brush only works for tiles, where
        //all 8 adjacent tiles exist
        for (std::size_t i = 0; i < 9; i++)
        {
            if (!tiles[i])
            {
                ungod::Logger::warning("At least one adjacent tile does not exist. Can't paint.");
                ungod::Logger::endl();
                return;
            }
        }

        eraseAdjacent(tiles, erasingTileID);
    }


    void TilemapBrush::eraseTile(const sf::Vector2f& pos, int erasingTileID)
    {
        if (!mTilemap)
            return;
        auto ind = mTilemap->getTileIndices(pos);
        eraseTile(ind.x, ind.y, erasingTileID);
    }


    void TilemapBrush::paintConnected2Last(const std::array<TileData, 9>& tiles, int d) const
    {
        int fix = fixTile(tiles[mPos[d]], CONN_PATH, (d+2)%4);

        if (fix != tiles[mPos[d]].id)
        {
            eraseAdjacent(tiles, mPathEnd[d]);
            setTileID(tiles[mPos[d]], fix);
        }
        else
        {
            eraseAdjacent(tiles, mIso);
        }
    }


    void TilemapBrush::eraseAdjacent(const std::array<TileData, 9>& tiles, int erasingTileID) const
    {
        setTileID(tiles[4], erasingTileID);
        for (unsigned i = 0; i < 4; i++)
        {
            setTileID(tiles[mPos[i]], fixTile(tiles[mPos[i]], CONN_NONE, (i+2)%4));
            setTileID(tiles[mPosCorner[i]], fixTileDiagonal(tiles[mPosCorner[i]], false, (i+2)%4));
        }
    }


    void TilemapBrush::paintNoAdjacent(const std::array<int, 9>& tiles) const
    {
        setTileID(tiles[4], mIso);
    }


    void TilemapBrush::paintAdjacentSingle(const std::array<int, 9>& tiles, int d) const
    {
        int fixedID = fixTile(tiles[mPos[d]], CONN_PATH, (d+2)%4);
        if (fixedID != tiles[mPos[d]])
        {
            setTileID(tiles[4], mPathEnd[d]);
            setTileID(tiles[mPos[d]], fixedID);
        }
        else
            setTileID(tiles[4], mIso);
    }


    void TilemapBrush::paintAdjacentCorner(const std::array<TileData, 9>& tiles, int d) const
    {
        int fix1 = fixTile(tiles[mPos[d]], CONN_LEFTBOUND, (d+2)%4);
        int fix2 = fixTile(tiles[mPos[(d+3)%4]], CONN_RIGHTBOUND, (d+1)%4);
        int diagFix = fixTileDiagonal(tiles[mPosCorner[d]], true, (d+2)%4);
        if (fix1 != tiles[mPos[d]].id && 
            fix2 != tiles[mPos[(d+3)%4]].id &&
            diagFix != tiles[mPosCorner[d]].id)
        {
            setTileID(tiles[4], mCorner[d]);
            setTileID(tiles[mPos[d]], fix1);
            setTileID(tiles[mPos[(d+3)%4]], fix2);
            setTileID(tiles[mPosCorner[d]], diagFix);
        }
        else
        {
            fix1 = fixTile(tiles[mPos[d]], CONN_PATH, (d+2)%4);
            fix2 = fixTile(tiles[mPos[(d+3)%4]], CONN_PATH, (d+1)%4);
            if (fix1 != tiles[mPos[d]].id && 
                fix2 != tiles[mPos[(d+3)%4]].id)
            {
                setTileID(tiles[4], mPathCurve[d]);
                setTileID(tiles[mPos[d]], fix1);
                setTileID(tiles[mPos[(d+3)%4]],fix2);
            }
            else if (fix1 != tiles[mPos[d]].id)
            {
                setTileID(tiles[4], mPathEnd[d]);
                setTileID(tiles[mPos[d]], fix1);
            }
            else if (fix2 != tiles[mPos[(d+3)%4]].id)
            {
                setTileID(tiles[4], mPathEnd[(d+3)%4]);
                setTileID(tiles[mPos[(d+3)%4]], fix2);
            }
            else
                setTileID(tiles[4], mIso);
        }
    }


    void TilemapBrush::paintAdjacentOpposite(const std::array<TileData, 9>& tiles, int d) const
    {
        int fix1 = fixTile(tiles[mPos[d]], CONN_PATH, (d+2)%4);
        int fix2 = fixTile(tiles[mPos[(d+2)%4]], CONN_PATH, d);
        if (fix1 != tiles[mPos[d]].id && fix2 != tiles[mPos[(d+2)%4]].id)
        {
            if (d%2==0)
                setTileID(tiles[4], mPathV);
            else
                setTileID(tiles[4], mPathH);
            setTileID(tiles[mPos[d]], fix1);
            setTileID(tiles[mPos[(d+2)%4]], fix2);
        }
        else if (fix1 != tiles[mPos[d]].id)
        {
            setTileID(tiles[4], mPathEnd[d]);
            setTileID(tiles[mPos[d]], fix1);
        }
        else if (fix2 != tiles[mPos[(d+2)%4]].id)
        {
            setTileID(tiles[4], mPathEnd[(d+2)%4]);
            setTileID(tiles[mPos[(d+2)%4]], fix2);
        }
        else
            setTileID(tiles[4], mIso);
    }


    void TilemapBrush::paintAdjacentOneOut(const std::array<TileData, 9>& tiles, int d) const
    {
        int fixD1 = fixTileDiagonal(tiles[mPosCorner[(d + 2) % 4]], true, d);
        int fixD2 = fixTileDiagonal(tiles[mPosCorner[(d + 3) % 4]], true, (d + 1) % 4);

        if (fixD1 != tiles[mPosCorner[(d + 2) % 4]].id &&
            fixD2 != tiles[mPosCorner[(d + 3) % 4]].id)
        {
            int fixFull = fixTile(tiles[mPos[(d+2)%4]], CONN_FULL, d);
            int fixHalf1 = fixTile(tiles[mPos[(d+1)%4]], CONN_RIGHTBOUND, (d+3)%4);
            int fixHalf2 = fixTile(tiles[mPos[(d+3)%4]], CONN_LEFTBOUND, (d+1)%4);
            setTileID(tiles[4], mTrans[(d+2)%4]);
            setTileID(tiles[mPos[(d+2)%4]], fixFull);
            setTileID(tiles[mPos[(d+1)%4]], fixHalf1);
            setTileID(tiles[mPos[(d+3)%4]], fixHalf2);
            setTileID(tiles[mPosCorner[(d+2)%4]], fixD1);
            setTileID(tiles[mPosCorner[(d+3)%4]], fixD2);
        }
        else
        {
            if (fixD1 != tiles[mPosCorner[(d + 2) % 4]].id)
            {
                int fixP1 = fixTile(tiles[mPos[(d + 1) % 4]], CONN_RIGHTBOUND, (d + 3) % 4);
                int fixP2 = fixTile(tiles[mPos[(d + 2) % 4]], CONN_LEFTBOUND, d);
                int fixP3 = fixTile(tiles[mPos[(d + 3) % 4]], CONN_PATH, (d + 1) % 4);
                setTileID(tiles[4], mCorner2pathB[(d + 2) % 4]);
                setTileID(tiles[mPos[(d + 1) % 4]], fixP1);
                setTileID(tiles[mPos[(d + 2) % 4]], fixP2);
                setTileID(tiles[mPos[(d + 3) % 4]], fixP3);
                setTileID(tiles[mPosCorner[(d + 2) % 4]], fixD1);
            }
            else if (fixD2 != tiles[mPosCorner[(d + 3) % 4]].id)
            {
                int fixP1 = fixTile(tiles[mPos[(d + 1) % 4]], CONN_PATH, (d + 3) % 4);
                int fixP2 = fixTile(tiles[mPos[(d + 2) % 4]], CONN_RIGHTBOUND, d);
                int fixP3 = fixTile(tiles[mPos[(d + 3) % 4]], CONN_LEFTBOUND, (d + 1) % 4);
                setTileID(tiles[4], mCorner2pathA[(d + 3) % 4]);
                setTileID(tiles[mPos[(d + 1) % 4]], fixP1);
                setTileID(tiles[mPos[(d + 2) % 4]], fixP2);
                setTileID(tiles[mPos[(d + 3) % 4]], fixP3);
                setTileID(tiles[mPosCorner[(d + 3) % 4]], fixD2);
            }
            else
            {
                int fixP1 = fixTile(tiles[mPos[(d + 2) % 4]], CONN_PATH, d);
                int fixP2 = fixTile(tiles[mPos[(d + 1) % 4]], CONN_PATH, (d + 3) % 4);
                int fixP3 = fixTile(tiles[mPos[(d + 3) % 4]], CONN_PATH, (d + 1) % 4);
                setTileID(tiles[4], mTShape[(d + 2) % 4]);
                setTileID(tiles[mPos[(d + 2) % 4]], fixP1);
                setTileID(tiles[mPos[(d + 1) % 4]], fixP2);
                setTileID(tiles[mPos[(d + 3) % 4]], fixP3);
            }
        }
    }


    void TilemapBrush::paintAdjacentAll(const std::array<TileData, 9>& tiles) const
    {
        enum class AdjAllType
        {
            ALL, INNER_CORNER, DIAG, T2Pprev, T2Pnext, C2PC, CROSS
        };

        //first: try to insert a basic tile
        //all 8 adjacent tiles must be connectable to the basic tile
        AdjAllType type = AdjAllType::ALL;
        int index = -1;
        std::array<int, 4> cornerFixes;
        for (unsigned i = 0; i < 4; i++)
        {
            cornerFixes[i] = fixTileDiagonal(tiles[mPosCorner[i]], true, (i + 2) % 4);
            if (cornerFixes[i] == tiles[mPosCorner[i]].id)
            {
                switch (type)
                {
                    case AdjAllType::ALL:
                    {
                        type = AdjAllType::INNER_CORNER;
                        index = i;
                        break;
                    }
                    case AdjAllType::INNER_CORNER:
                    {
                        if (index == (i + 2) % 4)
                            type = AdjAllType::DIAG;
                        else if (index == (i + 3) % 4)
                            type = AdjAllType::T2Pprev;
                        else if (index == (i + 1) % 4)
                            type = AdjAllType::T2Pnext;
                        break;
                    }
                    case AdjAllType::DIAG:
                    {
                        type = AdjAllType::C2PC;
                        index = 1;
                        break;
                    }
                    case AdjAllType::T2Pprev:
                    {
                        type = AdjAllType::C2PC;
                        if (index == (i + 1) % 4)
                            index = 2;
                        else
                            index = (index + 3)%4;
                        break;
                    }
                    case AdjAllType::C2PC:
                    {
                        type = AdjAllType::CROSS;
                        break;
                    }
                    default:
                        break;
                }
            }
        }
        std::array<int, 4> fixes;
        switch (type)
        {
            case AdjAllType::ALL:
            {
                for (unsigned i = 0; i < 4; i++)
                    fixes[i] = fixTile(tiles[mPos[i]], CONN_FULL, (i + 2) % 4);
                tiles[4]->setTileID(mBasic);
                break;
            }
            case AdjAllType::INNER_CORNER:
            {
                fixes[(index + 2) % 4] = fixTile(tiles[mPos[(index + 2) % 4]], CONN_FULL, index);
                fixes[(index + 3) % 4] = fixTile(tiles[mPos[(index + 3) % 4]], CONN_LEFTBOUND, (index + 1) % 4);
                fixes[index] = fixTile(tiles[mPos[index]], CONN_RIGHTBOUND, (index + 2) % 4);
                fixes[(index + 1) % 4] = fixTile(tiles[mPos[(index + 1) % 4]], CONN_FULL, (index + 3) % 4);

                tiles[4]->setTileID(mInnerCorner[(index+2)%4]);
                break;
            }
            case AdjAllType::DIAG:
            {
                fixes[index] = fixTile(tiles[mPos[index]], CONN_RIGHTBOUND, (index + 2) % 4);
                fixes[(index + 1) % 4] = fixTile(tiles[mPos[(index + 1) % 4]], CONN_LEFTBOUND, (index + 3) % 4);
                fixes[(index + 2) % 4] = fixTile(tiles[mPos[(index + 2) % 4]], CONN_RIGHTBOUND, index);
                fixes[(index + 3) % 4] = fixTile(tiles[mPos[(index + 3) % 4]], CONN_LEFTBOUND, (index + 1) % 4);

                if (index % 2 == 0)
                    tiles[4]->setTileID(mDiag1);
                else
                    tiles[4]->setTileID(mDiag2);
                break;
            }
            case AdjAllType::T2Pprev:
            {
                fixes[(index + 3) % 4] = fixTile(tiles[mPos[(index + 3) % 4]], CONN_LEFTBOUND, (index + 1) % 4);
                fixes[index] = fixTile(tiles[mPos[index]], CONN_PATH, (index + 2) % 4);
                fixes[(index + 1) % 4] = fixTile(tiles[mPos[(index + 1) % 4]], CONN_RIGHTBOUND, (index + 3) % 4);
                fixes[(index + 2) % 4] = fixTile(tiles[mPos[(index + 2) % 4]], CONN_FULL, index);
                tiles[4]->setTileID(mTrans2path[(index + 2) % 4]);
                break;
            }
            case AdjAllType::T2Pnext:
            {
                fixes[index] = fixTile(tiles[mPos[index]], CONN_RIGHTBOUND, (index + 2) % 4);
                fixes[(index + 1) % 4] = fixTile(tiles[mPos[(index + 1) % 4]], CONN_FULL, (index + 3) % 4);
                fixes[(index + 2) % 4] = fixTile(tiles[mPos[(index + 2) % 4]], CONN_LEFTBOUND, index);
                fixes[(index + 3) % 4] = fixTile(tiles[mPos[(index + 3) % 4]], CONN_PATH, (index + 1) % 4);
                tiles[4]->setTileID(mTrans2path[(index + 1) % 4]);
                break;
            }
            case AdjAllType::C2PC:
            {
                fixes[index] = fixTile(tiles[mPos[index]], CONN_LEFTBOUND, (index + 2) % 4);
                fixes[(index + 1) % 4] = fixTile(tiles[mPos[(index + 1) % 4]], CONN_PATH, (index + 3) % 4);
                fixes[(index + 2) % 4] = fixTile(tiles[mPos[(index + 2) % 4]], CONN_PATH, index);
                fixes[(index + 3) % 4] = fixTile(tiles[mPos[(index + 3) % 4]], CONN_RIGHTBOUND, (index + 1) % 4);
                tiles[4]->setTileID(mCorner2pathC[index]);
                break;
            }
            case AdjAllType::CROSS:
            {
                fixes[index] = fixTile(tiles[mPos[index]], CONN_PATH, (index + 2) % 4);
                fixes[(index + 1) % 4] = fixTile(tiles[mPos[(index + 1) % 4]], CONN_PATH, (index + 3) % 4);
                fixes[(index + 2) % 4] = fixTile(tiles[mPos[(index + 2) % 4]], CONN_PATH, index);
                fixes[(index + 3) % 4] = fixTile(tiles[mPos[(index + 3) % 4]], CONN_PATH, (index + 1) % 4);
                tiles[4]->setTileID(mCross);
                break;
            }
        }

        for (unsigned i = 0; i < 4; i++)
            tiles[mPos[i]]->setTileID(fixes[i]);

        for (unsigned i = 0; i < 4; i++)
            tiles[mPosCorner[i]]->setTileID(cornerFixes[i]);
    }


    void TilemapBrush::resetMemory()
    {
        mLast = false;
    }


    std::array<TileData, 9> TilemapBrush::getAdjacents(std::size_t ix, std::size_t iy)
    {
        //retrieve data for tile and all 8 adjacent tiles (nullptr if not existing)
        //indexing:
        // 012
        // 345
        // 678
        std::array<TileData, 9> tiles;
        unsigned iter = 0;
        for (int y = (int)iy-1; y < (int)iy+2; y++)
            for (int x = (int)ix-1; x < (int)ix+2; x++)
            {
                if (x >= 0 && y >= 0 && x < (int)mTilemap->getMapSizeX() && y < (int)mTilemap->getMapSizeY())
                {
                    tiles[iter].id = mTilemap->getTileID(x, y);
                    tiles[iter].x = x;
                    tiles[iter].y = y;
                }
                iter ++;
            }
        return tiles;
    }


    int TilemapBrush::fixTile(const TileData& tile, ConnectType ct, int d) const
    {
        switch (ct)
        {
            ///////////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////////////////
            case CONN_PATH:
                switch (mSetTiles[tile.id])
                {
                    case TRANS:
                        if (tile.id == mTrans[(d+2)%4])
                            return mTrans2path[(d+2)%4];
                        break;

                    case CORNER:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mCorner[i])
                            {
                                if (d == (i+2)%4)
                                    return mCorner2pathA[i];
                                else if (d == (i+1)%4)
                                    return mCorner2pathB[i];
                            }
                        }
                        break;

                    case CORNER2PATHA:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mCorner2pathA[i])
                            {
                                if (d == (i+1)%4)
                                    return mCorner2pathC[i];
                            }
                        }
                        break;

                    case CORNER2PATHB:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mCorner2pathB[i])
                            {
                                if (d == (i+2)%4)
                                    return mCorner2pathC[i];
                            }
                        }
                        break;

                    case PATH_HORIZ:
                        if (d%2 == 0)
                            return mTShape[d];
                        break;

                    case PATH_VERT:
                        if (d%2 == 1)
                            return mTShape[d];
                        break;

                    case T_SHAPE:
                        if (tile.id == mTShape[(d+2)%4])
                            return mCross;
                        break;

                    case PATH_END:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mPathEnd[i])
                            {
                                if (d == (i+1)%4)
                                    return mPathCurve[d];
                                else if (d == (i+3)%4)
                                    return mPathCurve[i];
                                else if (d == (i+2)%4)
                                {
                                    if (i%2==0)
                                        return mPathV;
                                    else
                                        return mPathH;
                                }
                            }
                        }
                        break;

                    case PATH_CURVE:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mPathCurve[i])
                            {
                                if (d==(i+1)%4)
                                    return mTShape[i];
                                else if (d==(i+2)%4)
                                    return mTShape[(i+3)%4];
                            }
                        }
                        break;

                    case ISO:
                        return mPathEnd[d];

                    default:
                        return tile.id;
                }
                break;
            ///////////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////////////////
            case CONN_LEFTBOUND:
                switch (mSetTiles[tile.id])
                {
                    case TRANS:
                        if (tile.id == mTrans[(d+2)%4])
                            return mInnerCorner[(d+2)%4];
                        break;


                    case CORNER:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mCorner[i])
                            {
                                if (d == (i+2)%4)
                                    return mTrans[(i+3)%4];
                            }
                        }
                        break;


                    case T_SHAPE:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mTShape[i] && d == (i+2)%4)
                                    return mCorner2pathC[(i+3)%4];
                        }
                        break;


                    case PATH_HORIZ:
                        if (d%2==0)
                            return mCorner2pathA[(d+1)%4];
                        break;


                    case PATH_VERT:
                        if (d%2==1)
                            return mCorner2pathA[(d+1)%4];
                        break;


                    case PATH_END:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mPathEnd[i])
                            {
                                if (d == (i+1)%4)
                                    return mCorner2pathA[(i+2)%4];
                                else if (d == (i+2)%4)
                                    return mCorner2pathB[(i+3)%4];
                                else if (d == (i+3)%4)
                                    return mCorner[i];
                            }
                        }
                        break;


                    case CORNER2PATHA:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mCorner2pathA[i] && d == (i+1)%4)
                            {
                                if (d%2 == 0)
                                    return mDiag1;
                                else
                                    return mDiag2;
                            }
                        }
                        break;


                    case CORNER2PATHB:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mCorner2pathB[i] && d == (i+2)%4)
                                return mTrans2path[(i+3)%4];
                        }
                        break;


                    case PATH_CURVE:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mPathCurve[i])
                            {
                                if (d == (i+1)%4)
                                    return mCorner2pathC[(i+2)%4];
                                else if (d == (i+2)%4)
                                    return mCorner2pathB[(i+3)%4];
                            }
                        }
                        break;


                    case ISO:
                        return mCorner[(d+1)%4];
                        break;


                    default:
                        return tile.id;
                }
                break;
            ///////////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////////////////
            case CONN_RIGHTBOUND:
                switch (mSetTiles[tile.id])
                {
                    case TRANS:
                        if (tile.id == mTrans[(d+2)%4])
                            return mInnerCorner[(d+3)%4];
                        break;


                    case CORNER:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mCorner[i])
                            {
                                if (d == (i+1)%4)
                                    return mTrans[i];
                            }
                        }
                        break;


                    case T_SHAPE:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mTShape[i] && d == (i+2)%4)
                                    return mCorner2pathC[(i+2)%4];
                        }
                        break;


                    case PATH_HORIZ:
                        if (d%2==0)
                            return mCorner2pathB[d];
                        break;


                    case PATH_VERT:
                        if (d%2==1)
                            return mCorner2pathB[d];
                        break;


                    case PATH_END:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mPathEnd[i])
                            {
                                if (d == (i+1)%4)
                                    return mCorner[(i+1)%4];
                                else if (d == (i+2)%4)
                                    return mCorner2pathA[(i+2)%4];
                                else if (d == (i+3)%4)
                                    return mCorner2pathB[(i+3)%4];
                            }
                        }
                        break;


                    case CORNER2PATHA:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mCorner2pathA[i] && d == (i+1)%4)
                                return mTrans2path[i];
                        }
                        break;


                    case CORNER2PATHB:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mCorner2pathB[i] && d == (i+2)%4)
                            {
                                if (d%2 == 0)
                                    return mDiag2;
                                else
                                    return mDiag1;
                            }
                        }
                        break;


                    case PATH_CURVE:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mPathCurve[i])
                            {
                                if (d == (i+1)%4)
                                    return mCorner2pathA[(i+1)%4];
                                else if (d == (i+2)%4)
                                    return mCorner2pathC[(i+2)%4];
                            }
                        }
                        break;


                    case ISO:
                        return mCorner[d];
                        break;


                    default:
                        return tile.id;
                }
                break;
            ///////////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////////////////
            case CONN_FULL:
                switch (mSetTiles[tile.id])
                {
                    case TRANS:
                        if (tile.id == mTrans[(d+2)%4])
                            return mBasic;
                        break;

                    case PATH_HORIZ:
                        if (d%2 == 0)
                            return mTrans[d];
                        break;

                    case PATH_VERT:
                        if (d%2 == 1)
                            return mTrans[d];
                        break;

                    case T_SHAPE:
                        if (tile.id == mTShape[(d+2)%4])
                            return mTrans2path[d];
                        break;

                    case CORNER:
                        break;

                    case CORNER2PATHA:
                        if (tile.id == mCorner2pathA[(d + 3) % 4])
                            return mInnerCorner[d];
                        break;

                    case CORNER2PATHB:
                        if (tile.id == mCorner2pathB[(d + 2) % 4])
                            return mInnerCorner[(d + 1) % 4];
                        break;

                    default:
                        return tile.id;
                }
                break;
            ///////////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////////////////
            case CONN_NONE:
                switch (mSetTiles[tile.id])
                {
                    case BASIC:
                        return mTrans[d];

                    case TRANS:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mTrans[i] && d == i)
                            {
                                if (i % 2 == 0)
                                    return mPathH;
                                else
                                    return mPathV;
                            }
                        }
                        break;

                    case PATH_END:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mPathEnd[i] && d == i)
                                return mIso;
                        }
                        break;

                    case PATH_VERT:
                        if (d%2 == 0)
                            return mPathEnd[(d+2)%4];
                        break;

                    case PATH_HORIZ:
                        if (d%2 == 1)
                            return mPathEnd[(d+2)%4];
                        break;

                    case PATH_CURVE:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mTrans[i])
                            {
                                if (d==i)
                                    return mPathEnd[(d+3)%4];
                                else if (d==(i+3)%4)
                                    return mPathEnd[d];
                            }
                        }
                        break;

                    case CROSS:
                        return mTShape[(d+2)%4];

                    case T_SHAPE:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile.id == mTrans[i])
                            {
                                if (i == d)
                                {
                                    if (i % 2 == 0)
                                        return mPathH;
                                    else
                                        return mPathV;
                                }
                                else if (d == (i+1)%4)
                                    return mPathCurve[i];
                                else if (d == (i+3)%4)
                                    return mPathCurve[(i+1)%4];
                            }
                        }
                        break;

                    default:
                        return tile.id;
                }
                break;
        }
        return tile.id;
    }


    int TilemapBrush::fixTileDiagonal(const TileData& tile, bool close, int d) const
    {
        if (!close)
        {
            switch (mSetTiles[tile.id])
            {
                case BASIC:
                    return mInnerCorner[(d+2)%4];

                case CORNER:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile.id == mCorner[i] && i == d)
                        {
                            return mPathCurve[d];
                        }
                    }
                    return tile.id;

                case INNERCORNER:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile.id == mInnerCorner[i] && i == d)
                        {
                            if (d%2 == 0)
                                return mDiag1;
                            else
                                return mDiag2;
                        }
                    }
                    return tile.id;

                case TRANS2PATH:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile.id == mTrans2path[i])
                        {
                            if (i == d)
                                return mCorner2pathB[(d+1)%4];
                            else if (i == (d+1)%4)
                                return mCorner2pathA[d];
                        }
                    }
                    return tile.id;

                case DIAG1:
                    if (d%2 == 1)
                        return mCorner[(d+2)%4];
                    return tile.id;

                case DIAG2:
                    if (d%2 == 0)
                        return mCorner[(d+2)%4];
                    return tile.id;

                case CORNER2PATHA:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile.id == mCorner2pathA[i] && d == i)
                            return mPathEnd[(d+2)%4];
                    }
                    return tile.id;

                case CORNER2PATHB:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile.id == mCorner2pathB[i] && d == i)
                            return mPathEnd[(d+1)%4];
                    }
                    return tile.id;

                case CORNER2PATHC:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile.id == mCorner2pathC[i] && d == i)
                            return mPathCurve[(d+2)%4];
                    }
                    return tile.id;

                default:
                    return tile.id;
            }
        }
        else
        {
            switch (mSetTiles[tile.id])
            {
                case PATH_CURVE:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile.id == mPathCurve[i])
                        {
                            if (d == i)
                                return mCorner[i];
                            else if (d == (i+1)%4)
                                return mCorner2pathA[(i+1)%4];
                            else if (d == (i+2)%4)
                                return mCorner2pathC[(i+2)%4];
                            else
                                return mCorner2pathB[(i+3)%4];
                        }
                    }
                    return tile.id;

                case PATH_END:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile.id == mPathEnd[i])
                        {
                            if (d == i)
                                return mCorner[i];
                            else if (d == (i+1)%4)
                                return mCorner[(i+1)%4];
                            else if (d == (i+2)%4)
                                return mCorner2pathA[(i+2)%4];
                            else
                                return mCorner2pathB[(i+3)%4];
                        }
                    }
                    return tile.id;

                case T_SHAPE:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile.id == mTShape[i])
                        {
                            if (d==i)
                                return mCorner2pathB[d];
                            else if (d == (i+1)%4)
                                return mCorner2pathA[d];
                            else if (d == (i+2)%4)
                                return mCorner2pathC[d];
                            else if (d == (i+3)%4)
                                return mCorner2pathC[d];
                        }
                    }
                    return tile.id;

                case CROSS:
                    return mCorner2pathC[d];

                case CORNER:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile.id == mCorner[i] && i == (d+2)%4)
                        {
                            if (d%2==1)
                                return mDiag1;
                            else
                                return mDiag2;
                        }
                    }
                    return tile.id;

                case INNERCORNER:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile.id == mInnerCorner[i] && i == (d+2)%4)
                            return mBasic;
                    }
                    break;

                case CORNER2PATHA:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile.id == mCorner2pathA[i])
                        {
                            if (d == (i + 3) % 4)
                                return mTrans[d];
                            else if (d == (i + 1) % 4)
                                return mTrans2path[i];
                            else if (d == (i + 2) % 4)
                            {
                                if (i % 2 == 0)
                                    return mDiag1;
                                else
                                    return mDiag2;
                            }
                        }
                    }
                    return tile.id;

                case CORNER2PATHB:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile.id == mCorner2pathB[i])
                        {
                            if (d == (i + 1) % 4)
                                return mTrans[i];
                            else if (d == (i + 2) % 4)
                            {
                                if (i % 2 == 0)
                                    return mDiag1;
                                else
                                    return mDiag2;
                            }
                            else if (d == (i + 3) % 4)
                                return mTrans2path[d];
                        }
                    }
                    return tile.id;

                case CORNER2PATHC:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile.id == mCorner2pathC[i])
                        {
                            if (d == (i + 1) % 4)
                                return mTrans2path[i];
                            if (d == (i + 2) % 4)
                            {
                                if (i % 2 == 0)
                                    return mDiag2;
                                else
                                    return mDiag1;
                            }
                            else if (d == (i + 3) % 4)
                                return mTrans2path[d];
                        }
                    }
                    return tile.id;

                case DIAG1:
                    if (d % 2 == 0)
                        return mInnerCorner[d];
                    return tile.id;

                case DIAG2:
                    if (d % 2 == 1)
                        return mInnerCorner[d];
                    return tile.id;

                case TRANS2PATH:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile.id == mTrans2path[i])
                        {
                            if (d == (i + 2) % 4)
                                return mInnerCorner[(i+1)%4];
                            if (d == (i + 3) % 4)
                                return mInnerCorner[i];
                        }
                    }
                    return tile.id;


                default:
                    return tile.id;
            }
        }
        return tile.id;
    }


    void TilemapBrush::setTileID(const TileData& tile, int id) const
    {
        if (id != -1)
        {
            mTilemap->setTile(tile.id, tile.x, tile.y);
            mChangeNotificator->notifyTileChanged(tile.id, tile.x, tile.y);
        }
        else
        {
            ungod::Logger::warning("Attempt to set an invalid tile ID. Does the tileset miss tiletypes?");
        }
    }

    void TilemapBrush::paint(const std::array<TileData, 9>& tiles, const std::bitset<4>& setTiles)
    {
        if (mSetTiles[tiles[4]].id != FOREIGN) //nothing to do
            return;

        switch (setTiles.to_ulong())
        {
        case 0:
            // 0
            paintNoAdjacent(tiles);
            break;
        case 1:
            //1 u
            paintAdjacentSingle(tiles, 0);
            break;
        case 2:
            //1 l
            paintAdjacentSingle(tiles, 3);
            break;
        case 3:
            //2 ul
            paintAdjacentCorner(tiles, 0);
            break;
        case 4:
            //1 r
            paintAdjacentSingle(tiles, 1);
            break;
        case 5:
            //2 ur
            paintAdjacentCorner(tiles, 1);
            break;
        case 6:
            //2 lr
            paintAdjacentOpposite(tiles, 1);
            break;
        case 7:
            //3 d
            paintAdjacentOneOut(tiles, 2);
            break;
        case 8:
            //1 d
            paintAdjacentSingle(tiles, 2);
            break;
        case 9:
            //2 ud
            paintAdjacentOpposite(tiles, 0);
            break;
        case 10:
            //2 dl
            paintAdjacentCorner(tiles, 3);
            break;
        case 11:
            //3 r
            paintAdjacentOneOut(tiles, 1);
            break;
        case 12:
            //2 dr
            paintAdjacentCorner(tiles, 2);
            break;
        case 13:
            //3 l
            paintAdjacentOneOut(tiles, 3);
            break;
        case 14:
            //3 u
            paintAdjacentOneOut(tiles, 0);
            break;
        case 15:
            paintAdjacentAll(tiles);
            //4
            break;
        }
    }

    std::array<std::string, 47> TilemapBrush::makeKeymap(const std::string& identifier)
    {
        return {
            std::string(identifier).append("_basic"),
            std::string(identifier).append("_iso"),
            std::string(identifier).append("_pathend_up"),
            std::string(identifier).append("_pathend_right"),
            std::string(identifier).append("_pathend_down"),
            std::string(identifier).append("_pathend_left"),
            std::string(identifier).append("_Tshape_up"),
            std::string(identifier).append("_Tshape_right"),
            std::string(identifier).append("_Tshape_down"),
            std::string(identifier).append("_Tshape_left"),
            std::string(identifier).append("_cross"),
            std::string(identifier).append("_path_horiz"),
            std::string(identifier).append("_path_vert"),
            std::string(identifier).append("_pathcurve_1"),
            std::string(identifier).append("_pathcurve_2"),
            std::string(identifier).append("_pathcurve_3"),
            std::string(identifier).append("_pathcurve_4"),
            std::string(identifier).append("_trans2path_up"),
            std::string(identifier).append("_trans2path_right"),
            std::string(identifier).append("_trans2path_down"),
            std::string(identifier).append("_trans2path_left"),
            std::string(identifier).append("_corner2path_A_1"),
            std::string(identifier).append("_corner2path_A_2"),
            std::string(identifier).append("_corner2path_A_3"),
            std::string(identifier).append("_corner2path_A_4"),
            std::string(identifier).append("_corner2path_B_1"),
            std::string(identifier).append("_corner2path_B_2"),
            std::string(identifier).append("_corner2path_B_3"),
            std::string(identifier).append("_corner2path_B_4"),
            std::string(identifier).append("_corner2path_C_1"),
            std::string(identifier).append("_corner2path_C_2"),
            std::string(identifier).append("_corner2path_C_3"),
            std::string(identifier).append("_corner2path_C_4"),
            std::string(identifier).append("_trans_up"),
            std::string(identifier).append("_trans_right"),
            std::string(identifier).append("_trans_down"),
            std::string(identifier).append("_trans_left"),
            std::string(identifier).append("_corner_1"),
            std::string(identifier).append("_corner_2"),
            std::string(identifier).append("_corner_3"),
            std::string(identifier).append("_corner_4"),
            std::string(identifier).append("_innercorner_1"),
            std::string(identifier).append("_innercorner_2"),
            std::string(identifier).append("_innercorner_3"),
            std::string(identifier).append("_innercorner_4"),
            std::string(identifier).append("_diag_1"),
            std::string(identifier).append("_diag_2")
        };
    }
}
