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

#include "ungod/physics/Collision.h"
#include "ungod/base/World.h"

namespace ungod
{
    Collider::Collider() :
        mUpleft(), mDownright(), mRotation(0.0f) {}

    Collider::Collider(const sf::Vector2f& upleft, const sf::Vector2f& downright, float rotation) :
        mUpleft(upleft), mDownright(downright), mRotation(rotation) {}

    const sf::Vector2f& Collider::getUpleft() const
    {
        return mUpleft;
    }

    const sf::Vector2f& Collider::getDownright() const
    {
        return mDownright;
    }

    sf::Vector2f Collider::getCenter() const
    {
        return mUpleft + 0.5f*(mDownright-mUpleft);
    }

    sf::IntRect Collider::getBoundingBox(const TransformComponent& t) const
    {
        return getBoundingBox(t.getTransform());
    }

    sf::IntRect Collider::getBoundingBox(sf::Transform transf) const
    {
        transf.rotate(mRotation, getCenter());
        sf::Vector2f p1 = transf.transformPoint(mUpleft);
        sf::Vector2f p2 = transf.transformPoint({mDownright.x, mUpleft.y});
        sf::Vector2f p3 = transf.transformPoint(mDownright);
        sf::Vector2f p4 = transf.transformPoint({mUpleft.x, mDownright.y});

        float minx = std::numeric_limits<float>::infinity();
        float maxx = -std::numeric_limits<float>::infinity();
        float miny = std::numeric_limits<float>::infinity();
        float maxy = -std::numeric_limits<float>::infinity();

        minx = std::min(minx, p1.x);
        maxx = std::max(maxx, p1.x);
        miny = std::min(miny, p1.y);
        maxy = std::max(maxy, p1.y);

        minx = std::min(minx, p2.x);
        maxx = std::max(maxx, p2.x);
        miny = std::min(miny, p2.y);
        maxy = std::max(maxy, p2.y);

        minx = std::min(minx, p3.x);
        maxx = std::max(maxx, p3.x);
        miny = std::min(miny, p3.y);
        maxy = std::max(maxy, p3.y);

        minx = std::min(minx, p4.x);
        maxx = std::max(maxx, p4.x);
        miny = std::min(miny, p4.y);
        maxy = std::max(maxy, p4.y);

        return {(int)minx, (int)miny, (int)(maxx-minx), (int)(maxy-miny)};
    }

    Collider Collider::translate(const TransformComponent& t) const
    {
        return { t.getTransform().transformPoint(mUpleft),
                 t.getTransform().transformPoint(mDownright) };
    }

    float Collider::getRotation() const
    {
        return mRotation;
    }

    void Collider::move(const sf::Vector2f& vec)
    {
        mUpleft += vec;
        mDownright += vec;
    }


    std::pair<bool, sf::Vector2f> satAlgorithm( const Collider& c1, const Collider& c2, const TransformComponent& t1, const TransformComponent& t2 )
    {
        sf::Transform finalTransf1;
        sf::Transform finalTransf2;
        finalTransf1 *= t1.getTransform();
        finalTransf2 *= t2.getTransform();
        finalTransf1.rotate(c1.getRotation(), c1.getCenter());
        finalTransf2.rotate(c2.getRotation(), c2.getCenter());

        //get the axis to test
        std::array<sf::Vector2f, 4> mAxis;
        std::array<sf::Vector2f, 4> mPoints1;
        std::array<sf::Vector2f, 4> mPoints2;

        mPoints1[0] = finalTransf1.transformPoint( c1.getDownright() );
        mPoints1[1] = finalTransf1.transformPoint( {c1.getUpleft().x, c1.getDownright().y} );
        mPoints1[2] = finalTransf1.transformPoint( c1.getUpleft() );
        mPoints1[3] = finalTransf1.transformPoint( {c1.getDownright().x, c1.getUpleft().y} );

        mPoints2[0] = finalTransf2.transformPoint( c2.getDownright() );
        mPoints2[1] = finalTransf2.transformPoint( {c2.getUpleft().x, c2.getDownright().y} );
        mPoints2[2] = finalTransf2.transformPoint( c2.getUpleft() );
        mPoints2[3] = finalTransf2.transformPoint( {c2.getDownright().x, c2.getUpleft().y} );

        mAxis[0] = mPoints1[3] - mPoints1[2];
        mAxis[1] = mPoints1[1] - mPoints1[2];
        mAxis[2] = mPoints2[3] - mPoints2[2];
        mAxis[3] = mPoints2[1] - mPoints2[2];

        float overlap;
        float smallestOverlap = std::numeric_limits<float>::max();
        sf::Vector2f offset;

        for (auto& axis : mAxis)
        {
            normalize(axis);
            sf::Vector2f perp = perpendicularVector( axis );

            float c1Left = std::numeric_limits<float>::infinity();
            float c1Right = -std::numeric_limits<float>::infinity();
            float c2Left = std::numeric_limits<float>::infinity();
            float c2Right = -std::numeric_limits<float>::infinity();

            float projection;

            for (auto& point : mPoints1)
            {
                projection = dotProduct(perp, point);
                c1Right = std::max(c1Right, projection);
                c1Left = std::min(c1Left, projection);
            }

            for (auto& point : mPoints2)
            {
                projection = dotProduct(perp, point);
                c2Right = std::max(c2Right, projection);
                c2Left = std::min(c2Left, projection);
            }

            overlap = std::min(c1Right, c2Right) - std::max(c1Left, c2Left);

            if (overlap < 0)
            {
                return { false, {0,0} };
            }
            if (overlap < smallestOverlap)
            {
                smallestOverlap = overlap;
                offset = perp*overlap;
            }
        }

        if (dotProduct( finalTransf1.transformPoint(c1.getCenter()) -
                        finalTransf2.transformPoint(c2.getCenter()), offset) < 0)
        {
            offset = -offset;
        }

        return { true, offset };
    }


    bool containsPoint(const Collider& collider, const TransformComponent& transf, const sf::Vector2f& point)
    {
        //translate the point into the collider "local" coordinates
        sf::Vector2f transfPoint = transf.getTransform().getInverse().transformPoint(point);
        if (collider.getRotation() != 0.0f)
        {
            //rotate the point in the coord.system of the collider
            sf::Transform inverseRotation;
            inverseRotation.rotate( -collider.getRotation(), collider.getCenter() );
            transfPoint = inverseRotation.transformPoint(transfPoint);

        }
        return transfPoint.x >= collider.getUpleft().x && transfPoint.y >= collider.getUpleft().y &&
               transfPoint.x <= collider.getDownright().x && transfPoint.y <= collider.getDownright().y;
    }


    void RigidbodyManager::onContentsChanged(const std::function<void(Entity, const sf::IntRect&)>& callback)
    {
        mContentsChangedSignal.connect(callback);
    }

    void RigidbodyManager::onContentRemoved(const std::function<void(Entity)>& callback)
    {
        mContentRemoved.connect(callback);
    }

    sf::Vector2f RigidbodyManager::getLowerBound(Entity e)
    {
        sf::Vector2f lowerBounds(0,0);
        ContextIteration<MAX_CONTEXTS>::getLowerBounds(*this, e, lowerBounds);
        return lowerBounds;
    }

    sf::Vector2f RigidbodyManager::getUpperBound(Entity e)
    {
        sf::Vector2f upperBounds(0,0);
        ContextIteration<MAX_CONTEXTS>::getUpperBounds(*this, e, upperBounds);
        return upperBounds;
    }

    void RigidbodyManager::moveColliders(Entity e, const sf::Vector2f& vec)
    {
        ContextIteration<MAX_CONTEXTS>::iterate(*this, e, vec);
    }
}
