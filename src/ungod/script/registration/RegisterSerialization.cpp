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

#include "ungod/script/registration/RegisterSerialization.h"
#include "ungod/serialization/Serializable.h"
#include "ungod/serialization/MetaData.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerSerialization(ScriptStateBase& state)
        {
            state.registerUsertype<MetaAttribute>("MetaAttribute",
                                                 "next", sol::overload([] (MetaAttribute& attr) { return attr.next(); },
                                                                       [] (MetaAttribute& attr, const std::string& name) { return attr.next(name.c_str()); }),
                                                 "name", &MetaAttribute::name,
                                                 "value", &MetaAttribute::value,
                                                 "valueAsBool", [] (MetaAttribute& attr) { return attr.convertValue<bool>(); },
                                                 "valueAsInt", [] (MetaAttribute& attr) { return attr.convertValue<int>(); },
                                                 "valueAsFloat", [] (MetaAttribute& attr) { return attr.convertValue<float>(); },
                                                 "valueAsString", [] (MetaAttribute& attr) { return attr.convertValue<std::string>(); },
                                                 "valueAsUnsigned", [] (MetaAttribute& attr) { return attr.convertValue<unsigned>(); });


            state.registerUsertype<MetaNode>("MetaNode",
                                             "firstNode", sol::overload([] (MetaNode& node) { return node.firstNode(); },
                                                                        [] (MetaNode& node, const std::string& name) { return node.firstNode(name.c_str()); }),
                                             "firstAttribute", sol::overload([] (MetaNode& node) { return node.firstAttribute(); },
                                                                        [] (MetaNode& node, const std::string& name) { return node.firstAttribute(name.c_str()); }),
                                             "next", sol::overload([] (MetaNode& node) { return node.next(); },
                                                                        [] (MetaNode& node, const std::string& name) { return node.next(name.c_str()); }),
                                             "prev", sol::overload([] (MetaNode& node) { return node.prev(); },
                                                                        [] (MetaNode& node, const std::string& name) { return node.prev(name.c_str()); }),
                                             "parent", &MetaNode::parent,
                                             "name", &MetaNode::name,
                                             "value", &MetaNode::value,
                                             "appendSubnode", &MetaNode::appendSubnode);


            state.registerUsertype<SerializationContext>("SerializationContext",
                                                         "appendSubnode", [] (SerializationContext& context, MetaNode node, const std::string& name)
                                                                            { return context.appendSubnode(node, name); },
                                                         "serializeProperty", sol::overload(
                                                                     [] (SerializationContext& context, const std::string& id, bool data, MetaNode serializer)
                                                                        { context.serializeProperty(id.c_str(), data, serializer); },
                                                                     [] (SerializationContext& context, const std::string& id, int data, MetaNode serializer)
                                                                        { context.serializeProperty(id.c_str(), data, serializer); },
                                                                     [] (SerializationContext& context, const std::string& id, float data, MetaNode serializer)
                                                                        { context.serializeProperty(id.c_str(), data, serializer); },
                                                                     [] (SerializationContext& context, const std::string& id, const std::string& data, MetaNode serializer)
                                                                        { context.serializeProperty(id.c_str(), data, serializer); },
                                                                     [] (SerializationContext& context, const std::string& id, unsigned data, MetaNode serializer)
                                                                        { context.serializeProperty(id.c_str(), data, serializer); } ));


            state.registerUsertype<DeserializationContext>("DeserializationContext");


            state.registerUsertype<MetaMap>("MetaMap",
                                                  "load", [] (MetaMap& metamap, const std::string& id) { metamap.load(id); },
                                                  "isLoaded", &MetaMap::isLoaded,
                                                  "getNodeWithKey", &MetaMap::getNodeWithKey);

            state.registerUsertype<MetaList>("MetaList",
                                                   "load", [] (MetaList& metalist, const std::string& id) { metalist.load(id); },
                                                   "isLoaded", &MetaList::isLoaded,
                                                   "getNodeAt", &MetaList::getNodeAt,
                                                   "getNodeCount", &MetaList::getNodeCount);

            state.registerUsertype<MetaTable>("MetaTable",
                                                   "load", [] (MetaTable& metatable, const std::string& id) { metatable.load(id); },
                                                   "isLoaded", &MetaTable::isLoaded,
                                                   "getNodeAt", &MetaTable::getNodeAt,
                                                   "getInnerSize", &MetaTable::getInnerSize,
                                                   "getOuterSize", &MetaTable::getOuterSize);


        }
    }
}
