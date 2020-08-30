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
*
* A short tutorial on how to write Entity-script files. Basically you can define an arbitrary subset of the set of
* possible methods. You just need to know which methods exists, when they are called and what parameters they get.
*
*
* Vocabulary:
* static -- the static container of the script, equal for each instance, also contains the callback methods
* entity --  a table that can be used to store data for the particular entity instance that is shared between all method calls
* entity provides some additional members:
* entity.handle : the underlying ungod entity handle
* entity.states : an object to control things like state switches for the entity (if StateBehavior)
* entity.node : the world graph node the entitiy lives in
*
*
* A note about the handle inside the entity environment: It is NOT safe to store references to this handle or to any component 
* accessable through the handle directly in script. They are designed for spontaneous access only. Handles can change when entity contents
* are moved in memory, e.g. when an entity changes nodes. However, the environment is guaranteed to persist for the runtime of the game 
* (and can therefore be stored). Also it is guaranteed that it always contains a valid handle to the underlying entity.
*
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
* onAnimationFrame(static, instance, key, frameIndex)
* -- This method is invoked when a running animation changes its frame (also called on first frame)
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

#include <queue>
#include <SFML/System/Vector2.hpp>
#include "ungod/script/Behavior.h"
#include "owls/Signal.h"

namespace ungod
{
    class Entity;
    class World;
    class Collider;
    class Application;
    class EntityBehaviorHandler;


    enum SCRIPT_EVENT_IDS {
        ON_INIT, ON_EXIT, ON_CREATION, ON_DESTRUCTION,
        ON_STATIC_CONSTR, ON_STATIC_DESTR,
        ON_SERIALIZED, ON_DESERIALIZED,
        ON_COLLISION_ENTER, ON_COLLISION, ON_COLLISION_EXIT,
        ON_MOUSE_ENTER, ON_MOUSE_EXIT, ON_MOUSE_CLICK, ON_MOUSE_RELEASED,
        ON_UPDATE,
        ON_BUTTON_DOWN, ON_BUTTON_RELEASED, ON_BUTTON_PRESSED,
        ON_MOVEMENT_BEGIN, ON_MOVEMENT_END, ON_DIRECTION_CHANGED,
        ON_ANIMATION_BEGIN, ON_ANIMATION_FRAME, ON_ANIMATION_END,
        ON_CUSTOM_EVENT,
        ON_AI_GET_STATE, ON_AI_ACTION,
        ON_ENTERED_NEW_NODE
    };

    using ScriptQueues = std::unordered_map< EntityBehaviorHandler*, std::queue<std::pair<Entity, std::string>> >;

    class EntityBehaviorManager
    {
    friend class EntityBehaviorHandler;
    public:

        static const std::vector<const char*> IDENTIFIERS;

        EntityBehaviorManager(Application& app);

        /** \brief Loads a new entity behavior script. The script is stored with the filename of the script as key (without path and file-extentions). */
        ScriptErrorCode loadBehaviorScript(const std::string& filepath);

        /** \brief Returns a script variable defined in global environment. Call is safe. If the variable is not defined, an empty optional is returned. */
        template<typename T>
        Optional<T> getGlobalVariable(const std::string& name) const;

        /** \brief Accesses the underlying behavior manager. */
        BehaviorManager<>& getBehaviorManager();

        /** \brief Reloads all scripts and reassigns the behaviors to their corresponding entities.
        * Returns a list of all reloaded script files along with the error codes. */
        std::vector<std::pair<std::string, ScriptErrorCode>> reload(const script::SharedState& state, script::Environment main);

        ~EntityBehaviorManager();

    private:
        BehaviorManager<> mBehaviorManager;
        owls::Signal<ScriptQueues&> mReloadSignal;
        owls::Signal<ScriptQueues&> mDissociateSignal;
        owls::SignalLink<void> mScriptStateChangedLink;

    private:
        owls::SignalLink<void, ScriptQueues&> onReloadSignal(const std::function<void(ScriptQueues&)>& callback);
        owls::SignalLink<void, ScriptQueues&> onDissociateSignal(const std::function<void(ScriptQueues&)>& callback);
    };



    template<typename T>
    Optional<T> EntityBehaviorManager::getGlobalVariable(const std::string& name) const
    {
        return mBehaviorManager.getGlobalEnvironment().get<Optional<T>>(name);
    }
}

#endif // ENTITY_BEHAVIOR_H
