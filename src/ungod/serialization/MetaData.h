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

#ifndef METADATA_H
#define METADATA_H

#include <unordered_map>
#include "ungod/serialization/MetaNode.h"
#include "ungod/ressource_management/Asset.h"

namespace ungod
{
    struct MetaBase
    {
        std::string id;
        MetaDocument doc;
    };

    struct MetaDataMap : public MetaBase
    {
        std::unordered_map< std::string, MetaNode > metamap;
    };

    struct MetaDataTable : public MetaBase
    {
        std::vector< std::vector< MetaNode > > metatable;
    };

    struct MetaDataList : public MetaBase
    {
        std::vector< MetaNode > metalist;
    };

    /**
    * \brief An asset to store XML-metadata that can be accessed with a string key.
    */
    class MetaMap : public Asset<MetaDataMap>
    {
    public:
        MetaMap(const std::string uID)  :
            Asset<MetaDataMap>(uID) {}
        MetaMap() : Asset<MetaDataMap>() {}

        /**
        * \brief Retrieve a node with the given key and nullptr otherwise
        */
        MetaNode getNodeWithKey(const std::string& key) const;

        virtual ~MetaMap() {}
    };

    /**
    * \brief An asset to store XML-metadata in list format, that can be accessed with an index.
    */
    class MetaList : public Asset<MetaDataList>
    {
    public:
        MetaList(const std::string uID)  :
            Asset<MetaDataList>(uID) {}
        MetaList() : Asset<MetaDataList>() {}

        /**
        * \brief Retrieve the node at the given position in the table if such a node exists.
        */
        MetaNode getNodeAt(const unsigned index) const;

        /** \brief Returns the number of meta nodes in the list. */
        std::size_t getNodeCount() const;

        virtual ~MetaList() {}
    };

    /**
    * \brief An asset to store XML-metadata in table format, that can be accessed with 2 indices.
    */
    class MetaTable : public Asset<MetaDataTable>
    {
    public:
        MetaTable(const std::string uID)  :
            Asset<MetaDataTable>(uID) {}
        MetaTable() : Asset<MetaDataTable>() {}

        /**
        * \brief Retrieve the node at the given position in the table if such a node exists.
        */
        MetaNode getNodeAt(std::size_t outerIndex, std::size_t innerIndex) const;

        /** \brief Returns the size of the outer dimension. */
        std::size_t getOuterSize() const;

        /** \brief Returns the size of the inner dimension at the given outer index. */
        std::size_t getInnerSize(std::size_t outerIndex) const;

        virtual ~MetaTable() {}
    };



    template<>
    struct LoadBehavior<MetaDataMap>
    {
        static bool loadFromFile(const std::string& filepath, MetaDataMap& data);
        static std::string getIdentifier() { return "MetaMap"; }
    };

    template<>
    struct LoadBehavior<MetaDataList>
    {
        static bool loadFromFile(const std::string& filepath, MetaDataList& data);
        static std::string getIdentifier() { return "MetaList"; }
    };

    template<>
    struct LoadBehavior<MetaDataTable>
    {
        static bool loadFromFile(const std::string& filepath, MetaDataTable& data);
        static std::string getIdentifier() { return "MetaTable"; }
    };
}

#endif // METADATA_H
