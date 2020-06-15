#ifndef UEDIT_COLLISION_ACTIONS_H
#define UEDIT_COLLISION_ACTIONS_H

#include "ungod/base/Entity.h"
#include "ungod/base/Visuals.h"
#include <unordered_map>

namespace uedit
{
    class ActionManager;

    template<std::size_t CONTEXT>
    class CollisionActions
    {
    public:
        CollisionActions(ActionManager& am) : mActionManager(am) {}


        void addCollider(ungod::Entity e, const ungod::Collider& c)
        {
            addCollider(e, e.modify<ungod::RigidbodyComponent<CONTEXT>>(), c);
        }
        void addCollider(ungod::Entity e, std::size_t index, const ungod::Collider& c)
        {
            addCollider(e, e.modify<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponent(index), c);
        }
        void addCollider(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, const ungod::Collider& c);


        void setRectDownRight(ungod::Entity e, const sf::Vector2f& downright)
        {
            setRectDownRight(e, e.modify<ungod::RigidbodyComponent<CONTEXT>>(), downright);
        }
        void setRectDownRight(ungod::Entity e, std::size_t index, const sf::Vector2f& downright)
        {
            setRectDownRight(e, e.modify<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponent(index), downright);
        }
        void setRectDownRight(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, const sf::Vector2f& downright);



        void setRectUpLeft(ungod::Entity e, const sf::Vector2f& upleft)
        {
            setRectUpLeft(e, e.modify<ungod::RigidbodyComponent<CONTEXT>>(), upleft);
        }
        void setRectUpLeft(ungod::Entity e, std::size_t index, const sf::Vector2f& upleft)
        {
            setRectUpLeft(e, e.modify<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponent(index), upleft);
        }
        void setRectUpLeft(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, const sf::Vector2f& upleft);



        void rotateRect(ungod::Entity e, float rotation)
        {
            rotateRect(e, e.modify<ungod::RigidbodyComponent<CONTEXT>>(), rotation);
        }
        void rotateRect(ungod::Entity e, std::size_t index, float rotation)
        {
            rotateRect(e, e.modify<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponent(index), rotation);
        }
        void rotateRect(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, float rotation);



        void setRectRotation(ungod::Entity e, float rotation)
        {
            setRectRotation(e, e.modify<ungod::RigidbodyComponent<CONTEXT>>(), rotation);
        }
        void setRectRotation(ungod::Entity e, std::size_t index, float rotation)
        {
            setRectRotation(e, e.modify<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponent(index), rotation);
        }
        void setRectRotation(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, float rotation);



        void setColliderPoint(ungod::Entity e, const sf::Vector2f& p, unsigned i)
        {
            setColliderPoint(e, e.modify<ungod::RigidbodyComponent<CONTEXT>>(), p, i);
        }
        void setColliderPoint(ungod::Entity e, std::size_t index, const sf::Vector2f& p, unsigned i)
        {
            setColliderPoint(e, e.modify<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponent(index), p, i);
        }
        void setColliderPoint(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, const sf::Vector2f& p, unsigned i);



        void addColliderPoint(ungod::Entity e, const sf::Vector2f& p)
        {
            addColliderPoint(e, e.modify<ungod::RigidbodyComponent<CONTEXT>>(), p);
        }
        void addColliderPoint(ungod::Entity e, std::size_t index, const sf::Vector2f& p)
        {
            addColliderPoint(e, e.modify<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponent(index), p);
        }
        void addColliderPoint(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, const sf::Vector2f& p);



        void moveCollider(ungod::Entity e, const sf::Vector2f& vec)
        {
            moveCollider(e, e.modify<ungod::RigidbodyComponent<CONTEXT>>(), vec);
        }
        void moveCollider(ungod::Entity e, std::size_t index, const sf::Vector2f& vec)
        {
            moveCollider(e, e.modify<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponent(index), vec);
        }
        void moveCollider(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& rb, const sf::Vector2f& vec);

    private:
        ActionManager& mActionManager;
    }
}

#endif