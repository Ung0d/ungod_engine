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

#include "ungod/content/TilemapBrush.h"
#include <bitset>

namespace ungod
{
    TilemapBrush::TilemapBrush(const std::string& identifier, TileMap& tm) : mLast(false), mTilemap(nullptr), mBasic(-1), mIso(-1), mCross(-1), mPathH(-1), mPathV(-1), mDiag1(-1), mDiag2(-1),
                             mPathEnd{-1,-1,-1,-1}, mCorner{-1,-1,-1,-1}, mInnerCorner{-1,-1,-1,-1}, mTShape{-1,-1,-1,-1},
                             mTrans2path{-1,-1,-1,-1}, mCorner2pathA{-1,-1,-1,-1}, mCorner2pathB{-1,-1,-1,-1}, mCorner2pathC{-1,-1,-1,-1},
                             mTrans{-1,-1,-1,-1}, mPathCurve{-1,-1,-1,-1}
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
                if (rest == "")
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


    void TilemapBrush::paintTile(std::size_t ix, std::size_t iy, bool connect)
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

        std::bitset<4> setTiles;
        std::array<int, 4> indices = {1,3,5,7};
        for (unsigned i = 0; i<4; i++)
        {
            if (tiles[indices[i]] && mSetTiles[tiles[indices[i]]->getTileID()] != FOREIGN)
                setTiles.set(i);
        }

        if (connect)
        {
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
        else if (mLast)
        {
            int d;
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
                    d = 1;
                else if (mLastX>0 && ix == mLastX-1)
                    d = 3;
            }
            paintConnected2Last(tiles, d);
        }

        mLast = true;
        mLastX = ix;
        mLastY = iy;
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


    void TilemapBrush::paintConnected2Last(const std::array<Tile*, 9>& tiles, int d) const
    {
        int fix = fixTile(tiles[mPos[d]], CONN_PATH, (d+2)%4);

        if (fix != tiles[mPos[d]]->getTileID())
        {
            eraseAdjacent(tiles, mPathEnd[d]);
            setTileID(tiles[mPos[d]], fix);
        }
        else
        {
            eraseAdjacent(tiles, mIso);
        }
    }


    void TilemapBrush::eraseAdjacent(const std::array<Tile*, 9>& tiles, int erasingTileID) const
    {
        setTileID(tiles[4], erasingTileID);
        for (unsigned i = 0; i < 4; i++)
        {
            setTileID(tiles[mPos[i]], fixTile(tiles[mPos[i]], CONN_NONE, (i+2)%4));
            setTileID(tiles[mPosCorner[i]], fixTileDiagonal(tiles[mPosCorner[i]], false, (i+2)%4));
        }
    }


    void TilemapBrush::paintNoAdjacent(const std::array<Tile*, 9>& tiles) const
    {
        setTileID(tiles[4], mIso);
    }


    void TilemapBrush::paintAdjacentSingle(const std::array<Tile*, 9>& tiles, int d) const
    {
        int fixedID = fixTile(tiles[mPos[d]], CONN_PATH, (d+2)%4);
        if (fixedID != mSetTiles[tiles[mPos[d]]->getTileID()])
        {
            setTileID(tiles[4], mPathEnd[d]);
            setTileID(tiles[mPos[d]], fixedID);
        }
        else
            setTileID(tiles[4], mIso);
    }


    void TilemapBrush::paintAdjacentCorner(const std::array<Tile*, 9>& tiles, int d) const
    {
        int fix1 = fixTile(tiles[mPos[d]], CONN_LEFTBOUND, (d+2)%4);
        int fix2 = fixTile(tiles[mPos[(d+3)%4]], CONN_RIGHTBOUND, (d+1)%4);
        int diagFix = fixTileDiagonal(tiles[mPosCorner[d]], true, (d+2)%4);
        if (fix1 != mSetTiles[tiles[mPos[d]]->getTileID()] && fix2 != mSetTiles[tiles[mPos[(d+3)%4]]->getTileID()])
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
            if (fix1 != mSetTiles[tiles[mPos[d]]->getTileID()] && fix2 != mSetTiles[tiles[mPos[(d+3)%4]]->getTileID()])
            {
                setTileID(tiles[4], mPathCurve[d]);
                setTileID(tiles[mPos[d]], fix1);
                setTileID(tiles[mPos[(d+3)%4]],fix2);
            }
            else if (fix1 != mSetTiles[tiles[mPos[d]]->getTileID()])
            {
                setTileID(tiles[4], mPathEnd[d]);
                setTileID(tiles[mPos[d]], fix1);
            }
            else if (fix2 != mSetTiles[tiles[mPos[(d+3)%4]]->getTileID()])
            {
                setTileID(tiles[4], mPathEnd[(d+3)%4]);
                setTileID(tiles[mPos[(d+3)%4]], fix2);
            }
            else
                setTileID(tiles[4], mIso);
        }
    }


    void TilemapBrush::paintAdjacentOpposite(const std::array<Tile*, 9>& tiles, int d) const
    {
        int fix1 = fixTile(tiles[mPos[d]], CONN_PATH, (d+2)%4);
        int fix2 = fixTile(tiles[mPos[(d+2)%4]], CONN_PATH, d);
        if (fix1 != mSetTiles[tiles[mPos[d]]->getTileID()] && fix2 != mSetTiles[tiles[mPos[(d+2)%4]]->getTileID()])
        {
            if (d%2==0)
                setTileID(tiles[4], mPathV);
            else
                setTileID(tiles[4], mPathH);
            setTileID(tiles[mPos[d]], fix1);
            setTileID(tiles[mPos[(d+2)%4]], fix2);
        }
        else if (fix1 != mSetTiles[tiles[mPos[d]]->getTileID()])
        {
            setTileID(tiles[4], mPathEnd[d]);
            setTileID(tiles[mPos[d]], fix1);
        }
        else if (fix2 != mSetTiles[tiles[mPos[(d+2)%4]]->getTileID()])
        {
            setTileID(tiles[4], mPathEnd[(d+2)%4]);
            setTileID(tiles[mPos[(d+2)%4]], fix2);
        }
        else
            setTileID(tiles[4], mIso);
    }


    void TilemapBrush::paintAdjacentOneOut(const std::array<Tile*, 9>& tiles, int d) const
    {
        int fixFull = fixTile(tiles[mPos[(d+2)%4]], CONN_FULL, d);
        int fixHalf1 = fixTile(tiles[mPos[(d+1)%4]], CONN_RIGHTBOUND, (d+3)%4);
        int fixHalf2 = fixTile(tiles[mPos[(d+3)%4]], CONN_LEFTBOUND, (d+1)%4);
        if (fixFull != tiles[mPos[(d+2)%4]]->getTileID() &&
            fixHalf1 != tiles[mPos[(d+1)%4]]->getTileID() &&
            fixHalf2 != tiles[mPos[(d+3)%4]]->getTileID())
        {
            setTileID(tiles[4], mTrans[(d+2)%4]);
            setTileID(tiles[mPos[(d+2)%4]], fixFull);
            setTileID(tiles[mPos[(d+1)%4]], fixHalf1);
            setTileID(tiles[mPos[(d+3)%4]], fixHalf2);
            setTileID(tiles[mPosCorner[(d+2)%4]], fixTileDiagonal(tiles[mPosCorner[(d+2)%4]], true, d));
            setTileID(tiles[mPosCorner[(d+3)%4]], fixTileDiagonal(tiles[mPosCorner[(d+3)%4]], true, (d+1)%4));
        }
        else
        {
            int fixP1 = fixTile(tiles[mPos[(d+2)%4]], CONN_PATH, d);
            int fixP2 = fixTile(tiles[mPos[(d+1)%4]], CONN_PATH, (d+3)%4);
            int fixP3 = fixTile(tiles[mPos[(d+3)%4]], CONN_PATH, (d+1)%4);
            if (fixP1 != tiles[mPos[(d+2)%4]]->getTileID() &&
                fixP2 != tiles[mPos[(d+1)%4]]->getTileID() &&
                fixP3 != tiles[mPos[(d+3)%4]]->getTileID())
            {
                setTileID(tiles[4], mTShape[(d+2)%4]);
                setTileID(tiles[mPos[(d+2)%4]], fixP1);
                setTileID(tiles[mPos[(d+1)%4]], fixP2);
                setTileID(tiles[mPos[(d+3)%4]], fixP3);
            }
            else if (fixP2 != tiles[mPos[(d+1)%4]]->getTileID() &&
                     fixFull != tiles[mPos[(d+2)%4]]->getTileID() &&
                     fixHalf2 != tiles[mPos[(d+3)%4]]->getTileID())
            {
                setTileID(tiles[4], mCorner2pathA[(d+3)%4]);
                setTileID(tiles[mPos[(d+2)%4]], fixFull);
                setTileID(tiles[mPos[(d+1)%4]], fixP2);
                setTileID(tiles[mPos[(d+3)%4]], fixHalf2);
                setTileID(tiles[mPosCorner[(d+3)%4]], fixTileDiagonal(tiles[mPosCorner[(d+3)%4]], true, (d+1)%4));
            }
            else if (fixP3 != tiles[mPos[(d+3)%4]]->getTileID() &&
                     fixFull != tiles[mPos[(d+2)%4]]->getTileID() &&
                     fixHalf1 != tiles[mPos[(d+1)%4]]->getTileID())
            {
                setTileID(tiles[4], mCorner2pathB[(d+2)%4]);
                setTileID(tiles[mPos[(d+2)%4]], fixFull);
                setTileID(tiles[mPos[(d+1)%4]], fixHalf1);
                setTileID(tiles[mPos[(d+3)%4]], fixP3);
                setTileID(tiles[mPosCorner[(d+2)%4]], fixTileDiagonal(tiles[mPosCorner[(d+2)%4]], true, d));
            }
            else
                setTileID(tiles[4], mIso);
        }
    }


    void TilemapBrush::paintAdjacentAll(const std::array<Tile*, 9>& tiles) const
    {
        //first: try to insert a basic tile
        //all 8 adjacent tiles must be connectable to the basic tile
        bool good = true;
        std::array<int, 4> fixes;
        for (unsigned i = 0; i < 4; i++)
        {
            fixes[i] = fixTile(tiles[mPos[i]], CONN_FULL, i);
            if (fixes[i] == tiles[mPos[i]]->getTileID())
                good = false;
        }
        std::array<int, 4> cornerFixes;
        for (unsigned i = 0; i < 4; i++)
        {
            cornerFixes[i] = fixTileDiagonal(tiles[mPosCorner[i]], true, i);
            if (cornerFixes[i] == tiles[mPosCorner[i]]->getTileID())
                good = false;
        }
        if (good)
        {
            for (unsigned i = 0; i < 4; i++)
            {
                tiles[mPos[i]]->setTileID(fixes[i]);
            }

            for (unsigned i = 0; i < 4; i++)
            {
                tiles[mPosCorner[i]]->setTileID(cornerFixes[i]);
            }
            tiles[4]->setTileID(mBasic);
            return;
        }

        //second: try to insert an inner corner, try all 4 possible orientations
        for (int d = 0; d < 4; d++)
        {
            int fixFull1 = fixTile(tiles[mPos[d]], CONN_FULL, d);
            int fixFull2 = fixTile(tiles[mPos[(d+3)%4]], CONN_FULL, (d+3)%4);
            int fixHalf1 = fixTile(tiles[mPos[(d+1)%4]], CONN_LEFTBOUND, (d+1)%4);
            int fixHalf2 = fixTile(tiles[mPos[(d+2)%4]], CONN_RIGHTBOUND, (d+2)%4);

            if (fixFull1 != tiles[mPos[d]]->getTileID() &&
                fixFull2 != tiles[mPos[(d+3)%4]]->getTileID() &&
                fixHalf1 != tiles[mPos[(d+1)%4]]->getTileID() &&
                fixHalf2 != tiles[mPos[(d+2)%4]]->getTileID())
            {
                int fixCorner1 = fixTileDiagonal(tiles[mPos[d]], true, d);
                int fixCorner2 = fixTileDiagonal(tiles[mPos[(d+3)%4]], true, (d+3)%4);
                int fixCorner3 = fixTileDiagonal(tiles[mPos[(d+1)%4]], true, (d+1)%4);
                int fixCornerNone = fixTileDiagonal(tiles[mPos[(d+2)%4]], false, (d+2)%4);

                if (fixFull1 != tiles[mPos[d]]->getTileID() &&
                fixFull2 != tiles[mPos[(d+3)%4]]->getTileID() &&
                fixHalf1 != tiles[mPos[(d+1)%4]]->getTileID() &&
                fixHalf2 != tiles[mPos[(d+2)%4]]->getTileID())
                {

                }
            }
        }
    }


    void TilemapBrush::resetMemory()
    {
        mLast = false;
    }


    std::array<Tile*, 9> TilemapBrush::getAdjacents(std::size_t ix, std::size_t iy)
    {
        //retrieve data for tile and all 8 adjacent tiles (nullptr if not existing)
        //indexing:
        // 012
        // 345
        // 678
        std::array<Tile*, 9> tiles = {nullptr};
        unsigned iter = 0;
        for (int y = (int)iy-1; y < (int)iy+2; y++)
            for (int x = (int)ix-1; x < (int)ix+2; x++)
            {
                if (x >= 0 && y >= 0 && x < (int)mTilemap->getMapSizeX() && y < (int)mTilemap->getMapSizeY())
                    tiles[iter] = mTilemap->getTiledata(x, y);
                iter ++;
            }
        return tiles;
    }


    int TilemapBrush::fixTile(const Tile* tile, ConnectType ct, int d) const
    {
        switch (ct)
        {
            ///////////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////////////////
            case CONN_PATH:
                switch (mSetTiles[tile->getTileID()])
                {
                    case TRANS:
                        if (tile->getTileID() == mTrans[(d+2)%4])
                            return mTrans2path[(d+2)%4];
                        break;

                    case CORNER:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile->getTileID() == mCorner[i])
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
                            if (tile->getTileID() == mCorner2pathA[i])
                            {
                                if (d == (i+1)%4)
                                    return mCorner2pathC[i];
                            }
                        }
                        break;

                    case CORNER2PATHB:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile->getTileID() == mCorner2pathB[i])
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
                        if (tile->getTileID() == mTrans[(d+2)%4])
                            return mCross;
                        break;

                    case PATH_END:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile->getTileID() == mPathEnd[i])
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
                            if (tile->getTileID() == mPathCurve[i])
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
                        return tile->getTileID();
                }
                break;
            ///////////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////////////////
            case CONN_LEFTBOUND:
                switch (mSetTiles[tile->getTileID()])
                {
                    case TRANS:
                        if (tile->getTileID() == mTrans[(d+2)%4])
                            return mInnerCorner[(d+2)%4];
                        break;


                    case CORNER:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile->getTileID() == mCorner[i])
                            {
                                if (d == (i+2)%4)
                                    return mTrans[(i+3)%4];
                            }
                        }
                        break;


                    case T_SHAPE:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile->getTileID() == mTShape[i] && d == (i+2)%4)
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
                            if (tile->getTileID() == mPathEnd[i])
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
                            if (tile->getTileID() == mCorner2pathA[i] && d == (i+1)%4)
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
                            if (tile->getTileID() == mCorner2pathB[i] && d == (i+2)%4)
                                return mTrans2path[(i+3)%4];
                        }
                        break;


                    case PATH_CURVE:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile->getTileID() == mPathCurve[i])
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
                        return tile->getTileID();
                }
                break;
            ///////////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////////////////
            case CONN_RIGHTBOUND:
                switch (mSetTiles[tile->getTileID()])
                {
                    case TRANS:
                        if (tile->getTileID() == mTrans[(d+2)%4])
                            return mInnerCorner[(d+3)%4];
                        break;


                    case CORNER:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile->getTileID() == mCorner[i])
                            {
                                if (d == (i+1)%4)
                                    return mTrans[i];
                            }
                        }
                        break;


                    case T_SHAPE:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile->getTileID() == mTShape[i] && d == (i+2)%4)
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
                            if (tile->getTileID() == mPathEnd[i])
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
                            if (tile->getTileID() == mCorner2pathA[i] && d == (i+1)%4)
                                return mTrans2path[i];
                        }
                        break;


                    case CORNER2PATHB:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile->getTileID() == mCorner2pathB[i] && d == (i+2)%4)
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
                            if (tile->getTileID() == mPathCurve[i])
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
                        return tile->getTileID();
                }
                break;
            ///////////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////////////////
            case CONN_FULL:
                switch (mSetTiles[tile->getTileID()])
                {
                    case TRANS:
                        if (tile->getTileID() == mTrans[(d+2)%4])
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
                        if (tile->getTileID() == mTrans[(d+2)%4])
                            return mTrans2path[d];
                        break;

                    case CORNER:
                        break;

                    case CORNER2PATHA:
                        break;

                    case CORNER2PATHB:
                        break;

                    default:
                        return tile->getTileID();
                }
                break;
            ///////////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////////////////
            case CONN_NONE:
                switch (mSetTiles[tile->getTileID()])
                {
                    case BASIC:
                        return mTrans[d];

                    case TRANS:
                        for (int i = 0; i < 4; i++)
                        {
                            if (tile->getTileID() == mTrans[i] && d == i)
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
                            if (tile->getTileID() == mPathEnd[i] && d == i)
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
                            if (tile->getTileID() == mTrans[i])
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
                            if (tile->getTileID() == mTrans[i])
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
                        return tile->getTileID();
                }
                break;
        }
        return tile->getTileID();
    }


    int TilemapBrush::fixTileDiagonal(const Tile* tile, bool close, int d) const
    {
        if (!close)
        {
            switch (mSetTiles[tile->getTileID()])
            {
                case BASIC:
                    return mInnerCorner[(d+2)%4];

                case CORNER:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile->getTileID() == mCorner[i] && i == d)
                        {
                            return mPathCurve[d];
                        }
                    }
                    return tile->getTileID();

                case INNERCORNER:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile->getTileID() == mInnerCorner[i] && i == d)
                        {
                            if (d%2 == 0)
                                return mDiag1;
                            else
                                return mDiag2;
                        }
                    }
                    return tile->getTileID();

                case TRANS2PATH:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile->getTileID() == mTrans2path[i])
                        {
                            if (i == d)
                                return mCorner2pathB[(d+1)%4];
                            else if (i == (d+1)%4)
                                return mCorner2pathA[d];
                        }
                    }
                    return tile->getTileID();

                case DIAG1:
                    if (d%2 == 1)
                        return mCorner[(d+2)%4];
                    return tile->getTileID();

                case DIAG2:
                    if (d%2 == 0)
                        return mCorner[(d+2)%4];
                    return tile->getTileID();

                case CORNER2PATHA:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile->getTileID() == mCorner2pathA[i] && d == i)
                            return mPathEnd[(d+2)%4];
                    }
                    return tile->getTileID();

                case CORNER2PATHB:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile->getTileID() == mCorner2pathB[i] && d == i)
                            return mPathEnd[(d+1)%4];
                    }
                    return tile->getTileID();

                case CORNER2PATHC:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile->getTileID() == mCorner2pathC[i] && d == i)
                            return mPathCurve[(d+2)%4];
                    }
                    return tile->getTileID();

                default:
                    return tile->getTileID();
            }
        }
        else
        {
            switch (mSetTiles[tile->getTileID()])
            {
                case PATH_CURVE:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile->getTileID() == mPathCurve[i])
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
                    return tile->getTileID();

                case PATH_END:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile->getTileID() == mPathEnd[i])
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
                    return tile->getTileID();

                case T_SHAPE:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile->getTileID() == mPathCurve[i])
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
                    return tile->getTileID();

                case CROSS:
                    return mCorner2pathC[d];

                case CORNER:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile->getTileID() == mCorner[i] && i == (d+2)%4)
                        {
                            if (d%2==1)
                                return mDiag1;
                            else
                                return mDiag2;
                        }
                    }
                    return tile->getTileID();

                case INNERCORNER:
                    for (int i = 0; i < 4; i++)
                    {
                        if (tile->getTileID() == mInnerCorner[i] && i == (d+2)%4)
                            return mBasic;
                    }
                    break;

                default:
                    return tile->getTileID();
            }
        }
        return tile->getTileID();
    }


    void TilemapBrush::setTileID(Tile* tile, int id)
    {
        if (id != -1)
            tile->setTileID(id);
        else
        {
            ungod::Logger::warning("Attempt to set an invalid tile ID. Does the tileset miss tiletypes?");
            ungod::Logger::endl();
        }
    }
}
