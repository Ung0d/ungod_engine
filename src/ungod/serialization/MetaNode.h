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

#ifndef METANODE_H
#define METANODE_H

#include "ungod/base/Logger.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include <tuple>
#include <memory>

namespace ungod
{
    /** \brief An identifier for a meta-node attribute together with a default value. */
    template<typename T>
    struct Identifier
    {
        std::string identifier;
        T defaultValue;

        Identifier(std::string id, const T& def) : identifier(id), defaultValue(def) {}
        Identifier(std::string id) : identifier(id), defaultValue() {}
    };

    /** \brief A class warpping a XML attribute. Hides the underlying framework from the rest
    * of the codebase. */
    class MetaAttribute
    {
    friend class MetaNode;
    public:
        /** \brief Default constructs a new metanode. */
        MetaAttribute() : mAttribute(nullptr) {}

        /** \brief constructor for internal usage. */
        MetaAttribute(rapidxml::xml_attribute<>* attr) : mAttribute(attr) {}

        /** \brief Implements booled checkup for the internal ptr. */
        explicit operator bool() const { return mAttribute; }

        /** \brief Implements ptr compare. */
        bool operator==(const MetaAttribute& other) const { return mAttribute == other.mAttribute; }

        /** \brief Implements ptr compare. */
        bool operator!=(const MetaAttribute& other) const { return !(*this == other);  }

        /** \brief Returns the next attribute. */
        MetaAttribute next(const char* const identifier = nullptr) const;

        /** \brief Returns the name of the attribute. */
        std::string name() const;

        /** \brief Returns the value of the attribute. */
        std::string value() const;

        /**
        * \brief convertValueer method string -> template type. Depending on type
        * there are the appropriate std-lib functions used, for fasted parsing.
        */
        template<typename T>
        T convertValue();

    private:
        rapidxml::xml_attribute<>* mAttribute;
    };

    /**
    * \brief A class wrapping a ptr to an xml node that provides useful methods to
    * operate on the node. ALso hides the XML-framework in use from the rest of the
    * codebase.
    */
    class MetaNode
    {
    friend class MetaDocument;
    public:
        /** \brief Default constructs a new metanode. */
        MetaNode() : mNode(nullptr) {}

        /** \brief constructor for internal usage. */
        MetaNode(rapidxml::xml_node<>* node) : mNode(node) {}

        /** \brief Implements booled checkup for the internal ptr. */
        explicit operator bool() const { return mNode; }

        /** \brief Implements ptr compare. */
        bool operator==(const MetaNode& other) const { return mNode == other.mNode; }

        /** \brief Implements ptr compare. */
        bool operator!=(const MetaNode& other) const { return !(*this == other);  }

        /** \brief Returns a handle to the first xml attribute. */
        MetaAttribute firstAttribute(const char* const identifier = nullptr) const;

        /** \brief Returns a handle to the first child node. */
        MetaNode firstNode(const char* const identifier = nullptr) const;

        /** \brief Returns next sibling node. */
        MetaNode next(const char* const identifier = nullptr) const;

        /** \brief Returns the previous sibling node. */
        MetaNode prev(const char* const identifier = nullptr) const;

        /** \brief Returns the parent node. */
        MetaNode parent() const;

        /** \brief Returns the name of the node. */
        std::string name() const;

        /** \brief Returns the value of the node. */
        std::string value() const;

        /** \brief Adds a new attribute to the node. */
        void appendAttribute(MetaAttribute attr);

        /** \brief Adds a new subnode to the node. */
        void appendSubnode(MetaNode node);

        /**
        * \brief Returns the value with given identifier and type DT from the node.
        * Additionally takes a default value that is returned when the identifier is not found.
        */
        template <typename DT>
        DT getAttribute(const char* const identifier, const DT& defaultValue = DT()) const;
        template <typename DT>
        DT getAttribute(const char* const identifier, MetaAttribute& attrIter, const DT& defaultValue = DT()) const;

        /** \brief Returns a tuple of value according to the requested types and the given Identifiers.
        * Takes advantage of the order of the nodes. They should be exactly like in the xml document. */
        template <typename ... DT>
        std::tuple<DT...> getAttributes(Identifier<DT>... ids) const;

    private:
        rapidxml::xml_node<>* mNode;


        /** \brief Template unpacking helper. */
        template <typename... DT>
        struct AttributeUnpacker;
    };


    /** \brief Wraps the a handle of the underlying xml-framework for a xml-document. */
    class MetaDocument
    {
    public:
        MetaDocument() {}

        /** \brief Parses the given xml document. */
        bool parse(const std::string& filepath);

        /** \brief Returns a handle to the first child node. */
        MetaNode firstNode(const char* const identifier = nullptr) const;

        /** \brief Allocates memory for a string in the document. The string can futher on be
        * used a an identifier or a value. */
        char* allocateString(const std::string& str);

        /** \brief Allocates a new attribute in the document and returns a handle to it.
        * The handle must be attached to an metnode object. */
        MetaAttribute allocateAttribute(const std::string& name, const std::string& value);

        /** \brief Allocates a new node in the document and returns a handle to it.
        * The handle can be attached as a subnode to an existing node. */
        MetaNode allocateNode(const std::string& name, const std::string& value = std::string());

        /** \brief Sets up a header node. */
        void setupHeader();

        /** \brief Sets up a root node. */
        MetaNode setupRoot(const std::string& name);

        friend std::ostream& operator<< (std::ostream& stream, const MetaDocument& doc)
        {
            stream << doc.mDocument;
            return stream;
        }

    private:
        rapidxml::xml_document<> mDocument;
        std::unique_ptr<rapidxml::file<>> mFile;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////IMPLEMENTATION///////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    inline T MetaAttribute::convertValue()
    {
        T t;
        std::stringstream sst(value());
        sst >> t;
        return t;
    }
    template<>
    inline int MetaAttribute::convertValue<int>()
    {
        try { return std::stoi(value()); }
        catch (const std::exception&) { return int(); }
    }
    template<>
    inline unsigned MetaAttribute::convertValue<unsigned>()
    {
        try { return std::stoi(value()); }
        catch (const std::exception&) { return int(); }
    }
    template<>
    inline float MetaAttribute::convertValue<float>()
    {
        try { return std::stof(value()); }
        catch (const std::exception&) { return int(); }
    }
    template<>
    inline bool MetaAttribute::convertValue<bool>()
    {
        try { return std::stoi(value()); }
        catch (const std::exception&) { return int(); }
    }
    template<>
    inline uint8_t MetaAttribute::convertValue<uint8_t>()
    {
        try { return std::stoi(value()); }
        catch (const std::exception&) { return uint8_t(); }
    }
    template<>
    inline std::string MetaAttribute::convertValue<std::string>()
    {
        return value();
    }


    template <typename DT>
    DT MetaNode::getAttribute(const char* const identifier, const DT& defaultValue) const
    {
        DT data(defaultValue);
        MetaAttribute attr(mNode->first_attribute(identifier));
        if (attr)
        {
            try { data = attr.convertValue<DT>(); }
            catch (const std::exception&)
            {
                data = defaultValue;
                Logger::error("Error while deserializing attribute ");
                Logger::error(attr.name());
                Logger::error(" of node ");
                Logger::error(name());
                Logger::endl();
            }
        }
        return data;
    }

    template <typename DT>
    DT MetaNode::getAttribute(const char* const identifier, MetaAttribute& attrIter, const DT& defaultValue) const
    {
        DT data = defaultValue;
        if(attrIter)
            attrIter = attrIter.next(identifier);
        else
            attrIter = firstAttribute(identifier);
        if(attrIter)
        {
            try { data = attrIter.convertValue<DT>(); }
            catch (const std::exception&)
            {
                data = defaultValue;
                Logger::error("Error while deserializing attribute", attrIter.name(), "of node", name());
            }
        }
        return data;
    }

    template <typename ... DT>
    std::tuple<DT...> MetaNode::getAttributes(Identifier<DT>... ids) const
    {
        MetaAttribute attr;
        return AttributeUnpacker<DT...>::unpackAttributes(*this, attr, ids...);
    }



    template <typename DT>
    struct MetaNode::AttributeUnpacker<DT>
    {
        static std::tuple<DT> unpackAttributes(MetaNode node, MetaAttribute& attr, Identifier<DT> id)
        {
            return std::make_tuple<DT>( node.getAttribute<DT>(id.identifier.c_str(), attr, id.defaultValue) );
        }
    };


    template <typename DT1, typename ... DT>
    struct MetaNode::AttributeUnpacker<DT1, DT...>
    {
        static std::tuple<DT1, DT...> unpackAttributes(MetaNode node, MetaAttribute& attr, Identifier<DT1> id, Identifier<DT>... ids)
        {
            DT1 dt = node.getAttribute<DT1>(id.identifier.c_str(), attr, id.defaultValue);
            return std::tuple_cat( std::make_tuple<DT1>(std::move(dt)), AttributeUnpacker<DT...>::unpackAttributes(node, attr, ids...) );
        }
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////FREE_METHODS/////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** \brief Invokes a function f for every subnode of node. */
    template<typename FUNC>
    inline void forEachSubnode(MetaNode node, FUNC f, const char* name = 0)
    {
        for(MetaNode sub = node.firstNode(name); sub; sub = sub.next(name))
            f(sub);
    }


    /** \brief Invokes a function f for every attribute of node. */
    template<typename FUNC>
    inline void forEachAttribute(MetaNode node, FUNC f, const char* name = 0)
    {
        for(MetaAttribute attr = node.firstAttribute(name); attr; attr = attr.next(name))
            f(attr);
    }


    /** \brief Returns an explicit selection of child nodes. The order of the given keys is the
    * expected order of the child nodes. The return result is empty if any key is not found or
    * if the subnode order does not match the key order. */
    std::vector<MetaNode> getExplicitSubnodes( MetaNode node, std::initializer_list<std::string> keys );
}

#endif // METANODE_H
