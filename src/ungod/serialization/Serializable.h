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

#ifndef UNGOD_SERIALIZABLE_H
#define UNGOD_SERIALIZABLE_H


#include "ungod/serialization/MetaNode.h"
#include "ungod/base/Utility.h"
#include <sstream>
#include <functional>
#include <unordered_map>
#include <cstring>
#include <list>
#include <queue>
#include <memory>
#include <cstdint>

namespace ungod
{
    class SerializationContext;
    class DeserializationContext;

    /**
    * \brief Defines a string-identifier for a specific object of type T.
    * This identifier could be the same for each object of type T, but must not be.
    * There must be an explicit specialization for each type T that shall be (de)serialized.
    */
    template <typename T>
    struct SerialIdentifier
    {
        static std::string get()
        /** \brief Returns a unique string as an identifier for this type. */
        {
            static_assert(AlwaysFalse<T>::value, "No explicit specialization for this type found!");
            return "";
        }
    };

    /**
    * \brief Defines how serialization shall be performed for type T.
    * There must be an explicit specialization for each type T that shall be serialized.
    */
    template <typename T, typename ... PARAM>
    struct SerialBehavior
    {
        /** \brief Defines how T shall be serialized. */
        static void serialize(const T& object, MetaNode serializer, SerializationContext& context, PARAM&& ... additionalParam)
        {
            static_assert(AlwaysFalse<T>::value, "No explicit specialization for this type found!");
        }
    };

    /**
    * \brief Defines how deserialization shall be performed for type T with return type T and additional parameters
    * PARAM. There must be an explicit specialization for each type T that shall be deserialized.
    */
    template <typename T, typename ... PARAM>
    struct DeserialBehavior
    {
        /** \brief Defines how T shall be deserialized.
        * Result is wrapped in a smart pointer of type SP.
        */
        static void deserialize(T& data, MetaNode deserializer, DeserializationContext& context, PARAM&& ... additionalParam)
        {
            static_assert(AlwaysFalse<T>::value, "No explicit specialization for this type found!");
        }
    };

    /**
    * \brief Defines a unique id for a specific object of type T.
    * This is used for shared-object matching and weak reference wiring.
    * Per default, the address in memory of the given object is returned.
    */
    template <typename T>
    struct SerialID
    {
        static std::uintptr_t get(const T& t)
        /** \brief Returns the adress of the given object. */
        {
            return reinterpret_cast<std::uintptr_t>(&t);
        }
    };


    /** \brief Basic interface that defines some static methods. */
    class BaseSerializable
    {
    friend class SerializationContext;
    friend class DeserializationContext;
    protected:
        /** "Deferred call"-utility methods to allow excplicit specializations
        * of SerialBehavior ect. to be registerted. */
        template<typename UPT, typename ... PARAM>
        static void deferredSerialize(const UPT& data, MetaNode serializer, SerializationContext& context, PARAM&& ... additionalParam);
        template<typename UPT>
        static std::string deferredGetIdentifier();
    };



    /** \brief Interface for non polymorphic objects. [CRTP] */
    template<typename T>
    class Serializable : public BaseSerializable
    {
    public:
        /** \brief Performs serialization. */
        template<typename ... PARAM>
        void serialize(MetaNode serializer, SerializationContext& context, PARAM&& ... additionalParam) const;

        /** \brief Returns the class identifier of this object. */
        std::string getIdentifier() const;
    };

    /**
    * \brief Interface for every polymorphic type that shall be (de)serialized.
    * Polymorphic serialization uses virtual methods and thus it is slower.
    */
    template<typename BASE, typename ... PARAM>
    class PolymorphicSerializable : public BaseSerializable
    {
    public:
        /** \brief Performs serialization if enabled. */
        virtual void serialize(MetaNode serializer, SerializationContext& context, PARAM&& ... additionalParam) const = 0;

        /** \brief Returns the class identifier of this object. */
        virtual std::string getIdentifier() const = 0;
    };

    /**
    * \brief Updates the type for serialization, when working with derived classes.
    * Classes in inheritance trees can call it in their constructors to update their
    * type for serialization. When serializing polymorphic pointers to base-classes,
    * serialization will always be performed for the right subclass type.
    */
    #define SERIALIZABLE_DERIVED \
    virtual void serialize(ungod::MetaNode serializer, ungod::SerializationContext& context) const override \
    { \
        typedef typename std::remove_cv<typename std::remove_reference<decltype(*this)>::type>::type UPT; \
        deferredSerialize<UPT>(*this, serializer, context); \
    } \
    virtual std::string getIdentifier() const override \
    { \
        typedef typename std::remove_cv<typename std::remove_reference<decltype(*this)>::type>::type UPT; \
        return deferredGetIdentifier<UPT>(); \
    }


    /**
    * \brief Base class for context objects.
    */
    struct Context
    {
    protected:
        static const std::string ROOT_NAME;
        static const std::string SERIALIZATION_ENTRY;
        static const std::string NULL_ADRESS;
        static const std::string POINTER_TO_SELF;
        static const std::string ROOT_OBJ;
        static const std::string POOL;
        static const std::string OBJECT_COUNT;
        static const std::string ROOT_REF;
        MetaDocument metaDoc; //<xml document

        //root node
        MetaNode serialRoot;

        //serializes a root obj
        MetaNode rootObj;

        //pool to store data of all other objects, sorted by their type-identifiers
        MetaNode pool;

    protected:
        /** \brief Returns the root node of the xml DOM-tree.*/
        MetaNode getDocumentRoot() const;

    };

    /**
    * \brief An object to maintain a serialization performance.
    */
    struct SerializationContext : public Context
    {
    private:
        //contains a metanode and the current number of subnodes
        struct NodeInfo
        {
            MetaNode node;
            unsigned subCount;
            NodeInfo(MetaNode cNode) : node(cNode), subCount(0) {}
        };

        struct SerialInfo
        {
            std::string strhash;
            unsigned index;
            MetaNode node;
            std::queue< std::pair<std::string, MetaNode> > weakQueue;
        };

        using IndexMap = std::unordered_map< uintptr_t, SerialInfo >;  //maps serialized objects on info about their serialization
        using NodeMap = std::unordered_map< std::string, NodeInfo >;    //maps object types on info about subcount and their corresponding metanodes

        //map contains all serialized objects so far together with the index of their storage node and a waiting queue for weak objects
        IndexMap indexMap;

        //map contains all serialized types so far
        NodeMap nodemap;


    private:
        //serializes the given object and returns node and index of where the serialization took place along with a
        //string hash describing the position
        template<typename T, typename ... PARAM>
        const SerialInfo& serialImpl(const T& data, const std::string& typeIdentifier, PARAM&& ... param);

    public:
        SerializationContext();


        /** \brief Utility methods that appends a subnode to the given node. */
        MetaNode appendSubnode(MetaNode node, const std::string& identifier, const std::string& value = std::string());


        /** \brief Stores the current xml-document containing all serialized information on hard-drive.
        * Creates all folders that do not exist. */
        void save(const std::string& path);


        /** \brief Converts a given object to string (template specialization for maximum efficiency. */
        template<typename T, typename std::enable_if<std::is_fundamental<T>::value, T>::type* = nullptr>
        static std::string convertToString(const T& data);

        /** \brief Converts a given object to string (template specialization for maximum efficiency. */
        template<typename T, typename std::enable_if<!std::is_fundamental<T>::value, T>::type* = nullptr>
        static std::string convertToString(const T& data);


        /** \brief Serializes the root object. The root object must be a unique,
        * non polymorphic object that triggers the whole serialization-process-tree. */
        template<typename T, typename ... PARAM>
        void serializeRootObject(const T& data, PARAM&& ... param);


        /**
        * \brief Appends a string convertible variable as a xml-attribute. Can basically serialize every type that can be attached to a output-stream.
        */
        template <typename T>
        void serializeProperty(const std::string& identifier, const T& data, MetaNode serializer);


        /**
        * \brief Serializes an object in unique or shared ownership. Note that any serialization of the same data (determined by adress)
        * more then one time will be omitted. This method is designed for objects that are owned. For serialization of a weak reference
        * to some serializeable content see "serializeWeak".
        * Serializes the given object inside the serialRoot-metanode and appends an attribute to the serializer with information to
        * where the serialization took place.
        */
        template<typename T, typename ... PARAM>
        inline void serializeObject(const std::string& objIdentifier, const T& data, MetaNode serializer, PARAM&& ... additionalParam)
        { serializeObject(objIdentifier, data, data.getIdentifier(), serializer, std::forward<PARAM>(additionalParam)...); }

        /** \brief Same as serialize unique, but with the option the explicitly specify the type identifier for the object. */
        template<typename T, typename ... PARAM>
        void serializeObject(const std::string& objIdentifier, const T& data, const std::string& typeIdentifier, MetaNode serializer, PARAM&& ... additionalParam);


        /**
        * \brief Serializes a weak reference to an serializeable object.
        * If the refered object was not serialized before, the weak ref will be queued, waiting for the
        * serializable to occur. If the refered object is not srialized at all, a nullref will be stored.
        * Not that invoking this method with a nullptr is valid and will store a nullref.
        * The weak reference will be added as an attribute to the serializer following these semantics:
        * If the refered object is null or refers to some already serialized content, the attribute
        * will be added directly when this method is called.
        * Otherwise the attribute will be added, when the refered object is serialized, or NULL at save-time,
        * when no serialization took place. Due to this behavior, it is most efficient, to serialize and
        * deserialize weak refs last, in order to gain maximum linked-list-iteration speed.
        */
        template<typename T>
        inline void serializeWeak(const std::string& objIdentifier, const T* data, MetaNode serializer);


        /**
        * \brief Serializes a CONTAINER of properties (basically everything that supports std::begin and std::end.
        * Appends a new node to the serializer with name <identifier> and puts the content of the vector in a long string separated by spaces.
        */
        template <typename T, template <typename...> typename CONTAINER>
        void serializePropertyContainer(const std::string& identifier, const CONTAINER<T>& data, MetaNode serializer);

        /** \brief Also serializes a container, but accesses elements with a getter object by index. */
        template <typename T>
        void serializePropertyContainer(const std::string& identifier, const std::function<T(std::size_t)>& getter, std::size_t num, MetaNode serializer);


        /**
        * \brief Serializes a CONTAINER of unique objects (basically everything that supports std::begin and std::end.
        * Appends a new node to the serializer with name <identifier> and puts the content of the vector in a long string separated by spaces.
        */
        template <typename T, template <typename...> typename CONTAINER, typename ... PARAM>
        inline void serializeObjectContainer(const std::string& identifier, const CONTAINER<T>& data, MetaNode serializer, PARAM&& ... additionalParam)
        {
            if (!data.empty())
                serializeObjectContainer(identifier, std::begin(data)->getIdentifier(), data, serializer, std::forward<PARAM>(additionalParam)...);
        }
        template <typename T, template <typename...> typename CONTAINER, typename ... PARAM>
        void serializeObjectContainer(const std::string& identifier, const std::string& typeIdentifier, const CONTAINER<T>& data, MetaNode serializer, PARAM&& ... additionalParam);

        /** \brief Also serializes a container, but accesses elements with a getter object by index. */
        template <typename T, typename ... PARAM>
        void serializeObjectContainer(const std::string& identifier, const std::function<const T&(std::size_t)>& getter, std::size_t num, MetaNode serializer, PARAM&& ... additionalParam)
        {
            if (num > 0)
                serializeObjectContainer(identifier, getter(0).getIdentifier(), getter, num, serializer, std::forward<PARAM>(additionalParam)...);
        }
        template <typename T, typename ... PARAM>
        void serializeObjectContainer(const std::string& identifier, const std::string& typeIdentifier, const std::function<const T&(std::size_t)>& getter, std::size_t num, MetaNode serializer, PARAM&& ... additionalParam);
    };



    /**
    * \brief An object to maintain a deserialization performance.
    */
    class DeserializationContext : public Context
    {
    private:
        using Factory = std::function<BaseSerializable*(void)>;
        using SerialBehavior = std::function<void(BaseSerializable*, MetaNode, DeserializationContext&)>;

        /** \brief Utility struct that represents an already serialized object. */
        struct NodeInfo
        {
            MetaNode node;
            BaseSerializable* ptr;
            bool deserializingNow;
            std::queue< std::function<void(BaseSerializable*)> > weakQueue;
            NodeInfo(MetaNode start) : node(start), ptr(nullptr), deserializingNow(false) {}
        };

        /** \brief Utility struct that represents a type. */
        struct TypeMapEntry
        {
            Factory factory;
            SerialBehavior behavior;
            std::vector<NodeInfo> nodes;
        };

        using TypeMap = std::unordered_map< std::string, TypeMapEntry >;

        TypeMap typeMap;

    private:
        //generic method to fetch an object from the pool of deserialize obbject
        template<typename EVAL>
        BaseSerializable* fetch(const EVAL& eval, const std::string& strhash);

        //fetching an already constructed object (no factory call)
        template<typename T, typename ... PARAM>
        void fetchNonGen(T& data, const std::string& strhash, PARAM&& ... param);

        //fetching an object by calling its factory method provided through instantiate
        template<typename T, typename ASSIGNER>
        void fetchGen(const std::string& strhash, const ASSIGNER& assigner);

        //fetches a weak ref
        template<typename T>
        void fetchWeak(const std::function<void(T&)>& assigner, const std::string& strhash);

    public:
        DeserializationContext() {}


        /** \brief Reads the given xml file and initializes the context. */
        bool read(const std::string& path);


        /** \brief Instantiates a new type in the factory map. The given callback must define how an raw object of
        * type T can be retrieved by the framework. A call of this method is mandatory before deserializing
        * polymorphic objects and object in shared ownership.
        * The given additional params will be used for all deserialized objects of type T. */
        template<typename T, typename ... PARAM>
        inline void instantiate(const Factory& fac, PARAM&& ... param)
        { instantiate<T>(SerialIdentifier<T>::get(), fac, std::forward<PARAM>(param)...); }

        template<typename T, typename ... PARAM>
        void instantiate(const std::string& typeIdentifier, const Factory& fac, PARAM&& ... param);


        /** \brief Utility method to split an object adress. */
        static std::pair<std::string, std::string> splitAdress( const std::string& adress, char delim );


        /** \brief Returns the number of objects of the specified type. */
        template<typename T>
        inline std::size_t count() const
        { return count(SerialIdentifier<T>::get()); }

        /** \brief Returns the number of objects of the specified type. */
        std::size_t count(const std::string& typeIdentifier) const;



        /** \brief Deserializes the root object. */
        template<typename T, typename ... PARAM>
        void deserializeRootObject(T& data, PARAM&& ... param);


        /** \brief Utility method that can be combined with a deserializeXXX call in order to get the first attribute of serializer. */
        template<typename FUNC>
        MetaAttribute first(const FUNC& func, const std::string& objIdentifier, MetaNode deserializer);


        /** \brief Utility method that can be combined with a deserializeXXX call in order to get the first attribute of serializer. */
        template<typename FUNC>
        MetaAttribute next(const FUNC& func, const std::string& objIdentifier, MetaNode deserializer, MetaAttribute attr);


        /** \brief Deserializes an elemental property and assigns it per ref (string, number, bool..) */
        template <typename T>
        decltype(auto) deserializeProperty(T& data, const T& defaultValue = T());

        /** \brief Deserializes an elemental property and assigns it through a setter object (string, number, bool..) */
        template <typename T, typename SETTER>
        decltype(auto) deserializeProperty(const SETTER& setter, const T& defaultValue = T());

        /** \brief Deserializes an object in unique ownership. There will be no shared ptr checking and the data object must be
        * already constructed and in a valid state. There will be no assignment for the data, just deserializing and no factory call.
        * */
        template <typename T, typename ... PARAM>
        decltype(auto) deserializeObject(T& data, PARAM&& ... param);



        /** \brief This method retrieves a serialized, shared and/or polymorphic object. The object retrieval takes place through the Factory method provided
        * through the instantiate call. The method constructs its own instance of the polymorphic subtype through the user-given factory
        * and performs deserialization on it. The type-deduction is performed based on the stored typeID under objectIdentifier, not on the
        * T type.
        *
        * Shared retrieval follows "multiple-request-single-deseralization"-semantics.
        *
        * Polymorphic subtypes are automatically handled.
        * */
        template <typename T, typename ASSIGNER>
        decltype(auto) deserializeInstantiation(const ASSIGNER& assigner);



        /** \brief Deserializes a weak reference to a serializeable object. "Weak-get" semantics are applied, meaning the referencing only occurs, if the
        * object is already deserialized or it will be deserialized through some "deserializeUnique (...shared, polym)" call later on.
        * Note that this method DO NOT DESERIALIZE on its own (by all means of "weak-semantics"). It just automatically refers to some other deserialized object
        * if this object is ever deserialized itself.
        *
        * The type T is the type of the underlying object. The assigner function object is intended to convert a reference to
        * to object to the weak wrapper instance.
        *
        * Take in mind, that the WEAK_WRAPPER reference may be queued and thus, the object hast so persist at least as long as
        * the deserialization goes.
        * */
        template <typename T>
        decltype(auto) deserializeWeak(const std::function<void(T&)>& assigner);




        /** \brief Deserializes a container.
        * Takes two function objects. The initializer is called once with the size of the deserialized container
        * (so one can reserve vector space and such stuff)
        * And the inserter is called for every deserialized element. */
        template<typename T>
        decltype(auto) deserializeContainer(const std::function<void(std::size_t)>& initializer, const std::function<void(const T&)>& inserter);



        /** \brief Deserializes a container of non-shared and non-polymorphic objects.
        * The initializer is called once with the number of objects, that are going to be serialized. The reffer is
        * that called once for each index 0 ... numObj and retrieves a reference to the i-th object, for which
        * deserialization is performed. */
        template <typename T, typename ... PARAM>
        decltype(auto) deserializeObjectContainer(const std::function<void(std::size_t)>& initializer, const std::function<T&(std::size_t)>& reffer, PARAM&& ... param);

        /** \brief Deserializes a container of polymorphic or shared objects, that are instantiated through the factory method given
        * with the "instantiate" call.
        * The initializer is called once with the number of objects, that are going to be serialized. The reffer is
        * that called once for each index 0 ... numObj and retrieves a reference to a WARPPER for the i-th object, for which
        * deserialization is performed. */
        template <typename T, typename ASSIGNER>
        decltype(auto) deserializeInstantiationContainer(const std::function<void(std::size_t)>& initializer,
                                                         const ASSIGNER& assigner);
    };


    /** \brief Serializable std::shared_ptr wrapper. */
    template<typename T>
    class SerializableSharedPtr : public Serializable<SerializableSharedPtr<T>>
    {
    public:
        std::shared_ptr<T> ptr;

         T* operator->()
         {
            return ptr.get();
         }
    };
}

#include "ungod/serialization/Serializable.inl"

#endif // UNGOD_SERIALIZABLE_H
