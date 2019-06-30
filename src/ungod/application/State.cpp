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
#include "ungod/application/State.h"
#include "ungod/application/Application.h"

namespace ungod
{
    State::State(Application& cUapp, StateID id) :
                mApp(&cUapp),
                mInitialized(false),
                mExpired(false),
                mTransparent(true),
                mTranscendent(true),
                mStateID(id) {}

    void State::initState()
    {
        if (!mInitialized)
        {
            mExpired = false;
            init();
            mInitialized = true;
        }
    }

    void State::closeState()
    {
        if (mInitialized)
        {
            close();
            mInitialized = false;
        }
    }

    void State::handleEvent(const sf::Event& curEvent)
    {
        if(curEvent.type == sf::Event::KeyPressed)
        {
            switch(curEvent.key.code)
            {
            case sf::Keyboard::Escape:
               mApp->quitApplication();
               break;
            default:
                break;
            }
        }
    }

    const ContextSettings& State::getSettings() const
    {
        return mSettings;
    }

    void State::expire()
    {
        mExpired = true;
    }

    bool State::isTransparent() const
    {
        return mInitialized && mTransparent;
    }

    bool State::isTranscendent() const
    {
        return mInitialized && mTranscendent;
    }

    bool State::isExpired() const
    {
        return mExpired;
    }

    StateID State::getID() const
    {
        return mStateID;
    }

    void State::setTransparency(bool transparent)
    {
        mTransparent = transparent;
    }

    void State::setTranscendency(bool transcendent)
    {
        mTranscendent = transcendent;
    }

    State::~State()
    {
        closeState();
    }



    StateManager::StateManager(Application& app) : mApp(&app), mStates() {}


    void StateManager::handleEvent(const sf::Event& curEvent)
    {
        addPending();
        if (!hasState()) return;
        auto state = mStates.end()-1;
        for (; state != mStates.begin(); --state)
        {
            if (!(*state)->isTranscendent())
                break;
        }
        for(; state != mStates.end(); ++state)
            (*state)->handleEvent(curEvent);
        cleanup();
    }


    void StateManager::update(const float delta)
    {
        addPending();
        if (!hasState()) return;
        auto state = mStates.end()-1;
        for (; state != mStates.begin(); --state)
        {
            if (!(*state)->isTranscendent())
                break;
        }
        for(; state != mStates.end(); ++state)
            (*state)->update(delta);
        cleanup();
    }


    void StateManager::render(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (!hasState()) return;
        auto state = mStates.end()-1;
        for (; state != mStates.begin(); --state)
        {
            if (!(*state)->isTransparent())
                break;
        }
        for(; state != mStates.end(); ++state)
            (*state)->render(target, states);
    }

    void StateManager::onCustomEvent(const CustomEvent& event)
    {
        addPending();
        if (!hasState()) return;
        auto state = mStates.end()-1;
        for (; state != mStates.begin(); --state)
        {
            if (!(*state)->isTranscendent())
                break;
        }
        for(; state != mStates.end(); ++state)
            (*state)->onCustomEvent(event);
    }

    bool StateManager::hasState() const
    {
        return mHash.size() > 0;
    }

    void StateManager::cleanup()
    {
        for (auto state = mHash.begin(); state != mHash.end();)
        {
            if (state->second->isExpired())
            {
                for (auto it = mStates.begin(); it != mStates.end(); ++it)
                {
                    if (*it == state->second.get())
                    {
                        mStates.erase(it);
                        break;
                    }
                }
                //is the state still pending?
                for (auto it = mPending.begin(); it != mPending.end(); ++it)
                {
                    if (it->state == state->second.get())
                    {
                        mPending.erase(it);
                        break;
                    }
                }
                state->second->closeState();
                state = mHash.erase(state);
            }
            else
                ++state;
        }
    }

    void StateManager::addPending()
    {
        while (!mPending.empty())
        {
            PendingAdded pending = mPending.front();
            mPending.pop_front();
            if (pending.reinsert)
                mStates.erase(std::remove(mStates.begin(), mStates.end(), pending.state), mStates.end());
            if (pending.toBack)
                mStates.emplace_back( pending.state );
            else
                mStates.emplace(mStates.begin(), pending.state);
            if (pending.activate)
                pending.state->initState();
        }
    }

    State* StateManager::getForegroundState() const
    {
        return mStates.back();
    }

    StateID StateManager::getForegroundStateID() const
    {
        return mStates.back()->getID();
    }

    void StateManager::moveToBackground(StateID id)
    {
        auto res = mHash.find(id);
        if (res != mHash.end())
        {
            mPending.emplace_back(res->second.get(), false, false, true);
        }
    }


    void StateManager::moveToForeground(StateID id)
    {
        auto res = mHash.find(id);
        if (res != mHash.end())
        {
            mPending.emplace_back(res->second.get(), true, false, true);
        }
    }


    bool StateManager::hasState(StateID id) const
    {
        auto res = mHash.find(id);
        return res != mHash.end();
    }


    State* StateManager::StateManager::getState(StateID id)
    {
        auto res = mHash.find(id);
        if (res != mHash.end())
            return res->second.get();
        return nullptr;
    }
}
