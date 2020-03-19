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

/**
* A short tutorial on how to write Entity-script files. Basically you can define an arbitrary subset of the set of
* possible methods. You just need to know which methods exists, when they are called and what parameters they get.
*
* Vocabulary:
* static -- the static container of the script, equal for each instance, also contains the callback methods
* instance --  a table that can be used to store data for the particular entity instance that is shared between all method calls
* instance provides some additional members:
* instance.entity : the underlying ungod entity
* instance.states : an object to control things like state switches for the entity (if StateBehavior)
* instance.world : the world the entitiy lives in
*
* inInit(static, instance)
* -- This method is invoked when a state is entered.
*
* onExit(static, instance)
* -- This method is invoked when a state is left.
*
* onStaticConstr(static)
* -- This method is invoked when an instance of a script is created and no other instances exist yet
*
* onStaticDestr(static)
* -- This method is invoked if the last existing instance of a script dies
*
* onSerialize(static, instance, serializer, context)
* -- This method is invoked on serialization
*
* onDeserialize(static, instance, deserializer, context)
* -- This method is invoked on deserialization
*
* onUpdate(static, instance, delta)
* -- This method is invoked constantly in a fixed interval of milliseconds, per default 5 times a second.
*
* onCreation(static, instance)
* -- This method is invoked when a script is assigned to the EntityBehavior component of the given entity. (NOT neccessarily when the entity itself is created)
*
* onMouseEnter(static, instance)
* -- This method is invoked when mouse cursor enters the bounding box of the entity.
*
* onMouseExit(static, instance)
* -- This method is invoked when mouse cursor exits the bounding box of the entity.
*
* onMouseClicked(static, instance)
* -- This method is invoked when mouse is clicked over the bounding box of the entity.
*
* onMouseReleased(static, instance)
* -- This method is invoked when mouse is released over the bounding box of the entity.
*
* onButtonDown(static, instance, binding)
* -- This method is invoked when a button is downed.
*
* onButtonPressed(static, instance, binding)
* -- This method is invoked when a button is pressed. (intial press)
*
* onButtonReleased(static, instance, binding)
* -- This method is invoked when a button is relased.
*
* onMovementBegin(static, instance, vel)
* -- This method is invoked when an entity starts moving in some direction.
*
* onMovementEnd(static, instance)
* -- This method is invoked when an moving entity stops its movement.
*
* onDirectionChanged(static, instance, old, current)
* -- This method is invoked when an entity changes its movement-direction.
*
* onCustomEvent(static, instance, event)
* -- This method is invoked when a custom event occurs
*
* onAnimationBegin(static, instance, key)
* -- This method is invoked when a custom event occurs
*
* onAnimationEnd(static, instance, key)
* -- This method is invoked when a custom event occurs
*
* onCollisionBegin(static, instance, other)
* -- This method is invoked when a semantics collision is detected for 2 entities that did not collide previously
*
* onCollision(static, instance, other, mdv, c1, c2)
* -- This method is invoked every frame a semantics collision is detected for 2 entities
*
* onCollisionEnd(static, instance, other)
* -- This method is invoked when 2 entities stop semantics collision
*/

#ifndef ENTITY_BEHAVIOR_H
#define ENTITY_BEHAVIOR_H

#include <unordered_set>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include "ungod/script/Behavior.h"
#include "ungod/script/CustomEvent.h"
#include "ungod/serialization/Serializable.h"
#include "ungod/base/Entity.h"
#include "ungod/script/EventHandler.h"

namespace ungod
{
    class World;
    class Collider;
    class Application;

    /**
    * \ingroup Components
    * \brief A component that holds a script-defined behavior for an entity. The triggers for that behavior are
    * aronous gameplay- and input-events or update calls within fixed intervals. */
    class EntityBehaviorComponent : public Serializable<EntityBehaviorComponent>
    {
    friend class EntityBehaviorManager;
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
    friend class EntityBehaviorManager;
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
    class EntityBehaviorManager : public Serializable<EntityBehaviorManager>
    {
    public:
        enum EVENT_IDS { ON_INIT, ON_EXIT, ON_CREATION, ON_DESTRUCTION,
                         ON_STATIC_CONSTR, ON_STATIC_DESTR,
                         ON_SERIALIZED, ON_DESERIALIZED,
                         ON_COLLISION_ENTER, ON_COLLISION, ON_COLLISION_EXIT,
                         ON_MOUSE_ENTER, ON_MOUSE_EXIT, ON_MOUSE_CLICK, ON_MOUSE_RELEASED,
                         ON_UPDATE,
                         ON_BUTTON_DOWN, ON_BUTTON_RELEASED, ON_BUTTON_PRESSED,
                         ON_MOVEMENT_BEGIN, ON_MOVEMENT_END, ON_DIRECTION_CHANGED,
                         ON_ANIMATION_BEGIN, ON_ANIMATION_END,
                         ON_CUSTOM_EVENT,
                         ON_AI_GET_STATE, ON_AI_ACTION};

        static const std::vector<const char*> IDENTIFIERS;

        EntityBehaviorManager();

        EntityBehaviorManager(const script::SharedState& state, script::Environment main);

        /** \brief Initializes the manager. */
        void init(World& world, ungod::Application& app);

        /** \brief Updates the manager and may invoke onUpdate scripts of entities. */
        void update(const std::list<Entity>& entities, float delta);

        /** \brief Forwards the custom event to the script behaviors. */
        void handleCustomEvent(const CustomEvent& event);

        /** \brief Loads a new entity behavior script. The script is stored with the filename of the script as key (without path and file-extentions). */
        ScriptErrorCode loadBehaviorScript(const std::string& filepath);

        /** \brief Assigns a behavior to the given entity. The behavior must have been loaded beforehand. */
        void assignBehavior(Entity e, const std::string& name);
        void assignBehavior(Entity e, const std::string& name, script::Environment param);

        /** \brief Dissociates the current behavior from the entity, if any. */
        void dissociateBehavior(Entity e);

        /** \brief Sets the update interval of the given entity. */
        void setUpdateInterval(Entity e, float interval);
        void setUpdateInterval(EntityUpdateTimer& timer, float interval);

        /** \brief Returns a script variable defined in global environment. Call is safe. If the variable is not defined, an empty optional is returned. */
        template<typename T>
        Optional<T> getGlobalVariable(const std::string& name) const;

        /** \brief Accesses the underlying behavior manager. */
        BehaviorManager<>& getBehaviorManager();

        /** \brief Reloads all scripts and reassigns the behaviors to their corresponding entities.
        * Returns a list of all reloaded script files along with the error codes. */
        std::vector<std::pair<std::string, ScriptErrorCode>> reload(ungod::Application& app);

        /** \brief Reloads all scripts and reassigns the behaviors to their corresponding entities.
        * Returns a list of all reloaded script files along with the error codes. */
        std::vector<std::pair<std::string, ScriptErrorCode>> reload(ungod::Application& app, const script::SharedState& state, script::Environment main);

        /** \brief Registers an entity as a listener to an event type. */
        script::EventListenerLink addEventListener(Entity e, const std::string& eventType);

        /** \brief Registers an arbitrary callable as a listener to an event type. */
        script::EventListenerLink addEventListener(const script::ProtectedFunc& func, const std::string& eventType);

    private:
        BehaviorManager<> mBehaviorManager;
        World* mWorld;
        std::unordered_set<Entity> mMetaEntities; ///< stores entities with no transform components (at the time of script assignment) in order to process them
        script::EventHandler mEventHandler;

    private:
        void entityCreation(Entity e) const;
        void entityDestruction(Entity e) const;
        void entityCollisionEnter(Entity e1, Entity e2) const;
        void entityCollisionExit(Entity e1, Entity e2) const;
        void entityCollision(Entity e1, Entity e2, const sf::Vector2f& mdv, const Collider& c1, const Collider& c2) const;
        void buttonPressed(const std::string& binding) const;
        void buttonDown(const std::string& binding) const;
        void buttonReleased(const std::string& binding) const;

        void scriptInternalInit(ungod::Application& app);
        void scriptInternalReassign();
        void scriptInternalDissociate();

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


    template<typename T>
    Optional<T> EntityBehaviorManager::getGlobalVariable(const std::string& name) const
    {
        return mBehaviorManager.getGlobalEnvironment().get<Optional<T>>(name);
    }
}

#endif // ENTITY_BEHAVIOR_H
