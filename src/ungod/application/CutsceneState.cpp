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

#include "ungod/application/CutsceneState.h"
#include "ungod/application/Application.h"

namespace ungod
{
        CutsceneState::CutsceneState(Application& app, StateID id, const std::string& gameScriptID) : State(app, id),
            mScriptCallbacks( app.getScriptState(), app.getGlobalScriptEnv(), { std::begin(GAME_CALLBACK_IDENTIFIERS), std::end(GAME_CALLBACK_IDENTIFIERS) } ),
            mGameScriptID(), mEnded(false)
        {
            setTranscendency(false);
            setTransparency(false);
            runScript(gameScriptID);
            mCutscene.setScreenSize(app.getWindow().getSize());
        }


        CutsceneState::CutsceneState(Application& app, StateID id) : State(app, id),
            mScriptCallbacks( app.getScriptState(), app.getGlobalScriptEnv(), { std::begin(GAME_CALLBACK_IDENTIFIERS), std::end(GAME_CALLBACK_IDENTIFIERS) } ),
            mGameScriptID(), mEnded(false)
        {
            setTranscendency(false);
            setTransparency(false);
            mCutscene.setScreenSize(app.getWindow().getSize());
        }


        void CutsceneState::runScript(const std::string& gameScriptID)
        {
            mGameScriptID = gameScriptID;

            //set up the script behavior
            mScriptCallbacks.loadScript(gameScriptID);
        }


        void CutsceneState::handleEvent(const sf::Event& curEvent)
        {
            if (curEvent.type == sf::Event::Resized)
            {
                mCutscene.setScreenSize(sf::Vector2u{curEvent.size.width, curEvent.size.height});
            }
        }


        void CutsceneState::update(const float delta)
        {
            if (mCutscene.update(delta))
            {
                mScriptCallbacks.execute(ON_SCENE_END, this, mCutscene.getSceneIndex());
            }
            if (!mCutscene.isPlaying() && !mEnded)
            {
                mEnded = true;
                mScriptCallbacks.execute(ON_CUTSCENE_END, this);
            }
        }


        void CutsceneState::render(sf::RenderTarget& target, sf::RenderStates states)
        {
            mCutscene.render(target, states);
        }


        void CutsceneState::init()
        {
            mScriptCallbacks.execute(ON_INIT, this);
        }

        void CutsceneState::close()
        {
            mScriptCallbacks.execute(ON_CLOSE, this);
        }
}
