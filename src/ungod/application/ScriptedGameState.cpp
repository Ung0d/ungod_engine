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
#include "ungod/script/Registration.h"
#include "ungod/application/Application.h"
#include "ungod/script/CustomEvent.h"
#include "ungod/base/World.h"
#include "ungod/content/EntityTypes.h"

namespace ungod
{
    constexpr std::array<const char*, ScriptedGameState::GameScriptCallbacks::NUM_CALLBACK> ScriptedGameState::GAME_CALLBACK_IDENTIFIERS;

    ScriptedGameState::ScriptedGameState(Application& app, StateID id, const std::string& gameScriptID)
            : State(app, id),
            mScriptCallbacks( app.getScriptState(), app.getGlobalScriptEnv(), { std::begin(GAME_CALLBACK_IDENTIFIERS), std::end(GAME_CALLBACK_IDENTIFIERS) } ),
            mGameScriptID(),
            mCamera(app.getWindow()),
            mLayers(mCamera),
            mRenderDebug(false),
            mDebugBounds(true),
            mDebugTexrects(true),
            mDebugColliders(true),
            mDebugAudioEmitters(true)
    {
        runScript(gameScriptID);
    }

    ScriptedGameState::ScriptedGameState(Application& app, StateID id)
            : State(app, id),
            mScriptCallbacks( app.getScriptState(), app.getGlobalScriptEnv(), { std::begin(GAME_CALLBACK_IDENTIFIERS), std::end(GAME_CALLBACK_IDENTIFIERS) } ),
            mGameScriptID(),
            mCamera(app.getWindow()),
            mLayers(mCamera),
            mRenderDebug(false),
            mDebugBounds(true),
            mDebugTexrects(true),
            mDebugColliders(true),
            mDebugAudioEmitters(true)
    {
    }

    void ScriptedGameState::runScript(const std::string& gameScriptID)
    {
        mGameScriptID = gameScriptID;

        //register functionality
        scriptRegistration::registerAssets(mScriptCallbacks);
        scriptRegistration::registerWorld(mScriptCallbacks, *mApp);
        scriptRegistration::registerGui(mScriptCallbacks);
        scriptRegistration::registerGameState(mScriptCallbacks);
        scriptRegistration::registerApplication(mScriptCallbacks, *mApp);
        scriptRegistration::registerWater(mScriptCallbacks, *mApp);
        scriptRegistration::registerTileMap(mScriptCallbacks, *mApp);

        //set up the script behavior
        mScriptCallbacks.loadScript(gameScriptID);
    }


    World* ScriptedGameState::addWorld(std::size_t i)
    {
        World* world = new World( mApp->getScriptState(), mApp->getGlobalScriptEnv(), this);

        //instantate the world
        auto unshadowVertex = mApp->getConfig().get<std::string>("light/unshadow_vertex_shader");
        auto unshadowFrag = mApp->getConfig().get<std::string>("light/unshadow_frag_shader");
        auto lightVertex = mApp->getConfig().get<std::string>("light/light_vertex_shader");
        auto lightFrag = mApp->getConfig().get<std::string>("light/light_frag_shader");
        auto pen = mApp->getConfig().get<std::string>("light/default_penumbra_texture");
        if (unshadowVertex && unshadowFrag && lightVertex && lightFrag && pen)
        {
            world->instantiate(*mApp, unshadowVertex.value(), unshadowFrag.value(), lightVertex.value(), lightFrag.value(), pen.value(), mApp->getGlobalScriptEnv());
        }
        else
        {
            Logger::error("Can not instantiate the world, because the config file seems to be corrupted.");
            Logger::endl();
            Logger::error("Consider deleting it and let the application generate a fresh one.");
            Logger::endl();
        }

        //set up signal callbacks
        world->getTransformManager().onPositionChanged([this] (Entity e, const sf::Vector2f& position) { mScriptCallbacks.execute(ON_POSITION_CHANGED, this, e, position); });
        world->getTransformManager().onSizeChanged([this] (Entity e, const sf::Vector2f& size) { mScriptCallbacks.execute(ON_SIZE_CHANGED, this, e, size); });
        world->getMovementManager().onBeginMovement([this] (Entity e, const sf::Vector2f& vel) { mScriptCallbacks.execute(ON_BEGIN_MOVEMENT, this, e, vel); });
        world->getMovementManager().onEndMovement([this] (Entity e) { mScriptCallbacks.execute(ON_END_MOVEMENT, this, e); });
        world->getMovementManager().onDirectionChanged([this] (Entity e, MovementComponent::Direction oldDirec, MovementComponent::Direction newDirec) { mScriptCallbacks.execute(ON_DIRECTION_CHANGED, this, e, oldDirec, newDirec); });
        world->getVisualsManager().onVisibilityChanged([this] (Entity e, bool visibility) { mScriptCallbacks.execute(ON_VISIBILITY_CHANGED, this, e, visibility); });
        world->getVisualsManager().onAnimationStart([this] (Entity e, const std::string& key) { mScriptCallbacks.execute(ON_ANIMATION_START, this, e, key); });
        world->getVisualsManager().onAnimationStop([this] (Entity e, const std::string& key) { mScriptCallbacks.execute(ON_ANIMATION_STOP, this, e, key); });
        world->getMovementCollisionManager().onCollision([this] (Entity e1, Entity e2, const sf::Vector2f& mdv, const Collider& c1, const Collider& c2) { mScriptCallbacks.execute(ON_MOV_COLLISION, this, e1, e2, mdv, c1, c2); });
        world->getMovementCollisionManager().onBeginCollision([this] (Entity e1, Entity e2) { mScriptCallbacks.execute(ON_BEGIN_MOV_COLLISION, this, e1, e2); });
        world->getMovementCollisionManager().onEndCollision([this] (Entity e1, Entity e2) { mScriptCallbacks.execute(ON_END_MOVE_COLLISION, this, e1, e2); });
        world->getSemanticsCollisionManager().onCollision([this] (Entity e1, Entity e2, const sf::Vector2f& mdv, const Collider& c1, const Collider& c2) { mScriptCallbacks.execute(ON_SEM_COLLISION, this, e1, e2, mdv, c1, c2); });
        world->getSemanticsCollisionManager().onBeginCollision([this] (Entity e1, Entity e2) { mScriptCallbacks.execute(ON_BEGIN_SEM_COLLISION, this, e1, e2); });
        world->getSemanticsCollisionManager().onEndCollision([this] (Entity e1, Entity e2) { mScriptCallbacks.execute(ON_END_SEM_COLLISION, this, e1, e2); });
        world->getInputManager().onPressed([this] (const std::string& binding) { mScriptCallbacks.execute(ON_PRESSED, this, binding); });
        world->getInputManager().onDown([this] (const std::string& binding) { mScriptCallbacks.execute(ON_DOWN, this, binding); });
        world->getInputManager().onReleased([this] (const std::string& binding) { mScriptCallbacks.execute(ON_RELEASED, this, binding); });
        world->getInputManager().onMouseEnter([this] (Entity e) { mScriptCallbacks.execute(ON_MOUSE_ENTER, this, e); });
        world->getInputManager().onMouseClick([this] (Entity e) { mScriptCallbacks.execute(ON_MOUSE_CLICK, this, e); });
        world->getInputManager().onMouseExit([this] (Entity e) { mScriptCallbacks.execute(ON_MOUSE_EXIT, this, e); });
        world->getInputManager().onMouseReleased([this] (Entity e) { mScriptCallbacks.execute(ON_MOUSE_RELEASED, this, e); });


        //register instantiations for deseilization
        registerTypes(*world);

        mLayers.registerLayer(RenderLayerPtr(world), i);

        return world;
    }

    World* ScriptedGameState::addWorld()
    {
        return addWorld(mLayers.getVector().size());
    }


    void ScriptedGameState::handleEvent(const sf::Event& curEvent)
    {
        mLayers.handleInput(curEvent, mApp->getWindow());
        mCamera.handleEvent(curEvent);
    }


    void ScriptedGameState::update(const float delta)
    {
        mCamera.update(delta);

        mLayers.update(delta);

        mScriptCallbacks.execute(ON_UPDATE, this, delta);

        //if (mIntervalTimer.getElapsedTime().asMilliseconds() >= UPDATE_INTERVAL)
        //{
            //mIntervalTimer.restart();

           //mScriptCallbacks.execute(ON_UPDATE, this, UPDATE_INTERVAL);
        //}
    }


    void ScriptedGameState::render(sf::RenderTarget& target, sf::RenderStates states)
    {
        mLayers.render(target, states);
        if (mRenderDebug)
            mLayers.renderDebug(target, states, mDebugBounds, mDebugTexrects, mDebugColliders, mDebugAudioEmitters);
    }


    void ScriptedGameState::onCustomEvent(const CustomEvent& event)
    {
        mScriptCallbacks.execute(ON_CUSTOM_EVENT, this, event);
        mLayers.handleCustomEvent(event);
    }


    void ScriptedGameState::save(const std::string& fileid)
    {
        SerializationContext context;
        context.serializeRootObject(*this);
        context.save(fileid);
    }


    void ScriptedGameState::load(const std::string& fileid)
    {
        DeserializationContext context;
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
}

