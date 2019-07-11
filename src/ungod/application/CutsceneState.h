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

#ifndef UNGOD_CUTSCENE_STATE_H
#define UNGOD_CUTSCENE_STATE_H

#include "ungod/application/State.h"
#include "ungod/content/Cutscene.h"

namespace ungod
{
    /** \brief A state that displays a cutscene. The contents of the cutscene can be defined in scripts. */
    class CutsceneState : public State
    {
    public:
        /** \brief Constructor. */
        CutsceneState(Application& app, StateID id, const std::string& gameScriptID);

        /** \brief Constructor. */
        CutsceneState(Application& app, StateID id);

        /** \brief Runs the given script for the state. */
        void runScript(const std::string& gameScriptID);

        /** \brief Handles an input event. */
        virtual void handleEvent(const sf::Event& curEvent) override;

        /** \brief Update the internal logic of the state. */
        virtual void update(const float delta) override;

        /** \brief Renders the state. */
        virtual void render(sf::RenderTarget& target, sf::RenderStates states) override;

        virtual void init() override;

        virtual void close() override;

        Cutscene& getCutscene() { return mCutscene; }
        const Cutscene& getCutscene() const { return mCutscene; }

    private:
        /** \brief Defines ids for script callbacks. */
        enum GameScriptCallbacks { ON_INIT, ON_SCENE_END, ON_CLOSE,
                               NUM_CALLBACK /*Dont use*/ };

        /** \brief Defines names for script callbacks. */
        static constexpr std::array<const char*, NUM_CALLBACK>  GAME_CALLBACK_IDENTIFIERS = {"onInit", "onSceneEnd", "onClose"};

    private:
        Cutscene mCutscene;
        CallbackInvoker mScriptCallbacks;
        std::string mGameScriptID;
    };
}

#endif // UNGOD_CUTSCENE_STATE_H
