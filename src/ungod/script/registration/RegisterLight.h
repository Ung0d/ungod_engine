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

#ifndef REGISTER_LIGHT_H
#define REGISTER_LIGHT_H

#include "ungod/script/Behavior.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

namespace ungod
{
    class Entity;
    class LightHandler;
    class LightEmitterComponent;

    namespace scriptRegistration
    {
        class LightHandlerFrontEnd
        {
        public:
            LightHandlerFrontEnd(Entity& e, LightHandler& h) : mEntity(e), mHandler(h) {}
            void setLightPosition(const sf::Vector2f& pos);
            void setLightPosition(const sf::Vector2f& pos, std::size_t index);
            void setLightScale(const sf::Vector2f& scale);
            void setLightScale(const sf::Vector2f& scale, std::size_t index);
            void setLightColor(const sf::Color& color);
            void setLightColor(const sf::Color& color, std::size_t index);
            void setColliderPoint(const sf::Vector2f& point, std::size_t pointIndex);
            void setColliderPoint(const sf::Vector2f& point, std::size_t pointIndex, std::size_t colliderIndex);
            void setColliderPointCount(std::size_t num);
            void setColliderPointCount(std::size_t num, std::size_t colliderIndex);
            void setColliderPoints(script::Environment points);
            void setColliderPoints(script::Environment points, std::size_t index);
            void setLightOverShape(bool los);
            void setLightOverShape(bool los, std::size_t index);
            void setLightActive(bool active);
            void setLightActive(bool active, std::size_t index);
            bool isLightActive();
            bool isLightActive(std::size_t index);
            void setAffectorCallback(const std::function<void(float, LightEmitterComponent&)>& callback);
            void setAffectorCallback(const std::function<void(float, LightEmitterComponent&)>& callback, std::size_t lightIndex);
            void setAffectorCallback(const std::function<void(float, LightEmitterComponent&)>& callback, std::size_t lightIndex, std::size_t affectorIndex);
            void setAffectorActive(bool active);
            void setAffectorActive(bool active, std::size_t affectorIndex);
            bool isAffectorActive();
            bool isAffectorActive(std::size_t affectorIndex);
        private:
            Entity& mEntity;
            LightHandler& mHandler;
        };

        /** \brief Registers light functionality for scripts. */
        void registerLight(ScriptStateBase& state);
    }
}

#endif

