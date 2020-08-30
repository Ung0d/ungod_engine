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


#ifndef UNGOD_VERTEX_ARRAY_H
#define UNGOD_VERTEX_ARRAY_H

#include <array>
#include <SFML/Graphics.hpp>
#include "ungod/serialization/MetaNode.h"

namespace ungod
{
    class VertexArray
    {
    public:
        constexpr static unsigned MAX_NUM_RECT = 5;

    public:
        /** \brief Default constructs the array. */
        VertexArray();

        void render(sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Returns the maximum number of texture rects of the VertexArray-components. */
        static constexpr unsigned maxTextureRectCount() { return MAX_NUM_RECT; }

        /** \brief Returns the current number of texture rects of the VertexArray-components. */
        unsigned textureRectCount() const { return mRectsUsed; }

        /** \brief Returns a reference to the underlying sfml vertex array. */
        sf::Vertex* getVertices();
        const sf::Vertex* getVertices() const;

        /** \brief Returns the internal position of the texture rect with the given index.
        * This is NOT the position of the VertexArray in the world. The VertexArray component itself
        * does not know about this position. */
        sf::Vector2f getPosition(unsigned index) const;

        /** \brief Returns the texture rect with given index. */
        sf::FloatRect getTextureRect(unsigned index) const;

        /** \brief Returns the bounding rect of the whole vertex array. */
        sf::FloatRect getBounds() const;

        /** \brief Returns the i-th rect. */
        sf::FloatRect getBounds(unsigned index) const;

        /**\brief Returns the color of the i-th rect. */
        const sf::Color& getRectColor(unsigned index) const;

        const sf::Vector2f& getPoint(unsigned rectIndex, unsigned pointIndex) const;

        void setRectPosition(const sf::Vector2f& pos, unsigned index);

        void moveRect(const sf::Vector2f& mv, unsigned index);

        void setRectColor(const sf::Color& color, unsigned index);

        void setRectOpacity(float opacity, unsigned index);

        bool newTextureRect(const sf::FloatRect& rect);

        bool newTextureRect(MetaNode node);

        void setTextureRect(const sf::FloatRect& rect, unsigned index);

        void setTextureRect(MetaNode node, unsigned index);

        void removeLastTextureRect();

        void setPoints(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3, const sf::Vector2f& p4, unsigned index);

        void flipX();
        void flipY();
        void flipX(unsigned i);
        void flipY(unsigned i);
        bool isFlippedX(unsigned i) const;
        bool isFlippedY(unsigned i) const;

        /** \brief Maps a position in entity local coordinates to the corresponding rect index. 
        * If the position intersects multiple rects, the rect with lowest index is returned. If no rect contains 
        * the given position, -1 is returned. */
        int getRectIndex(const sf::Vector2f& pos) const;

    private:
        std::array<sf::Vertex, 4 * MAX_NUM_RECT> mVertices;
        unsigned mRectsUsed;
    };
}

#endif // UNGOD_VERTEX_ARRAY_HJ
