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

#include "ungod/visual/Sprite.h"
#include <cmath>

namespace ungod
{
    void Sprite::render(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform *= getTransform();
        target.draw(&mVertices[0], 4u, sf::Quads, states);
    }

    const sf::Vector2f& Sprite::getPosition() const
    {
        return mTransformable.getPosition();
    }

    const sf::Vector2f& Sprite::getScale() const
    {
        return mTransformable.getScale();  //x equals y
    }

    float Sprite::getRotation() const
    {
        return mTransformable.getRotation();
    }

    const sf::Vector2f& Sprite::getOrigin() const
    {
        return mTransformable.getOrigin();
    }

    const sf::Transform& Sprite::getTransform() const
    {
        return mTransformable.getTransform();
    }

    sf::FloatRect Sprite::getTextureRect() const
    {
        return { mVertices[0].texCoords, mVertices[2].texCoords - mVertices[0].texCoords };
    }

    sf::FloatRect Sprite::getBounds() const
    {
        return mTransformable.getTransform().transformRect( {mVertices[0].position, mVertices[2].position - mVertices[0].position} );
    }

    sf::FloatRect Sprite::getUntransformedBounds() const
    {
        return {mVertices[0].position, mVertices[2].position - mVertices[0].position};
    }

    sf::Vertex* Sprite::getVertices()
    {
        return &mVertices[0];
    }

    void Sprite::setPosition(const sf::Vector2f& position)
    {
        sf::Vector2f rounded{std::round(position.x), std::round(position.y)} ;
        mTransformable.setPosition(rounded);
    }

    void Sprite::setPosition(float x, float y)
    {
        mTransformable.setPosition(std::round(x), std::round(y));
    }

    void Sprite::move(const sf::Vector2f& mv)
    {
        mTransformable.move(mv);  //TODO round here?
    }

    void Sprite::setRotation(float rotation)
    {
        mTransformable.setRotation(rotation);
    }

    void Sprite::setOrigin(const sf::Vector2f& origin)
    {
        mTransformable.setOrigin(origin);
    }

    void Sprite::setScale(const sf::Vector2f& scale)
    {
        mTransformable.setScale(scale);
    }

    void Sprite::setScale(float scalex, float scaley)
    {
        mTransformable.setScale(scalex, scaley);
    }

    void Sprite::setTextureRect(const sf::FloatRect& rect)
    {
        mVertices[0].texCoords = sf::Vector2f(rect.left, rect.top);
        mVertices[1].texCoords = sf::Vector2f(rect.left + rect.width, rect.top);
        mVertices[2].texCoords = sf::Vector2f(rect.left + rect.width, rect.top + rect.height);
        mVertices[3].texCoords = sf::Vector2f(rect.left, rect.top + rect.height);
        mVertices[1].position = sf::Vector2f(mVertices[0].position.x + rect.width, mVertices[0].position.y);
        mVertices[2].position = sf::Vector2f(mVertices[0].position.x + rect.width, mVertices[0].position.y + rect.height);
        mVertices[3].position = sf::Vector2f(mVertices[0].position.x, mVertices[0].position.y + rect.height);
    }

    void Sprite::setColor(const sf::Color& color)
    {
        for (unsigned i = 0; i < 4; ++i)
            mVertices[i].color = color;
    }

    void Sprite::setOpacity(float opacity)
    {
        for (unsigned i = 0; i < 4; ++i)
            mVertices[i].color.a = (sf::Uint8)(255*opacity);
    }

    void Sprite::setTextureRect(MetaNode node)
    {
        auto result = node.getAttributes<float, float, float, float, float, float>(
                    {"pos_x", 0}, {"pos_y", 0}, {"width", 0}, {"height", 0}, {"offset_x", 0}, {"offset_y", 0} );
        setTextureRect(sf::FloatRect(std::get<0>(result), std::get<1>(result), std::get<2>(result), std::get<3>(result)));
        setPosition({std::get<4>(result), std::get<5>(result)});
    }

    void Sprite::flipX()
    {
        std::swap(mVertices[0].texCoords, mVertices[1].texCoords);
        std::swap(mVertices[3].texCoords, mVertices[2].texCoords);
    }

    void Sprite::flipY()
    {
        std::swap(mVertices[0].texCoords, mVertices[3].texCoords);
        std::swap(mVertices[1].texCoords, mVertices[2].texCoords);
    }
}
