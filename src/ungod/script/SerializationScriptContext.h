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

#ifndef UNGOD_SERIALIZATION_SCRIPT_CONTEXT_H
#define UNGOD_SERIALIZATION_SCRIPT_CONTEXT_H

#include <string>
#include "ungod/script/Script.h"
#include "ungod/serialization/Serializable.h"
#include "ungod/base/Entity.h"

namespace ungod
{
    namespace script
    {
        class SerializationScriptContext
        {
        public:
            SerializationScriptContext(SerializationContext& context, MetaNode node) :
                mContext(context), mNode(node) {}

            template<typename T>
            void serialize(const std::string& name, Environment env)
            {
                sol::optional<T> data = env[name];
                if (data)
                    mContext.serializeProperty<T>(name, *data, mNode);
                else
                    Logger::warning("Attempt to serialize a non existing variable", name, "from a table.");
            }

            template<>
            void serialize<Entity>(const std::string& name, Environment env)
            {
                sol::optional<Entity> data = env[name];
                if (data)
                    mContext.serializeWeak(name, *data, data->getInstantiation()->getSerialIdentifier(), mNode);
                else
                    Logger::warning("Attempt to serialize a non existing entity", name, "from a table.");
            }

        private:
            SerializationContext& mContext;
            MetaNode mNode;
        };

        class DeserializationScriptContext
        {
        public:
            DeserializationScriptContext(DeserializationContext& context, MetaNode node) :
                mContext(context), mNode(node), mAttr() {}

            template<typename T>
            void deserialize(const std::string& name, Environment env)
            {
                T data;
                if (!mAttr)
                    mAttr = mContext.first(mContext.deserializeProperty<T>(data), name, mNode);
                else
                    mAttr = mContext.next(mContext.deserializeProperty<T>(data), name, mNode, mAttr);
                env[name] = data;
            }

            template<>
            void deserialize<Entity>(const std::string& name, Environment env)
            {
                if (!mAttr)
                    mAttr = mContext.first(mContext.deserializeWeak<Entity>([env, name](Entity& e) mutable { env[name] = e; }), name, mNode);
                else
                    mAttr = mContext.next(mContext.deserializeWeak<Entity>([env, name](Entity& e) mutable { env[name] = e; }), name, mNode, mAttr);
            }

        private:
            DeserializationContext& mContext;
            MetaNode mNode;
            MetaAttribute mAttr;
        };
    }
}


#endif // !UNGOD_SERIALIZATION_SCRIPT_CONTEXT_H