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

namespace ungod
{
    namespace scriptRegistration
    {
        void registerApplication(ScriptStateBase& state, Application& app)
        {
            state.registerUsertype<State>("State",
                                   "isTransparent", &State::isTransparent,
                                   "isTranscendent", &State::isTranscendent,
                                   "setTransparency", &State::setTransparency,
                                   "setTranscendency", &State::setTranscendency,
                                   "isExpired", &State::isExpired,
                                   "expire", &State::expire,
                                   "getID", &State::getID,
                                   "initState", &State::initState,
                                   "closeState", &State::closeState);

            state.registerFunction("quitApplication", [&app] () { app.quitApplication(); });
            state.registerFunction("buildVersion", [&app] () { return app.getBuildVersion(); });
            state.registerFunction("addGameState", [&app] (StateID id, const std::string& scriptfile) { return app.getStateManager().addState<ScriptedGameState>(id, scriptfile); });
            state.registerFunction("addMenuState", [&app] (StateID id, const std::string& scriptfile) { return app.getStateManager().addState<ScriptedMenuState>(id, scriptfile); });
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

            state.registerFunction("getMousePosition", [&app]() { return sf::Mouse::getPosition(app.getWindow()); });
            state.registerFunction("getWindowSize", [&app]() { return sf::Vector2i{(int)app.getWindow().getSize().x, (int)app.getWindow().getSize().y}; });
            state.registerFunction("world2Screen", [&app](const sf::Vector2f& pos, const ScriptedGameState& state) { return app.getWindow().mapCoordsToPixel(pos, state.getCamera().getView()); });
            state.registerFunction("screen2World", [&app](const sf::Vector2i& pos, const ScriptedGameState& state) { return app.getWindow().mapPixelToCoords(pos, state.getCamera().getView()); });

            state.registerFunction("emit", [&app](const std::string& type, script::Environment data) { return app.emitCustomEvent(type, data); });
        }
    }
}
