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

#ifndef UNGOD_ENTITY_BEHAVIOR_HANDLER_H
#define UNGOD_ENTITY_BEHAVIOR_HANDLER_H


#include <unordered_set>
#include <SFML/System/Clock.hpp>
#include "ungod/script/EntityBehavior.h"
#include "ungod/base/Entity.h"
#include "ungod/script/CustomEvent.h"
#include "ungod/serialization/Serializable.h"
#include "ungod/script/EventHandler.h"

namespace ungod
{
    struct DeserialMemory;
    class WorldGraph;
    class WorldGraphNode;

    /**
    * \ingroup Components
    * \brief A component that holds a script-defined behavior for an entity. The triggers for that behavior are
    * aronous gameplay- and input-events or update calls within fixed intervals. */
    class EntityBehaviorComponent : public Serializable<EntityBehaviorComponent>
    {
        friend class EntityBehaviorHandler;
        friend struct DeserialBehavior<EntityBehaviorComponent, Entity, DeserialMemory&>;
    private:
        StateBehaviorPtr<> mBehavior;

    public:
        EntityBehaviorComponent() = default;

        /** \brief Returns a script variable defined in this behavior. Call is safe. If the variable is not defined, an empty optional is returned. */
        template<typename T>
        Optional<T> getStateVariable(const std::string& stateName, const std::string& name) const;

        /** \brief Returns true if and only if a valid behavior is assigned. Note that this does not mean,
        * that the behavior is safe to use. If the underlying script had an error, this is still true, but
        * the script has no valid script environment set. Check hasValidEnvironment, to check whether this
        * is the case. Note that hasValidEnvironment also checks for validity first. This method here is basically
        * useful, to make sure that a script assignment was made before, independently of the fact, that the loaded script
        * had errors or not. Methods that work independently of the script-error-state are:
        * - getScriptName(). */
        bool valid() const;

        /** \brief Returns true if the behavior was successfully initialized before and a script enviroment is set.
        * Note that the behavior connection could be valid (it was assigned by the manager), but the underlying scriot
        * had errors. In this case a valid() call is true, but there is still no script-environment set. Use this method
        * to make absolutely sure, that 1) a behavior is assigned and 2) the assigned behavior was executed without problems  */
        bool hasValidEnvironment() const;

        /** \brief Returns true if the behavior was successfully initialized before and a script enviroment is set.
        * See also hasValidEnvionment() ref. */
        bool hasValidStateEnvironment(const std::string& stateName) const;

        /** \brief Returns the instance script-enviroment. A test whether a behavior was successfully assigned before is mandatory. */
        script::Environment getEnvironment() const;

        /** \brief Returns the instance script-enviroment. A test whether a behavior was successfully assigned before is mandatory. */
        script::Environment getStateEnvironment(const std::string& stateName) const;

        /** \brief Returns the top level identifier of the assigned script. The behavior must be in a valid state. */
        const std::string& getScriptName() const;
    };


    /**
    * \ingroup Components
    * \brief A component that holds a timer for controlling the continous update of entities. */
    class EntityUpdateTimer : public Serializable<EntityUpdateTimer>
    {
        friend class EntityBehaviorHandler;
    private:
        sf::Clock mTimer;
        float mInterval;

    public:
        EntityUpdateTimer() : mInterval(200.0f) {}

        EntityUpdateTimer(float cInterval) : mInterval(cInterval) {}
    };

    /** \brief A manager object that couples the ungod::Entity object with script-defined behavior.
   * That means, there is a set of actions defined here, that are catched by this manager and
   * evaluated in script files. */
    class EntityBehaviorHandler
    {
    public:
        EntityBehaviorHandler();

        /** \brief Initializes the manager. */
        void init(World& world);

        /** \brief Updates the manager and may invoke onUpdate scripts of entities. */
        void update(const std::list<Entity>& entities, float delta);

        /** \brief Forwards the custom event to the script behaviors. */
        void handleCustomEvent(const CustomEvent& event);

        /** \brief Assigns a behavior to the given entity. The behavior must have been loaded beforehand. */
        void assignBehavior(Entity e, const std::string& name);
        void assignBehavior(Entity e, const std::string& name, script::Environment param);

        /** \brief Inits the behavior of the entity. A behavior has to be attached. */
        void initBehavior(Entity e);

        /** \brief Dissociates the current behavior from the entity, if any. */
        void dissociateBehavior(Entity e);

        /** \brief Sets the update interval of the given entity. */
        void setUpdateInterval(Entity e, float interval);
        void setUpdateInterval(EntityUpdateTimer& timer, float interval);

        /** \brief Registers an entity as a listener to an event type. */
        script::EventListenerLink addEventListener(Entity e, const std::string& eventType);

        /** \brief Registers an arbitrary callable as a listener to an event type. */
        script::EventListenerLink addEventListener(const script::ProtectedFunc& func, const std::string& eventType);

        ~EntityBehaviorHandler();

    private:
        const World* mWorld;
        std::unordered_set<Entity> mMetaEntities; ///< stores entities with no transform components (at the time of script assignment) in order to process them
        script::EventHandler mEventHandler;
        owls::SignalLink<void, ScriptQueues&> mDissociateLink;
        owls::SignalLink<void, ScriptQueues&> mReloadLink;
        owls::SignalLink<void, std::string> mButtonDownLink;
        owls::SignalLink<void, std::string> mButtonReleasedLink;
        owls::SignalLink<void, std::string> mButtonPressedLink;
        owls::SignalLink<void, Entity, WorldGraph&, WorldGraphNode&, WorldGraphNode&> mEntityChangedNodeLink;

    private:
        void entityCreation(Entity e) const;
        void entityDestruction(Entity e) const;
        void entityCollisionEnter(Entity e1, Entity e2) const;
        void entityCollisionExit(Entity e1, Entity e2) const;
        void entityCollision(Entity e1, Entity e2, const sf::Vector2f& mdv, const Collider& c1, const Collider& c2) const;
        void buttonPressed(const std::string& binding) const;
        void buttonDown(const std::string& binding) const;
        void buttonReleased(const std::string& binding) const;
        void entitySerialized(Entity e, MetaNode serializer, SerializationContext& context);
        void entityDeserialized(Entity e, MetaNode deserializer, DeserializationContext& context);
        void scriptInternalReassign(ScriptQueues& toReload);
        void scriptInternalDissociate(ScriptQueues& toReload);

        template<typename ... PARAM>
        void callbackInvoker(std::size_t index, Entity e, PARAM&& ... param)
        {
            if (e.has<EntityBehaviorComponent>() && e.modify<EntityBehaviorComponent>().mBehavior)
                e.modify<EntityBehaviorComponent>().mBehavior->execute(index, std::forward<PARAM>(param)...);
        }
    };


    ///////impl

    template<typename T>
    Optional<T> EntityBehaviorComponent::getStateVariable(const std::string& stateName, const std::string& name) const
    {
        if (mBehavior)
            return mBehavior->getStateVariable<T>(stateName, name);
        else
            return Optional<T>();
    }
}

#endif // UNGOD_ENTITY_BEHAVIOR_HANDLER_H
