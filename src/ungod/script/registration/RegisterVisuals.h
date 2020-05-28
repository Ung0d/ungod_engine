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

#ifndef REGISTER_VISUALS_H
#define REGISTER_VISUALS_H

#include "ungod/script/Behavior.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>

namespace ungod
{
    class Application;
    class Entity;
    class VisualsHandler;

    namespace scriptRegistration
    {
        class VisualHandlerFrontEnd
        {
        public:
            VisualHandlerFrontEnd(Entity& e, VisualsHandler& h) : mEntity(e), mHandler(h) {}
            bool isLoaded() const;
            bool isVisible() const;
            void setVisible(bool visible);
            float getOpacity() const;
            void loadTexture(const std::string& imageID);
            void loadMetadata(const std::string& file);
            unsigned getVertexRectCount() const;
            sf::Vector2f getVertexRectPosition(unsigned index) const;
            sf::FloatRect getVertexTexureRect(unsigned index) const;
            sf::FloatRect getVertexBounds() const;
            bool newVertexTextureRect(const sf::FloatRect& rect);
            bool newVertexTextureRect(const std::string& key);
            void setVertexRectPosition(const sf::Vector2f& pos, unsigned index);
            void setVertexPoints(unsigned index, const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3, const sf::Vector2f& p4);
            bool newVertexTextureRect();
            void setVertexRectColor(const sf::Color& color, unsigned index);
            const sf::Vector2f& getPoint(unsigned rectIndex, unsigned pointIndex);
            const sf::Vector2f& getSpritePosition() const;
            const sf::Vector2f& getSpritePosition(unsigned index) const;
            const sf::Vector2f& getSpriteScale() const;
            const sf::Vector2f& getSpriteScale(unsigned index) const;
            float getSpriteRotation() const;
            float getSpriteRotation(unsigned index) const;
            const sf::Vector2f& getSpriteOrigin() const;
            const sf::Vector2f& getSpriteOrigin(unsigned index) const;
            sf::FloatRect getSpriteTextureRect() const;
            sf::FloatRect getSpriteTextureRect(unsigned index) const;
            sf::FloatRect getSpriteBounds() const;
            sf::FloatRect getSpriteBounds(unsigned index) const;
            sf::FloatRect getSpriteUntransformedBounds() const;
            sf::FloatRect getSpriteUntransformedBounds(unsigned index) const;
            void setSpriteTextureRect(const sf::FloatRect& rect);
            void setSpriteTextureRect(const sf::FloatRect& rect, unsigned index);
            void setSpriteTextureRect(const std::string& key);
            void setSpriteTextureRect(const std::string& key, unsigned index);
            void setSpritePosition(const sf::Vector2f& pos);
            void setSpritePosition(const sf::Vector2f& pos, unsigned index);
            void setSpriteRotation(float rotation);
            void setSpriteRotation(float rotation, unsigned index);
            void setSpriteScale(const sf::Vector2f& scale);
            void setSpriteScale(const sf::Vector2f& scale, unsigned index);
            void setSpriteOrigin(const sf::Vector2f& origin);
            void setSpriteOrigin(const sf::Vector2f& origin, unsigned index);
            void setSpriteColor(const sf::Color& color);
            void setSpriteColor(const sf::Color& color, unsigned index);
            void bindSpriteToAnimation();
            void bindSpriteToAnimation(unsigned spriteIndex, unsigned animationIndex);
            void bindVertexToAnimation(unsigned vertexIndex);
            void bindVertexToAnimation(unsigned vertexIndex, unsigned multiAnimationIndex);
            void setAffectorActive(bool active);
            bool isAffectorActive() const;
            void setAffectorActive(bool active, unsigned index);
            bool isAffectorActive(unsigned index) const;
            bool isAnimationLooping() const;
            bool isAnimationLooping(unsigned index) const;
            bool isAnimationRunning() const;
            bool isAnimationRunning(unsigned index) const;
            std::string getAnimationKey() const;
            std::string getAnimationKey(unsigned index) const;
            unsigned getFrameIndex() const;
            unsigned getFrameIndex(unsigned index) const;
            float getAnimationSpeed() const;
            float getAnimationSpeed(unsigned index) const;
            void setAnimationState(const std::string& key);
            void setAnimationState(const std::string& key, unsigned multiAnimationIndex);
            void newAnimationState(const std::string& key);
            void setAnimationRunning(bool running);
            void setAnimationRunning(bool running, unsigned multiAnimationIndex);
            void setAnimationSpeed(float speed);
            void setAnimationSpeed(float speed, unsigned multiAnimationIndex);
            bool isBigSpriteLoaded() const;
            bool isBigSpriteVisible() const;
            std::string getBigSpriteFilePath() const;
            const sf::Vector2f& getBigSpritePosition() const;
            const sf::Vector2f& getBigSpriteScale() const;
            const sf::Vector2f& getBigSpriteOrigin() const;
            float getBigSpriteRotation() const;
            sf::FloatRect getBigSpriteBounds() const;
            sf::Color getBigSpriteColor() const;
            void loadBigTexture(const std::string& imageID, LoadPolicy policy);
            void setBigSpriteVisibility(bool visible);
            void setBigSpritePosition(const sf::Vector2f& position);
            void setBigSpriteScale(const sf::Vector2f& scale);
            void setBigSpriteOrigin(const sf::Vector2f& origin);
            void setBigSpriteColor(const sf::Color& color);
            void setBigSpriteRotation(float rotation);
            sf::Vector2f getLowerBound() const;
            sf::Vector2f getUpperBound() const;
            void setOpacity(float opac);
        private:
            Entity& mEntity;
            VisualsHandler& mHandler;
        };

        /** \brief Registers visuals functionality for scripts. */
        void registerVisuals(ScriptStateBase& state, Application& app);
    }
}

#endif

