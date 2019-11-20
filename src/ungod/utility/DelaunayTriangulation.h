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

#ifndef UNGOD_DELAUNAY_TRIANGULATION_H
#define UNGOD_DELAUNAY_TRIANGULATION_H

#include "ungod/utility/Traits.h"
#include "quadtree/Quadtree.h"

#include <list>
#include <vector>
#include <queue>
#include <iostream>

namespace ungod
{
    /** \brief Identifier for a single vertex. */
    using Vertex = std::size_t;

    /** \brief Encapsulates a triangle. */
    struct Triangle
    {
        Triangle(Vertex cv0, Vertex cv1, Vertex cv2) : v0(cv0), v1(cv1), v2(cv2) {}

        Vertex v0;
        Vertex v1;
        Vertex v2;
    };

    /** \brief Encapsulates an edge between 2 vertices. */
    struct Edge
    {
        Edge(Vertex cv0, Vertex cv1) : v0(cv0), v1(cv1) {}

        Vertex v0;
        Vertex v1;
    };

    namespace detail
    {
        /** \brief Bundles a triangle along with precoputed properties. */
        struct TriangleData
        {
            TriangleData(Triangle t) : triangle(t), circumX(0), circumY(0), circumRadius(0) {}

            template<typename POINT, typename GETTER>
            void init(const GETTER& pointGetter);

            Triangle triangle;
            float circumX;
            float circumY;
            float circumRadius;
        };

        using TriangleIter = typename std::list<TriangleData>::const_iterator;


        /** \brief Attributes for an edge. */
        template<typename POINT>
        struct EdgeAttributes
        {
            std::list<TriangleIter> adjacent;
        };

        /** \brief Datastructure that stores a triangulation. */
        template<typename POINT>
        class TriangulationData
        {
        public:
            TriangulationData() {}

            void init(std::size_t numVertex);

            template<typename GETTER>
            TriangleIter addTriangle(const Triangle& triangle, const GETTER& pointGetter);

            void removeTriangle(TriangleIter triangle);

            void removeAdjacentTriangles(const Edge& e);

            const EdgeAttributes<POINT>& getEdgeAttributes(const Edge& e) const;

            const std::list<TriangleData>& getTriangleContainer() const;

        private:
            void accomodateEdge(Vertex v0, Vertex v1, TriangleIter triangle);
            void removeEdge(Vertex v0, Vertex v1, TriangleIter triangle);

        private:
            std::list< TriangleData > mTriangles;  //stores data of the triangles
            std::vector< std::vector< EdgeAttributes<POINT> > > mEdgeAttributes;  //triangle matrix, that maps edges to its data (a,b) and (b,a) map to the same entry
        };
    }


    /** \brief An algorithm object that performs delauny triangulation for a given set of points. */
    template<typename POINT>
    class DelaunayTriangulation
    {
    public:
        DelaunayTriangulation() {}

        void run(const std::vector<POINT>& points, const POINT& upperBound, const POINT& lowerBound);

        detail::TriangulationData<POINT>& getData() { return mTriangulation; }

    private:
        //returns true if the circumcycle of the triangle <t1,t2,t3> contains the vertex v
        template<typename GETTER>
        bool circumcycleContains(const GETTER& pointGetter, detail::TriangleIter triangle, Vertex v);

        void checkSharedStatus(Vertex v1, Vertex v2, detail::TriangleIter cur, const std::vector<detail::TriangleIter>& badTriangles, std::vector<Edge>& polygon);
        void cleanupVertices(Vertex v1, Vertex v2);

    private:
        detail::TriangulationData<POINT> mTriangulation;  //holds the resulting triangulation
    };



    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////// IMPLEMENTATION  /////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace detail
    {
            template<typename POINT, typename GETTER>
            void TriangleData::init(const GETTER& pointGetter)
            {
                float ab = (Point2DTraits<POINT>::getX(pointGetter(triangle.v0)) * Point2DTraits<POINT>::getX(pointGetter(triangle.v0))) +
                            (Point2DTraits<POINT>::getY(pointGetter(triangle.v0)) * Point2DTraits<POINT>::getY(pointGetter(triangle.v0)));
                float cd = (Point2DTraits<POINT>::getX(pointGetter(triangle.v1)) * Point2DTraits<POINT>::getX(pointGetter(triangle.v1))) +
                            (Point2DTraits<POINT>::getY(pointGetter(triangle.v1)) * Point2DTraits<POINT>::getY(pointGetter(triangle.v1)));
                float ef = (Point2DTraits<POINT>::getX(pointGetter(triangle.v2)) * Point2DTraits<POINT>::getX(pointGetter(triangle.v2))) +
                            (Point2DTraits<POINT>::getY(pointGetter(triangle.v2)) * Point2DTraits<POINT>::getY(pointGetter(triangle.v2)));

                circumX = (ab * (Point2DTraits<POINT>::getY(pointGetter(triangle.v2)) - Point2DTraits<POINT>::getY(pointGetter(triangle.v1))) +
                                  cd * (Point2DTraits<POINT>::getY(pointGetter(triangle.v0)) - Point2DTraits<POINT>::getY(pointGetter(triangle.v2))) +
                                  ef * (Point2DTraits<POINT>::getY(pointGetter(triangle.v1)) - Point2DTraits<POINT>::getY(pointGetter(triangle.v0)))) /
                                    (Point2DTraits<POINT>::getX(pointGetter(triangle.v0)) * (Point2DTraits<POINT>::getY(pointGetter(triangle.v2)) - Point2DTraits<POINT>::getY(pointGetter(triangle.v1))) +
                                     Point2DTraits<POINT>::getX(pointGetter(triangle.v1)) * (Point2DTraits<POINT>::getY(pointGetter(triangle.v0)) - Point2DTraits<POINT>::getY(pointGetter(triangle.v2))) +
                                     Point2DTraits<POINT>::getX(pointGetter(triangle.v2)) * (Point2DTraits<POINT>::getY(pointGetter(triangle.v1)) - Point2DTraits<POINT>::getY(pointGetter(triangle.v0)))) / 2.f;

                circumY = (ab * (Point2DTraits<POINT>::getX(pointGetter(triangle.v2)) - Point2DTraits<POINT>::getX(pointGetter(triangle.v1))) +
                                  cd * (Point2DTraits<POINT>::getX(pointGetter(triangle.v0)) - Point2DTraits<POINT>::getX(pointGetter(triangle.v2))) +
                                  ef * (Point2DTraits<POINT>::getX(pointGetter(triangle.v1)) - Point2DTraits<POINT>::getX(pointGetter(triangle.v0)))) /
                                    (Point2DTraits<POINT>::getY(pointGetter(triangle.v0)) * (Point2DTraits<POINT>::getX(pointGetter(triangle.v2)) - Point2DTraits<POINT>::getX(pointGetter(triangle.v1))) +
                                     Point2DTraits<POINT>::getY(pointGetter(triangle.v1)) * (Point2DTraits<POINT>::getX(pointGetter(triangle.v0)) - Point2DTraits<POINT>::getX(pointGetter(triangle.v2))) +
                                     Point2DTraits<POINT>::getY(pointGetter(triangle.v2)) * (Point2DTraits<POINT>::getX(pointGetter(triangle.v1)) - Point2DTraits<POINT>::getX(pointGetter(triangle.v0)))) / 2.f;

                circumRadius = std::sqrt(((Point2DTraits<POINT>::getX(pointGetter(triangle.v0)) - circumX) * (Point2DTraits<POINT>::getX(pointGetter(triangle.v0)) - circumX)) +
                                                ((Point2DTraits<POINT>::getY(pointGetter(triangle.v0)) - circumY) * (Point2DTraits<POINT>::getY(pointGetter(triangle.v0)) - circumY)));
            }

            template<typename POINT>
            void TriangulationData<POINT>::init(std::size_t numVertex)
            {
                mEdgeAttributes.resize(numVertex);
                for (std::size_t i = 1; i <= numVertex; ++i)
                    mEdgeAttributes[i-1].resize(i);
            }

            template<typename POINT>
            void TriangulationData<POINT>::removeTriangle(TriangleIter t)
            {
                removeEdge(t->triangle.v0, t->triangle.v1, t);
                removeEdge(t->triangle.v1, t->triangle.v2, t);
                removeEdge(t->triangle.v2, t->triangle.v0, t);
                mTriangles.erase(t);
            }

            template<typename POINT>
            void TriangulationData<POINT>::removeAdjacentTriangles(const Edge& e)
            {
                EdgeAttributes<POINT>& attr = (e.v1 > e.v0) ? mEdgeAttributes[e.v1][e.v0] : mEdgeAttributes[e.v0][e.v1];
                for (const auto& t : attr.adjacent)
                {
                    mTriangles.erase(t);
                }
                attr.adjacent.clear();
            }

            template<typename POINT>
            const EdgeAttributes<POINT>& TriangulationData<POINT>::getEdgeAttributes(const Edge& e) const
            {
                if (e.v1 > e.v0)
                    return mEdgeAttributes[e.v1][e.v0];
                else
                    return mEdgeAttributes[e.v0][e.v1];
            }

            template<typename POINT>
            const std::list<TriangleData>& TriangulationData<POINT>::getTriangleContainer() const
            {
                return mTriangles;
            }


            template<typename POINT>
            void TriangulationData<POINT>::accomodateEdge(Vertex v0, Vertex v1, TriangleIter triangle)
            {
                if (v1 > v0)
                    std::swap(v0, v1);  //make sure v1 <= v0 is always true

                EdgeAttributes<POINT>& attr = mEdgeAttributes[v0][v1];
                attr.adjacent.emplace_back(triangle);
            }


            template<typename POINT>
            void TriangulationData<POINT>::removeEdge(Vertex v0, Vertex v1, TriangleIter triangle)
            {
                if (v1 > v0)
                    std::swap(v0, v1);  //make sure v1 <= v0 is always true

                EdgeAttributes<POINT>& attr = mEdgeAttributes[v0][v1];
                for (auto it = std::begin(attr.adjacent); it != std::end(attr.adjacent); ++it)
                {
                    if (*it == triangle)
                    {
                        attr.adjacent.erase(it);
                        break;
                    }
                }
            }

            template<typename POINT>
            template<typename GETTER>
            TriangleIter TriangulationData<POINT>::addTriangle(const Triangle& triangle, const GETTER& pointGetter)
            {
                mTriangles.emplace_back( triangle );
                mTriangles.back().init<POINT>(pointGetter);
                TriangleIter backIt = --std::end(mTriangles);
                accomodateEdge(triangle.v0, triangle.v1, backIt);
                accomodateEdge(triangle.v1, triangle.v2, backIt);
                accomodateEdge(triangle.v2, triangle.v0, backIt);
                return backIt;
            }
    }


    template<typename POINT>
    void DelaunayTriangulation<POINT>::run(const std::vector<POINT>& points, const POINT& upperBound, const POINT& lowerBound)
    {
        std::size_t numPoints = points.size();

        //determine super triangle
        float dx = Point2DTraits<POINT>::getX(lowerBound) - Point2DTraits<POINT>::getX(upperBound);
        float dy = Point2DTraits<POINT>::getY(lowerBound) - Point2DTraits<POINT>::getY(upperBound);

        std::array<POINT, 3> superpoints;
        superpoints[0] = Point2DTraits<POINT>::create(Point2DTraits<POINT>::getX(upperBound) + dx/2, Point2DTraits<POINT>::getY(upperBound) - dy);
        superpoints[1] = Point2DTraits<POINT>::create(Point2DTraits<POINT>::getX(upperBound) - dx, Point2DTraits<POINT>::getY(lowerBound));
        superpoints[2] = Point2DTraits<POINT>::create(Point2DTraits<POINT>::getX(lowerBound) + dx, Point2DTraits<POINT>::getY(lowerBound));

        auto pointGetter = [=] (Vertex v) { return (v < numPoints) ? points[v] : superpoints[v-numPoints]; };

        mTriangulation.init( numPoints+3 );
        mTriangulation.addTriangle( {numPoints,numPoints+1,numPoints+2}, pointGetter );

        std::vector<detail::TriangleIter> badTriangles;
        std::vector<Edge> polygon;

        //iterate over the input points
        for (Vertex v = 0; v < numPoints; ++v)
        {
            // first find all the triangles that are no longer valid due to the insertion
            for (detail::TriangleIter t = mTriangulation.getTriangleContainer().begin(); t != mTriangulation.getTriangleContainer().end(); ++t)
            {
                if (circumcycleContains(pointGetter, t, v))
                {
                    badTriangles.emplace_back(t);
                }
            }
            // find the boundary of the polygonal hole
            for (const auto& titer : badTriangles)
            {
                const Triangle& triangle = titer->triangle;
                checkSharedStatus(triangle.v0,triangle.v1, titer, badTriangles, polygon);
                checkSharedStatus(triangle.v1,triangle.v2, titer, badTriangles, polygon);
                checkSharedStatus(triangle.v2,triangle.v0, titer, badTriangles, polygon);
            }
            // remove them from the datastructure
            for (const auto& titer : badTriangles)
            {
                mTriangulation.removeTriangle(titer);
            }
            // re-triangulate the polygonal hole
            for (const auto& edge : polygon)
            {
                mTriangulation.addTriangle({edge.v0, edge.v1, v}, pointGetter);
            }

            badTriangles.clear();
            polygon.clear();
        }

        //cleanup triangles that contain a point from the original supervertex
        cleanupVertices(numPoints, numPoints+1);
        cleanupVertices(numPoints+1, numPoints+2);
        cleanupVertices(numPoints+2, numPoints);
        for (std::size_t i = 0; i < numPoints; ++i)
        {
            cleanupVertices(numPoints, i);
            cleanupVertices(numPoints+1, i);
            cleanupVertices(numPoints+2, i);
        }
    }

    template<typename POINT>
    template<typename GETTER>
    bool DelaunayTriangulation<POINT>::circumcycleContains(const GETTER& pointGetter, detail::TriangleIter triangle, Vertex v)
    {
        float dist = std::sqrt(((Point2DTraits<POINT>::getX(pointGetter(v)) - triangle->circumX) * (Point2DTraits<POINT>::getX(pointGetter(v)) - triangle->circumX)) +
                                        ((Point2DTraits<POINT>::getY(pointGetter(v)) - triangle->circumY) * (Point2DTraits<POINT>::getY(pointGetter(v)) - triangle->circumY)));

        return dist <= triangle->circumRadius;
    }


    template<typename POINT>
    void DelaunayTriangulation<POINT>::checkSharedStatus(Vertex v1, Vertex v2,
                                                         detail::TriangleIter cur,
                                                         const std::vector<detail::TriangleIter>& badTriangles,
                                                         std::vector<Edge>& polygon)
    {
        const detail::EdgeAttributes<POINT>& attr = mTriangulation.getEdgeAttributes( {v1, v2} );

        for (const auto& bad : badTriangles)
        {
            for (const auto& t : attr.adjacent)
            {
                if (bad == t && bad != cur)
                    return;
            }
        }

        //edge is border edge of the polygon
        polygon.emplace_back(v1, v2);
    }

    template<typename POINT>
    void DelaunayTriangulation<POINT>::cleanupVertices(Vertex v1, Vertex v2)
    {
        const detail::EdgeAttributes<POINT>& attr = mTriangulation.getEdgeAttributes( {v1, v2} );
        for (auto it = std::begin(attr.adjacent); it != std::end(attr.adjacent);)
        {
            detail::TriangleIter tcur = *it;
            ++it;
            mTriangulation.removeTriangle(tcur);
        }
    }
}

#endif // UNGOD_DELAUNAY_TRIANGULATION_H
