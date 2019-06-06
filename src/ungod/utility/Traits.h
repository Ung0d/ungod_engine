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

#ifndef UNGOD_TRAITS_H
#define UNGOD_TRAITS_H

#include "ungod/base/Utility.h"

namespace ungod
{
    /** \brief Type traits for 2d points. There must be an explicit specialization */
    template <typename T>
    struct Point2DTraits
    {
        static T create(float x, float y)
        {
            static_assert(AlwaysFalse<T>::value, "No explicit specialization for this type found!");
            return T();
        }

        static float getX(const T&)
        {
            static_assert(AlwaysFalse<T>::value, "No explicit specialization for this type found!");
            return 0.0f;
        }

        static float getY(const T&)
        {
            static_assert(AlwaysFalse<T>::value, "No explicit specialization for this type found!");
            return 0.0f;
        }
    };

    /** \brief Type traits for 2d convex polygons. There must be an explicit specialization */
    template <typename POLYGON, typename POINT>
    struct Polygon2DTraits
    {
        static std::size_t getPointCount(const POLYGON&)
        {
            static_assert(AlwaysFalse<POLYGON>::value, "No explicit specialization for this type found!");
            return 0u;
        }

        static POINT getPoint(const POLYGON&, std::size_t)
        {
            static_assert(AlwaysFalse<POLYGON>::value, "No explicit specialization for this type found!");
            return POINT();
        }

        static POINT getUpperBounds(const POLYGON&)
        {
            static_assert(AlwaysFalse<POLYGON>::value, "No explicit specialization for this type found!");
            return POINT();
        }

        static POINT getLowerBounds(const POLYGON&)
        {
            static_assert(AlwaysFalse<POLYGON>::value, "No explicit specialization for this type found!");
            return POINT();
        }

        static bool contains(const POLYGON&, const POINT&)
        {
            static_assert(AlwaysFalse<POLYGON>::value, "No explicit specialization for this type found!");
            return false;
        }
    };
}

#endif // TRAITS_H
