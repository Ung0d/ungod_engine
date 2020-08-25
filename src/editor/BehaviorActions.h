#ifndef UEDIT_BEHAVIOR_ACTIONS_H
#define UEDIT_BEHAVIOR_ACTIONS_H

#include "wx/window.h"
#include "ungod/script/Script.h"

namespace ungod
{
    class Entity;
}

namespace uedit
{
    class ActionManager;

    class BehaviorActions
    {
    public:
        BehaviorActions(ActionManager& am) : mActionManager(am) {}

        void assignBehavior(ungod::Entity e, const std::string& name);
        void assignBehavior(ungod::Entity e, const std::string& name, ungod::script::Environment param);

    private:
        ActionManager& mActionManager;
    };
}

#endif