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

#include "ungod/visual/VertexArray.h"
#include <cmath>

namespace ungod
{
    VertexArray::VertexArray() : mVertices(sf::Quads, 0) {}

    void VertexArray::render(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(mVertices, states);
    }

    std::size_t VertexArray::textureRectCount() const
    {
        return mVertices.getVertexCount()/4;
    }


    sf::VertexArray& VertexArray::getVertexArray()
    {
        return mVertices;
    }


    const sf::VertexArray& VertexArray::getVertexArray() const
    {
        return mVertices;
    }

    sf::Vector2f VertexArray::getPosition(std::size_t index) const
    {
        return mVertices[index*4].position;
    }


    sf::FloatRect VertexArray::getTextureRect(std::size_t index) const
    {
        return { mVertices[index*4].texCoords, mVertices[index*4 + 2].texCoords - mVertices[index*4].texCoords };
    }

    sf::FloatRect VertexArray::getBounds() const
    {
        return mVertices.getBounds();
    }

    sf::FloatRect VertexArray::getBounds(std::size_t index) const
    {
        return { mVertices[index*4].position, mVertices[index*4 + 2].position - mVertices[index*4].position };
    }

    const sf::Color& VertexArray::getRectColor(std::size_t index) const
    {
        return mVertices[index].color;
    }

    const sf::Vector2f& VertexArray::getPoint(std::size_t rectIndex, std::size_t pointIndex) const
    {
        return mVertices[rectIndex*4 + pointIndex].position;
    }

    void VertexArray::initRects(std::size_t num)
    {
        mVertices.resize(4*num);
    }

    void VertexArray::setTextureRect(const sf::FloatRect& rect, std::size_t index)
    {
        mVertices[index*4].texCoords = sf::Vector2f(rect.left, rect.top);
        mVertices[index*4+1].texCoords = sf::Vector2f(rect.left + rect.width, rect.top);
        mVertices[index*4+2].texCoords = sf::Vector2f(rect.left + rect.width, rect.top + rect.height);
        mVertices[index*4+3].texCoords = sf::Vector2f(rect.left, rect.top + rect.height);
    }

    void VertexArray::setTextureRect(MetaNode node, std::size_t index)
    {
        auto result = node.getAttributes<float, float, float, float, float, float>(
                    {"pos_x", 0}, {"pos_y", 0}, {"width", 0}, {"height", 0}, {"offset_x", 0}, {"offset_y", 0} );
        setTextureRect(sf::FloatRect(std::get<0>(result), std::get<1>(result), std::get<2>(result), std::get<3>(result)), index);
        setRectPosition({std::get<4>(result), std::get<5>(result)}, index);
    }

    void VertexArray::setRectPosition(const sf::Vector2f& pos, std::size_t index)
    {
        sf::Vector2f rounded{std::round(pos.x), std::round(pos.y)};

        float width = mVertices[index*4+1].texCoords.x - mVertices[index*4].texCoords.x;
		float height = mVertices[index*4+2].texCoords.y - mVertices[index*4+1].texCoords.y;
        //set the new position
        mVertices[index*4].position = rounded;
        mVertices[index*4+1].position = sf::Vector2f(rounded.x + width, rounded.y);
        mVertices[index*4+2].position = sf::Vector2f(rounded.x + width, rounded.y + height);
        mVertices[index*4+3].position = sf::Vector2f(rounded.x, rounded.y + height);
    }

    void VertexArray::moveRect(const sf::Vector2f& mv, std::size_t index)
    {
        sf::Vector2f rounded{std::round(mv.x), std::round(mv.y)};

        mVertices[index*4].position += rounded;
        mVertices[index*4+1].position += rounded;
        mVertices[index*4+2].position += rounded;
        mVertices[index*4+3].position += rounded;
    }

    void VertexArray::setRectColor(const sf::Color& color, std::size_t index)
    {
        for (unsigned i = 0; i < 4; ++i)
            mVertices[index*4 + i].color = color;
    }

    void VertexArray::setRectOpacity(float opacity, std::size_t index)
    {
        for (unsigned i = 0; i < 4; ++i)
            mVertices[index*4 + i].color.a = (sf::Uint8)(255*opacity);
    }

    std::size_t VertexArray::newTextureRect(const sf::FloatRect& rect)
    {
        mVertices.resize(mVertices.getVertexCount() + 4);
        std::size_t index = (mVertices.getVertexCount()/4)-1;
        setTextureRect(rect, index);
        return index;
    }

    void VertexArray::setPoints(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3, const sf::Vector2f& p4, std::size_t index)
    {
        mVertices[index*4].position = p1;
        mVertices[index*4+1].position = p2;
        mVertices[index*4+2].position = p3;
        mVertices[index*4+3].position = p4;
    }

    void VertexArray::flipX()
    {
        for (unsigned i = 0; i < textureRectCount(); i++)
        {
            sf::Vertex* quad = &mVertices[i*4];
            std::swap(quad[0].texCoords, quad[1].texCoords);
            std::swap(quad[3].texCoords, quad[2].texCoords);
        }
    }

    void VertexArray::flipY()
    {
        for (unsigned i = 0; i < textureRectCount(); i++)
        {
            sf::Vertex* quad = &mVertices[(std::size_t)i*4];
            std::swap(quad[0].texCoords, quad[3].texCoords);
            std::swap(quad[1].texCoords, quad[2].texCoords);
        }
    }
}
