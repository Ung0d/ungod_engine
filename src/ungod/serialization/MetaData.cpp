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

#include "ungod/serialization/MetaData.h"

namespace ungod
{
    MetaNode MetaMap::getNodeWithKey(const std::string& key) const
    {
        auto metainfo = get()->metamap.find(key);
        if(metainfo != get()->metamap.end())
            return metainfo->second;
        else
            return nullptr;
    }


    MetaNode MetaList::getNodeAt(const unsigned index) const
    {
        if(index < get()->metalist.size())
        {
            return get()->metalist[index];
        }
        return nullptr;
    }

    std::size_t MetaList::getNodeCount() const
    {
        return get()->metalist.size();
    }


    MetaNode MetaTable::getNodeAt(std::size_t outerIndex, std::size_t innerIndex) const
    {
        if(outerIndex < get()->metatable.size() &&
           innerIndex < get()->metatable[outerIndex].size())
        {
            return get()->metatable[outerIndex][innerIndex];
        }
        return nullptr;
    }

    std::size_t MetaTable::getOuterSize() const
    {
        return get()->metatable.size();
    }

    std::size_t MetaTable::getInnerSize(std::size_t outerIndex) const
    {
        return get()->metatable[outerIndex].size();
    }



    bool LoadBehavior<MetaDataMap>::loadFromFile(const std::string& filepath, MetaDataMap& data)
    {
        if (!data.doc.parse(filepath))
            return false;
        if (!data.doc.firstNode())
            return false;
        for (MetaNode node = data.doc.firstNode().firstNode(); node; node = node.next())
        {
            const std::string key(node.name());
            data.metamap.insert({key, node});
        }
        return true;
    }

    bool LoadBehavior<MetaDataList>::loadFromFile(const std::string& filepath, MetaDataList& data)
    {
        if (!data.doc.parse(filepath))
            return false;
        if (!data.doc.firstNode())
            return false;
        for(MetaNode node = data.doc.firstNode().firstNode(); node; node = node.next())
        {
            data.metalist.emplace_back(node);
        }
        return true;
    }

    bool LoadBehavior<MetaDataTable>::loadFromFile(const std::string& filepath, MetaDataTable& data)
    {
        if (!data.doc.parse(filepath))
            return false;
        if (!data.doc.firstNode())
            return false;
        for(MetaNode node = data.doc.firstNode().firstNode(); node; node = node.next())
        {
            std::vector< MetaNode > tableRow;
            for(MetaNode subnode = node.firstNode(); subnode; subnode = subnode.next())
            {
                tableRow.push_back(subnode);
            }
            data.metatable.emplace_back(tableRow);
        }
        return true;
    }
}
