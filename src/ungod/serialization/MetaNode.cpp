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

#include "MetaNode.h"

namespace ungod
{
    MetaAttribute MetaAttribute::next(const char* const identifier) const
    {
        return mAttribute->next_attribute(identifier);
    }


    std::string MetaAttribute::name() const
    {
        return { std::string(mAttribute->name(), mAttribute->name_size()) };
    }


    std::string MetaAttribute::value() const
    {
        return { std::string(mAttribute->value(), mAttribute->value_size()) };
    }



    MetaAttribute MetaNode::firstAttribute(const char* const identifier) const
    {
        return mNode->first_attribute(identifier);
    }


    std::string MetaNode::name() const
    {
        return { std::string(mNode->name(), mNode->name_size()) };
    }


    std::string MetaNode::value() const
    {
        return { std::string(mNode->value(), mNode->value_size()) };
    }

    void MetaNode::appendAttribute(MetaAttribute attr)
    {
        mNode->append_attribute( attr.mAttribute );
    }

    void MetaNode::appendSubnode(MetaNode node)
    {
        mNode->append_node(node.mNode);
    }

    MetaNode MetaNode::next(const char* const identifier) const
    {
        return mNode->next_sibling(identifier);
    }

    MetaNode MetaNode::prev(const char* const identifier) const
    {
        return mNode->previous_sibling(identifier);
    }

    MetaNode MetaNode::firstNode(const char* const identifier) const
    {
        return mNode->first_node(identifier);
    }

    MetaNode MetaNode::parent() const
    {
        return mNode->parent();
    }


    bool MetaDocument::parse(const std::string& filepath)
    {
        try
        {
            mFile = std::make_unique<rapidxml::file<>>(filepath.c_str());
            mDocument.parse<rapidxml::parse_fastest>(mFile->data());
            return true;
        }
        catch(rapidxml::parse_error &e)
        {
            Logger::error("Cant parse metadata file", filepath);
        }
        catch(const std::runtime_error& e)
        {
            Logger::error("Cant open metadata file", filepath);
        }
        return false;
    }

    MetaNode MetaDocument::firstNode(const char* const identifier) const
    {
        return mDocument.first_node(identifier);
    }

    char* MetaDocument::allocateString(const std::string& str)
    {
        return mDocument.allocate_string(str.c_str(), str.size());
    }

    MetaAttribute MetaDocument::allocateAttribute(const std::string& name, const std::string& value)
    {
        const char* allocName = allocateString(name);
        const char* allocValue = allocateString(value);
        return mDocument.allocate_attribute(allocName, allocValue, name.size(), value.size());
    }

    MetaNode MetaDocument::allocateNode(const std::string& name, const std::string& value)
    {
        const char* allocName = allocateString(name);
        const char* allocValue = allocateString(value);
        return mDocument.allocate_node(rapidxml::node_element, allocName, allocValue, name.size(), value.size());
    }

    void MetaDocument::setupHeader()
    {
        MetaNode decl = mDocument.allocate_node(rapidxml::node_declaration);
        decl.appendAttribute(allocateAttribute("version", "1.0"));
        decl.appendAttribute(allocateAttribute("encoding", "UTF-8"));
        mDocument.append_node(decl.mNode);
    }

    MetaNode MetaDocument::setupRoot(const std::string& name)
    {
        const char* allocName = allocateString(name);
        MetaNode root = mDocument.allocate_node(rapidxml::node_element, allocName, 0, name.size());
        mDocument.append_node(root.mNode);
        return root;
    }


    std::vector<MetaNode> getExplicitSubnodes( MetaNode node, std::initializer_list<std::string> keys )
    {
        std::vector<MetaNode> result;

        if (keys.size() == 0) return result;

        auto iter = keys.begin();

        //start with the first key
        MetaNode current = node.firstNode(iter->c_str());
        if (!current) return result;
        result.push_back( current );
        ++iter;

        for (; iter != keys.end(); ++iter)
        {
            current = current.next(iter->c_str());
            if (!current)
                return std::vector<MetaNode>();
            else
                result.push_back(current);
        }

        return result;  //if nothing went wrong
    }
}
