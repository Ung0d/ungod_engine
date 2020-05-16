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

#ifndef STATE_H
#define STATE_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "ungod/base/ContextSettings.h"
#include "ungod/script/CustomEvent.h"
#include <unordered_map>

namespace ungod
{
    class Application;

    using StateID = std::size_t;

    /** \brief Interface for a game state. */
    class State
    {
    public:
        /**
        * \brief Constructor for a new uninitialized state.
        */
        State(Application& app, StateID id);

        /**
        * \brief Handle the given input Event. Called once for every occuring Event during a frame.
        */
        virtual void handleEvent(const sf::Event& curEvent);

        /**
        * \brief Update the internal logic of the state.
        */
        virtual void update(const float delta) {}

        /** \brief Renders the state. */
        virtual void render(sf::RenderTarget& target, sf::RenderStates states) {}

        /** \brief Reacts on a custom game event. */
        virtual void onCustomEvent(const CustomEvent& event) {}

        /** \brief Returns a handle to the underlying context settings object. */
        const ContextSettings& getSettings() const;

        /** \brief Sets the expired flag on true. The state is removed at the end of the current gameLoop. */
        void expire();

        /** \brief Returns true, if the state allows rendering of lower states. */
        bool isTransparent() const;

        /** \brief Returns true, if the state allows updating of lower states. */
        bool isTranscendent() const;

        /** \brief Returns true, if the expired method of the state was called before. */
        bool isExpired() const;

        /** \brief Returns the unique integer id of the state. */
        StateID getID() const;

        /** \brief Sets the transparent flag. */
        void setTransparency(bool transparent);

        /** \brief Sets the transcendent flag. */
        void setTranscendency(bool transcendent);

        /** \brief initializes the state if it was not initialized before. */
        void initState();

        /** \brief Closes the state if it was initialized before. */
        void closeState();

        /** \brief Returns the application that runs this state. */
        Application& getApp() const { return mApp; }

    protected:
        Application& mApp;
        bool mInitialized;
        bool mExpired;
        ContextSettings mSettings;
        bool mTransparent;
        bool mTranscendent;
        StateID mStateID;

    protected:
        /**
        * \brief Interface method for subclasses. Called when the state is activated
        * while not initialized.
        */
        virtual void init() {}

        /**
        * \brief Interface method for subclasses. Called when a state is closed.
        * Usage: Free all memory allocated during init here. A closed state is
        * expected to be nearly without any space usage.
        */
        virtual void close() {}

    public:
        virtual ~State();
    };

    using StateStack = std::vector< State* >;
    using StateHash = std::unordered_map< StateID, std::unique_ptr<State> >;

    /** \brief A manager object for states. They are collected on a stack, that is processed from up to down.
    * Updating stops at the first untranscendent state, rendering at the first untransparent state. */
    class StateManager
    {
    public:
        StateManager(Application& app);

        /** \brief Sends event data to the states to evaluate it. */
        void handleEvent(const sf::Event& curEvent);

        /** \brief Update the internal logic of the states. */
        void update(const float delta);

        /** \brief Renders the states. */
        void render(sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Forwards a custom event to all states as long as they are transcendent. */
        void onCustomEvent(const CustomEvent& event);

        /** \brief Moves to the given state to the front of the state stack. */
        void moveToForeground(StateID id);

        /** \brief Moves to the given state to the end of the state stack. */
        void moveToBackground(StateID id);

        /** \brief Returns true if there is at least one active state. */
        bool hasState() const;

        /** \brief Returns true if there is a state with the given type active. */
        bool hasState(StateID id) const;

        /** \brief Adds a new active state to the manager.
        * The init method of the new state is called. */
        template <typename STATE, typename ... ARGS>
        void addState(StateID id, ARGS&& ... args);

        /** \brief Adds a new inactive state to the manager. Init method of the
        * new stack is not called. */
        template <typename STATE, typename ... ARGS>
        void addInactiveState(StateID id, ARGS&& ... args);

        /** \brief Retrieves the state with given type. */
        State* getState(StateID id);
        template <typename STATE>
        STATE* getState(StateID id);

        /** \brief Cleans out expired states of the stack. */
        void cleanup();

        /** \brief Adds pending new states. */
        void addPending();

        /** \brief Accesses the state from the foreground of the state stack. */
        State* getForegroundState() const;

        /** \brief Returns the id of the state from the foreground of the state stack. */
        StateID getForegroundStateID() const;

    private:
        Application* mApp;
        StateStack mStates;
        StateHash mHash;

        struct PendingAdded
        {
            State* state;
            bool toBack;
            bool activate;
            bool reinsert;

            PendingAdded(State* state, bool toBack, bool activate, bool reinsert) :
                state(state), toBack(toBack), activate(activate), reinsert(reinsert) {}
        };

        std::list<PendingAdded> mPending;
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////IMPLEMENTATION///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    template <typename STATE, typename ... ARGS>
    void StateManager::addState(StateID id, ARGS&& ... args)
    {
        auto ins = mHash.emplace( id, std::unique_ptr<State>(new STATE(*mApp, id, std::forward<ARGS>(args)...)) );
        if (ins.second)
        {
            mPending.emplace_back(ins.first->second.get(), true, true, false);
        }
    }

    template <typename STATE, typename ... ARGS>
    void StateManager::addInactiveState(StateID id, ARGS&& ... args)
    {
        auto ins = mHash.emplace( id, std::unique_ptr<State>(new STATE(*mApp, id, std::forward<ARGS>(args)...)) );
        if (ins.second)
        {
            mPending.emplace_back(ins.first->second.get(), true, false, false);
        }
    }

    template <typename STATE>
    STATE* StateManager::getState(StateID id)
    {
        return static_cast<STATE*>(getState(id));
    }
}
#endif // STATE_H
