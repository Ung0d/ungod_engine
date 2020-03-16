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

#ifndef UNGOD_SCRIPT_EVENT_HANDLER_H
#define UNGOD_SCRIPT_EVENT_HANDLER_H

#include <unordered_map>
#include <set>
#include "ungod/script/CustomEvent.h"
#include "owls/Signal.h"
#include <SFML/System/Clock.hpp>

namespace ungod
{
    namespace script
    {
        namespace detail
        {
            struct DelayedEvent
            {
                DelayedEvent(float dt, const CustomEvent& e) : dispatchTime(dt), evt(e) {}
                float dispatchTime;
                CustomEvent evt;
            };
            
            struct DelayedEventCompare 
            {
                bool operator() (const DelayedEvent& lhs, const DelayedEvent& rhs) const 
                {
                    return lhs.dispatchTime < rhs.dispatchTime;
                }
            };
        }

        using EventListenerLink = owls::SignalLink<void, const CustomEvent&>;
        using Signal = owls::Signal<const CustomEvent&>;

        /**
        * \brief A class that handles event listenings and emission from and to scripted entities.
        * Entities have to listen to an event type to receive events. */
        class EventHandler
        {
        public:
            EventHandler() = default;

            void handleCustomEvent(const CustomEvent& evt);

            template<typename CB>
            EventListenerLink addListener(const CB& callback, const std::string& eventType);

            void dispatchDelayed();

        private:
            std::unordered_map< std::string, std::unique_ptr<Signal> > mListeners;
            std::set<detail::DelayedEvent, detail::DelayedEventCompare> mDelayedEvents;
            sf::Clock mClock;

        private:
            void dispatch(const CustomEvent& evt);
        };


        template<typename CB>
        EventListenerLink EventHandler::addListener(const CB& callback, const std::string& eventType)
        {
            auto signal = mListeners.emplace(eventType, std::make_unique<Signal>());
            return signal.first->second->connect(callback);
        }
    }
}

#endif // UNGOD_SCRIPT_EVENT_HANDLER_H
