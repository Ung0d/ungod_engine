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
#include "ungod/content/EntityTypes.h"

///this file describes how to (de)serialize entities

namespace ungod
{
    class Entity;
    class World;
    class InstantiationBase;
    struct DeserialMemory;
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

    template<>
    struct SerialIdentifier< EntityInstantiation< EntityBaseComponents, EntityOptionalComponents > >
    {
        static std::string get() { return "Entity"; }
    };

    template<>
    struct SerialIdentifier< EntityInstantiation< ActorBaseComponents, ActorOptionalComponents > >
    {
        static std::string get() { return "Actor"; }
    };

    template<>
    struct SerialIdentifier< EntityInstantiation< MetaObjectBaseComponents, MetaObjectOptionalComponents > >
    {
        static std::string get() { return "MetaObject"; }
    };

    template<>
    struct SerialIdentifier< EntityInstantiation< LightBaseComponents, LightOptionalComponents > >
    {
        static std::string get() { return "Light"; }
    };

    template<>
    struct SerialIdentifier< EntityInstantiation< TriggerBaseComponents, TriggerOptionalComponents > >
    {
        static std::string get() { return "Trigger"; }
    };

    template<>
    struct SerialIdentifier< EntityInstantiation< WorldObjectBaseComponents, WorldObjectOptionalComponents > >
    {
        static std::string get() { return "WorldObject"; }
    };

    template<>
    struct SerialIdentifier< EntityInstantiation< ParticleSystemBaseComponents, ParticleSystemOptionalComponents > >
    {
        static std::string get() { return "ParticleSystem"; }
    };

    template<>
    struct SerialIdentifier< EntityInstantiation< AudioEmitterBaseComponents, AudioEmitterOptionalComponents > >
    {
        static std::string get() { return "AudioEmitter"; }
    };

    template<typename ... CB, typename ... CO>
    struct SerialBehavior< EntityInstantiation< BaseComponents<CB...>, OptionalComponents<CO...> >, Entity>
    {
        static void serialize(const EntityInstantiation< BaseComponents<CB...>, OptionalComponents<CO...> >& data,
                              MetaNode serializer, SerializationContext& context, Entity e);
    };

    template<typename ... CB, typename ... CO>
    struct DeserialBehavior< EntityInstantiation< BaseComponents<CB...>, OptionalComponents<CO...> >, Entity, DeserialMemory&>
    {
        static void deserialize(EntityInstantiation< BaseComponents<CB...>, OptionalComponents<CO...> >& data,
                                MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialMemory);
    };


    template<>
    struct SerialBehavior<Entity>
    {
        inline static void serialize(const Entity& data, MetaNode serializer, SerializationContext& context);
    };


    template<typename ... BASE, typename ... OPTIONAL>
    struct DeserialBehavior< Entity, DeserialMemory&, BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> >
    {
        inline static void deserialize(Entity& data, MetaNode deserializer, DeserializationContext& context,
                                 DeserialMemory& deserialMemory, BaseComponents<BASE...>, OptionalComponents<OPTIONAL...>);
    };

    template <>
    struct SerialID<Entity>
    {
        static std::uintptr_t get(const Entity& t);
    };
}

#endif // ENTITY_SERIAL_H
