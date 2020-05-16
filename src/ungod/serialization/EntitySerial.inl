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

#include "ungod/serialization/SerialComponents.h"
#include "ungod/serialization/CollisionSerial.h"
#include "ungod/serialization/SerialMulticomponent.h"

namespace ungod
{
    /** \brief Builds a string by concatenating all identifiers of the components in the parameter pack. */
    template<typename ... C>
    struct StringBuilder;

    //recursive part
    template<typename C1, typename ... C>
    struct StringBuilder<C1, C...>
    {
        static std::string build(std::string s = "")
        {
            return StringBuilder<C...>::build(s + SerialIdentifier<C1>::get() + std::string("_"));
        }
    };

    //terminating part
    template<typename C1>
    struct StringBuilder<C1>
    {
        static std::string build(std::string s = "")
        {
            return s + SerialIdentifier<C1>::get();
        }

    };

    //void part, invoked when attempting to build from an empty component list
    template<>
    struct StringBuilder<>
    {
        static std::string build(std::string = "") { return ""; }
    };


    /** \brief Serializes a set of base components. */
    template<typename ... C>
    struct BaseComponentSerial;

    //recursive part
    template<typename C1, typename ... C>
    struct BaseComponentSerial<C1, C...>
    {
        static void serialize(Entity e, MetaNode node, SerializationContext& context, const World& world)
        {
            context.serializeObject<C1, Entity, const World&, const Application&>(SerialIdentifier<C1>::get().c_str(), e.get<C1>(), node, Entity(e), world);
            BaseComponentSerial<C...>::serialize(e, node, context, world);
        }
    };

    //terminating part
    template<typename C1>
    struct BaseComponentSerial<C1>
    {
        static void serialize(Entity e, MetaNode node, SerializationContext& context, const World& world)
        {
            context.serializeObject<C1, Entity, const World&, const Application&>(SerialIdentifier<C1>::get().c_str(), e.get<C1>(), node, Entity(e), world);
        }
    };

    //void part, invoked when a component set is empty
    template<>
    struct BaseComponentSerial<>
    {
        static void serialize(Entity, MetaNode, SerializationContext&, const World&) {}
    };


    /** \brief Serializes a set of optional components. (difference is the existence test) */
    template<typename ... C>
    struct OptionalComponentSerial;

    //recursive part
    template<typename C1, typename ... C>
    struct OptionalComponentSerial<C1, C...>
    {
        static void serialize(Entity e, MetaNode node, SerializationContext& context, const World& world)
        {
            if (e.has<C1>())
                context.serializeObject<C1, Entity, const World&, const Application&>(SerialIdentifier<C1>::get().c_str(), e.get<C1>(), node, Entity(e), world);
            OptionalComponentSerial<C...>::serialize(e, node, context, world);
        }
    };

    //terminating part
    template<typename C1>
    struct OptionalComponentSerial<C1>
    {
        static void serialize(Entity e, MetaNode node, SerializationContext& context, const World& world)
        {
            if (e.has<C1>())
                context.serializeObject<C1, Entity, const World&, const Application&>(SerialIdentifier<C1>::get().c_str(), e.get<C1>(), node, Entity(e), world);
        }
    };

    //void part, invoked when a component set is empty
    template<>
    struct OptionalComponentSerial<>
    {
        static void serialize(Entity, MetaNode, SerializationContext&, const World&) {}
    };


    /** \brief Deserializes a parameter pack of components. */
    template<typename ... C>
    struct BaseComponentDeserial;

    //recursive part
    template<typename C1, typename ... C>
    struct BaseComponentDeserial<C1, C...>
    {
        static void deserialize(MetaNode node, MetaAttribute attr, DeserializationContext& context, Entity e, World& world)
        {
            attr = context.next( context.deserializeObject(e.modify<C1>(), Entity(e), world), SerialIdentifier<C1>::get(), node, attr );
            BaseComponentDeserial<C...>::deserialize(node, attr, context, e, world);
        }
    };

    //terminating part
    template<typename C1>
    struct BaseComponentDeserial<C1>
    {
        static void deserialize(MetaNode node, MetaAttribute attr, DeserializationContext& context, Entity e, World& world)
        {
            context.next( context.deserializeObject(e.modify<C1>(), Entity(e), world), SerialIdentifier<C1>::get(), node, attr );
        }
    };

    //initialization part
    template<typename ... C>
    struct BaseComponentDeserialInit;

    //initialization part
    template<typename C1, typename ... C>
    struct BaseComponentDeserialInit<C1, C...>
    {
        static void deserialize(MetaNode node, DeserializationContext& context, Entity e, World& world)
        {
            MetaAttribute attr = context.first( context.deserializeObject(e.modify<C1>(), Entity(e), world), SerialIdentifier<C1>::get(), node);
            BaseComponentDeserial<C...>::deserialize(node, attr, context, e, world);
        }
    };

    //initialization part
    template<typename C1>
    struct BaseComponentDeserialInit<C1>
    {
        static void deserialize(MetaNode node, DeserializationContext& context, Entity e, World& world)
        {
            context.first( context.deserializeObject(e.modify<C1>(), Entity(e), world), SerialIdentifier<C1>::get(), node);
        }
    };

    //void spec
    template<>
    struct BaseComponentDeserialInit<>
    {
        static void deserialize(MetaNode node, DeserializationContext& context, Entity e, World& world) {}
    };



    /** \brief Deserializes a parameter pack of optional components. */
    template<typename ... C>
    struct OptionalComponentDeserial;

    //recursive part
    template<typename C1, typename ... C>
    struct OptionalComponentDeserial<C1, C...>
    {
        static void deserialize(MetaNode node, MetaAttribute attr, DeserializationContext& context, Entity e, World& world)
        {
            if ((attr && attr.next() && attr.next().name() == SerialIdentifier<C1>::get()) || node.firstAttribute(SerialIdentifier<C1>::get().c_str()))
            {
                e.add<C1>();
                attr = context.next( context.deserializeObject(e.modify<C1>(), Entity(e), world), SerialIdentifier<C1>::get(), node, attr );
            }
            OptionalComponentDeserial<C...>::deserialize(node, attr, context, e, world);
        }
    };

    //terminating part
    template<typename C1>
    struct OptionalComponentDeserial<C1>
    {
        static void deserialize(MetaNode node, MetaAttribute attr, DeserializationContext& context, Entity e, World& world)
        {
            if ((attr && attr.next() && attr.next().name() == SerialIdentifier<C1>::get()) || node.firstAttribute(SerialIdentifier<C1>::get().c_str()))
            {
                e.add<C1>();
                attr = context.next( context.deserializeObject(e.modify<C1>(), Entity(e), world), SerialIdentifier<C1>::get(), node, attr );
            }
        }
    };

    //initialization part
    template<typename ... C>
    struct OptionalComponentDeserialInit;

    template<typename C1, typename ... C>
    struct OptionalComponentDeserialInit<C1, C...>
    {
        static void deserialize(MetaNode node, DeserializationContext& context, Entity e, World& world)
        {
            MetaAttribute attr;
            if (node.firstAttribute(SerialIdentifier<C1>::get().c_str()))
            {
                e.add<C1>();
                attr = context.first( context.deserializeObject(e.modify<C1>(), Entity(e), world), SerialIdentifier<C1>::get(), node );
            }
            OptionalComponentDeserial<C...>::deserialize(node, attr, context, e, world);
        }
    };

    //initialization part
    template<typename C1>
    struct OptionalComponentDeserialInit<C1>
    {
        static void deserialize(MetaNode node, DeserializationContext& context, Entity e, World& world)
        {
            if (node.firstAttribute(SerialIdentifier<C1>::get().c_str()))
            {
                e.add<C1>();
                context.first( context.deserializeObject(e.modify<C1>(), Entity(e), world), SerialIdentifier<C1>::get(), node );
            }
        }
    };

    //void spec
    template<>
    struct OptionalComponentDeserialInit<>
    {
        static void deserialize(MetaNode node, DeserializationContext& context, Entity e, World& world) {}
    };


    template<typename ... CB, typename ... CO>
    std::string SerialIdentifier< EntityInstantiation< BaseComponents<CB...>, OptionalComponents<CO...> > >::get()
    {
        static const std::string identifier = std::string("Base::") + StringBuilder<CB...>::build() + std::string("::Optional::") + StringBuilder<CO...>::build() + std::string("::");
        return identifier;
    }


    template<typename ... CB, typename ... CO>
    void SerialBehavior< EntityInstantiation< BaseComponents<CB...>, OptionalComponents<CO...> >, Entity, const World&>::
        serialize(const EntityInstantiation< BaseComponents<CB...>, OptionalComponents<CO...> >& data,
                  MetaNode serializer, SerializationContext& context, Entity e, const World& world)
    {
        MetaNode baseNode = context.appendSubnode(serializer, "b");
        BaseComponentSerial<CB...>::serialize(e, baseNode, context, world);
        MetaNode optNode = context.appendSubnode(serializer, "o");
        OptionalComponentSerial<CO...>::serialize(e, optNode, context, world);
    }


    template<typename ... CB, typename ... CO>
    void DeserialBehavior< EntityInstantiation< BaseComponents<CB...>, OptionalComponents<CO...> >, Entity, World&>::
        deserialize(EntityInstantiation< BaseComponents<CB...>, OptionalComponents<CO...> >& data,
                    MetaNode deserializer, DeserializationContext& context, Entity e, World& world)
    {
        MetaNode baseNode = deserializer.firstNode("b");
        if (baseNode)
        {
            MetaNode optNode = baseNode.next("o");
            if (optNode)
            {
                BaseComponentDeserialInit<CB...>::deserialize(baseNode, context, e, world);
                OptionalComponentDeserialInit<CO...>::deserialize(optNode, context, e, world);
                return;
            }
        }
        ungod::Logger::warning("Missing at least one mandatory node when trying to deserialize an entity. Skipping that entity.");
    }

    inline void SerialBehavior< Entity, const World& >::
        serialize(const Entity& data, MetaNode serializer, SerializationContext& context, const World& world)
    {
        data.mInstantiation->serialize(serializer, context, Entity(data), world);

        //context.serializeObject<InstantiationBase,Entity, const World&, const Application&>("i", *data.mInstantiation, serializer, Entity(data), world, app);
        //world.notifySerialized(data, serializer, context);
    }

    template<typename ... BASE, typename ... OPTIONAL>
    inline void DeserialBehavior< Entity, World&, BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> >::
        deserialize(Entity& data, MetaNode deserializer, DeserializationContext& context, World& world,
                    BaseComponents<BASE...>, OptionalComponents<OPTIONAL...>)
    {
        DeserialBehavior<EntityInstantiation<BaseComponents<BASE...>, OptionalComponents<OPTIONAL...>>, Entity, World&>::
            deserialize(static_cast<EntityInstantiation<BaseComponents<BASE...>, OptionalComponents<OPTIONAL...>>&>(*data.getInstantiation()),
                        deserializer, context, Entity(data), world);

        /*context.first(
            context.deserializeObject<EntityInstantiation<BaseComponents<BASE...>, OptionalComponents<OPTIONAL...>>, Entity, World&, const Application&>
                (static_cast<EntityInstantiation<BaseComponents<BASE...>, OptionalComponents<OPTIONAL...>>&>(*data.getInstantiation()),
                 Entity(data), world, app),
                      "i", deserializer);*/
        world.notifyDeserialized(data, deserializer, context);
    }

    inline std::uintptr_t SerialID<Entity>::get(const Entity& t)
    {
        return reinterpret_cast<std::uintptr_t>(&t.getHandle().getEntityData());
    }

   /* inline void DeserialBehavior< Entity, World& >::
        deserialize(Entity& data, MetaNode deserializer, DeserializationContext& context, World& world)
    {
        context.getPolymorphicUniqueProperty<InstantiationBase, Entity, World&>(data.mInstantiation, "instantiation", deserializer, Entity(data), world);
        world.notifyDeserialized(data, deserializer, context);
    }*/
}
