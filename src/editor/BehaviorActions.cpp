#include "BehaviorActions.h"
#include "ActionManager.h"
#include "ungod/base/Entity.h"
#include "ungod/script/EntityBehavior.h"

namespace uedit
{
    void BehaviorActions::assignBehavior(ungod::Entity e, const std::string& name)
    {
        std::string oldScript;
        if (e.get<ungod::EntityBehaviorComponent>().valid())
            oldScript = e.get<ungod::EntityBehaviorComponent>().getScriptName();
        mActionManager.action(std::function([this, e, name]()
            {
                e.getWorld().getBehaviorHandler().assignBehavior(e, name);
            }),
            std::function([this, e, oldScript]() 
                { 
                    if (oldScript == "")
                        e.getWorld().getBehaviorHandler().dissociateBehavior(e);
                    else
                        e.getWorld().getBehaviorHandler().assignBehavior(e, oldScript);
                }));
    }

    void BehaviorActions::assignBehavior(ungod::Entity e, const std::string& name, ungod::script::Environment param)
    {
        std::string oldScript;
        if (e.get<ungod::EntityBehaviorComponent>().valid())
            oldScript = e.get<ungod::EntityBehaviorComponent>().getScriptName();
        mActionManager.action(std::function([this, e, name, param]()
            {
                e.getWorld().getBehaviorHandler().assignBehavior(e, name, param);
                e.getWorld().getBehaviorHandler().initBehavior(e);
            }),
            std::function([this, e, oldScript]()
                {
                    if (oldScript == "")
                        e.getWorld().getBehaviorHandler().dissociateBehavior(e);
                    else
                    {
                        e.getWorld().getBehaviorHandler().assignBehavior(e, oldScript);
                        e.getWorld().getBehaviorHandler().initBehavior(e);
                    }
                }));
    }
}