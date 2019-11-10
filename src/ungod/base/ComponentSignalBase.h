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

#ifndef UNGOD_COMPONENT_SIGNAL_BASE_H
#define UNGOD_COMPONENT_SIGNAL_BASE_H

#include "dom/dom.h"
#include "owls/Signal.h"
#include "ungod/base/Entity.h"
#include "ungod/content/EntityTypes.h"


namespace ungod
{
    class ComponentSignalBase : public dom::Universe<>
    {
    public:
        ComponentSignalBase() = default;

        /** \brief Registers new callback for the ComponentAdded-signal. */
        template<typename C>
        void onComponentAdded(const std::function<void(Entity)>& callback);

        /** \brief Registers new callback for the ComponentRemoved-signal. */
        template<typename C>
        void onComponentRemoved(const std::function<void(Entity)>& callback);

        /** \brief Emits the ComponentAdded-signal. */
        template<typename C>
        void notifyComponentAdded(Entity e);

        /** \brief Emits the ComponentAdded-signal. */
        template<typename C>
        void notifyComponentRemoved(Entity e);

    private:
        /** \brief A base class for component signal emitters. */
        class EmitterBase 
		{
		public:
			virtual ~EmitterBase() {}
		};

        /** \brief Definitive class to emit added and removed signals for a component type C. */
        template<typename C>
        struct ComponentSignalEmitter : EmitterBase
        {
            owls::Signal<Entity> addedSignal;
            owls::Signal<Entity> removedSignal;
        };

        std::array< std::unique_ptr<EmitterBase>, dom::DEFAULT_COMPONENT_COUNT> mComponentSignals;
    };

    template<typename C>
    void ComponentSignalBase::onComponentAdded(const std::function<void(Entity)>& callback)
    {
        if (!mComponentSignals[ dom::ComponentTraits<C>::getID() ])
        {
            mComponentSignals[ dom::ComponentTraits<C>::getID() ] =
                std::unique_ptr<EmitterBase>( new ComponentSignalEmitter<C>() );
        }
        ComponentSignalEmitter<C>& se = static_cast<ComponentSignalEmitter<C>&>( *(mComponentSignals[ dom::ComponentTraits<C>::getID() ].get()) );
        se.addedSignal.connect(callback);
    }


    template<typename C>
    void ComponentSignalBase::onComponentRemoved(const std::function<void(Entity)>& callback)
    {
        if (!mComponentSignals[ dom::ComponentTraits<C>::getID() ])
        {
            mComponentSignals[ dom::ComponentTraits<C>::getID() ] =
                std::unique_ptr<EmitterBase>( new ComponentSignalEmitter<C>() );
        }
        ComponentSignalEmitter<C>& se = static_cast<ComponentSignalEmitter<C>&>( *(mComponentSignals[ dom::ComponentTraits<C>::getID() ].get()) );
        se.removedSignal.connect(callback);
    }


    template<typename C>
    void ComponentSignalBase::notifyComponentAdded(Entity e)
    {
        if (mComponentSignals[ dom::ComponentTraits<C>::getID() ])
        {
            ComponentSignalEmitter<C>& se = static_cast<ComponentSignalEmitter<C>&>( *(mComponentSignals[ dom::ComponentTraits<C>::getID() ].get()) );
            se.addedSignal(e);
        }
    }


    template<typename C>
    void ComponentSignalBase::notifyComponentRemoved(Entity e)
    {
        if (mComponentSignals[ dom::ComponentTraits<C>::getID() ])
        {
            ComponentSignalEmitter<C>& se = static_cast<ComponentSignalEmitter<C>&>( *(mComponentSignals[ dom::ComponentTraits<C>::getID() ].get()) );
            se.removedSignal(e);
        }
    }


    #include "ungod/base/Entity.inl"
}

#endif // UNGOD_COMPONENT_SIGNAL_BASE_H
