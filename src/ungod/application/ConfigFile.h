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

#ifndef UNGOD_CONFIGFILE_H
#define UNGOD_CONFIGFILE_H

#include <unordered_map>
#include <tuple>
#include <optional>
#include "owls/Signal.h"

namespace ungod
{
    class Configuration;

    namespace configDetail
    {
        class ConfigItem
        {
        friend class ungod::Configuration;
        public:
            ConfigItem() = default;
            ConfigItem(const std::string& val);

            template<typename T>
            ConfigItem(const T& t);

            template<typename T>
            void setValue(const T& t);

            template<typename T>
            T getValue() const;

        private:
            std::string mValue;
        };

        class ConfigGroup
        {
        friend class ungod::Configuration;
        public:
            ConfigGroup() = default;

            template<typename T>
            void setValue(const std::string& item, const T& t);

            template<typename T>
            std::optional<T> getValue(const std::string& item) const;

        private:
            std::unordered_map<std::string, ConfigItem> mParam;
        };
    }


    /** \brief A configuration file consisting of config groups. */
    class Configuration
    {
    public:
        /** \brief Default constructor loads an empty configuration. */
        Configuration() = default;

        /** \brief Constructor loads a given configuration. */
        Configuration(const std::string& filepath);

        /** \brief Loads a config file. All content is copied into the Configuration object for easy editing. The content
        * of the file is not persistent in memory. */
        void load(const std::string& filepath);

        /** \brief Saves the contents of the configuration on hard drive. */
        void save();

        /** \brief Set calls take a string of the format "group/item" as argument. */
        template<typename T>
        void set(const std::string& item, const T& t);

        /** \brief Set calls take a string of the format "group/item" as argument. */
        template<typename T>
        std::optional<T> get(const std::string& item);

        /** \brief Returns a default value if not found. */
        template<typename T>
        T getOr(const std::string& item, const T& def);

        /** \brief Connects a callback to the on config changed signal.*/
        void onConfigurationChanged(const std::function<void(Configuration&, const std::string&)>& callback);

    private:
        std::tuple<std::string, std::string> split(const std::string& item);

    private:
        std::string mFilepath;
        std::unordered_map<std::string, configDetail::ConfigGroup> mContent;
        owls::Signal<Configuration&, const std::string&> mConfigChanged;
    };




    template<typename T>
    void Configuration::set(const std::string& item, const T& t)
    {
        auto tup = split(item);
        auto res = mContent.emplace(std::get<0>(tup), configDetail::ConfigGroup{});
        res.first->second.setValue(std::get<1>(tup), t);
        mConfigChanged(*this, item);
    }


    template<typename T>
    std::optional<T> Configuration::get(const std::string& item)
    {
        auto tup = split(item);
        auto res = mContent.find(std::get<0>(tup));
        if (res != mContent.end())
        {
            return res->second.getValue<T>(std::get<1>(tup));
        }
        return {};
    }


    template<typename T>
    T Configuration::getOr(const std::string& item, const T& def)
    {
        auto t = get<T>(item);
        if (!t)
            return def;
        else
            return t.value();
    }


    namespace configDetail
    {
        template<typename T>
        ConfigItem::ConfigItem(const T& t)
        {
            setValue(t);
        }

        template<typename T>
        void ConfigItem::setValue(const T& t)
        {
            mValue = std::to_string(t);
        }

        template<>
        void ConfigItem::setValue(const std::string& t);

        template<>
        std::string ConfigItem::getValue() const;

        template<typename T>
        T ConfigItem::getValue() const
        {
            std::stringstream convert(mValue);
            T value;
            convert >> value;
            return value;
        }


        template<typename T>
        void ConfigGroup::setValue(const std::string& item, const T& t)
        {
            auto res = mParam.emplace(item, configDetail::ConfigItem(t));
            res.first->second.setValue(t);
        }

        template<typename T>
        std::optional<T> ConfigGroup::getValue(const std::string& item) const
        {
            auto res = mParam.find(item);
            if (res != mParam.end())
            {
                res->second.getValue<T>();
            }
            return {};
        }
    }
}

#endif // UNGOD_CONFIGFILE_H
