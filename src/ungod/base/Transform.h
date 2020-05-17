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

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <SFML/Graphics/Transformable.hpp>
#include "owls/Signal.h"
#include "ungod/base/Entity.h"
#include "ungod/serialization/Serializable.h"

namespace ungod
{
    /**
    * \ingroup Components
    * \brief A component that contains the transform matrix of an object
    * along with its size. */
    class TransformComponent : public Serializable<TransformComponent>
    {
    friend class TransformHandler;
    friend struct SerialBehavior<TransformComponent>;
    friend struct DeserialBehavior<TransformComponent, Entity, DeserialMemory&>;
    public:
        TransformComponent() : mTransform(), mUpperBound(0, 0), mLowerBound(0, 0), mBaseLineOffsets(0.0f, 0.0f) {}

        /** \brief Returns the transform of the component. */
        const sf::Transform& getTransform() const;

        /** \brief Returns the transform with rounded position. If the scale has no fractional part and rotation is a multiple of 90 degree,
        * pixel rendering is perfect. */
        sf::Transform getRoundedTransform() const;

        /** \brief Returns the bounding box/size of the transform. */
        sf::Vector2f getSize() const;

        /** \brief Returns the world-intrinsic position of the transform. */
        sf::Vector2f getPosition() const;

        /** \brief Returns the world-intrinsic position of the transforms center. */
        sf::Vector2f getCenterPosition() const;

        /** \brief Returns the world position of the entity origin (0,0) . */
        sf::Vector2f getOriginPosition() const;

        /** \brief Returns the local upper bounds of the transform. */
        const sf::Vector2f& getUpperBounds() const;

        /** \brief Returns the local lower bounds of the transform. */
        const sf::Vector2f& getLowerBounds() const;

        /** \brief Returns the scale. */
        const sf::Vector2f& getScale() const;

        /** \brief Returns the bounds of the transform in world coordinates. */
        sf::FloatRect getBounds() const;

        /** \brief Returns the base line offsets. */
        const sf::Vector2f& getBaseLineOffsets() const;

        /** \brief Returns the left anchor point of the base line in global coordinates. */
        sf::Vector2f getLeftAnchor() const;

        /** \brief Returns the right anchor point of the base line in global coordinates. */
        sf::Vector2f getRightAnchor() const;

    private:
        sf::Transformable mTransform;
        sf::Vector2f mUpperBound;
        sf::Vector2f mLowerBound;
        sf::Vector2f mBaseLineOffsets;  ///< stores offsets (default 0,0) for the base-line-points of the entity
    };


    /** \brief A manager structure that handles all operations that modify transform components. */
    class TransformHandler
    {
    public:
        TransformHandler(quad::QuadTree<Entity>& quadtree) : mQuadTree(quadtree) {}

        /** \brief Sets position for the given entity. Emits a position changed signal. */
        void setPosition(Entity e, const sf::Vector2f& position);

        /** \brief Sets scale for the given entity. */
        void setScale(Entity e, float scale);
        void setScale(Entity e, const sf::Vector2f& scale);

        /** \brief Moves the entities transform along the given vector. Emits a position changed signal. */
        void move(Entity e, const sf::Vector2f& vec);

        /** \brief Sets the base line offsets for the given entity. The first component will serve as an offset for
        * the left anchor point and the second for the right respectively. */
        void setBaseLineOffsets(Entity e, const sf::Vector2f& baselineoffsets);

        /** \brief Registers new callback for the SizeChanged signal. */
        void onSizeChanged(const std::function<void(Entity, const sf::Vector2f&)>& callback);

        /** \brief Registers new callback for the PositionChanged signal. */
        void onPositionChanged(const std::function<void(Entity, const sf::Vector2f&)>& callback);

        /** \brief Registers new callback for the ScaleChanged signal. */
        void onScaleChanged(const std::function<void(Entity, const sf::Vector2f&)>& callback);

        /** \brief Registers new callback for the MoveContents signal. */
        void onMoveContents(const std::function<void(Entity, const sf::Vector2f&)>& callback);

        /** \brief Registers new callback for the LowerBound-request. */
        void onLowerBoundRequest(const std::function<sf::Vector2f(Entity)>& callback);

        /** \brief Registers new callback for the UpperBound-request. */
        void onUpperBoundRequest(const std::function<sf::Vector2f(Entity)>& callback);

        /** \brief Callback if the contents (for example a sprite rect) of a entity have changed.
        * The TransformManager does not care about what kind of content that was, it just processes
        * the bounding rect of the altered content and checks if it is still inside the transform-bounds. */
        void handleContentsChanged(Entity e, const sf::FloatRect& rect);

        /** \brief Callback if contents of an entity are removed.
        * The TransformManager does not care about what kind of content that was, it just updates the entity bounds accordingly. */
        void handleContentsRemoved(Entity e);

    private:
        quad::QuadTree<Entity>& mQuadTree;
        owls::Signal<Entity, const sf::Vector2f&> mPositionChangedSignal;
        owls::Signal<Entity, const sf::Vector2f&> mScaleChangedSignal;
        owls::Signal<Entity, const sf::Vector2f&> mSizeChangedSignal;
        owls::Signal<Entity, const sf::Vector2f&> mMoveContentsSignal;
        owls::Request<sf::Vector2f(Entity)> mLowerBoundRequest;
        owls::Request<sf::Vector2f(Entity)> mUpperBoundRequest;
    };
}

#endif // TRANSFORM_H

