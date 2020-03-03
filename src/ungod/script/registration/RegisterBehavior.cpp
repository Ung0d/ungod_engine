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

#include "ungod/script/registration/RegisterBehavior.h"
#include "ungod/script/EntityBehavior.h"
#include "ungod/base/Entity.h"
#include "ungod/base/World.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerBehavior(ScriptStateBase& state)
        {
            state.registerEnum<ScriptErrorCode>("ScriptErrorCode",
                                     {{"ScriptOK", ScriptErrorCode::SCRIPT_OK},
                                     {"ScriptNotFound", ScriptErrorCode::SCRIPT_NOT_FOUND},
                                     {"ScriptLoadError", ScriptErrorCode::SCRIPT_LOAD_ERROR},
                                     {"ScriptExecutionError", ScriptErrorCode::SCRIPT_EXECUTION_ERROR},
                                     {"ScriptAlreadyLoaded", ScriptErrorCode::SCRIPT_ALREADY_LOADED}});

           script::Usertype<EntityBehaviorComponent> ebType = state.registerUsertype<EntityBehaviorComponent>("EntityBehavior");
           ebType["hasValidEnvironment"] = &EntityBehaviorComponent::hasValidEnvironment,
           ebType["hasValidStateEnvironment"] = &EntityBehaviorComponent::hasValidStateEnvironment,
           ebType["getEnvironment"] = &EntityBehaviorComponent::getEnvironment,
           ebType["getStateEnvironment"] = &EntityBehaviorComponent::getStateEnvironment;


		   script::Usertype<EntityBehaviorManager> ebmanagerType = state.registerUsertype<EntityBehaviorManager>("EntityBehaviorManager");
		   ebmanagerType["loadBehaviorScript"] = &EntityBehaviorManager::loadBehaviorScript;
		   ebmanagerType["assignBehavior"] = sol::overload(
			   [](EntityBehaviorManager& em, Entity e, const std::string& name)
			   { em.assignBehavior(e, name); },
			   [](EntityBehaviorManager& em, Entity e, const std::string& name, script::Environment param)
			   { em.assignBehavior(e, name, param); });
		   ebmanagerType["setUpdateInterval"] = [] (EntityBehaviorManager& ebm, Entity e, float interval) { ebm.setUpdateInterval(e, interval); };
        

           state.registerFunction("listen", 
               [](Entity e, const std::string& type) { return e.getWorld().getBehaviorManager().addEventListener(e, type);  });

           script::Usertype<script::EventListenerLink> evtListenerLink = state.registerUsertype<script::EventListenerLink>("ScriptEventListenerLink");
           evtListenerLink["disconnect"] = &script::EventListenerLink::disconnect;
        }
    }
}
