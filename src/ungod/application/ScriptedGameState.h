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

#ifndef SCRIPTED_GAME_STATE_H
#define SCRIPTED_GAME_STATE_H

#include "ungod/application/ScriptedMenuState.h"
#include "ungod/gui/Gui.h"
#include "ungod/visual/Camera.h"
#include "ungod/serialization/SerialGameState.h"
#include "ungod/visual/RenderLayer.h"

namespace ungod
{
    /** \brief A state that handles a menu (main-menu, options-menu ect). The contents can be defined in scripts. */
    class ScriptedGameState : public State, public Serializable<ScriptedGameState>
    {
    friend class SerialBehavior<ScriptedGameState>;
    friend class DeserialBehavior<ScriptedGameState>;
    public:
        /** \brief Constructor. */
        ScriptedGameState(Application& app, StateID id, const std::string& gameScriptID);

        /** \brief Constructor. */
        ScriptedGameState(Application& app, StateID id);

        /** \brief Runs the given script for the state. */
        void runScript(const std::string& gameScriptID);

        /** \brief Creates a new world and adds it to the i-th position of the layer stack. */
        World* addWorld(std::size_t i);
        World* addWorld();

        /** \brief Handles an input event. */
        virtual void handleEvent(const sf::Event& curEvent) override;

        /** \brief Update the internal logic of the state. */
        virtual void update(const float delta) override;

        /** \brief Renders the state. */
        virtual void render(sf::RenderTarget& target, sf::RenderStates states) override;

        /** \brief Processes a custom event. */
        virtual void onCustomEvent(const CustomEvent& event) override;

        /** \brief Access the camera. */
        Camera& getCamera() { return mCamera; }
        const Camera& getCamera() const { return mCamera; }

        /** \brief Grants access to the underyling script environment. */
        script::Environment getEnvironment() const { return mScriptCallbacks.getEnvironment(); }

        /** \brief Activates or deactivates the debug rendering mode. */
        void toggleDebugmode(bool debug) { mRenderDebug = debug; }

        /** \brief Activates or deactivates the debug entity bounds rendering mode. */
        void debugEntityBounds(bool bounds) { mDebugBounds = bounds; }

        /** \brief Activates or deactivates the debug entity bounds rendering mode. */
        void debugTexrects(bool texretcs) { mDebugTexrects = texretcs; }

        /** \brief Activates or deactivates the debug entity bounds rendering mode. */
        void debugColliders(bool colliders) { mDebugColliders = colliders; }

        /** \brief Returns entity bounds debug render state. */
        bool debugEntityBoundsActive() const {return mDebugBounds;}

        /** \brief Returns texture rects debug render state. */
        bool debugTextureRectsActive() const {return mDebugTexrects;}

        /** \brief Returns entity bounds debug render state. */
        bool debugCollidersActive() const {return mDebugColliders;}

        /** \brief Returns entity bounds debug render state. */
        bool debugAudioEmittersActive() const {return mDebugAudioEmitters;}

        /** \brief Activates or deactivates the debug entity bounds rendering mode. */
        void debugAudioEmitters(bool audioemitters) { mDebugAudioEmitters = audioemitters; }

        /** \brief Returns entity bounds debug render state. */
        bool debugLightEmittersActive() const {return mDebugLightEmitters;}

        /** \brief Activates or deactivates the debug entity bounds rendering mode. */
        void debugLightEmitters(bool lightemitters) { mDebugLightEmitters = lightemitters; }

        /** \brief Saves the game state and its contents to the given file if */
        void save(const std::string& fileid);

        /** \brief Loads the game state and its contents from the given file if */
        void load(const std::string& fileid);

        /** \brief Accesses the layer container. */
        const RenderLayerContainer& getLayers() const { return mLayers; }
        RenderLayerContainer& getLayers() { return mLayers; }

    public:
        virtual void init() override;

        virtual void close() override;

    private:
        /** \brief Defines ids for script callbacks. */
        enum GameScriptCallbacks { ON_INIT, ON_CLOSE,
                               ON_UPDATE, ON_CUSTOM_EVENT,
                               ON_SIZE_CHANGED, ON_POSITION_CHANGED,
                               ON_BEGIN_MOVEMENT, ON_END_MOVEMENT,
                               ON_DIRECTION_CHANGED, ON_VISIBILITY_CHANGED,
                               ON_ANIMATION_START, ON_ANIMATION_STOP,
                               ON_MOV_COLLISION, ON_BEGIN_MOV_COLLISION,
                               ON_END_MOVE_COLLISION, ON_SEM_COLLISION,
                               ON_BEGIN_SEM_COLLISION, ON_END_SEM_COLLISION,
                               ON_PRESSED, ON_DOWN,
                               ON_RELEASED, ON_MOUSE_ENTER,
                               ON_MOUSE_CLICK, ON_MOUSE_EXIT,
                               ON_MOUSE_RELEASED,
                               NUM_CALLBACK /*Dont use*/ };

        /** \brief Defines names for script callbacks. */
        static constexpr std::array<const char*, NUM_CALLBACK>  GAME_CALLBACK_IDENTIFIERS = {"onInit", "onClose",
                                                                          "onUpdate", "onCustomEvent",
                                                                          "onSizeChanged", "onPositionChanged",
                                                                          "onBeginMovement", "onEndMovement",
                                                                          "onDirectionChanged", "onVisibilityChanged",
                                                                          "onAnimationStart", "onAnimationStop",
                                                                          "onMovCollision", "onBeginMovCollision",
                                                                          "onEndMovCollision", "onSemCollision",
                                                                          "onBeginSemCollision", "onEndSemCollision",
                                                                          "onPressed", "onDown",
                                                                          "onReleased", "onMouseEnter",
                                                                          "onMouseClick", "onMouseExit",
                                                                          "onMouseReleased"};

        static constexpr float UPDATE_INTERVAL = 200.0f;  //<default time between update script-calls in ms

        sf::Clock mIntervalTimer;

        CallbackInvoker mScriptCallbacks;
        std::string mGameScriptID;

        Camera mCamera;
        RenderLayerContainer mLayers;
        bool mRenderDebug;
        bool mDebugBounds;
        bool mDebugTexrects;
        bool mDebugColliders;
        bool mDebugAudioEmitters;
        bool mDebugLightEmitters;
    };
}

#endif // SCRIPTED_GAME_STATE_H

