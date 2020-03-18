#include "Behavior.h"

namespace ungod
{
    namespace detail
    {
        //null objects
        MetaCache MetaCache::sNullMeta;
        InstanceCache InstanceCache::sNullInstance(MetaCache::null());

        MetaCache::MetaCache(NamedEnvironment meta, const IdentifierLookup& callbackIdentifiers)
        {
            init(meta, callbackIdentifiers);
        }

        void MetaCache::init(NamedEnvironment meta, const IdentifierLookup& callbackIdentifiers)
        {
            mEnvironment = meta.env;
            mName = meta.name;
            mCallbacks.resize(callbackIdentifiers.size());
            for (std::size_t i = 0; i < callbackIdentifiers.size(); ++i)
            {
                //use the optional interface to check whether the required methods are defined
                mCallbacks[i] = mEnvironment->get<OptionalCallback>( callbackIdentifiers[i] );
            }
        }

        void InstanceCache::init(script::Environment parentEnv)
        {
            if (mMeta->isValid())
                mEnvironment = parentEnv;
                //mEnvironment = parentEnv.create_named(mMeta->getName()); old,,,, depr???
        }

        OptionalEnvironment InstanceCache::getEnvironment() const
        {
            return mEnvironment;
        }

        script::Environment InstanceCache::getStaticEnvironment() const
        {
            return mMeta->getEnvironment();
        }


        void MetaBehavior::init(NamedEnvironment global, const IdentifierLookup& callbackIdentifiers)
        {
            mGlobal.init(global, callbackIdentifiers);
        }

        script::Environment MetaBehavior::getStaticEnvironment() const
        {
            return mGlobal.getEnvironment();
        }

        const std::string& MetaBehavior::getName() const { return mName; }


        void MetaStateBehavior::init(NamedEnvironment global, const std::list<NamedEnvironment>& states, const IdentifierLookup& callbackIdentifiers)
        {
            MetaBehavior::init(global, callbackIdentifiers);
            init(states, callbackIdentifiers);
        }

        void MetaStateBehavior::init(const std::list<NamedEnvironment>& states, const IdentifierLookup& callbackIdentifiers)
        {
            mStates.reserve(states.size());
            mInitState = states.begin()->name;
            for (const auto& s : states)
            {
                mStates.emplace_back(s, callbackIdentifiers);
                mStatesPrototype.emplace(s.name, mStates.back());
            }
        }
    } //detail

    ScriptStateBase::ScriptStateBase()
    {
        reset();
    }

    ScriptStateBase::ScriptStateBase(const script::SharedState& state, script::Environment main)
    {
        reset(state, main);
    }

    const script::SharedState& ScriptStateBase::getSharedState() const
    {
        return mScriptState;
    }

    void ScriptStateBase::reset()
    {
        mMainEnv = script::Environment{};
        mScriptState = std::make_shared<script::State>();
        mScriptState->open_libraries(sol::lib::base, sol::lib::math, sol::lib::package, sol::lib::string);
        mMainEnv = mScriptState->create_named_table("ungod");
    }

    void ScriptStateBase::reset(const script::SharedState& state, script::Environment main)
    {
        mMainEnv = script::Environment{};
        mScriptState = state;
        mScriptState->open_libraries(sol::lib::base, sol::lib::math, sol::lib::package, sol::lib::string);
        mMainEnv = main;
    }


    CallbackInvoker::CallbackInvoker(const detail::IdentifierLookup& identifiers)
        : ScriptStateBase(), mIdentifiers(identifiers) {}

    CallbackInvoker::CallbackInvoker(const script::SharedState& state, script::Environment main, const detail::IdentifierLookup& identifiers)
        : ScriptStateBase(state, main), mIdentifiers(identifiers) {}

    ScriptErrorCode CallbackInvoker::loadScript(const std::string& filepath)
    {
        mScript.load(filepath, script::StateRef(*mScriptState));

        if (!mScript.isLoaded())
            return SCRIPT_NOT_FOUND;

        if (!mScript.isValid())
            return SCRIPT_LOAD_ERROR;

        sol::optional<detail::NamedEnvironment> global;

        boost::filesystem::path p{filepath};
        detail::NamedEnvironment env {mScriptState->create_named_table(p.stem().string()), p.stem().string()};

        if (!mScript.runScript())
            return SCRIPT_EXECUTION_ERROR;

        mCallbackCache.init(env, mIdentifiers);

        return SCRIPT_OK;
    }

    script::Environment CallbackInvoker::getEnvironment() const
    {
        return mCallbackCache.getEnvironment();
    }

    bool CallbackInvoker::isValid() const
    {
        return mScript.isLoaded() && mScript.isValid() && mCallbackCache.isValid();
    }

    void CallbackInvoker::reload()
    {
        //reset the script and the meta cache, to deref from the old script state and avoid errors
        mScript.reset();
        mCallbackCache = detail::MetaCache::null();

        //reset the script state and load again
        ScriptStateBase::reset();
        loadScript(mScript.getFilePath());
    }

    void CallbackInvoker::reload(const script::SharedState& state, script::Environment main)
    {
        //reset the script and the meta cache, to deref from the old script state and avoid errors
        mScript.reset();
        mCallbackCache = detail::MetaCache::null();

        //reset the script state and load again
        ScriptStateBase::reset(state, main);
        loadScript(mScript.getFilePath());
    }
}
