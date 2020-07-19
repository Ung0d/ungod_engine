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

#include "ungod/content/tilemap/FloodFill.h"
#include <stack>

namespace ungod
{
    void floodFill(TileMap& tilemap, unsigned ix, unsigned iy, const std::vector<int>& replacementIDs)
    {
        int targetID = tilemap.getTileID(ix, iy);
        //if (targetID == -1)
            //return;

        std::vector<bool> visited((std::size_t)tilemap.getMapSizeX()* (std::size_t)tilemap.getMapSizeY(), false);

        std::stack<std::pair<unsigned, unsigned>> nodestack;
        nodestack.emplace(ix, iy);

        while (!nodestack.empty())
        {
            auto cur = nodestack.top();
            nodestack.pop();

            if (visited[tilemap.getMapSizeX()*cur.second + cur.first])
                continue;

            visited[tilemap.getMapSizeX()*cur.second + cur.first] = true;

            unsigned west = cur.first;
            unsigned east = cur.first;
            while (west > 0 && tilemap.getTileID(west - 1, cur.second) == targetID)
            {
                --west;
            }
            while (east+1 < tilemap.getMapSizeX() && tilemap.getTileID(east + 1, cur.second) == targetID)
            {
                ++east;
            }
            for (unsigned i = west; i <= east; ++i)
            {
                if (replacementIDs.size() == 1)
                    tilemap.setTile(replacementIDs[0], i, cur.second);
                else
                    tilemap.setTile(replacementIDs[NumberGenerator::getRandBetw(0, (unsigned)replacementIDs.size() - 1)], i, cur.second);

                if (cur.second > 0 && tilemap.getTileID(i, cur.second-1) == targetID)
                    nodestack.emplace(i,cur.second-1);

                if (i > 0 && cur.second > 0 && tilemap.getTileID(i-1, cur.second-1) == targetID)
                    nodestack.emplace(i-1,cur.second-1);

                if (i < tilemap.getMapSizeX()-1 && cur.second > 0 && tilemap.getTileID(i+1, cur.second-1) == targetID)
                    nodestack.emplace(i+1,cur.second-1);

                if (cur.second < tilemap.getMapSizeY()-1 && tilemap.getTileID(i, cur.second+1) == targetID)
                    nodestack.emplace(i,cur.second+1);

                if (i > 0 && cur.second < tilemap.getMapSizeY()-1 && tilemap.getTileID(i-1, cur.second+1) == targetID)
                    nodestack.emplace(i-1,cur.second+1);

                if (i < tilemap.getMapSizeX()-1 && cur.second < tilemap.getMapSizeY()-1 && tilemap.getTileID(i+1, cur.second+1) == targetID)
                    nodestack.emplace(i+1,cur.second+1);
            }
        }
    }
}
