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

#include <SFML/Graphics.hpp>
#include "ungod/serialization/MetaNode.h"

namespace ungod
{
    class VertexArray
    {
    public:
        /** \brief Default constructs the array. */
        VertexArray();

        void render(sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Returns the number of texture rects of the VertexArray-components. */
        std::size_t textureRectCount() const;

        /** \brief Returns a reference to the underlying sfml vertex array. */
        sf::VertexArray& getVertexArray();
        const sf::VertexArray& getVertexArray() const;

        /** \brief Returns the internal position of the texture rect with the given index.
        * This is NOT the position of the VertexArray in the world. The VertexArray component itself
        * does not know about this position. */
        sf::Vector2f getPosition(std::size_t index) const;

        /** \brief Returns the texture rect with given index. */
        sf::FloatRect getTextureRect(std::size_t index) const;

        /** \brief Returns the bounding rect of the whole vertex array. */
        sf::FloatRect getBounds() const;

        /** \brief Returns the i-th rect. */
        sf::FloatRect getBounds(std::size_t index) const;

        /**\brief Returns the color of the i-th rect. */
        const sf::Color& getRectColor(std::size_t index) const;

        const sf::Vector2f& getPoint(std::size_t rectIndex, std::size_t pointIndex) const;

        void initRects(std::size_t num);

        void setTextureRect(const sf::FloatRect& rect, std::size_t index);

        void setTextureRect(MetaNode node, std::size_t index);

        void setRectPosition(const sf::Vector2f& pos, std::size_t index);

        void moveRect(const sf::Vector2f& mv, std::size_t index);

        void setRectColor(const sf::Color& color, std::size_t index);

        void setRectOpacity(float opacity, std::size_t index);

        std::size_t newTextureRect(const sf::FloatRect& rect);

        void setPoints(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3, const sf::Vector2f& p4, std::size_t index);

        void flipX();

        void flipY();

    private:
        sf::VertexArray mVertices;
    };
}

#endif // UNGOD_VERTEX_ARRAY_HJ
