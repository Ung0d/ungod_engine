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

#ifndef REGISTER_PARTICLE_SYSTEM_H
#define REGISTER_PARTICLE_SYSTEM_H

#include "ungod/script/Behavior.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>

namespace ungod
{
    class Entity;
    class ParticleSystemHandler;

    namespace scriptRegistration
    {
        class ParticleSystemHandlerFrontEnd
        {
        public:
            ParticleSystemHandlerFrontEnd(Entity& e, ParticleSystemHandler& h) : mEntity(e), mHandler(h) {}
            void clearAffectors();
            void setMaxForce(float maxforce);
            void setMaxVelocity(float maxvel);
            void setParticleSpeed(float speed);
            void setUniversalEmitter();
            void setFixedPosition(const sf::Vector2f& position);
            void setEllipseDist(const sf::Vector2f& center, const sf::Vector2f& radius);
            void setRectangleDist(const sf::Vector2f& topleft, const sf::Vector2f& sizes);
            void setLineSegmentDist(const sf::Vector2f& point1, const sf::Vector2f& point2);
            void setFixedVelocity(const sf::Vector2f& vel);
            void setConeDist(const sf::Vector2f& d1, const sf::Vector2f& d2);
            void setIntervalTick(float imsmin, float imsmax, int inumparticle);
            void setOneShotTick(int inumparticle);
            void setLifetimeInterval(float ltmin, float ltmax);
            void addDirectionalForce(const sf::Vector2f& force);
            void addDisplaceForce(float speed, float circle, float angle);
            void addAnimatedParticles(const std::string& metaID, const std::string& key, std::size_t numAnim);
            void addParticleFadeOut();
            void addColorShift(const sf::Color& colorBegin, const sf::Color& colorEnd);
            void addParticleRotation(float speed);
            void addParticleScale(const sf::Vector2f& scalesBegin, const sf::Vector2f& scalesEnd);
            void addVelocityBasedRotation();
            void addExplicitTexrectInitalizer(const sf::IntRect& rect);
            void addKeyTexrectInitalizer(const std::string& metaID, const std::string& key);
            void addMultipleKeyTexrectInitalizer(const std::string& metaID, script::Environment keys);
        private:
            Entity& mEntity;
            ParticleSystemHandler& mHandler;
        };

        /** \brief Registers input functionality for scripts. */
        void registerParticleSystem(ScriptStateBase& state);
    }
}


#endif
