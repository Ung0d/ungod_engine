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

#include "ungod/physics/Movement.h"
#include <forward_list>

#ifndef UNGOD_STEERING_H
#define UNGOD_STEERING_H

namespace ungod
{
    template<class GETTER> class SteeringHandler;
    template<class GETTER> class SteeringManager;
    template<class GETTER> using SteeringFunc = std::function< void(Entity, MovementHandler&, const GETTER& getter) >;

    /** \brief A class that models a collection of single steering-commands
    * that are bundled to a complex behavior. */
    template<class GETTER>
    class SteeringPattern
    {
    friend class SteeringManager<GETTER>;
    friend class SteeringHandler<GETTER>;
    public:
        const std::string& getIdentifier() const { return mIdentifier; }

    private:
        std::string mIdentifier;
        std::forward_list< SteeringFunc<GETTER> > mSteeringFuncs;
    };


    template <class GETTER> using PatternMap = std::unordered_map< std::string, std::unique_ptr<SteeringPattern<GETTER>> >;

    /**
    * \ingroup Components
    * \brief A component that can be attached to an entity in order to give it
    * a linkage to a predefined steering behavior. */
    template <class GETTER>
    class SteeringComponent : public ungod::Serializable<SteeringComponent<GETTER>>
    {
    friend class SteeringHandler<GETTER>;
    friend struct SerialBehavior<SteeringComponent<GETTER>, Entity>;
    friend struct DeserialBehavior<SteeringComponent<GETTER>, Entity, DeserialMemory&>;
    public:
        SteeringComponent() : mActive(false), mInit(false), mSteeringPattern(nullptr) {}

        inline bool isActive() const { return mActive; }

    private:
        bool mActive;
        bool mInit;
        SteeringPattern<GETTER>* mSteeringPattern;
        GETTER mParam;
    };


    /** \brief A manager for steering-components. */
    template <class GETTER>
    class SteeringManager
    {
    public:
        SteeringManager() {}

        /** \brief Initilizes a new steering pattern with the given key. */
        SteeringPattern<GETTER>* newPattern(const std::string& key);

        /** \brief Returns a handle to the given pattern for faster access. Useful when chaining
        * multiple operations on the same pattern. */
        SteeringPattern<GETTER>* getPattern(const std::string& key) const;

        /** \brief Attaches a steering to the pattern. */
        void attachSteering(const std::string& key, const SteeringFunc<GETTER>& steering);
        void attachSteering(SteeringPattern<GETTER>* pattern, const SteeringFunc<GETTER>& steering);

        /** \brief Deletes the given pattern and all attached funcs. All PatternHandles will become invalid. */
        void removePattern(const std::string& key);
        void removePattern(SteeringPattern<GETTER>* profile);

    private:
        PatternMap<GETTER> mPatterns;
    };


    /** \brief A handler for connecting predefined steerings to entities. */
    template <class GETTER>
    class SteeringHandler
    {
    public:
        SteeringHandler() : mSteeringManager(nullptr) {}

        void init(const SteeringManager<GETTER>& steeringMngr) { mSteeringManager = &steeringMngr; }

        /** \brief Performs steerings for the given period of deltatime. */
        void update(const std::list<Entity>& entities, float delta, MovementHandler& mvm);

        /** \brief Actives or deactives steering for the given entity. */
        void setActive(Entity e, bool active);

        /** \brief Connects a steering pattern to the given entity. */
        void connectPattern(Entity e, const std::string& key, const GETTER& param);
        void connectPattern(Entity e, SteeringPattern<GETTER>* pattern, const GETTER& param);

        /** \brief Disconnects the steering pattern from the given entity. */
        void disconnectPattern(Entity e);

    private:
        const SteeringManager<GETTER>* mSteeringManager;
    };










    template <class GETTER>
    SteeringPattern<GETTER>* SteeringManager<GETTER>::newPattern(const std::string& key)
    {
        return mPatterns.emplace(key, std::make_unique<SteeringPattern<GETTER>>()).first->second.get();
    }

    template <class GETTER>
    SteeringPattern<GETTER>* SteeringManager<GETTER>::getPattern(const std::string& key) const
    {
        return mPatterns.find(key)->second.get();
    }

    template <class GETTER>
    void SteeringManager<GETTER>::attachSteering(const std::string& key, const SteeringFunc<GETTER>& steering)
    {
        attachSteering(getPattern(key), steering);
    }

    template <class GETTER>
    void SteeringManager<GETTER>::attachSteering(SteeringPattern<GETTER>* pattern, const SteeringFunc<GETTER>& steering)
    {
        pattern->mSteeringFuncs.push_front(steering);
    }

    template <class GETTER>
    void SteeringManager<GETTER>::removePattern(const std::string& key)
    {
        mPatterns.erase(key);
    }

    template <class GETTER>
    void SteeringManager<GETTER>::removePattern(SteeringPattern<GETTER>* profile)
    {
        mPatterns.erase(profile->getIdentifier());
    }



    template <class GETTER>
    void SteeringHandler<GETTER>::update(const std::list<Entity>& entities, float delta, MovementHandler& mvm)
    {
        dom::Utility<Entity>::iterate<TransformComponent, SteeringComponent<GETTER>>(entities,
            [delta, &mvm, this](Entity e, TransformComponent& transf, SteeringComponent<GETTER>& steering)
            {
                if (!steering.mActive)
                    return;

                auto& funcs = steering.mSteeringPattern->mSteeringFuncs;
                for (const auto& f : funcs)
                {
                    f(e, mvm, steering.mParam);
                }
            });
    }


    template <class GETTER>
    void SteeringHandler<GETTER>::setActive(Entity e, bool active)
    {
        e.modify<SteeringComponent<GETTER>>().mActive = active;
    }

    template <class GETTER>
    void SteeringHandler<GETTER>::connectPattern(Entity e, const std::string& key, const GETTER& param)
    {
        connectPattern(e, mSteeringManager->getPattern(key), param);
    }

    template <class GETTER>
    void SteeringHandler<GETTER>::connectPattern(Entity e, SteeringPattern<GETTER>* pattern, const GETTER& param)
    {
        SteeringComponent<GETTER>& sc = e.modify<SteeringComponent<GETTER>>();
        sc.mSteeringPattern = pattern;
        sc.mParam = param;
        sc.mInit = true;
        sc.mActive = true;
    }

    template <class GETTER>
    void SteeringHandler<GETTER>::disconnectPattern(Entity e)
    {
        SteeringComponent<GETTER>& sc = e.modify<SteeringComponent<GETTER>>();
        sc.mSteeringPattern = nullptr;
        sc.mInit = false;
        sc.mActive = false;
        sc.mParam = {};
    }
}

#endif // UNGOD_STEERING_H
