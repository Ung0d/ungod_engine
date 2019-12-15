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

namespace ungod
{
    template<typename UPT, typename ... PARAM>
    void BaseSerializable::deferredSerialize(const UPT& data, MetaNode serializer, SerializationContext& context, PARAM&& ... additionalParam)
    {
        SerialBehavior<UPT, PARAM...>::serialize(data, serializer, context, std::forward<PARAM>(additionalParam) ... );
    }

    template<typename UPT>
    std::string BaseSerializable::deferredGetIdentifier()
    {
        return SerialIdentifier<UPT>::get();
    }

    template<typename T>
    template<typename ... PARAM>
    void Serializable<T>::serialize(MetaNode serializer, SerializationContext& context, PARAM&& ... additionalParam) const
    {
        deferredSerialize<T>(static_cast<const T&>(*this), serializer, context, std::forward<PARAM>(additionalParam) ...);
    }

    template<typename T>
    std::string Serializable<T>::getSerialIdentifier() const
    {
        return deferredGetIdentifier<T>();
    }


    template<typename T, typename std::enable_if<std::is_fundamental<T>::value, T>::type*>
    std::string SerializationContext::convertToString(const T& data)
    {
        return std::to_string(data);
    }

    template<typename T, typename std::enable_if<!std::is_fundamental<T>::value, T>::type*>
    std::string SerializationContext::convertToString(const T& data)
    {
        std::stringstream sst;
        sst << data;
        return sst.str();
    }




    template<typename T, typename ... PARAM>
    const SerializationContext::SerialInfo& SerializationContext::serialImpl(const T& data, const std::string& typeIdentifier, PARAM&& ... additionalParam)
    {
        //get the type master node or create a new one if non-existing
        auto empl = nodemap.emplace( typeIdentifier, NodeInfo(nullptr) );
        if (empl.second) //no subnode exists; create one
            empl.first->second.node = appendSubnode(pool, typeIdentifier.c_str());

        //object already serialized? (shared instance call?)
        auto res = indexMap.emplace(SerialID<T>::get(data), SerialInfo());
        if (res.second) //first time serialize
        {
            //define the serial info and insert it under the data-adress
            res.first->second.index = empl.first->second.subCount++;
            res.first->second.node = appendSubnode(empl.first->second.node, SERIALIZATION_ENTRY.c_str());
            res.first->second.strhash = typeIdentifier + "@" + std::to_string(res.first->second.index);

            //serialize the object
            data.serialize(res.first->second.node, *this, std::forward<PARAM>(additionalParam)...);

            //if there are weak refs queued, give them the strhash
            while (!res.first->second.weakQueue.empty())
            {
                serializeProperty(res.first->second.weakQueue.front().first, res.first->second.strhash, res.first->second.weakQueue.front().second);
                res.first->second.weakQueue.pop();
            }
        }

        return res.first->second;
    }


    template<typename T, typename ... PARAM>
    void SerializationContext::serializeRootObject(const T& data, PARAM&& ... additionalParam)
    {
        serializeObject(ROOT_REF.c_str(), data, data.getSerialIdentifier(), rootObj, std::forward<PARAM>(additionalParam)...);
    }

    template <typename T>
    void SerializationContext::serializeProperty(const std::string& identifier, const T& data, MetaNode serializer)
    {
        MetaAttribute attr = metaDoc.allocateAttribute(identifier.c_str(), convertToString(data).c_str());
        serializer.appendAttribute(attr);
    }


    template<typename T, typename ... PARAM>
    void SerializationContext::serializeObject(const std::string& objIdentifier, const T& data,
                                               const std::string& typeIdentifier, MetaNode serializer, PARAM&& ... additionalParam)
    {
        const SerialInfo& serialInfo = serialImpl(data, typeIdentifier, std::forward<PARAM>(additionalParam)...);

        serializeProperty(objIdentifier, serialInfo.strhash, serializer);
    }


    template<typename T>
    inline void SerializationContext::serializeWeak(const std::string& objIdentifier, const T* data, MetaNode serializer)
    {
        if (data)
        {
            auto res = indexMap.emplace(SerialID<T>::get(*data), SerialInfo());
            if (res.second || res.first->second.weakQueue.size() > 0)
            {
                res.first->second.weakQueue.emplace(objIdentifier, serializer);
            }
            else
            {
                serializeProperty(objIdentifier, res.first->second.strhash, serializer);
            }
        }
        else
        {
            serializeProperty(objIdentifier, NULL_ADRESS.c_str(), serializer);
        }
    }


    template <typename T, template <typename...> typename CONTAINER>
    void SerializationContext::serializePropertyContainer(const std::string& identifier, const CONTAINER<T>& data, MetaNode serializer)
    {
        //attempt to serialize content in the following form:
        //<number of objects> obj1 obj2 obj2 ...
        std::stringstream build;
        build << data.size();
        auto it = std::begin(data);
        if (it != std::end(data)) //if there is any data in the container
        {
            build << " " << convertToString(*it);
            ++it;
            for (; it != std::end(data); ++it)
            {
                build << " " << convertToString(*it);
            }
        }
        serializeProperty(identifier, std::string(build.str()), serializer);
    }


    template <typename T>
    void SerializationContext::serializePropertyContainer(const std::string& identifier, const std::function<T(std::size_t)>& getter, std::size_t num, MetaNode serializer)
    {
        //attempt to serialize content in the following form:
        //<number of objects> obj1 obj2 obj2 ...
        std::stringstream build;
        build << num;
        if (num > 0)
        {
            build << " " << convertToString(getter(0));
            for (std::size_t i = 1; i < num; ++i)
            {
                build << " " << convertToString(getter(i));
            }
        }
        serializeProperty(identifier, std::string(build.str()), serializer);
    }



    template <typename T, template <typename...> typename CONTAINER, typename ... PARAM>
    void SerializationContext::serializeObjectContainer(const std::string& identifier, const std::string& typeIdentifier, const CONTAINER<T>& data, MetaNode serializer, PARAM&& ... additionalParam)
    {
        //attempt to serialize content in the following form:
        //<number of objects> adrToObj1 adrToObj2 adrToObj3 ...
        std::stringstream build;
        build << data.size();
        auto it = std::begin(data);
        if (it != std::end(data)) //if there is any data in the container
        {
            {
                const SerialInfo& serialInfo = serialImpl(*it, typeIdentifier, std::forward<PARAM>(additionalParam)...);
                build << " " << serialInfo.strhash;
                ++it;
            }
            for (; it != std::end(data); ++it)
            {
                const SerialInfo& serialInfo = serialImpl(*it, typeIdentifier, std::forward<PARAM>(additionalParam)...);
                build << " " << serialInfo.strhash;
            }
        }
        serializeProperty(identifier, std::string(build.str()), serializer);
    }


    template <typename T, typename ... PARAM>
    void SerializationContext::serializeObjectContainer(const std::string& identifier, const std::string& typeIdentifier, const std::function<const T&(std::size_t)>& getter, std::size_t num, MetaNode serializer, PARAM&& ... additionalParam)
    {
        //attempt to serialize content in the following form:
        //<number of objects> adrToObj1 adrToObj2 adrToObj3 ...
        std::stringstream build;
        build << num;
        if (num > 0)
        {
            {
                const T& g = getter(0);
                const SerialInfo& serialInfo = serialImpl(g, typeIdentifier, std::forward<PARAM>(additionalParam)...);
                build << " " << serialInfo.strhash;
            }
            for (std::size_t i = 1; i < num; ++i)
            {
                const T& g = getter(i);
                const SerialInfo& serialInfo = serialImpl(g, typeIdentifier, std::forward<PARAM>(additionalParam)...);
                build << " " << serialInfo.strhash;
            }
        }
        serializeProperty(identifier, std::string(build.str()), serializer);
    }



    template<typename EVAL>
    BaseSerializable* DeserializationContext::fetch(const EVAL& eval, const std::string& strhash)
    {
        auto split = splitAdress(strhash, '@');

        std::size_t index;
        try { index = stoi(split.second); } catch (const std::exception&) { return nullptr; }
        auto res = typeMap.find(split.first);
        if (res != typeMap.end() && res->second.nodes.size() > index)
        {
            NodeInfo& nodeinfo = res->second.nodes[index];

            if (!nodeinfo.deserializingNow)
            {
                nodeinfo.deserializingNow = true;
                eval(nodeinfo, res->second);
                while (!nodeinfo.weakQueue.empty())
                {
                    nodeinfo.weakQueue.front()();
                    nodeinfo.weakQueue.pop();
                }
                nodeinfo.deserializingNow = false;
                return nodeinfo.ptr->get();
            }
            else
            {
                Logger::warning("Pointer cycle detected!");
                Logger::endl();
                Logger::warning("Skipping deserialization of this object.");
                Logger::endl();
            }
        }
        else
        {
            Logger::warning("Invalid ref detected during fetch.");
            Logger::endl();
            Logger::warning("Did you miss the instantiate call for type ");
            Logger::warning(split.first);
            Logger::warning("?");
            Logger::endl();
            Logger::warning("Is the save-file corrupted?");
            Logger::endl();
            Logger::warning("Skipping deserialization of this object.");
            Logger::endl();
        }
        return nullptr;
    }


    template<typename T, typename ... PARAM>
    void DeserializationContext::fetchNonGen(T& data, const std::string& strhash, PARAM&& ... param)
    {
        if (strhash != NULL_ADRESS)
        {
            fetch([&](NodeInfo& nodeinfo, TypeMapEntry&)
              {
                    nodeinfo.ptr->set(&data);
                    DeserialBehavior<T, PARAM...>::deserialize(data, nodeinfo.node, *this, std::forward<PARAM>(param)...);
              }, strhash );
        }
    }


    template<typename T, typename ASSIGNER>
    void DeserializationContext::fetchGen(const std::string& strhash, const ASSIGNER& assigner)
    {
        if (strhash != NULL_ADRESS)
        {
            BaseSerializable* ptr = fetch([this, strhash](NodeInfo& nodeinfo, TypeMapEntry& typeentry)
              {
                  if (typeentry.behavior)
                  {
                    nodeinfo.ptr->set(typeentry.factory());
                    typeentry.behavior(nodeinfo.ptr->get(), nodeinfo.node, *this);
                  }
                  else
                  {
                    Logger::warning("Requesting ");
                    Logger::warning(strhash);
                    Logger::warning(" but no valid factory is assigned.");
                    Logger::endl();
                    Logger::warning("Did you miss the instantiate call?");
                    Logger::endl();
                    Logger::warning("Skipping deserialization of this object.");
                    Logger::endl();
                  }
              }, strhash );
            if (ptr)
            {
                assigner(static_cast<T*>(ptr));
            }
        }
    }


    template<typename T>
    void DeserializationContext::fetchWeak(const std::function<void(T&)>& assigner, const std::string& strhash)
    {
        if (strhash != NULL_ADRESS)
        {
            auto split = splitAdress(strhash, '@');

            std::size_t index;
            try { index = stoi(split.second); } catch (const std::exception&) { return; }
            auto res = typeMap.find(split.first);
            if (res != typeMap.end() && res->second.nodes.size() > index)
            {
                NodeInfo& nodeinfo = res->second.nodes[index];
                if (nodeinfo.ptr->valid())
                {
                    assigner(*(reinterpret_cast<T*>(nodeinfo.ptr->get())));
                }
                else  //wait until deserialization takes place
                {
                    nodeinfo.weakQueue.push([assigner, &nodeinfo]() { assigner(*(reinterpret_cast<T*>(nodeinfo.ptr->get()))); });
                }
            }
            else
            {
                Logger::warning("Invalid ref detected during weak fetch.");
                Logger::endl();
                Logger::warning("Did you miss the instantiate call for type ");
                Logger::warning(split.first);
                Logger::warning("?");
                Logger::endl();
                Logger::warning("Is the save-file corrupted?");
                Logger::endl();
                Logger::warning("Skipping deserialization of this object.");
                Logger::endl();
                return;
            }
        }
    }


    //helpers
    template <class T> std::reference_wrapper<T> mayWrap(T& val) { return std::ref(val); }
    template <class T> T&& mayWrap(T&& val) { return std::forward<T>(val); }

    template<typename T, typename ... PARAM>
    void DeserializationContext::instantiate(const std::string& typeIdentifier, const Factory& fac, PARAM&& ... param)
    {
        using namespace std::placeholders;
        auto res = typeMap.emplace(typeIdentifier, TypeMapEntry{});
        res.first->second.factory = fac;
        auto boundParam = std::bind(&DeserialBehavior<T, PARAM...>::deserialize, _1, _2, _3, mayWrap(std::forward<PARAM>(param))... );
        res.first->second.behavior = [boundParam](BaseSerializable* base, MetaNode node, DeserializationContext& context)
        {
            boundParam(static_cast<T&>(*base), node, context);
        };
    }


    template<typename SEM>
    void DeserializationContext::changeStorageSemantics(const std::string& typeIdentifier)
    {
        auto res = typeMap.emplace(typeIdentifier, TypeMapEntry{});
        res.first->second.semanticsProducer = [](){ return new SEM(); };
    }


    template<typename T, typename ... PARAM>
    void DeserializationContext::deserializeRootObject(T& data, PARAM&& ... param)
    {
        MetaAttribute attr = rootObj.firstAttribute(ROOT_REF.c_str());
        if (attr)
        {
            fetchNonGen(data, attr.value(), std::forward<PARAM>(param)...);
        }
    }


    template<typename FUNC>
    MetaAttribute DeserializationContext::first(const FUNC& func, const std::string& objIdentifier, MetaNode deserializer)
    {
       MetaAttribute attr = deserializer.firstAttribute(objIdentifier.c_str());
       func(attr);
       return attr;
    }


    template<typename FUNC>
    MetaAttribute DeserializationContext::next(const FUNC& func, const std::string& objIdentifier, MetaNode deserializer, MetaAttribute attr)
    {
        if (attr)
            attr = attr.next(objIdentifier.c_str());
        if (!attr)
            attr = deserializer.firstAttribute(objIdentifier.c_str());
        func(attr);
        return attr;
    }


    template <typename T>
    decltype(auto) DeserializationContext::deserializeProperty(T& data, const T& defaultValue)
    {
        return [&] (MetaAttribute attr)
        {
            if (attr)
                data = attr.convertValue<T>();
            else
                data = defaultValue;
        };
    }


    template <typename T, typename SETTER>
    decltype(auto) DeserializationContext::deserializeProperty(const SETTER& setter, const T& defaultValue)
    {
        return [&] (MetaAttribute attr)
        {
            if (attr)
                setter(attr.convertValue<T>());
            else
                setter(defaultValue);
        };
    }


    template <typename T, typename ... PARAM>
    decltype(auto) DeserializationContext::deserializeObject(T& data, PARAM&& ... param)
    {
        return [&] (MetaAttribute attr)
        {
            if (attr)
                fetchNonGen(data, attr.value(), std::forward<PARAM>(param)...);
        };
    }

    template <typename T, typename ASSIGNER>
    decltype(auto) DeserializationContext::deserializeInstantiation(const ASSIGNER& assigner)
    {
        return [&] (MetaAttribute attr)
        {
            if (attr)
                fetchGen<T>(attr.value(), assigner);
        };
    }


    template <typename T>
    decltype(auto) DeserializationContext::deserializeWeak(const std::function<void(T&)>& assigner)
    {
        return [this, &assigner] (MetaAttribute attr)
        {
            if (attr)
                fetchWeak(assigner, attr.value());
        };
    }


    template<typename T>
    decltype(auto) DeserializationContext::deserializeContainer(const std::function<void(std::size_t)>& initializer, const std::function<void(const T&)>& inserter)
    {
        return [&] (MetaAttribute attr)
        {
            if (!attr)
                return;
            std::stringstream stream(attr.value());
            std::size_t siz;
            if (stream >> siz)
            {
                initializer(siz);
            }
            typename std::decay<T>::type t;
            std::size_t i = 0;
            while (stream >> t && i < siz)
            {
                inserter(t);
                i++;
            };
        };
    }


    template <typename T, typename ... PARAM>
    decltype(auto) DeserializationContext::deserializeObjectContainer(const std::function<void(std::size_t)>& initializer, const std::function<T&(std::size_t)>& reffer, PARAM&& ... param)
    {
        return [&] (MetaAttribute attr)
        {
            if (!attr)
                return;
            std::stringstream stream(attr.value());
            std::size_t siz;
            if (stream >> siz)
            {
                initializer(siz);
            }
            std::string strhash;
            std::size_t i = 0;
            while (stream >> strhash && i < siz)
            {
                fetchNonGen(reffer(i), strhash, std::forward<PARAM>(param)...);
                i++;
            };
        };
    }


    template <typename T, typename ASSIGNER>
    decltype(auto) DeserializationContext::deserializeInstantiationContainer(const std::function<void(std::size_t)>& initializer,
                                                                             const ASSIGNER& assigner)
    {
        return [&] (MetaAttribute attr)
        {
            if (!attr)
                return;
            std::stringstream stream(attr.value());
            std::size_t siz;
            if (stream >> siz)
            {
                initializer(siz);
            }
            std::string strhash;
            std::size_t i = 0;
            while (stream >> strhash && i < siz)
            {
                using namespace std::placeholders;
                fetchGen<T>(strhash, std::bind(assigner, _1, i));
                i++;
            };
        };
    }
}
