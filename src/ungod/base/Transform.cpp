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

#include "ungod/base/Transform.h"

namespace ungod
{
    const sf::Transform& TransformComponent::getTransform() const { return mTransform.getTransform(); }


    sf::Transform TransformComponent::getRoundedTransform() const
    {
        sf::Transformable rounded = mTransform;
        rounded.setPosition( std::round(rounded.getPosition().x), std::round(rounded.getPosition().y) );
        return rounded.getTransform();
    }


    sf::Vector2f TransformComponent::getSize() const { return { getScale().x * (mLowerBound.x - mUpperBound.x), getScale().y * (mLowerBound.y - mUpperBound.y) }; }


    sf::Vector2f TransformComponent::getPosition() const { return mTransform.getTransform().transformPoint(mUpperBound); }


    sf::Vector2f TransformComponent::getCenterPosition() const
    {
        return mTransform.getTransform().transformPoint(0.5f*(mLowerBound + mUpperBound));
    }


    const sf::Vector2f& TransformComponent::getScale() const { return mTransform.getScale(); }


    const sf::Vector2f& TransformComponent::getUpperBounds() const { return mUpperBound; }


    const sf::Vector2f& TransformComponent::getLowerBounds() const { return mLowerBound; }


    sf::FloatRect TransformComponent::getBounds() const
    {
        return sf::FloatRect(getPosition(), getSize());
    }

    const sf::Vector2f& TransformComponent::getBaseLineOffsets() const
    {
        return mBaseLineOffsets;
    }

    sf::Vector2f TransformComponent::getLeftAnchor() const
    {
        sf::Vector2f local = sf::Vector2f{ mUpperBound.x, mLowerBound.y - mBaseLineOffsets.x };
        return mTransform.getTransform().transformPoint(local);
    }

    sf::Vector2f TransformComponent::getRightAnchor() const
    {
        sf::Vector2f local = sf::Vector2f{ mLowerBound.x, mLowerBound.y - mBaseLineOffsets.y };
        return mTransform.getTransform().transformPoint(local);
    }


    TransformManager::TransformManager(quad::QuadTree<Entity>& quadtree) : mQuadTree(quadtree) {}


    void TransformManager::setPosition(Entity e, const sf::Vector2f& position)
    {
        e.modify<TransformComponent>().mTransform.setPosition(position);
        //emit signal
        mPositionChangedSignal.emit(e, position);
        //update quadtree
        mQuadTree.changedProperties(e);
    }


    void TransformManager::setScale(Entity e, float scale)
    {
        setScale(e, {scale, scale});
    }

    void TransformManager::setScale(Entity e, const sf::Vector2f& scale)
    {
        e.modify<TransformComponent>().mTransform.setScale(scale);
        //emit signal
        mSizeChangedSignal.emit(e, e.modify<TransformComponent>().getSize());
        mScaleChangedSignal.emit(e, scale);
        //update quadtree
        mQuadTree.changedProperties(e);
    }


    void TransformManager::move(Entity e, const sf::Vector2f& vec)
    {
        e.modify<TransformComponent>().mTransform.move(vec);
        //emit signal
        mPositionChangedSignal.emit(e, e.modify<TransformComponent>().getPosition());
        //update quadtree
        mQuadTree.changedProperties(e);
    }


    void TransformManager::setBaseLineOffsets(Entity e, const sf::Vector2f& baselineoffsets)
    {
        e.modify<TransformComponent>().mBaseLineOffsets = baselineoffsets;
    }


    void TransformManager::onSizeChanged(const std::function<void(Entity, const sf::Vector2f&)>& callback)
    {
        mSizeChangedSignal.connect(callback);
    }


    void TransformManager::onPositionChanged(const std::function<void(Entity, const sf::Vector2f&)>& callback)
    {
        mPositionChangedSignal.connect(callback);
    }


    void TransformManager::onScaleChanged(const std::function<void(Entity, const sf::Vector2f&)>& callback)
    {
        mScaleChangedSignal.connect(callback);
    }


    void TransformManager::onMoveContents(const std::function<void(Entity, const sf::Vector2f&)>& callback)
    {
        mMoveContentsSignal.connect(callback);
    }


    void TransformManager::onLowerBoundRequest(const std::function<sf::Vector2f(Entity)>& callback)
    {
        mLowerBoundRequest.connect(callback);
    }


    void TransformManager::onUpperBoundRequest(const std::function<sf::Vector2f(Entity)>& callback)
    {
        mUpperBoundRequest.connect(callback);
    }


    void TransformManager::handleContentsChanged(Entity e, const sf::FloatRect& rect)
    {
        TransformComponent& transf = e.modify<TransformComponent>();

        float origLeft = rect.left;
		float origTop = rect.top;

        bool rightExceed = origLeft + rect.width > transf.mLowerBound.x;
        bool bottomExceed = origTop + rect.height > transf.mLowerBound.y;
        bool leftExceed = origLeft < transf.mUpperBound.x;
        bool topExceed = origTop < transf.mUpperBound.y;
        bool shrink = false;

        if (rightExceed)
            transf.mLowerBound.x = origLeft + rect.width;
        if (bottomExceed)
            transf.mLowerBound.y = origTop + rect.height;
        if (leftExceed)
            transf.mUpperBound.x = origLeft;
        if (topExceed)
            transf.mUpperBound.y = origTop;

        if (!rightExceed || !bottomExceed)
        {
            sf::Vector2f bounds{0,0};
            mLowerBoundRequest([&bounds] (const sf::Vector2f& v)
            {
                bounds.x = std::max(bounds.x, v.x);
                bounds.y = std::max(bounds.y, v.y);
            }, e);
            shrink = bounds.x < transf.mLowerBound.x || bounds.y < transf.mLowerBound.y;
            transf.mLowerBound = bounds;
        }

        if (!leftExceed || !topExceed)
        {
            sf::Vector2f bounds{0,0};
            mUpperBoundRequest([&bounds] (const sf::Vector2f& v)
            {
                bounds.x = std::min(bounds.x, v.x);
                bounds.y = std::min(bounds.y, v.y);
            }, e);
            shrink = shrink || bounds.x > transf.mLowerBound.x || bounds.y > transf.mLowerBound.y;
            transf.mUpperBound = bounds;
        }

        if (rightExceed || bottomExceed || leftExceed || topExceed || shrink)
        {
            //emit signal
            mSizeChangedSignal(e, e.get<TransformComponent>().getSize());
            //update quadtree
            mQuadTree.changedProperties(e);
        }
    }

    void TransformManager::handleContentsRemoved(Entity e)
    {
        TransformComponent& transf = e.modify<TransformComponent>();

        bool shrink = false;

        sf::Vector2f lbounds{0,0};
        mLowerBoundRequest([&lbounds] (const sf::Vector2f& v)
        {
            lbounds.x = std::max(lbounds.x, v.x);
            lbounds.y = std::max(lbounds.y, v.y);
        }, e);
        shrink = lbounds.x < transf.mLowerBound.x || lbounds.y < transf.mLowerBound.y;
        transf.mLowerBound = lbounds;

        sf::Vector2f ubounds{0,0};
        mUpperBoundRequest([&ubounds] (const sf::Vector2f& v)
        {
            ubounds.x = std::min(ubounds.x, v.x);
            ubounds.y = std::min(ubounds.y, v.y);
        }, e);
        shrink = shrink || ubounds.x > transf.mLowerBound.x || ubounds.y > transf.mLowerBound.y;
        transf.mUpperBound = ubounds;

        if (shrink)
        {
            //emit signal
            mSizeChangedSignal(e, e.get<TransformComponent>().getSize());
            //update quadtree
            mQuadTree.changedProperties(e);
        }
    }
}
