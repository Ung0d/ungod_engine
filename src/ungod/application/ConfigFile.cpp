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

#include "ungod/application/ConfigFile.h"

namespace ungod
{
    Configuration::Configuration(const std::string& filepath)
    {
        load(filepath);
    }


    void Configuration::load(const std::string& filepath)
    {
        MetaList metalist{filepath};
        if (!metalist.isLoaded())
            return;
        mFilepath = filepath;
        mContent.clear();
        for (unsigned i = 0; i < metalist.getNodeCount(); i++)
        {
            auto node = metalist.getNodeAt(i);
            auto res = mContent.emplace(node.name(), configDetail::ConfigGroup{});
            forEachAttribute(node, [&] (MetaAttribute attr)
                           {
                                res.first->second.mParam.emplace(attr.name(), configDetail::ConfigItem{attr.value()});
                           });
        }
        mConfigChanged(*this, "");
    }


    void Configuration::save()
    {
        auto root = mDoc.setupRoot("config");
        for (const auto& group : mContent)
        {
            auto node = mDoc.allocateNode(group.first);
            root.appendSubnode( node );
            for (const auto& item : group.second.mParam)
            {
                node.appendAttribute( mDoc.allocateAttribute(item.first, item.second.mValue) );
            }
        }

        std::ofstream file(mFilepath);
        file << mDoc;
        file.close();
    }


    std::tuple<std::string, std::string> Configuration::split(const std::string& item)
    {
        auto pos = item.find("/");
        if (pos != std::string::npos)
        {
            return {item.substr(0, pos), item.substr(pos+1)};
        }
        else
        {
            Logger::warning(item);
            Logger::warning(" is no valid specifier for a config item. Use the format 'group/item'.");
            Logger::endl();
            return {item,""};
        }
    }

    void Configuration::onConfigurationChanged(const std::function<void(Configuration&, const std::string&)>& callback)
    {
        mConfigChanged.connect(callback);
    }


    namespace configDetail
    {
        ConfigItem::ConfigItem(const std::string& val) : mValue(val) {}

        template<>
        void ConfigItem::setValue(const std::string& t)
        {
            mValue = t;
        }

        template<>
        std::string ConfigItem::getValue() const
        {
            return mValue;
        }
    }
}

