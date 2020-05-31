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

#ifndef REGISTER_MOVEMENT_H
#define REGISTER_MOVEMENT_H

#include "ungod/script/Behavior.h"
#include "ungod/physics/Movement.h"
#include "ungod/physics/Path.h"
#include "ungod/physics/Steering.h"

namespace ungod
{
    namespace scriptRegistration
    {
        class MovementHandlerFrontEnd
        {
        public:
            MovementHandlerFrontEnd(Entity& e, MovementHandler& h) : mEntity(e), mHandler(h) {}
            const sf::Vector2f& getVelocity() const;
            const sf::Vector2f& getAcceleration() const;
            MovementComponent::Direction getDirection() const;
            float getMaxVelocity() const;
            float getMaxForce() const;
            float getBaseSpeed() const;
            void setMaximumVelocity(float maxvel);
            void setBaseSpeed(float basespeed);
            void setMaximumForce(float maxforce);
            void accelerate(const sf::Vector2f& acceleration);
            void resetAcceleration();
            void flee(const sf::Vector2f& targetPos, float speed);
            void arrival(const sf::Vector2f& targetPos, float speed, int arrivalRadius);
            void pursuit(Entity target, float speed, unsigned arrivalRadius, float damp);
            void evade(Entity target, float speed, float circleDistance, float damp);
            void stop();
            void displace(float speed, float circleDistance, float angleRange);
            void avoidObstacles(float avoidStrength, float maxSight);
            void directMovement(const sf::Vector2f& vec);
            void slowDown(float strength);
        private:
            Entity& mEntity;
            MovementHandler& mHandler;
        };


        template<typename GETTER>
        class SteeringHandlerFrontEnd
        {
        public:
            SteeringHandlerFrontEnd(Entity& e, SteeringHandler<GETTER>& h) : mEntity(e), mHandler(h) {}
            bool isActive() const
            {
                return mEntity.get<SteeringComponent<GETTER>>().isActive();
            }
            void setActive(bool active)
            {
                mHandler.setActive(mEntity, active);
            }
            void disconnectPattern()
            {
                mHandler.disconnectPattern(mEntity);
            }
            void connectPattern(const std::string& key, const script::Environment& param)
            {
                mHandler.connectPattern(mEntity, key, param);
            }
            void connectPattern(SteeringPattern<script::Environment>* s, const script::Environment& param)
            {
                mHandler.connectPattern(mEntity, s, param);
            }
        private:
            Entity& mEntity;
            SteeringHandler<GETTER>& mHandler;
        };


        class PathPlannerFrontEnd
        {
        public:
            PathPlannerFrontEnd(Entity& e, PathPlanner& p) : mEntity(e), mPathPlanner(p) {}
            void setPath(script::Environment points);
            void setPath(script::Environment points, PathFollowingPolicy policy, float speed, float radius);
        private:
            Entity& mEntity;
            PathPlanner& mPathPlanner;
        };


        /** \brief Registers movement functionality for scripts. */
        void registerMovement(ScriptStateBase& state);

        namespace detail
        {
            //unpacks a parameter from the parameter env
            template<typename T, typename KEY>
            inline T unpackParameter(script::Environment env, const KEY& name)
            {
                auto res = env.get<Optional<T>>(name);
                if (res)
                    return *res;
                else
                {
                    Logger::error("Could not retrieve parameter", name, "from the given environment.");
                    return T();
                }
            }
        }
    }
}

#endif //REGISTER_MOVEMENT_H
