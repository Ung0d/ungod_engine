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

#include "ungod/script/registration/RegisterGameState.h"
#include "ungod/application/ScriptedGameState.h"
#include "ungod/base/World.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerGameState(ScriptStateBase& state)
        {
            script::Usertype<ScriptedGameState> stateType = state.registerUsertype<ScriptedGameState>("ScriptedGameState", sol::base_classes, sol::bases<State>());
            stateType["toggleDebugRender"] = &ScriptedGameState::toggleDebugRender;
            stateType["toggleDebugInfo"] = &ScriptedGameState::toggleDebugInfo;
            stateType["debugEntityBounds"] = &ScriptedGameState::debugEntityBounds;
            stateType["debugTexrects"] = &ScriptedGameState::debugTexrects;
            stateType["debugColliders"] = &ScriptedGameState::debugColliders;
            stateType["debugAudioEmitters"] = &ScriptedGameState::debugAudioEmitters;
            stateType["load"] = &ScriptedGameState::load;
            stateType["save"] = &ScriptedGameState::save;
            stateType["worldGraph"] = [] (ScriptedGameState& state) -> WorldGraph& {return state.getWorldGraph();};
            stateType["behavior"] = [](ScriptedGameState& state) -> EntityBehaviorManager& {return state.getEntityBehaviorManager(); };
            stateType["steering"] = [](ScriptedGameState& state) -> SteeringManager<script::Environment>& {return state.getSteeringManager(); };
            stateType["light"] = [](ScriptedGameState& state) -> LightManager& {return state.getLightManager(); };
			stateType["updateWorldGraph"] = [](ScriptedGameState& state) 
					{ 
                        auto* node = state.getWorldGraph().getActiveNode();
                        if (node)
                            state.getWorldGraph().updateReferencePosition(
                                state.getWorldGraph().getCamera().getCenter() + node->getPosition()); 
                    };
        }
    }
}
