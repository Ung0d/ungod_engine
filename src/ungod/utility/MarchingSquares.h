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

#ifndef MARCHING_SQUARES_H
#define MARCHING_SQUARES_H

#include <vector>
#include "ungod/utility/Traits.h"

namespace ungod
{
    /** \brief An algorithm-object that performs the marching squares algorithm on a set of
    * (convex) polygons in 2d space. It returns outlines around all obstacles in the 2d environment. */
    template<typename POLYGON_TYPE, typename POINT_TYPE>
    class MarchingSquares
    {
    public:
        /** \brief Default constructed object, needs to be initialized. */
        MarchingSquares() : mGridDensityX(0.0f), mGridDensityY(0.0f), mImageSizeX(0u), mImageSizeY(0u) {}

        /** \brief Feed the algorithm object with informations about the 2d world and the required grid density.
        * \param globalUpperBounds .... top left point of a bounding rect around all provided colliders
        * \param globalLowerBounds .... bottom right point of a bounding rect around all provided colliders
        * \param gridDensityX .... defines the grid density in horizontal dimension
        * \param gridDensityY .... defines the grid density in vertical dimension */
        MarchingSquares(const POINT_TYPE& globalUpperBounds, const POINT_TYPE& globalLowerBounds, float gridDensityX, float gridDensityY);

        /** \brief Initializes a default constructed object. */
        void init(const POINT_TYPE& globalUpperBounds, const POINT_TYPE& globalLowerBounds, float gridDensityX, float gridDensityY);

        /** \brief Runs the algorithm on the given set of input data.  */
        template<typename INPUT_ITER>
        void run(INPUT_ITER polygonBegin, INPUT_ITER polygonEnd);

    private:
        std::size_t point2index(const POINT_TYPE& point) const;

        POINT_TYPE centerPoint(std::size_t index) const;

    private:
        POINT_TYPE mGlobalUpperBounds;
        POINT_TYPE mGlobalLowerBounds;
        float mGridDensityX;
        float mGridDensityY;
        std::size_t mImageSizeX;
        std::size_t mImageSizeY;
        std::vector<bool> mBitImage;
    };


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////   IMPLEMENTATION   //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    template<typename POLYGON_TYPE, typename POINT_TYPE>
    MarchingSquares<POLYGON_TYPE, POINT_TYPE>::MarchingSquares(const POINT_TYPE& globalUpperBounds, const POINT_TYPE& globalLowerBounds, float gridDensityX, float gridDensityY)
    {
        init(globalUpperBounds, globalLowerBounds, gridDensityX, gridDensityY);
    }


    template<typename POLYGON_TYPE, typename POINT_TYPE>
    void MarchingSquares<POLYGON_TYPE, POINT_TYPE>::init(const POINT_TYPE& globalUpperBounds, const POINT_TYPE& globalLowerBounds, float gridDensityX, float gridDensityY)
    {
        mGlobalUpperBounds = globalUpperBounds;
        mGlobalLowerBounds = globalLowerBounds;
        mGridDensityX = gridDensityX;
        mGridDensityY = gridDensityY;

        float globalSizeX = Point2DTraits<POINT_TYPE>::getX(globalLowerBounds) - Point2DTraits<POINT_TYPE>::getX(globalUpperBounds);
        float globalSizeY = Point2DTraits<POINT_TYPE>::getY(globalLowerBounds) - Point2DTraits<POINT_TYPE>::getY(globalUpperBounds);
        mImageSizeX = std::ceil( globalSizeX / gridDensityX );
        mImageSizeY = std::ceil( globalSizeY / gridDensityY );

        mBitImage.clear();
        mBitImage.resize(imageSizeX*imageSizeY, false);  //enconding: false = free space, true = blocked
    }


    template<typename POLYGON_TYPE, typename POINT_TYPE>
    std::size_t MarchingSquares<POLYGON_TYPE, POINT_TYPE>::point2index(const POINT_TYPE& point) const
    {
        std::size_t normalizedX = std::floor((Point2DTraits<POINT_TYPE>::getX(point) - Point2DTraits<POINT_TYPE>::getX(mGlobalUpperBounds)) / mGridDensityX);
        std::size_t normalizedY = std::floor((Point2DTraits<POINT_TYPE>::getX(point) - Point2DTraits<POINT_TYPE>::getX(mGlobalUpperBounds)) / mGridDensityX);
        return normalizedY * mImageSizeX + normalizedX;
    }


    template<typename POLYGON_TYPE, typename POINT_TYPE>
    POINT_TYPE MarchingSquares<POLYGON_TYPE, POINT_TYPE>::centerPoint(std::size_t index) const
    {
        float x = Point2DTraits<POINT_TYPE>::getX(mGlobalUpperBounds) + ((index % mImageSizeX) + 0.5f)*mGridDensityX;
        float y = Point2DTraits<POINT_TYPE>::getY(mGlobalUpperBounds) + (std::floor((float)index / (float)mImageSizeY) + 0.5f)*mGridDensityY;
        return Point2DTraits<POINT_TYPE>::create(x, y);
    }


    template<typename POLYGON_TYPE, typename POINT_TYPE>
    template<typename INPUT_ITER>
    void MarchingSquares<POLYGON_TYPE, POINT_TYPE>::run(INPUT_ITER polygonBegin, INPUT_ITER polygonEnd, const POINT_TYPE& globalUpperBounds, const POINT_TYPE& globalLowerBounds, float gridDensityX, float gridDensityY)
    {
        //first step: create the bit image related to the provided colliders
        for (;polygonBegin != polygonEnd; ++polygonBegin)
        {
            //retrieve the upper bounds
            POINT_TYPE polygonUpperBounds = Polygon2DTraits<POLYGON_TYPE, POINT_TYPE>::getUpperBounds(*polygonBegin);
            POINT_TYPE polygonLowerBounds = Polygon2DTraits<POLYGON_TYPE, POINT_TYPE>::getLowerBounds(*polygonBegin);

            //transform the upper bounds to bit-ranges in the image field, that are related to the 2d-world polygons
            std::size_t xBegin = point2index(polygonUpperBounds);
            std::size_t xEnd = point2index( Point2DTraits<POINT_TYPE>::create( Point2DTraits<POINT_TYPE>::getX(polygonLowerBounds), Point2DTraits<POINT_TYPE>::getY(polygonUpperBounds) ) );
            std::size_t yBegin = point2index( Point2DTraits<POINT_TYPE>::create( Point2DTraits<POINT_TYPE>::getX(polygonUpperBounds), Point2DTraits<POINT_TYPE>::getY(polygonLowerBounds) ) );
            std::size_t yEnd = point2index(polygonLowerBounds);

            //iterate over the bit ranges and set bits, if the center points of the cells are within the polygon
            for (std::size_t y = 0; y < yEnd - yBegin; ++y)
                for (std::size_t x = xBegin; x <= xEnd; ++x)
                {
                    std::size_t curIndex = y*mImageSizeX + x;
                    mBitImage[curIndex] = Polygon2DTraits<POLYGON_TYPE, POINT_TYPE>::contains(*polygonBegin, centerPoint(curIndex));
                }
        }
    }
}

#endif // MARCHING_SQUARES_H
