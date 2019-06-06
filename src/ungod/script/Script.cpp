#include "Script.h"
#include <boost/filesystem.hpp>

namespace ungod
{
    namespace script
    {
        bool ScriptData::load(const std::string& filepath, StateRef state)
        {
            mValid = false;
            //load script data
            mState = state;
            try
            {
                sol::load_result r = mState->load_file(filepath);
                mValid = r.valid();
                if (mValid)
                    mCallback = r;
                else
                {
                    sol::error err = r;
                    Logger::error("Error while loading script.");
                    Logger::endl();
                    Logger::error(err.what());
                    Logger::endl();
                }
                return true;
            }
            catch (const std::exception& e)
            {
                Logger::error("Error while loading file ");
                Logger::error(filepath);
                Logger::endl();
                Logger::error(e.what());
                Logger::endl();
                return false;
            }
        }

        bool ScriptData::run()
        {
            auto res = mCallback();

            if (!res.valid())
            {
                sol::error err = res;
                Logger::error("Error while running script.");
                Logger::endl();
                Logger::error(err.what());
                Logger::endl();
                return false;
            }
            return true;
        }

        bool ScriptData::isValid()
        {
            return mValid;
        }

        void ScriptData::reset()
        {
            mValid = false;
            mCallback = script::ProtectedFunc();
            mState = script::OptionalStateRef();
        }
    }

    bool LoadBehavior<script::ScriptData, script::StateRef>::loadFromFile(const std::string& filepath, script::ScriptData& data, script::StateRef state)
    {
        if ( boost::filesystem::exists(filepath) )
        {
            return data.load(filepath, state);
        }
        else
            return false;
    }


    Script::Script() : Asset<script::ScriptData, script::StateRef>() {}

    Script::Script(const std::string& filepath, script::StateRef state)
         : Asset<script::ScriptData, script::StateRef>(filepath, SYNC, std::forward<script::StateRef>(state)) {}

    void Script::load(const std::string& filepath, script::StateRef state)
    {
        Asset<script::ScriptData, script::StateRef>::load(filepath, SYNC, std::forward<script::StateRef>(state));
    }

    script::ScriptData* Script::get() const { return Asset<script::ScriptData, script::StateRef>::get(); }

    std::string Script::getFilePath() const { return Asset<script::ScriptData, script::StateRef>::getFilePath(); }

    bool Script::isLoaded() const { return Asset<script::ScriptData, script::StateRef>::isLoaded(); }

    bool Script::runScript()
    {
        return get()->run();
    }

    bool Script::isValid() const
    {
        return get()->isValid();
    }

    void Script::reset() const
    {
        get()->reset();
    }



    InvokeableScript::InvokeableScript(const std::string& filepath, const std::string& methodName, script::StateRef state)
        : Script(filepath, state), mMethodName(methodName) {}

    void InvokeableScript::load(const std::string& filepath, const std::string& methodName, script::StateRef state)
    {
        Script::load(filepath, state);
        mMethodName = methodName;
    }

    bool InvokeableScript::runScript()
    {
        if ( get()->run() )
        {
            if (get()->getState())
                mFunc = get()->getState()->get<script::OptionalProtectedFunc>( mMethodName );
            return true;
        }
        return false;
    }


    void embedEnv(script::Environment env1, script::Environment env2)
    {
        env1.for_each([&env2](sol::object const& key, sol::object const& value)
            {
                env2.set(key, value);
            });
    }
}
