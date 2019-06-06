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

#ifndef REGISTRATION_DETAIL_H
#define REGISTRATION_DETAIL_H

#include "ungod/script/Behavior.h"
#include "ungod/base/MultiComponent.h"

namespace ungod
{
    namespace scriptRegistration
    {
    namespace detail
    {
        /** \brief Invokes a script-function as a callback. */
        template<typename ... T>
        void callback(const ungod::script::ProtectedFunc& func, T&& ... param)
        {
            auto result = func(std::forward<T>(param)...);
            if ( !result.valid() )
            {
                sol::error err = result;
                Logger::error("An error occured during a script.");
                Logger::endl();
                Logger::error(err.what());
                Logger::endl();
            }
        }

        /** \brief Registers a multicomponent... */
        template<typename C>
        void registerMultiComponent(ScriptStateBase& state, const std::string& name)
        {
            state.registerUsertype<MultiComponent<C>>(name,
                                                "getComponent", [] (MultiComponent<C>& multi, std::size_t i) -> const C&  { return multi.getComponent(i); },
                                                "getComponentCount", &MultiComponent<C>::getComponentCount);
        }

        /** \brief Converts a script enviroment to a c++ vector. */
        template<typename T>
        bool table2vec(std::vector<T>& vec, script::Environment env)
        {
            vec.reserve(env.size());
            bool success = true;
            env.for_each([&vec, &success] (std::pair<sol::object, sol::object> e)
                            {
                                if (e.second.is<T>())
                                    vec.emplace_back(e.second.as<T>());
                                else
                                {
                                    success = false;
                                    Logger::warning("An element in the script enviroment is not of the required type. Cant append it to the vector.");
                                    Logger::endl();
                                }
                            });
            return success;
        }
    }
    }
}

#endif // REGISTRATION_DETAIL_H
