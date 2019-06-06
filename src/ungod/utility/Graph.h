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


#ifndef UNGOD_GRAPH_H
#define UNGOD_GRAPH_H

#include <vector>
#include <queue>
#include <limits>

#include "ungod/serialization/Serializable.h"

namespace ungod
{
    namespace graph
    {
        struct EdgeInstantiator : public Serializable<EdgeInstantiator>
        {
            EdgeInstantiator() : u(0u), v(0u) {}
            EdgeInstantiator(std::size_t cu, std::size_t cv) : u(cu), v(cv) {}
            std::size_t u, v;
        };

        /** \brief Represents an edge object in the adjacency graph. */
        struct ALEdge
        {
        public:
            ALEdge(std::size_t cDestination, std::size_t cIndex) : destination(cDestination), index(cIndex) {}

            std::size_t destination;
            std::size_t index;
        };

        /** \brief An undirected graph represented through adjacency lists. */
        class UndirectedAdjacencyLists : public Serializable<UndirectedAdjacencyLists>
        {
        friend class DeserialBehavior<UndirectedAdjacencyLists>;
        public:
            /** \brief Default constructor. */
            UndirectedAdjacencyLists() : mNumEdges(0) {}

            /** \brief Initializes the graph with the given number of vertices. INPUT_ITER must be an input iterator. */
            template<typename INPUT_ITER>
            UndirectedAdjacencyLists(std::size_t numVertex, INPUT_ITER beginEdges, INPUT_ITER endEdges);

            /** \brief Initializes the graph with the given vertices. */
            template<typename INPUT_ITER>
            void init(std::size_t numVertex, INPUT_ITER beginEdges, INPUT_ITER endEdges);

            /** \brief Returns the number of vertices the graph current has. */
            std::size_t getVertexCount() const { return mAdjacencies.size(); }

            /** \brief Returns the number of edges the graph current has. */
            std::size_t getEdgeCount() const { return mNumEdges; }

            /** \brief Returns a vector of neighbors for the given vertex. */
            const std::vector<ALEdge>& getNeighbors(std::size_t i) const { return mAdjacencies[i]; }

        private:
            std::size_t mNumEdges;
            std::vector< std::vector<ALEdge> > mAdjacencies;
        };



        /** \brief Algorithm object that invokes A* search on the given graph. The algorithm works
        * with an aribtrary datastructure that stores the edge weights of the graph. An WEIGHT_GETTER must be provided, that
        * is a function object getting a std::size_t as argument and that returns the edge-weight for the given edge index.
        Ü Note that the CostEstimator is required to be consistent, that means it fullfills the condition COST_ESTIMATOR(a) <= WEIGHT_GETTER(a,b) + COST_ESTIMATOR(b) */
        class AStarAlg
        {
        public:
            struct VertexProperties
            {
                VertexProperties() : finished(false), origin(0), gscore(std::numeric_limits<float>::infinity()), fscore(std::numeric_limits<float>::infinity()) {}

                bool finished;
                std::size_t origin;
                float gscore;
                float fscore;
            };

        public:
            /** \brief Initializes the algorithm object with the data it operates on. */
            AStarAlg(const UndirectedAdjacencyLists& al)  :
                mAdjacencyLists(al)
            {
                mVertexProperties.resize(al.getVertexCount());
            }

            /** \brief Runs the algorithm. Overrides the results of the previous call to run. */
            template<typename WEIGHT_GETTER, typename COST_ESTIMATOR>
            void run(std::size_t start, std::size_t goal, const WEIGHT_GETTER& weightGetter, const COST_ESTIMATOR& costEstimator);

            /** \brief Accesses the calculated properties of the given vertex. */
            const VertexProperties& getProperties(std::size_t i) const { return mVertexProperties[i]; }

        private:
            const UndirectedAdjacencyLists& mAdjacencyLists;
            std::vector<VertexProperties> mVertexProperties;
        };


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////   IMPLEMENTATION   //////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        template<typename INPUT_ITER>
        UndirectedAdjacencyLists::UndirectedAdjacencyLists(std::size_t numVertex, INPUT_ITER beginEdges, INPUT_ITER endEdges)
        {
            init(numVertex, beginEdges, endEdges);
        }


        template<typename INPUT_ITER>
        void UndirectedAdjacencyLists::init(std::size_t numVertex, INPUT_ITER beginEdges, INPUT_ITER endEdges)
        {
            mAdjacencies.resize(numVertex);
            mNumEdges = std::distance(beginEdges, endEdges);
            std::size_t edgeIndex = 0;
            for (;beginEdges != endEdges; ++beginEdges)
            {
                mAdjacencies[beginEdges->u].emplace_back(beginEdges->v, edgeIndex);
                mAdjacencies[beginEdges->v].emplace_back(beginEdges->u, edgeIndex++);
            }
        }




        template<typename WEIGHT_GETTER, typename COST_ESTIMATOR>
        void AStarAlg::run(std::size_t start, std::size_t goal, const WEIGHT_GETTER& weightGetter, const COST_ESTIMATOR& costEstimator)
        {
            //initialize
            for (std::size_t i = 0; i < mAdjacencyLists.getVertexCount(); ++i)
            {
                mVertexProperties[i].finished = false;
                mVertexProperties[i].origin = 0;
                mVertexProperties[i].fscore = std::numeric_limits<float>::infinity();
                mVertexProperties[i].gscore = std::numeric_limits<float>::infinity();
            }

            mVertexProperties[start].gscore = 0.0f;
            mVertexProperties[start].fscore = costEstimator(start, goal);

            //set up priority queue
            auto comp = [this] (std::size_t a, std::size_t b) { return mVertexProperties[a].fscore > mVertexProperties[b].fscore; };
            std::priority_queue<std::size_t, std::vector<std::size_t>, decltype(comp) > open {comp};
            open.emplace(start);

            //loop n search
            while (!open.empty())
            {
                std::size_t current = open.top();
                open.pop();

                if (current == goal)
                    break;

                if (!mVertexProperties[current].finished)
                {
                    for (const auto& e : mAdjacencyLists.getNeighbors(current))
                    {
                        float tentativeScore = mVertexProperties[current].gscore + costEstimator(current, e.destination);
                        if (tentativeScore < mVertexProperties[e.destination].gscore)
                        {
                            mVertexProperties[e.destination].origin = current;
                            mVertexProperties[e.destination].gscore = tentativeScore;
                            mVertexProperties[e.destination].fscore = tentativeScore + costEstimator(e.destination, goal);
                            open.emplace(e.destination);
                        }
                    }
                    mVertexProperties[current].finished = true;
                }
            }
        }
    }
}

#endif // UNGOD_GRAPH_H
