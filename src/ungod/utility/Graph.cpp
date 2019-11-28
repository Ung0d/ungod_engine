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

#include "ungod/utility/Graph.h"
#include <stack>

namespace ungod
{
    namespace graph
    {
        void UndirectedAdjacencyLists::setVertexCount(unsigned n)
        {
            for (unsigned i = n; i < mAdjacencies.size(); i++)
                for (const auto& e : mAdjacencies[i])
                    mAdjacencies[e.destination].remove_if([i] (const ALEdge& e) { return e.destination == i; });
            mAdjacencies.resize(n);
        }

        void UndirectedAdjacencyLists::addEdge(unsigned i, unsigned j)
        {
            mAdjacencies[i].emplace_back(j);
            mAdjacencies[j].emplace_back(i);
			mNumEdges++;
        }

        void UndirectedAdjacencyLists::removeEdge(unsigned i, unsigned j)
        {
			std::size_t lenprev = mAdjacencies[i].size();
            mAdjacencies[i].remove_if([j] (const ALEdge& e) { return e.destination == j; });
            mAdjacencies[j].remove_if([i] (const ALEdge& e) { return e.destination == i; });
			if (lenprev > mAdjacencies[i].size())
				mNumEdges--;
        }

         void BFS::VertexProperties::reset(unsigned index)
         {
            visited = false;
            prev = index;
            distance = std::numeric_limits<unsigned>::infinity();
         }

        void BFS::run(unsigned start, unsigned distanceCap)
        {
            //initialize
            for (unsigned i = 0; i < mAdjacencyLists.getVertexCount(); i++)
                mVertexProperties[i].reset(i);

            mVertexProperties[start].visited = true;
            mVertexProperties[start].distance = 0u;

            std::stack<unsigned> nodeQ;
            nodeQ.push(start);

            while (!nodeQ.empty())
            {
                unsigned cur = nodeQ.top();
                nodeQ.pop();
                mOnNodeDiscovered(cur);
                for (const auto& e : mAdjacencyLists.getNeighbors(cur))
                {
                    if (!mVertexProperties[e.destination].visited)
                    {
                        mVertexProperties[e.destination].visited = true;
                        mVertexProperties[e.destination].prev = cur;
                        mVertexProperties[e.destination].distance = mVertexProperties[cur].distance+1;
                        if (mVertexProperties[e.destination].distance <= distanceCap)
                            nodeQ.push(e.destination);
                    }
                }
            }
        }
    }
}
