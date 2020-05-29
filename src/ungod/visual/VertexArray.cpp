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
    VertexArray::VertexArray() : mVertices(), mRectsUsed(0u) {}

    void VertexArray::render(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(getVertices(), 4u*mRectsUsed, sf::Quads, states);
    }


    sf::Vertex* VertexArray::getVertices()
    {
        return &mVertices[0];
    }


    const sf::Vertex* VertexArray::getVertices() const
    {
        return &mVertices[0];
    }

    sf::Vector2f VertexArray::getPosition(unsigned index) const
    {
        return mVertices[index * 4u].position;
    }


    sf::FloatRect VertexArray::getTextureRect(unsigned index) const
    {
         return { mVertices[index*4u].texCoords, mVertices[index*4u + 2].texCoords - mVertices[index*4u].texCoords };
    }

    sf::FloatRect VertexArray::getBounds() const
    {
        if (mRectsUsed == 0u)
            return {};
        sf::Vector2f topleft = mVertices[0].position;
        sf::Vector2f botright = mVertices[0].position;
        for (unsigned i = 1; i < mRectsUsed*4u; i++)
        {
            topleft.x = std::min(topleft.x, mVertices[i].position.x);
            topleft.y = std::min(topleft.y, mVertices[i].position.y);
            botright.x = std::max(botright.x, mVertices[i].position.x);
            botright.y = std::max(botright.y, mVertices[i].position.y);
        }
        return {topleft, botright - topleft};
    }

    sf::FloatRect VertexArray::getBounds(unsigned index) const
    {
        return { mVertices[index*4].position, mVertices[index*4u + 2].position - mVertices[index*4u].position };
    }

    const sf::Color& VertexArray::getRectColor(unsigned index) const
    {
        return mVertices[index].color;
    }

    const sf::Vector2f& VertexArray::getPoint(unsigned rectIndex, unsigned pointIndex) const
    {
        return mVertices[rectIndex*4u + pointIndex].position;
    }

    void VertexArray::setRectPosition(const sf::Vector2f& pos, unsigned index)
    {
        sf::Vector2f rounded{std::round(pos.x), std::round(pos.y)};

        float width = std::abs(mVertices[index*4u+1].texCoords.x - mVertices[index*4u].texCoords.x);
		float height = std::abs(mVertices[index*4u+2].texCoords.y - mVertices[index*4u+1].texCoords.y);
        //set the new position
        mVertices[index*4u].position = rounded;
        mVertices[index*4u+1].position = sf::Vector2f(rounded.x + width, rounded.y);
        mVertices[index*4u+2].position = sf::Vector2f(rounded.x + width, rounded.y + height);
        mVertices[index*4u+3].position = sf::Vector2f(rounded.x, rounded.y + height);
    }

    void VertexArray::moveRect(const sf::Vector2f& mv, unsigned index)
    {
        sf::Vector2f rounded{std::round(mv.x), std::round(mv.y)};

        mVertices[index*4u].position += rounded;
        mVertices[index*4u+1].position += rounded;
        mVertices[index*4u+2].position += rounded;
        mVertices[index*4u+3].position += rounded;
    }

    void VertexArray::setRectColor(const sf::Color& color, unsigned index)
    {
        for (unsigned i = 0; i < 4u; ++i)
            mVertices[index*4u + i].color = color;
    }

    void VertexArray::setRectOpacity(float opacity, unsigned index)
    {
        for (unsigned i = 0; i < 4u; ++i)
            mVertices[index*4u + i].color.a = (sf::Uint8)(255*opacity);
    }

    bool VertexArray::newTextureRect(const sf::FloatRect& rect)
    {
        if (mRectsUsed < MAX_NUM_RECT)
        {
            mVertices[mRectsUsed * 4u].texCoords = sf::Vector2f(rect.left, rect.top);
            mVertices[mRectsUsed * 4u + 1].texCoords = sf::Vector2f(rect.left + rect.width, rect.top);
            mVertices[mRectsUsed * 4u + 2].texCoords = sf::Vector2f(rect.left + rect.width, rect.top + rect.height);
            mVertices[mRectsUsed * 4u + 3].texCoords = sf::Vector2f(rect.left, rect.top + rect.height);
            mRectsUsed++;
            return true;
        }
        else
        {
            Logger::error("The maximum allowed number of rects (", MAX_NUM_RECT, ") is already reached.");
            return false;
        }
    }

    bool VertexArray::newTextureRect(MetaNode node)
    {
        auto result = node.getAttributes<float, float, float, float, float, float>(
            { "pos_x", 0 }, { "pos_y", 0 }, { "width", 0 }, { "height", 0 }, { "offset_x", 0 }, { "offset_y", 0 });
        bool done = newTextureRect(sf::FloatRect(std::get<0>(result), std::get<1>(result), std::get<2>(result), std::get<3>(result)));
        if (done)
            setRectPosition({ std::get<4>(result), std::get<5>(result) }, mRectsUsed-1);
        return done;
    }

    void VertexArray::setPoints(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3, const sf::Vector2f& p4, unsigned index)
    {
        mVertices[index*4u].position = p1;
        mVertices[index*4u+1].position = p2;
        mVertices[index*4u+2].position = p3;
        mVertices[index*4u+3].position = p4;
    }

    void VertexArray::flipX()
    {
        for (unsigned i = 0; i < textureRectCount(); i++)
            flipX(i);
    }

    void VertexArray::flipY()
    {
        for (unsigned i = 0; i < textureRectCount(); i++)
            flipY(i);
    }

    void VertexArray::flipX(unsigned i)
    {
        sf::Vertex* quad = &mVertices[i * 4u];
        std::swap(quad[0].texCoords, quad[1].texCoords);
        std::swap(quad[3].texCoords, quad[2].texCoords);
    }

    void VertexArray::flipY(unsigned i)
    {
        sf::Vertex* quad = &mVertices[i * 4u];
        std::swap(quad[0].texCoords, quad[3].texCoords);
        std::swap(quad[1].texCoords, quad[2].texCoords);
    }

    bool VertexArray::isFlippedX(unsigned i) const
    {
        return mVertices[i * 4u].texCoords.x > mVertices[i * 4u + 1].texCoords.x;
    }

    bool VertexArray::isFlippedY(unsigned i) const
    {
        return mVertices[i * 4u].texCoords.y > mVertices[i * 4u + 3].texCoords.y;
    }
}
