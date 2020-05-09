#include "Script.h"
#include <boost/filesystem.hpp>

namespace ungod
{
    Script::Script(const std::string& filepath, script::StateRef state) { load(filepath, state); }

    bool Script::load(const std::string& filepath, script::StateRef state)
    {
        if (boost::filesystem::exists(filepath))
        {
            mFilepath.clear();
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
                mFilepath = filepath;
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
        else
        {
            Logger::error("Script file ");
            Logger::error(filepath);
            Logger::error(" does not exists.");
            Logger::endl();
            return false;
        }
    }

    const std::string& Script::getFilePath() const { return mFilepath; }

    bool Script::isLoaded() const { return mFilepath.size() > 0; }

    bool Script::run()
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

    bool Script::isValid() const
    {
        return mValid;
    }

    void Script::reset() 
    {
        mValid = false;
        mFilepath.clear();
        mCallback = script::ProtectedFunc();
        mState = script::OptionalStateRef();
    }



    InvokeableScript::InvokeableScript(const std::string& filepath, const std::string& methodName, script::StateRef state)
        : Script(filepath, state), mMethodName(methodName) {}

    bool InvokeableScript::load(const std::string& filepath, const std::string& methodName, script::StateRef state)
    {
        mMethodName = methodName;
        return Script::load(filepath, state);
    }

    bool InvokeableScript::run()
    {
        if (Script::run())
        {
            if (mState)
                mFunc = mState->get<script::OptionalProtectedFunc>( mMethodName );
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
