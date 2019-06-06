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

#include "ungod/content/FloodFill.h"
#include <stack>

namespace ungod
{
    void floodFill(TileMap& tilemap, std::size_t ix, std::size_t iy, const std::vector<int>& replacementIDs, bool activate)
    {
        Tile* target = tilemap.getTiledata(ix, iy);
        std::vector<bool> visited(tilemap.getMapSizeX()*tilemap.getMapSizeY(), false);
        if (!target)
            return;
        int targetID = target->getTileID();
        std::stack<std::pair<std::size_t, std::size_t>> nodestack;
        nodestack.emplace(ix, iy);
        while (!nodestack.empty())
        {
            auto cur = nodestack.top();
            nodestack.pop();

            if (visited[tilemap.getMapSizeX()*cur.second + cur.first])
                continue;

            visited[tilemap.getMapSizeX()*cur.second + cur.first] = true;

            std::size_t west = cur.first;
            std::size_t east = cur.first;
            while (west > 0 && tilemap.getTiledata(west - 1, cur.second)->getTileID() == targetID)
            {
                --west;
            }
            while (east+1 < tilemap.getMapSizeX() && tilemap.getTiledata(east + 1, cur.second)->getTileID() == targetID)
            {
                ++east;
            }
            for (std::size_t i = west; i <= east; ++i)
            {
                Tile* curTile = tilemap.getTiledata(i, cur.second);
                if (!curTile->isActive())
                    curTile->setActive(activate);
                curTile->setTileID( replacementIDs[ NumberGenerator::getRandBetw(0, replacementIDs.size()-1) ] );

                if (cur.second > 0 && tilemap.getTiledata(i, cur.second-1)->getTileID() == targetID)
                    nodestack.emplace(i,cur.second-1);

                if (i > 0 && cur.second > 0 && tilemap.getTiledata(i-1, cur.second-1)->getTileID() == targetID)
                    nodestack.emplace(i-1,cur.second-1);

                if (i < tilemap.getMapSizeX()-1 && cur.second > 0 && tilemap.getTiledata(i+1, cur.second-1)->getTileID() == targetID)
                    nodestack.emplace(i+1,cur.second-1);

                if (cur.second < tilemap.getMapSizeY()-1 && tilemap.getTiledata(i, cur.second+1)->getTileID() == targetID)
                    nodestack.emplace(i,cur.second+1);

                if (i > 0 && cur.second < tilemap.getMapSizeY()-1 && tilemap.getTiledata(i-1, cur.second+1)->getTileID() == targetID)
                    nodestack.emplace(i-1,cur.second+1);

                if (i < tilemap.getMapSizeX()-1 && cur.second < tilemap.getMapSizeY()-1 && tilemap.getTiledata(i+1, cur.second+1)->getTileID() == targetID)
                    nodestack.emplace(i+1,cur.second+1);
            }
        }
    }
}
