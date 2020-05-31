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

#include "ungod/script/EntityBehavior.h"
#include "ungod/application/Application.h"

namespace ungod
{
    const std::vector<const char*> EntityBehaviorManager::IDENTIFIERS = { "onInit", "onExit", "onCreation", "onDestruction",
                                                                        "onStaticConstr", "onStaticDestr",
                                                                        "onSerialize", "onDeserialize",
                                                                        "onCollisionEnter", "onCollision", "onCollisionExit",
                                                                        "onMouseEnter", "onMouseExit", "onMouseClick", "onMouseReleased",
                                                                        "onUpdate",
                                                                        "onButtonDown", "onButtonReleased", "onButtonPressed",
                                                                        "onMovementBegin", "onMovementEnd", "onDirectionChanged",
                                                                        "onAnimationBegin", "onAnimationFrame", "onAnimationEnd",
                                                                        "onCustomEvent",
                                                                        "onAIGetState", "onAIAction",
                                                                        "onEnteredNewNode" };

    EntityBehaviorManager::EntityBehaviorManager(Application& app)
        : mBehaviorManager(app.getScriptState(), app.getGlobalScriptEnv(), IDENTIFIERS, ON_CREATION, ON_INIT, ON_EXIT, ON_STATIC_CONSTR, ON_STATIC_DESTR)
    {
        mScriptStateChangedLink = app.onScriptStateChanged([this, &app]()
            {
                reload(app.getScriptState(), app.getGlobalScriptEnv());
            });
    }

    ScriptErrorCode EntityBehaviorManager::loadBehaviorScript(const std::string& filepath)
    {
        auto success = mBehaviorManager.loadBehavior(filepath);
        /*boost::filesystem::path p = filepath;
        detail::OptionalEnvironment staticEnv = mBehaviorManager.getStaticEnvironment(p.stem().string());
        if (staticEnv) staticEnv.value()["world"] = mWorld;*/
        return success;
    }



    BehaviorManager<>& EntityBehaviorManager::getBehaviorManager()
    {
        return mBehaviorManager;
    }

    std::vector<std::pair<std::string, ScriptErrorCode>> EntityBehaviorManager::reload(const script::SharedState& state, script::Environment main)
    {
        ScriptQueues toReload;
        mDissociateSignal(toReload);
        auto err = mBehaviorManager.reload(state, main);
        mReloadSignal(toReload);
        return err;
    }

    owls::SignalLink<void, ScriptQueues&> EntityBehaviorManager::onReloadSignal(const std::function<void(ScriptQueues&)>& callback)
    {
        return mReloadSignal.connect(callback);
    }

    owls::SignalLink<void, ScriptQueues&> EntityBehaviorManager::onDissociateSignal(const std::function<void(ScriptQueues&)>& callback)
    {
        return mDissociateSignal.connect(callback);
    }

    EntityBehaviorManager::~EntityBehaviorManager()
    {
        mScriptStateChangedLink.disconnect();
    }
}
