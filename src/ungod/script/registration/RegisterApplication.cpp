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

#include "ungod/script/registration/RegisterApplication.h"
#include "ungod/application/Application.h"
#include "ungod/application/ScriptedGameState.h"
#include "ungod/application/ScriptedMenuState.h"
#include "ungod/application/CutsceneState.h"
#include "ungod/base/World.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerApplication(ScriptStateBase& state, Application& app)
        {
			script::Usertype<State> stateType = state.registerUsertype<State>("State");
			stateType["isTransparent"] = &State::isTransparent;
			stateType["isTranscendent"] = &State::isTranscendent;
			stateType["setTransparency"] = &State::setTransparency;
			stateType["setTranscendency"] = &State::setTranscendency;
			stateType["isExpired"] = &State::isExpired;
			stateType["expire"] = &State::expire;
			stateType["getID"] = &State::getID;
			stateType["initState"] = &State::initState;
			stateType["closeState"] = &State::closeState;

            state.registerFunction("quitApplication", [&app] () { app.quitApplication(); });
            state.registerFunction("buildVersion", [&app] () { return app.getBuildVersion(); });
            state.registerFunction("addGameState", [&app] (StateID id, const std::string& scriptfile) { return app.getStateManager().addState<ScriptedGameState>(id, scriptfile); });
            state.registerFunction("addMenuState", [&app] (StateID id, const std::string& scriptfile) { return app.getStateManager().addState<ScriptedMenuState>(id, scriptfile); });
            state.registerFunction("addCutsceneState", [&app] (StateID id, const std::string& scriptfile) { return app.getStateManager().addState<CutsceneState>(id, scriptfile); });
            state.registerFunction("addInactiveGameState", [&app] (StateID id, const std::string& scriptfile) { return app.getStateManager().addState<ScriptedGameState>(id, scriptfile); });
            state.registerFunction("addInactiveMenuState", [&app] (StateID id, const std::string& scriptfile) { return app.getStateManager().addState<ScriptedMenuState>(id, scriptfile); });
            state.registerFunction("moveStateToForeground", [&app] (StateID id) { return app.getStateManager().moveToForeground(id); });
            state.registerFunction("moveStateToBackground", [&app] (StateID id) { return app.getStateManager().moveToBackground(id); });
            state.registerFunction("getState", [&app] (StateID id) { return app.getStateManager().getState(id); } );
            state.registerFunction("getGameState", [&app] (StateID id) { return app.getStateManager().getState<ScriptedGameState>(id); } );
            state.registerFunction("getMenuState", [&app] (StateID id) { return app.getStateManager().getState<ScriptedMenuState>(id); } );
            state.registerFunction("hasState", [&app] (StateID id) { return app.getStateManager().hasState(id); } );
            state.registerFunction("log", [] (const std::string& txt) { Logger::info(txt); Logger::endl(); });
            state.registerFunction("logWarning", [] (const std::string& txt) { Logger::warning(txt); Logger::endl(); });
            state.registerFunction("logError", [] (const std::string& txt) { Logger::error(txt); Logger::endl(); });

            state.registerFunction("setFloatConfig", [&app] (const std::string& item, float x) { app.getConfig().set<float>(item, x); });
            state.registerFunction("setIntConfig", [&app] (const std::string& item, int x) { app.getConfig().set<int>(item, x); });
            state.registerFunction("setBoolConfig", [&app] (const std::string& item, bool x) { app.getConfig().set<bool>(item, x); });
            state.registerFunction("setStringConfig", [&app] (const std::string& item, const std::string& x) { app.getConfig().set<std::string>(item, x); });

            state.registerFunction("getFloatConfigOr", [&app] (const std::string& item, float x) -> float { return app.getConfig().getOr<float>(item, x); });
            state.registerFunction("getIntConfigOr", [&app] (const std::string& item, int x) -> int { return app.getConfig().getOr<int>(item, x); });
            state.registerFunction("getBoolConfigOr", [&app] (const std::string& item, bool x) -> bool { return app.getConfig().getOr<bool>(item, x); });
            state.registerFunction("getStringConfigOr", [&app] (const std::string& item, const std::string& x) -> std::string { return app.getConfig().getOr<std::string>(item, x); });

            state.registerFunction("getMousePosition", [&app]() { return sf::Mouse::getPosition(app.getWindow()); });
            state.registerFunction("getWindowSize", [&app]() { return sf::Vector2i{(int)app.getWindow().getSize().x, (int)app.getWindow().getSize().y}; });
            state.registerFunction("isFullscreen", [&app]() { return app.isFullscreen(); });
            state.registerFunction("isVsyncEnabled", [&app]() { return app.vsyncEnabled(); });

            state.registerFunction("world2Screen", sol::overload([&app](const sf::Vector2f& pos, const ScriptedGameState& state)
                                   { return app.getWindow().mapCoordsToPixel(pos, state.getCamera().getView()); },
                                   [&app](const sf::Vector2f& pos, const World& world)
                                   { return app.getWindow().mapCoordsToPixel(pos, world.getState()->getCamera().getView()); }));

            state.registerFunction("screen2World", sol::overload([&app](const sf::Vector2i& pos, const ScriptedGameState& state)
                                   { return app.getWindow().mapPixelToCoords(pos, state.getCamera().getView()); },
                                   [&app](const sf::Vector2i& pos, const World& world)
                                   { return app.getWindow().mapPixelToCoords(pos, world.getState()->getCamera().getView()); }));

            state.registerFunction("emit", sol::overload(
                [&app](const std::string& type, script::Environment data) { return app.emitCustomEvent(type, data); },
                [&app](const std::string& type, script::Environment data, float delay) { return app.emitCustomEvent(type, data, delay); }));

            state.registerFunction("setCursor", [&app](const std::string& cursor) { app.setCursor(cursor); });

            state.registerFunction("randFloat", [&app](float bot, float top) { return ungod::NumberGenerator::getFloatRandBetw(bot, top); });
            state.registerFunction("randInt", [](int bot, int top) { return ungod::NumberGenerator::getRandBetw(bot, top); });
            state.registerFunction("normRand", [](float mu, float rho) { return ungod::NumberGenerator::getNormRand(mu, rho); });
        }
    }
}
