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

#ifndef SCRIPTED_MENU_STATE_H
#define SCRIPTED_MENU_STATE_H

#include "ungod/application/State.h"
#include "ungod/script/Behavior.h"
#include "ungod/script/CustomEvent.h"
#include "ungod/base/Input.h"
#include "ungod/audio/Audio.h"
#include "ungod/gui/Gui.h"
#include "ungod/visual/Camera.h"

namespace ungod
{
    /** \brief A state that handles a menu (main-menu, options-menu ect). The contents can be defined in scripts. */
    class ScriptedMenuState : public State
    {
    public:
        /** \brief Constructor. */
        ScriptedMenuState(Application& app, StateID id, const std::string& scriptID);

        /** \brief Handles an input event. */
        virtual void handleEvent(const sf::Event& curEvent) override;

        /** \brief Update the internal logic of the state. */
        virtual void update(const float delta) override;

        /** \brief Renders the state. */
        virtual void render(sf::RenderTarget& target, sf::RenderStates states) override;

        /** \brief Processes a custom event. */
        virtual void onCustomEvent(const CustomEvent& event) override;

        /** \brief Access the underlying gui. */
        Gui& getGui() { return mGui; }

        /** \brief Access the underlying audio-manager. */
        AudioManager& getAudioManager() { return mAudioManager; }

        /** \brief Access the underlying input-handler. */
        InputHandler& getInputHandler() { return mInputHandler; }

        /** \brief Accesses the underlying script environment. */
        script::Environment getEnvironment() const { return mScriptCallbacks.getEnvironment(); }

    public:
        virtual void init() override;

        virtual void close() override;

    private:
        /** \brief Defines ids for script callbacks. */
        enum MenuScriptCallbacks { ON_INIT, ON_CLOSE,
                               ON_UPDATE, ON_CUSTOM_EVENT,
                               ON_BUTTON_PRESSED, ON_BUTTON_DOWN, ON_BUTTON_RELEASED,
                               NUM_CALLBACK /*Dont use*/ };

        /** \brief Defines names for script callbacks. */
        static constexpr std::array<const char*, NUM_CALLBACK>  MENU_CALLBACK_IDENTIFIERS = {"onInit",
                                                                          "onClose",
                                                                          "onUpdate",
                                                                          "onCustomEvent",
                                                                          "onButtonPressed",
                                                                          "onButtonDown",
                                                                          "onButtonReleased"};

    protected:

        static constexpr float UPDATE_INTERVAL = 20.0f;  //<default time between update script-calls in ms

        sf::Clock mIntervalTimer;

        CallbackInvoker mScriptCallbacks;
        std::string mMenuScriptID;

        Gui mGui;
        InputHandler mInputHandler;
        AudioManager mAudioManager;
        Camera mCamera;
    };
}

#endif // SCRIPTED_MENU_STATE_H
