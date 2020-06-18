#ifndef UEDIT_TRANSFORM_ACTIONS_H
#define UEDIT_TRANSFORM_ACTIONS_H

#include "ungod/base/Entity.h"
#include "ungod/base/Transform.h"
#include <unordered_map>

namespace uedit
{
    class ActionManager;

    class TransformActions
    {
    public:
        TransformActions(ActionManager& am) : mActionManager(am) {}

        void setEntityPosition(ungod::Entity e, const sf::Vector2f& pos);
        void setEntityScale(ungod::Entity e, const sf::Vector2f& scale);
        void setEntityBaseLineOffset(ungod::Entity e, const sf::Vector2f& offset);

        void startEntityMoveSession(ungod::Entity e);
        void moveEntity(ungod::Entity e, const sf::Vector2f& mv);
        void endEntityMoveSession(ungod::Entity e);

    private:
        ActionManager& mActionManager;
        std::unordered_map<ungod::Entity, sf::Vector2f> mEntityMovements;
    };
}

#endif