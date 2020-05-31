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

#ifndef REGISTER_COLLISION_H
#define REGISTER_COLLISION_H

#include "ungod/script/Behavior.h"
#include "ungod/physics/CollisionHandler.h"
#include "ungod/script/registration/RegistrationDetail.h"

namespace ungod
{
    class Entity;

    namespace scriptRegistration
    {
        template <std::size_t CONTEXT>
        class RigidbodyHandlerFrontEnd
        {
        public:
            RigidbodyHandlerFrontEnd(Entity& e, RigidbodyHandler<CONTEXT>& h) : mEntity(e), mHandler(h) {}
            void addCollider(const Collider& collider) { mHandler.addCollider(mEntity, collider); }
            void addCollider(unsigned i, const Collider& collider) { mHandler.addCollider(mEntity, i, collider); }
            void addRotatedRect(const sf::Vector2f& upleft, const sf::Vector2f& downright, float rotation) { mHandler.addRotatedRect(mEntity, upleft, downright, rotation); }
            void addRotatedRect(unsigned i, const sf::Vector2f& upleft, const sf::Vector2f& downright, float rotation) { mHandler.addRotatedRect(mEntity, i, upleft, downright, rotation); }
            void addConvexPolygon(script::Environment points) { mHandler.addConvexPolygon(mEntity, env2vec<sf::Vector2f>(points)); }
            void addConvexPolygon(unsigned i, script::Environment points) { mHandler.addConvexPolygon(mEntity, i, env2vec<sf::Vector2f>(points)); }
            void addEdgeChain(script::Environment points) { mHandler.addEdgeChain(mEntity, env2vec<sf::Vector2f>(points)); }
            void addEdgeChain(unsigned i, script::Environment points) { mHandler.addEdgeChain(mEntity, i, env2vec<sf::Vector2f>(points)); }
            void clearCollider() { mHandler.clearCollider(mEntity); }
            void clearCollider(unsigned i) { mHandler.clearCollider(mEntity, i); }
            void setRectDownRight(const sf::Vector2f& p) { mHandler.setRectDownRight(mEntity, p); }
            void setRectDownRight(unsigned i, const sf::Vector2f& p) { mHandler.setRectDownRight(mEntity, i, p); }
            void setRectUpLeft(const sf::Vector2f& p) { mHandler.setRectUpLeft(mEntity, p); }
            void setRectUpLeft(unsigned i, const sf::Vector2f& p) { mHandler.setRectUpLeft(mEntity, i, p); }
            void setRectRotation(float rotation) { mHandler.setRectRotation(mEntity, rotation); }
            void setRectRotation(unsigned i, float rotation) { mHandler.setRectRotation(mEntity, i, rotation); }
            void setPoint(unsigned pointIndex, const sf::Vector2f& p) { mHandler.setPoint(mEntity, pointIndex, p); }
            void setPoint(unsigned multiIndex, unsigned pointIndex, const sf::Vector2f& p) { mHandler.setPoint(mEntity, multiIndex, pointIndex, p); }
            void addPoint(const sf::Vector2f& p) { mHandler.addPoint(mEntity, p); }
            void addPoint(unsigned i, const sf::Vector2f& p) { mHandler.addPoint(mEntity, i, p); }
            void setActive(bool active) { mHandler.setActive(mEntity, active); }
            void setActive(unsigned i, bool active) { mHandler.setActive(mEntity, i, active); }
            ColliderType getType() const { return mEntity.get<RigidbodyComponent<CONTEXT>>().getCollider().getType(); }
            RotatedRectConstAggregator asRectangle() const { return RotatedRectConstAggregator{ mEntity.get<RigidbodyComponent<CONTEXT>>().getCollider() }; }
            PointSetConstAggregator asPolygon() const { return PointSetConstAggregator{ mEntity.get<RigidbodyComponent<CONTEXT>>().getCollider() }; }
            PointSetConstAggregator asEdgeChain() const { return PointSetConstAggregator{ mEntity.get<RigidbodyComponent<CONTEXT>>().getCollider() }; }
            ColliderType getType(unsigned i) const { return mEntity.get<MultiRigidbodyComponent<CONTEXT>>().getComponent(i).getCollider().getType(); }
            RotatedRectConstAggregator asRectangle(unsigned i) const { return RotatedRectConstAggregator{ mEntity.get<MultiRigidbodyComponent<CONTEXT>>().getComponent(i).getCollider() }; }
            PointSetConstAggregator asPolygon(unsigned i) const { return PointSetConstAggregator{ mEntity.get<MultiRigidbodyComponent<CONTEXT>>().getComponent(i).getCollider() }; }
            PointSetConstAggregator asEdgeChain(unsigned i) const { return PointSetConstAggregator{ mEntity.get<MultiRigidbodyComponent<CONTEXT>>().getComponent(i).getCollider() }; }
        private:
            Entity& mEntity;
            RigidbodyHandler<CONTEXT>& mHandler;
        };


        /** \brief Registers collision functionality for scripts. */
        template <std::size_t CONTEXT>
        void registerCollision(ScriptStateBase& state)
        {
            script::Usertype<RigidbodyHandlerFrontEnd<CONTEXT>> rmType =
                    state.registerUsertype<RigidbodyHandlerFrontEnd<CONTEXT>>(STRCAT("RigidbodyHandlerFrontEnd", NUM2STR(CONTEXT)));
            rmType["addCollider"] = sol::overload(
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, const Collider& collider) { rm.addCollider(collider); },
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, unsigned i, const Collider& collider) { rm.addCollider(i, collider);  });
            rmType["addRotatedRect"] = sol::overload(
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, const sf::Vector2f& upleft, const sf::Vector2f& downright, float rotation)
                                            { rm.addRotatedRect(upleft, downright, rotation); },
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, unsigned i, const sf::Vector2f& upleft, const sf::Vector2f& downright, float rotation)
                                            { rm.addRotatedRect(i, upleft, downright, rotation);  });
            rmType["addConvexPolygon"] = sol::overload(
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, script::Environment points) 
                            { rm.addConvexPolygon(points); },
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, unsigned i, script::Environment points)
                            { rm.addConvexPolygon(i, points);  });
            rmType["addEdgeChain"] = sol::overload(
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, script::Environment points) 
                            { rm.addEdgeChain(points); },
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, unsigned i, script::Environment points) 
                            { rm.addEdgeChain(i, points);  });
            rmType["clearCollider"] = sol::overload(
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm) { rm.clearCollider(); },
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, unsigned i) { rm.clearCollider(i);  });
            rmType["setRectDownRight"] = sol::overload(
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, const sf::Vector2f& p) { rm.setRectDownRight(p); },
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, unsigned i, const sf::Vector2f& p) { rm.setRectDownRight(i, p);  });
            rmType["setRectUpLeft"] = sol::overload(
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, const sf::Vector2f& p) { rm.setRectUpLeft(p); },
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, unsigned i, const sf::Vector2f& p) { rm.setRectUpLeft(i, p);  });
            rmType["setRectRotation"] = sol::overload(
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, float r) { rm.setRectRotation(r); },
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, unsigned i, float r) { rm.setRectRotation(i, r);  });
            rmType["setPoint"] = sol::overload(
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, unsigned pointIndex, const sf::Vector2f& point) { rm.setPoint(pointIndex, point); },
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, unsigned multiIndex, unsigned pointIndex, const sf::Vector2f& point) { rm.setPoint(multiIndex, pointIndex, point);  });
            rmType["addPoint"] = sol::overload(
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, const sf::Vector2f& point) { rm.addPoint(point); },
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, unsigned multiIndex, const sf::Vector2f& point) { rm.addPoint(multiIndex, point);  });
            rmType["setActive"] = sol::overload(
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, bool active) { rm.setActive(active); },
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, unsigned i,bool active) { rm.setActive(i, active);  });
            rmType["getType"] = sol::overload(
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm) { rm.getType(); },
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, unsigned i) { rm.getType(i);  });
            rmType["asRectangle"] = sol::overload(
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm) { rm.asRectangle(); },
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, unsigned i) { rm.asRectangle(i);  });
            rmType["asPolygon"] = sol::overload(
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm) { rm.asPolygon(); },
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, unsigned i) { rm.asPolygon(i);  });
            rmType["asEdgeChain"] = sol::overload(
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm) { rm.asEdgeChain(); },
                [](RigidbodyHandlerFrontEnd<CONTEXT>& rm, unsigned i) { rm.asEdgeChain(i);  });
        }

        /** \brief Registers rigidbody attachment and manipulation functionality for scripts. */
        void registerRigidbody(ScriptStateBase& state);
    }
}

#endif

