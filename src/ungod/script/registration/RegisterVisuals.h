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
            void loadTexture(const std::string& imageID, LoadPolicy policy);
            void loadMetadata(const std::string& file);
            std::size_t getVertexRectCount() const;
            sf::Vector2f getVertexRectPosition(std::size_t index) const;
            sf::FloatRect getVertexTexureRect(std::size_t index) const;
            sf::FloatRect getVertexBounds() const;
            void initVertexRects(std::size_t num);
            void setVertexTextureRect(const sf::FloatRect& rect, std::size_t index);
            void setVertexTextureRect(const std::string& key, std::size_t index);
            void setVertexRectPosition(const sf::Vector2f& pos, std::size_t index);
            void setVertexPoints(std::size_t index, const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3, const sf::Vector2f& p4);
            std::size_t newVertexRect();
            std::size_t newVertexRect(const sf::FloatRect& rect);
            std::size_t newVertexRect(const std::string& key);
            void setVertexRectColor(const sf::Color& color, std::size_t index);
            const sf::Vector2f& getPoint(std::size_t rectIndex, std::size_t pointIndex);
            const sf::Vector2f& getSpritePosition() const;
            const sf::Vector2f& getSpritePosition(std::size_t index) const;
            const sf::Vector2f& getSpriteScale() const;
            const sf::Vector2f& getSpriteScale(std::size_t index) const;
            float getSpriteRotation() const;
            float getSpriteRotation(std::size_t index) const;
            const sf::Vector2f& getSpriteOrigin() const;
            const sf::Vector2f& getSpriteOrigin(std::size_t index) const;
            sf::FloatRect getSpriteTextureRect() const;
            sf::FloatRect getSpriteTextureRect(std::size_t index) const;
            sf::FloatRect getSpriteBounds() const;
            sf::FloatRect getSpriteBounds(std::size_t index) const;
            sf::FloatRect getSpriteUntransformedBounds() const;
            sf::FloatRect getSpriteUntransformedBounds(std::size_t index) const;
            void setSpriteTextureRect(const sf::FloatRect& rect);
            void setSpriteTextureRect(const sf::FloatRect& rect, std::size_t index);
            void setSpriteTextureRect(const std::string& key);
            void setSpriteTextureRect(const std::string& key, std::size_t index);
            void setSpritePosition(const sf::Vector2f& pos);
            void setSpritePosition(const sf::Vector2f& pos, std::size_t index);
            void setSpriteRotation(float rotation);
            void setSpriteRotation(float rotation, std::size_t index);
            void setSpriteScale(const sf::Vector2f& scale);
            void setSpriteScale(const sf::Vector2f& scale, std::size_t index);
            void setSpriteOrigin(const sf::Vector2f& origin);
            void setSpriteOrigin(const sf::Vector2f& origin, std::size_t index);
            void setSpriteColor(const sf::Color& color);
            void setSpriteColor(const sf::Color& color, std::size_t index);
            void bindSpriteToAnimation();
            void bindSpriteToAnimation(std::size_t spriteIndex, std::size_t animationIndex);
            void bindVertexToAnimation(std::size_t vertexIndex);
            void bindVertexToAnimation(std::size_t vertexIndex, std::size_t multiAnimationIndex);
            void setAffectorActive(bool active);
            bool isAffectorActive() const;
            void setAffectorActive(bool active, std::size_t index);
            bool isAffectorActive(std::size_t index) const;
            bool isAnimationLooping() const;
            bool isAnimationLooping(std::size_t index) const;
            bool isAnimationRunning() const;
            bool isAnimationRunning(std::size_t index) const;
            std::string getAnimationKey() const;
            std::string getAnimationKey(std::size_t index) const;
            unsigned getFrameIndex() const;
            unsigned getFrameIndex(std::size_t index) const;
            float getAnimationSpeed() const;
            float getAnimationSpeed(std::size_t index) const;
            void setAnimationState(const std::string& key);
            void setAnimationState(const std::string& key, std::size_t multiAnimationIndex);
            void newAnimationState(const std::string& key);
            void setAnimationRunning(bool running);
            void setAnimationRunning(bool running, std::size_t multiAnimationIndex);
            void setAnimationSpeed(float speed);
            void setAnimationSpeed(float speed, std::size_t multiAnimationIndex);
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

