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

#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include "dom/dom.h"
#include "quadtree/QuadTree.h"
#include "ungod/serialization/Serializable.h"
#include "ungod/serialization/EntitySerial.h"

/**
* \defgroup Components
*/

namespace ungod
{
    class World;
    class InstantiationBase;

    /**
    * \brief An entity in the "world" of every ungod-application. Very lightweight.
    * Is based on an dom::entity with a Transform and a Visual component per default.
    *
    * dom::entity
    * - arbitrary components (see dom library) can be attached to all entities
    *
    * Observer and Observable
    * - all entities can observe other observables and can be observed by other observers
    * - all entities can catch and emit Events
    *
    * Serializable
    * - all entities can be serialized to xml and deserialized (using deserialization constructor) later
    */
    class Entity : public Serializable<Entity>
    {
    friend class World;
     friend struct SerialBehavior< Entity, const World&>;
     friend struct DeserialBehavior< Entity, World&>;

    public:
        /** \brief Default constructs a invalid entity. */
		Entity() : mInstantiation(nullptr) {}
        Entity(const Entity& e) = default;
        Entity& operator=(const Entity& e) = default;

        /** \brief Constructs a new entity by assigning a handle. */
        template<typename... BASE, typename... OPTIONAL>
        Entity(dom::EntityHandle<>&& e, BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co);

        /** \brief Returns true if the entity has a Movement-component. */
        bool isStatic() const;

        /** \brief Returns true if the entity has a Rigidbody-component. */
        bool isSolid() const;

        /** \brief Returns true, if the entity is still valid, that means the underlying data was not deleted. */
        bool valid() const;
        explicit operator bool() const { return valid(); }

        bool operator==(const Entity& other) const;

        bool operator!=(const Entity& other) const;

        /**
        * \brief Test if the entity has a specific component of type C. Returns true if and only if the
        * component is assigned. O(1), just a single bit check.
        */
        template<typename C>
        bool has() const;

        /**
        * \brief Adds a new component of type C to the entity. C must be an "optional component" type.
        */
        template<typename C>
        void add() const;

        /** \brief same as add, but uses dom::instantiate to construct components with parameters. */
        template<typename C>
        void add(dom::ComponentInstantiator<C> ci) const;

        /**
        * \brief Gets a const reference to the requested component. 
        * Throws an "No-Component-Found"-error if has<C>() is false when this method is called. O(1).
        */
        template<typename C>
        const C& get() const;

        /**
        * \brief Gets a const reference to the requested component. 
        * Throws an "No-Component-Found"-error if has<C>() is false when this method is called. O(1).
        */
        template<typename C>
        C& modify() const;

        /**
        * \brief Removes the component of type C from the entity. If the component doesnt exists, the method does nothing.
        * C must be an "optional-component" type.
        */
        template<typename C>
        void rem() const;

        /**
        * \brief Accesses the world the entity is in.
        */
        ungod::World& getWorld() const;

        /** \brief Initializes a given number of multicomponents. A Multicomponent of given type must be attached. */
        template<typename MULTI, typename ... PARAM>
        void initMulti(std::size_t num, PARAM&&... param) const;

        /** \brief Returns the unique id of the entity. */
        EntityID getID() const;

        /** \brief Instantiates a component with parameters. */
        template<typename C, typename ... PARAM>
        dom::ComponentInstantiator<C> instantiate(PARAM&&... param);

        /** \brief Gets the underlying instantiation. */
        InstantiationBase* getInstantiation() const;

        /** \brief Accesses the dom-side entity handle. (dom is the underlying ECS-lib) */
        dom::EntityHandle<> getHandle() const { return mHandle; }

		/** \brief Returns the global (world independent) position of the entity. Undefined
		* if no transform component is assigned. */
		sf::Vector2f getGlobalPosition() const;

		/** \brief Returns the global (world independent) center position of the entity. Undefined
		* if no transform component is assigned. */
		sf::Vector2f getGlobalCenterPosition() const;

    private:
        dom::EntityHandle<> mHandle;
        InstantiationBase* mInstantiation;
    };


    /** \brief Mainstains a set of base components. */
    template<typename ... C>
    struct BaseComponents {};


    /** \brief Mainstains a set of optional components. */
    template<typename ... C>
    struct OptionalComponents {};


    /** \brief Base class for all EntityInstantiations for polymorphic storage. */
    class InstantiationBase : public PolymorphicSerializable<InstantiationBase, Entity, const World&>
    {
    friend class Entity;
    friend class World;

    public:
        template<typename... BASE, typename... OPTIONAL>
        static InstantiationBase* getInstantiation(BaseComponents<BASE...>, OptionalComponents<OPTIONAL...>);

        /** \brief Returns an integer code that describes the underlying Instantiation. For default instantiations
        * always -1 is returned. Custom instantiations can have their own codes by specializing the InstantiationCode traits object. */
        virtual int getCode() const = 0;

        template<typename C>
        bool checkOptional() { return checkOptional(dom::ComponentTraits<C>::getID()); }

    protected:
        //checks if a component is listed as optional before adding/removing it
        virtual bool checkOptional(unsigned short id) const = 0;

        //makes a copy based on the set of assigned components
        virtual Entity makeCopy(const dom::EntityHandle<>& e) const = 0;

		//makes a copy based on the set of assigned components
		virtual Entity makeForeignCopy(const dom::EntityHandle<>& e, dom::Universe<>& target) const = 0;

        //performs cleanup, when the entitys is destroyed
        virtual void cleanup(Entity e) const = 0;

		virtual ~InstantiationBase() {}
    };

    /**
    * \brief An EntityInstantiation is a polymorphic object, that has a specific set of "base"-components that are
    * automatically assigned on construction. In addition it has a set of "optional"-components, that may be present or
    * not. When serializing or deserializing an entitiy, it will rely on its Instantiation instead of "testing all components".
    */
    template<typename BASE, typename OPTIONAL>
    class EntityInstantiation;

    template<typename... BASE, typename... OPTIONAL>
    class EntityInstantiation< BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> > : public InstantiationBase
    {
    friend class Entity;
    friend class World;

    public:
        static EntityInstantiation< BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> >* getInstantiation() { return &sInstantiation; }

        virtual int getCode() const override;

    protected:
        virtual bool checkOptional(unsigned short id) const override;

        virtual Entity makeCopy(const dom::EntityHandle<>& e) const override;

		virtual Entity makeForeignCopy(const dom::EntityHandle<>& e, dom::Universe<>& target) const override;

        virtual void cleanup(Entity e) const override;

    private:
        static std::bitset< dom::DEFAULT_COMPONENT_COUNT > generateLookupTable();

        static EntityInstantiation< BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> > sInstantiation;
        static const std::bitset< dom::DEFAULT_COMPONENT_COUNT > sOptionalsLookupTable;

    public:
        virtual void serialize(ungod::MetaNode serializer, ungod::SerializationContext& context, Entity&& e, const World& world) const override
        {
            deferredSerialize<EntityInstantiation< BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> >, Entity, const World&>(*this, serializer, context, Entity(e), world);
        }

        virtual std::string getSerialIdentifier() const override
        {
            return deferredGetIdentifier<EntityInstantiation< BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> >>();
        }
    };


    template<typename... BASE, typename... OPTIONAL>
        EntityInstantiation< BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> >
        EntityInstantiation< BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> >::sInstantiation;

    template<typename... BASE, typename... OPTIONAL>
    const std::bitset< dom::DEFAULT_COMPONENT_COUNT >
        EntityInstantiation< BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> >::
            sOptionalsLookupTable = generateLookupTable();
}

namespace quad
{
    /** \brief We have to provide the ElementTraits for ungod::Entity */
    template <>
    struct ElementTraits<ungod::Entity>
    {
        static bool isStatic(const ungod::Entity& e);

        static Vector2f getPosition(const ungod::Entity& e);

        static Vector2f getSize(const ungod::Entity& e);

        static std::size_t getID(const ungod::Entity& e);
    };
}

namespace std
{
    template <> struct hash<ungod::Entity>
    {
      std::size_t operator () (const ungod::Entity& e) const
      {
        //return std::hash<EntityID>(e.getID());   // gcc
        return std::hash<EntityID>()(e.getID());   // msvc
      }
    };

    template <> struct hash<const ungod::Entity>
    {
      std::size_t operator () (const ungod::Entity& e) const
      {
        //return std::hash<EntityID>(e.getID());   // gcc
		  return std::hash<EntityID>()(e.getID());   // msvc
      }
    };
}


#endif // ENTITY_H


