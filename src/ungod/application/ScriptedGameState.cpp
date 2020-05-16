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

#include "ungod/application/ScriptedGameState.h"
#include "ungod/application/Application.h"
#include "ungod/script/CustomEvent.h"
#include "ungod/base/World.h"
#include "ungod/content/EntityTypes.h"
#include "ungod/serialization/DeserialInit.h"

namespace ungod
{
    constexpr std::array<const char*, ScriptedGameState::GameScriptCallbacks::NUM_CALLBACK> ScriptedGameState::GAME_CALLBACK_IDENTIFIERS;

    ScriptedGameState::ScriptedGameState(Application& app, StateID id, const std::string& gameScriptID)
            : State(app, id),
            mScriptCallbacks( app.getScriptState(), app.getGlobalScriptEnv(), { std::begin(GAME_CALLBACK_IDENTIFIERS), std::end(GAME_CALLBACK_IDENTIFIERS) } ),
            mGameScriptID(),
            mCamera(app.getWindow()),
            mWorldGraph(),
            mRenderDebug(false),
            mDebugBounds(true),
            mDebugTexrects(true),
            mDebugColliders(true),
            mDebugAudioEmitters(true),
            mDebugLightEmitters(true),
            mEntityBehaviorManager(mApp),
            mLightManager(mApp)
    {
        runScript(gameScriptID);
    }

    ScriptedGameState::ScriptedGameState(Application& app, StateID id)
            : State(app, id),
            mScriptCallbacks( app.getScriptState(), app.getGlobalScriptEnv(), { std::begin(GAME_CALLBACK_IDENTIFIERS), std::end(GAME_CALLBACK_IDENTIFIERS) } ),
            mGameScriptID(),
            mCamera(app.getWindow()),
            mWorldGraph(),
            mRenderDebug(false),
            mDebugBounds(true),
            mDebugTexrects(true),
            mDebugColliders(true),
            mDebugAudioEmitters(true),
            mDebugLightEmitters(true),
            mEntityBehaviorManager(mApp),
            mLightManager(mApp)
    {
    }

    void ScriptedGameState::runScript(const std::string& gameScriptID)
    {
        mGameScriptID = gameScriptID;

        //set up the script behavior
        mScriptCallbacks.loadScript(gameScriptID);

        mScriptStateChangedLink = mApp.onScriptStateChanged([this]()
            {
                mScriptCallbacks.reload(mApp.getScriptState(), mApp.getGlobalScriptEnv());
            });
    }


    void ScriptedGameState::handleEvent(const sf::Event& curEvent)
    {
        mWorldGraph.handleInput(curEvent, mApp.getWindow());
        mCamera.handleEvent(curEvent);
    }


    void ScriptedGameState::update(const float delta)
    {
        mCamera.update(delta);
        mWorldGraph.update(delta);
        mScriptCallbacks.execute(ON_UPDATE, this, delta);
    }


    void ScriptedGameState::render(sf::RenderTarget& target, sf::RenderStates states)
    {
        mWorldGraph.render(target, states);
        if (mRenderDebug)
            mWorldGraph.renderDebug(target, states, mDebugBounds, mDebugTexrects, mDebugColliders, mDebugAudioEmitters, mDebugLightEmitters);
    }


    void ScriptedGameState::onCustomEvent(const CustomEvent& event)
    {
        mScriptCallbacks.execute(ON_CUSTOM_EVENT, this, event);
        mWorldGraph.handleCustomEvent(event);
    }


    void ScriptedGameState::save(const std::string& fileid, bool saveNodes)
    {
        SerializationContext context;
        //saves world graph information
        context.serializeRootObject(*this);
        context.save(fileid);
        //saves content of individual nodes
        if (saveNodes)
            mWorldGraph.save(*this);
    }


    void ScriptedGameState::load(const std::string& fileid)
    {
        DeserializationContext context;
        initContext(context);
        context.read(fileid);

        context.deserializeRootObject(*this);
    }


    void ScriptedGameState::init()
    {
        mScriptCallbacks.execute(ON_INIT, this);
    }


    void ScriptedGameState::close()
    {
        mScriptCallbacks.execute(ON_CLOSE, this);
    }

    ScriptedGameState::~ScriptedGameState()
    {
        mScriptStateChangedLink.disconnect();
    }
}

