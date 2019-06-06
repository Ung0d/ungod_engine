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

#ifndef UNGOD_SPRITE_H
#define UNGOD_SPRITE_H

#include <SFML/Graphics.hpp>
#include <array>
#include "ungod/Serialization/MetaNode.h"

namespace ungod
{
    class Sprite
    {
    public:
        Sprite() = default;

        void render(sf::RenderTarget& target, sf::RenderStates states) const;

        const sf::Vector2f& getPosition() const;

        const sf::Vector2f& getScale() const;

        float getRotation() const;

        const sf::Vector2f& getOrigin() const;

        const sf::Transform& getTransform() const;

        sf::FloatRect getTextureRect() const;

        sf::FloatRect getBounds() const;

        sf::FloatRect getUntransformedBounds() const;

        sf::Vertex* getVertices();

        void setPosition(const sf::Vector2f& position);

        void setPosition(float x, float y);

        void move(const sf::Vector2f& mv);

        void setRotation(float rotation);

        void setOrigin(const sf::Vector2f& origin);

        void setScale(const sf::Vector2f& scale);

        void setScale(float scalex, float scaley);

        void setTextureRect(const sf::FloatRect& rect);

        void setColor(const sf::Color& color);

        void setOpacity(float opacity);

        void setTextureRect(MetaNode node);

        void flipX();

        void flipY();

    private:
        std::array<sf::Vertex, 4> mVertices;
        sf::Transformable mTransformable;
    };
}

#endif // UNGOD_SPRITE_H
