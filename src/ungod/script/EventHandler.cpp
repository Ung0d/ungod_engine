#include "EventHandler.h"
#include "ungod/base/Entity.h"
#include "ungod/script/EntityBehavior.h"

namespace ungod
{
    namespace script
    {
        void EventScheduler::handleCustomEvent(const CustomEvent& evt)
        {
            float delay = evt.get_or("delay", 0.0f);
            if (delay > 0.0f)
            {
                float dt = mClock.getElapsedTime().asSeconds() + delay;
                mDelayedEvents.emplace(dt, evt);
            }
            else
                dispatch(evt);
        }

        void EventScheduler::dispatchDelayed()
        {
            float curTime = mClock.getElapsedTime().asSeconds();
            while (!mDelayedEvents.empty() && mDelayedEvents.begin()->dispatchTime <= curTime)
            {
                dispatch(mDelayedEvents.begin()->evt);
                mDelayedEvents.erase(mDelayedEvents.begin());
            }
        }

        void EventScheduler::dispatch(const CustomEvent& evt)
        {
            mEventSignal(evt);
        }

        EventListenerLink EventScheduler::addListener(const std::function<void(const CustomEvent&)>& callback)
        {
            return mEventSignal.connect(callback);
        }


        void EventHandler::handleCustomEvent(const CustomEvent& evt) 
        {
            auto signal = mListeners.find(evt["type"]);
            if (signal != mListeners.end())
                signal->second->emit(evt);
        }
    }
}
