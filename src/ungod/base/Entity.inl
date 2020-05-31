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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////IMPLEMENTATION//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace detail
{
    //unpacker that emits component added signals
    template<typename... C>
    struct AddUnpacker;

    template<typename C1, typename... C>
    struct AddUnpacker<C1, C...>
    {
        static void unpack(Entity e)
        {
            static_cast<ComponentSignalBase&>(e.getHandle().getUniverse()).notifyComponentAdded<C1>(e);
            AddUnpacker<C...>::unpack(e);
        }
    };

    template<typename C1>
    struct AddUnpacker<C1>
    {
        static void unpack(Entity e)
        {
            static_cast<ComponentSignalBase&>(e.getHandle().getUniverse()).notifyComponentAdded<C1>(e);
        }
    };

    template<>
    struct AddUnpacker<>
    {
        static void unpack(Entity e) {}
    };
}

template<typename... BASE, typename... OPTIONAL>
Entity::Entity(dom::EntityHandle<>&& e, BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co) : mHandle(e)
{
    mInstantiation = InstantiationBase::getInstantiation(cb, co);
    detail::AddUnpacker<BASE...>::unpack(*this);
}

template<typename C>
void Entity::add() const
{
    if (mInstantiation->checkOptional<C>())
    {
        mHandle.add<C>();
        static_cast<ComponentSignalBase&>(mHandle.getUniverse()).notifyComponentAdded<C>(*this);
    }
    else
        ungod::Logger::warning("Tried to add a component to an entity, that is not listed as optional-component.");
}

template<typename C>
void Entity::add(dom::ComponentInstantiator<C> ci) const
{
    if (mInstantiation->checkOptional<C>())
    {
        mHandle.add<C>(ci);
        static_cast<ComponentSignalBase&>(mHandle.getUniverse()).notifyComponentAdded<C>(*this);
    }
    else
        ungod::Logger::warning("Tried to add a component to an entity, that is not listed as optional-component.");
}


template<typename C>
bool Entity::has() const { return mHandle.has<C>(); }


template<typename C>
const C& Entity::get() const { return mHandle.get<C>(); }


template<typename C>
C& Entity::modify() const { return mHandle.modify<C>(); }


template<typename C, typename ... PARAM>
dom::ComponentInstantiator<C> Entity::instantiate(PARAM&&... param) { return mHandle.instantiate<C>(std::forward<PARAM>(param)...); }


template<typename C>
void Entity::rem() const
{
    if (mInstantiation->checkOptional<C>())
    {
        mHandle.rem<C>();
        static_cast<ComponentSignalBase&>(mHandle.getUniverse()).notifyComponentRemoved<C>(*this);
    }
    else
        ungod::Logger::warning("Tried to remove a component from an entity, that is not listed as optional-component.");
}

template<typename MULTI, typename ... PARAM>
void Entity::initMulti(std::size_t num, PARAM&&... param) const
{
    modify<MULTI>().init( num, mHandle.getUniverse(), std::forward<PARAM>(param)... );
}


template<typename... BASE, typename... OPTIONAL>
InstantiationBase* InstantiationBase::getInstantiation(BaseComponents<BASE...>, OptionalComponents<OPTIONAL...>)
{
    return static_cast<InstantiationBase*>(
            EntityInstantiation<BaseComponents<BASE...>, OptionalComponents<OPTIONAL...>>::getInstantiation() );
}


//helper struct that builds a the lookup table for optional types
template<typename ... C>
struct LookupTableBuilder;

template<typename C1, typename ... C>
struct LookupTableBuilder<C1, C...>
{
    static void build(std::bitset< dom::DEFAULT_COMPONENT_COUNT >& tableInit)
    {
        tableInit.set( dom::ComponentTraits<C1>::getID() );
        LookupTableBuilder<C...>::build(tableInit);
    }
};

template<typename C1>
struct LookupTableBuilder<C1>
{
    static void build(std::bitset< dom::DEFAULT_COMPONENT_COUNT >& tableInit)
    {
        tableInit.set( dom::ComponentTraits<C1>::getID() );
    }
};

template<>
struct LookupTableBuilder<>
{
    static void build(std::bitset< dom::DEFAULT_COMPONENT_COUNT >&) {}
};



template<typename... BASE, typename... OPTIONAL>
bool EntityInstantiation< BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> >::checkOptional(unsigned short id) const
{
    return sOptionalsLookupTable.test(id);
}

template<typename... BASE, typename... OPTIONAL>
Entity EntityInstantiation< BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> >::makeCopy(const dom::EntityHandle<>& e) const
{
    return Entity( e.checkedCopy<BASE..., OPTIONAL...>(), BaseComponents<BASE...>(), OptionalComponents<OPTIONAL...>() );
}

template<typename... BASE, typename... OPTIONAL>
Entity EntityInstantiation< BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> >::makeForeignCopy(const dom::EntityHandle<>& e, dom::Universe<>& target) const
{
	return Entity(e.checkedForeignCopy<BASE..., OPTIONAL...>(target), BaseComponents<BASE...>(), OptionalComponents<OPTIONAL...>());
}

namespace detail
{
    //unpacker that emits component removed signals
    template<typename... C>
    struct RemUnpacker;

    template<typename C1, typename... C>
    struct RemUnpacker<C1, C...>
    {
        static void unpack(Entity e)
        {
            static_cast<ComponentSignalBase&>(e.getHandle().getUniverse()).notifyComponentRemoved<C1>(e);
            RemUnpacker<C...>::unpack(e);
        }
    };

    template<typename C1>
    struct RemUnpacker<C1>
    {
        static void unpack(Entity e)
        {
            static_cast<ComponentSignalBase&>(e.getHandle().getUniverse()).notifyComponentRemoved<C1>(e);
        }
    };

    template<>
    struct RemUnpacker<>
    {
        static void unpack(Entity e) {}
    };


    //unpacker that emits component removed signals
    template<typename... C>
    struct OptionalRemUnpacker;

    template<typename C1, typename... C>
    struct OptionalRemUnpacker<C1, C...>
    {
        static void unpack(Entity e)
        {
            if (e.has<C1>())
                static_cast<ComponentSignalBase&>(e.getHandle().getUniverse()).notifyComponentRemoved<C1>(e);
            OptionalRemUnpacker<C...>::unpack(e);
        }
    };

    template<typename C1>
    struct OptionalRemUnpacker<C1>
    {
        static void unpack(Entity e)
        {
            if (e.has<C1>())
                static_cast<ComponentSignalBase&>(e.getHandle().getUniverse()).notifyComponentRemoved<C1>(e);
        }
    };

    template<>
    struct OptionalRemUnpacker<>
    {
        static void unpack(Entity e) {}
    };
}

template<typename... BASE, typename... OPTIONAL>
void EntityInstantiation< BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> >::cleanup(Entity e) const
{
    detail::RemUnpacker<BASE...>::unpack(e);
    detail::OptionalRemUnpacker<OPTIONAL...>::unpack(e);
}

template<typename... BASE, typename... OPTIONAL>
int EntityInstantiation< BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> >::getCode() const
{
    return InstantiationCode< BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> >::get();
}

template<typename... BASE, typename... OPTIONAL>
std::bitset< dom::DEFAULT_COMPONENT_COUNT > EntityInstantiation<BaseComponents<BASE...>, OptionalComponents<OPTIONAL...>>::generateLookupTable()
{
    std::bitset< dom::DEFAULT_COMPONENT_COUNT > lookupTable;
    LookupTableBuilder<OPTIONAL...>::build(lookupTable);
    return lookupTable;
}
