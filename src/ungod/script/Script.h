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

#ifndef SCRIPT_H
#define SCRIPT_H

#include "sol.hpp"
#include "ungod/ressource_management/Asset.h"

namespace ungod
{
    namespace script
    {
        using State = sol::state;
        template<typename T>
        using Usertype = sol::usertype<T>;
        using SharedState = std::shared_ptr<sol::state>;
        using StateRef = sol::state_view;
        using OptionalStateRef = sol::optional< StateRef >;
        using Environment = sol::table;
        using ProtectedFunc = sol::protected_function;
        using OptionalProtectedFunc = sol::optional<sol::protected_function>;

        /** \brief Wraps data concerning a single script. */
        class ScriptData
        {
        friend class LoadBehavior<ScriptData, StateRef>;
        private:
            ProtectedFunc mCallback;
            OptionalStateRef mState;
            bool mValid;

        private:
            /** \brief Loads a script file. */
            bool load(const std::string& filepath, StateRef state);

        public:
            ScriptData() : mValid(false) {}

            /** \brief Runs a previously loaded script. */
            bool run();

            /** \brief Accesses the underlying script state. */
            const OptionalStateRef& getState() const { return mState; }

            /** \brief Runs if the loaded script was successfully executed. */
            bool isValid();

            /** \brief Resets the internals, that is removing all refs to the script state from this ScriptData
            * in order to give the possibility to cleanly destroy the script state. A call of this method invalidates the data. */
            void reset();
        };
    }

    //define how to load
    template<>
    class LoadBehavior<script::ScriptData, script::StateRef>
    {
        static bool loadFromFile(const std::string& filepath, script::ScriptData& data, script::StateRef state);
        static std::string getIdentifier() { return "ScriptData"; }
    };

    /** \brief Expose the actual script object. */
    class Script : protected Asset<script::ScriptData, script::StateRef>
    {
    public:
        //constructors
        Script();
        Script(const std::string& filepath, script::StateRef state);

        /** \brief Loads the script but does not run it. */
        void load(const std::string& filepath, script::StateRef state);

        //expose protected functionality
        script::ScriptData* get() const;
        std::string getFilePath() const;
        bool isLoaded() const;

        /** \brief Runs the underlying script and looks up the callback method. */
        bool runScript();

        /** \brief Runs if the loaded script was successfully executed. */
        bool isValid() const;

        /** \brief Resets the underlying script and removes all refs from the data to the script state.
        * It will be no longer valid. This automatically invalidates all other Script assets refering to that scriptdata. */
        void reset() const;
    };


    /** \brief A script object along with a method-callback that can be invoked.
    * The script must define a global accessable method with the given name. */
    class InvokeableScript : public Script
    {
    public:
        //constructors
        InvokeableScript() = default;
        InvokeableScript(const std::string& filepath, const std::string& methodName, script::StateRef state);

        /** \brief Loads the script but does not run it. */
        void load(const std::string& filepath, const std::string& methodName, script::StateRef state);

        /** \brief Runs the underlying script and looks up the callback method. */
        bool runScript();

        /** \brief Runs the callback-method if it was defined in the script. A call to runScript() is mandatory before. */
        template<typename ... PARAM>
        void invokeCallback(PARAM&& ... param) const;

    private:
        script::OptionalProtectedFunc mFunc;
        std::string mMethodName;
    };


    /** \brief A free method that converts a script environment into a vector. */
    template <typename T>
    std::vector<T> env2vec(script::Environment env)
    {
        std::vector<T> res;
        res.reserve(env.size());
        env.for_each([&res](sol::object key, sol::object value)
        {
            if (value.is<T>())
                res.emplace_back(value.as<T>());
        });
        return res;
    }

    /** \brief A free method that converts a script environment into a list. */
    template <typename T>
    std::list<T> env2list(script::Environment env)
    {
        std::list<T> res;
        env.for_each([&res](sol::object key, sol::object value)
        {
            if (value.is<T>())
                res.emplace_back(value.as<T>());
        });
        return res;
    }

    /** \brief A free method, that copies the contents of env1 into env2. */
    void embedEnv(script::Environment env1, script::Environment env2);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename ... PARAM>
    void InvokeableScript::invokeCallback(PARAM&& ... param) const
    {
        if (mFunc) (*mFunc)(std::forward<PARAM>(param)...);
    }
}

#endif // SCRIPT_H
