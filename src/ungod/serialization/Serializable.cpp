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

#include "Serializable.h"
#include "boost/filesystem.hpp"

namespace ungod
{
    const std::string Context::ROOT_NAME = "serialroot";
    const std::string Context::SERIALIZATION_ENTRY = "s";
    const std::string Context::NULL_ADRESS = "nullptr";
    const std::string Context::POINTER_TO_SELF = "self";
    const std::string Context::ROOT_OBJ = "root_obj";
    const std::string Context::POOL = "pool";
    const std::string Context::OBJECT_COUNT = "object_count";
    const std::string Context::ROOT_REF = "ref";

    MetaNode Context::getDocumentRoot() const
    {
        return metaDoc.firstNode(ROOT_NAME.c_str());
    }


    SerializationContext::SerializationContext()
    {
        metaDoc.setupHeader();
        serialRoot = metaDoc.setupRoot(ROOT_NAME.c_str());
        rootObj = appendSubnode(serialRoot, ROOT_OBJ.c_str());
        pool = appendSubnode(serialRoot, POOL.c_str());
    }

    MetaNode SerializationContext::appendSubnode(MetaNode node, const std::string& identifier, const std::string& value)
    {
        MetaNode sub = metaDoc.allocateNode(identifier.c_str(), value.c_str());
        node.appendSubnode(sub);
        return sub;
    }

    void SerializationContext::save(const std::string& path)
    {
        //create folders if not exist
        boost::filesystem::path savePath(path);
        if (!savePath.parent_path().empty() && !boost::filesystem::exists(savePath.parent_path()))
        {
            boost::system::error_code err;
            boost::filesystem::create_directories(savePath.parent_path(), err);
            if (err) return;
        }

        //set all remaining weak refs to null
        for (auto& p : indexMap)
        {
            while (!p.second.weakQueue.empty())
            {
                serializeProperty(p.second.weakQueue.front().first, NULL_ADRESS.c_str(), p.second.weakQueue.front().second);
                p.second.weakQueue.pop();
            }
        }

        //notify parents nodes, how many subnodes they have (= number of serialized objects of a type)
        for (auto& n : nodemap)
        {
             n.second.node.appendAttribute(
                metaDoc.allocateAttribute(
                    OBJECT_COUNT.c_str(), std::to_string(n.second.subCount).c_str()));
        }

        //write file
        std::ofstream file(path);
        file << metaDoc;
        file.close();
    }


    bool DeserializationContext::read(const std::string& path)
    {
        if (!metaDoc.parse(path))
            return false;

        //iterate over all subnodes in the document and add them to the nodemap
        serialRoot = metaDoc.firstNode(ROOT_NAME.c_str());
        if (!serialRoot) return false;
        rootObj = serialRoot.firstNode(Context::ROOT_OBJ.c_str());
        pool = serialRoot.firstNode(Context::POOL.c_str());
        if (!rootObj || !pool)
            return false;

        forEachSubnode(pool, [this](MetaNode s)
        {
            MetaAttribute countAttr = s.firstAttribute(OBJECT_COUNT.c_str());
            if (countAttr)
            {
                auto empl = typeMap.emplace(s.name(), TypeMapEntry{});
                auto& vec = empl.first->second.nodes;
                vec.reserve( countAttr.convertValue<std::size_t>() );
                forEachSubnode(s, [&vec, empl](MetaNode u) { vec.emplace_back(u, empl.first->second.semanticsProducer()); });
            }
        });

        return true;
    }


    std::pair<std::string, std::string> DeserializationContext::splitAdress( const std::string& adress, char delim )
    {
        auto startPos = adress.cbegin();
        auto endPos = adress.cend();
        auto splitPos = adress.find(delim);
        if(splitPos != std::string::npos)
        {
            auto splitIt = adress.cbegin();
            std::advance(splitIt, splitPos);
            return { std::string( startPos, splitIt ), std::string( std::next(splitIt), endPos ) };
        }
        else
            return { "" , "" };
    }


    std::size_t DeserializationContext::count(const std::string& identifier) const
    {
        auto result = typeMap.find( identifier );
        if (result != typeMap.end())
        {
            return result->second.nodes.size();
        }
        return 0u;
    }
}
