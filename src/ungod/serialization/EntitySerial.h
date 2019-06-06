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

#ifndef ENTITY_SERIAL_H
#define ENTITY_SERIAL_H

#include "ungod/serialization/Serializable.h"

///this file describes how to (de)serialize entities

namespace ungod
{
    class Entity;
    class World;
    class Application;
    class InstantiationBase;
    template<typename BASE, typename OPTIONAL>
    class EntityInstantiation;
    template<typename ... BASE>
    struct BaseComponents;
    template<typename ... OPTIONAL>
    struct OptionalComponents;

    template<>
    struct SerialIdentifier< InstantiationBase >
    {
        inline static std::string get() { return "InstantiationBase"; }
    };

    template<typename ... CB, typename ... CO>
    struct SerialIdentifier< EntityInstantiation< BaseComponents<CB...>, OptionalComponents<CO...> > >
    {
        static std::string get();
    };

    template<typename ... CB, typename ... CO>
    struct SerialBehavior< EntityInstantiation< BaseComponents<CB...>, OptionalComponents<CO...> >, Entity, const World& , const Application&>
    {
        static void serialize(const EntityInstantiation< BaseComponents<CB...>, OptionalComponents<CO...> >& data,
                              MetaNode serializer, SerializationContext& context, Entity e, const World&, const Application& app);
    };

    template<typename ... CB, typename ... CO>
    struct DeserialBehavior< EntityInstantiation< BaseComponents<CB...>, OptionalComponents<CO...> >, Entity, World& , const Application&>
    {
        static void deserialize(EntityInstantiation< BaseComponents<CB...>, OptionalComponents<CO...> >& data,
                                MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application& app);
    };


    template<>
    struct SerialBehavior< Entity, const World&, const Application& >
    {
        inline static void serialize(const Entity& data, MetaNode serializer, SerializationContext& context, const World& world, const Application& app);
    };


    //non-polymorphic instantiation deserialize
    template<typename ... BASE, typename ... OPTIONAL>
    struct DeserialBehavior< Entity, World&, const Application&, BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> >
    {
        inline static void deserialize(Entity& data, MetaNode deserializer, DeserializationContext& context,
                                       World& world, const Application& app, BaseComponents<BASE...>, OptionalComponents<OPTIONAL...>);
    };

    template <>
    struct SerialID<Entity>
    {
        inline static std::uintptr_t get(const Entity& t);
    };

    //polymorphic instantiation deserialize
    /*template<>
    struct DeserialBehavior< Entity, World& >
    {
        inline static void deserialize(Entity& data, MetaNode deserializer, DeserializationContext& context, World& world);
    };*/
}

#endif // ENTITY_SERIAL_H
