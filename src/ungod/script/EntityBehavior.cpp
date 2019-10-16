#include "ungod/script/EntityBehavior.h"
#include "ungod/base/World.h"
#include "ungod/script/Registration.h"

namespace ungod
{
    bool EntityBehaviorComponent::valid() const
    {
        return mBehavior.get();
    }

    bool EntityBehaviorComponent::hasValidEnvironment() const
    {
        return valid() && static_cast<bool>(mBehavior->Behavior<StateBehavior<Entity, World*, script::Environment>*, Entity, World*, script::Environment>::getEnvironment());
    }

    bool EntityBehaviorComponent::hasValidStateEnvironment(const std::string& stateName) const
    {
        return valid() && static_cast<bool>(mBehavior->getEnvironment(stateName));
    }

    script::Environment EntityBehaviorComponent::getEnvironment() const
    {
        return mBehavior->Behavior<StateBehavior<Entity, World*, script::Environment>*, Entity, World*, script::Environment>::getEnvironment().value();
    }

    script::Environment EntityBehaviorComponent::getStateEnvironment(const std::string& stateName) const
    {
        return mBehavior->getEnvironment(stateName).value();
    }

    const std::string& EntityBehaviorComponent::getScriptName() const
    {
        return mBehavior->getScriptName();
    }


    const std::vector<const char*> EntityBehaviorManager::IDENTIFIERS = { "onInit", "onExit", "onCreation", "onDestruction",
                                                                          "onStaticConstr", "onStaticDestr",
                                                                          "onSerialize", "onDeserialize",
                                                                          "onCollisionEnter", "onCollision", "onCollisionExit",
                                                                          "onMouseEnter", "onMouseExit", "onMouseClick", "onMouseReleased",
                                                                          "onUpdate",
                                                                          "onButtonDown", "onButtonReleased", "onButtonPressed",
                                                                          "onMovementBegin", "onMovementEnd", "onDirectionChanged",
                                                                          "onAnimationBegin", "onAnimationEnd",
                                                                          "onCustomEvent" };

    EntityBehaviorManager::EntityBehaviorManager()
        : mBehaviorManager(IDENTIFIERS, ON_CREATION, ON_INIT, ON_EXIT, ON_STATIC_CONSTR, ON_STATIC_DESTR), mWorld(nullptr) {}

    EntityBehaviorManager::EntityBehaviorManager(const script::SharedState& state, script::Environment main)
        : mBehaviorManager(state, main, IDENTIFIERS, ON_CREATION, ON_INIT, ON_EXIT, ON_STATIC_CONSTR, ON_STATIC_DESTR), mWorld(nullptr) {}

    void EntityBehaviorManager::init(World& world, script::Environment global, ungod::Application& app)
    {
        mWorld = &world;

        scriptInternalInit(global, app);

        mWorld->onEntityCreation([this] (Entity e) { entityCreation(e); });
        mWorld->onEntityDestruction([this] (Entity e) { entityDestruction(e); });
        mWorld->onEntitySerialized([this] (Entity e, MetaNode serializer, SerializationContext& context) { callbackInvoker(ON_SERIALIZED, e, serializer, context, mWorld, mGlobalEnv); });
        mWorld->onEntityDeserialized([this] (Entity e, MetaNode deserializer, DeserializationContext& context) { callbackInvoker(ON_DESERIALIZED, e, deserializer, context, mWorld, mGlobalEnv); });

        mWorld->getInputManager().onDown([this] (const std::string& binding) { buttonDown(binding); });
        mWorld->getInputManager().onReleased([this] (const std::string& binding) { buttonReleased(binding); });
        mWorld->getInputManager().onPressed([this] (const std::string& binding) { buttonPressed(binding); });
        mWorld->getInputManager().onMouseClick([this] (Entity e) { callbackInvoker(ON_MOUSE_CLICK, e, mWorld, mGlobalEnv); });
        mWorld->getInputManager().onMouseEnter([this] (Entity e) { callbackInvoker(ON_MOUSE_ENTER, e, mWorld, mGlobalEnv); });
        mWorld->getInputManager().onMouseExit([this] (Entity e) { callbackInvoker(ON_MOUSE_EXIT, e, mWorld, mGlobalEnv); });
        mWorld->getInputManager().onMouseReleased([this] (Entity e) { callbackInvoker(ON_MOUSE_RELEASED, e, mWorld, mGlobalEnv); });

        mWorld->getMovementManager().onBeginMovement([this] (Entity e, const sf::Vector2f& vel) { callbackInvoker(ON_MOVEMENT_BEGIN, e, vel, mWorld, mGlobalEnv); });
        mWorld->getMovementManager().onEndMovement([this] (Entity e) { callbackInvoker(ON_MOVEMENT_END, e, mWorld, mGlobalEnv); });
        mWorld->getMovementManager().onDirectionChanged([this] (Entity e, MovementComponent::Direction old, MovementComponent::Direction current) { callbackInvoker(ON_DIRECTION_CHANGED, e, old, current, mWorld, mGlobalEnv); });

        mWorld->getVisualsManager().onAnimationStart([this] (Entity e, const std::string& key) { callbackInvoker(ON_ANIMATION_BEGIN, e, key, mWorld, mGlobalEnv); });
        mWorld->getVisualsManager().onAnimationStop([this] (Entity e, const std::string& key) { callbackInvoker(ON_ANIMATION_END, e, key, mWorld, mGlobalEnv); });

        mWorld->getSemanticsCollisionManager().onBeginCollision([this] (Entity e1, Entity e2) { entityCollisionEnter(e1, e2); });
        mWorld->getSemanticsCollisionManager().onCollision([this] (Entity e1, Entity e2, const sf::Vector2f& mdv, const Collider& c1, const Collider& c2)
                                                                { entityCollision(e1, e2, mdv, c1, c2); });
        mWorld->getSemanticsCollisionManager().onEndCollision([this] (Entity e1, Entity e2) { entityCollisionExit(e1, e2); });
    }


    void EntityBehaviorManager::update(const std::list<Entity>& entities, float delta) const
    {
        dom::Utility<Entity>::iterate<EntityBehaviorComponent, EntityUpdateTimer>(entities,
          [delta, this] (Entity e, EntityBehaviorComponent& behavior, EntityUpdateTimer& timer)
          {
              if (timer.mTimer.getElapsedTime().asMilliseconds() >= timer.mInterval && behavior.valid())
              {
                  behavior.mBehavior->execute(ON_UPDATE, e, timer.mInterval, mWorld, mGlobalEnv);
                  timer.mTimer.restart();
              }
          });

        for (const auto& e : mMetaEntities)
        {
            if (e.has<EntityUpdateTimer>())
            {
              EntityUpdateTimer& timer = e.modify<EntityUpdateTimer>();
              EntityBehaviorComponent& behavior = e.modify<EntityBehaviorComponent>();
              if (timer.mTimer.getElapsedTime().asMilliseconds() >= timer.mInterval && behavior.valid())
              {
                  behavior.mBehavior->execute(ON_UPDATE, e, timer.mInterval, mWorld, mGlobalEnv);
                  timer.mTimer.restart();
              }
            }
        }
    }

    void EntityBehaviorManager::handleCustomEvent(const CustomEvent& event) const
    {
        dom::Utility<Entity>::iterate<EntityBehaviorComponent>(mWorld->getEntitiesInUpdateRange().getList(),
          [&event, this] (Entity e, EntityBehaviorComponent& behavior)
          {
              if (behavior.valid())
              {
                  behavior.mBehavior->execute(ON_CUSTOM_EVENT, e, event, mWorld, mGlobalEnv);
              }
          });

        for (const auto& e : mMetaEntities)
        {
          EntityBehaviorComponent& behavior = e.modify<EntityBehaviorComponent>();
          if (behavior.valid())
          {
              behavior.mBehavior->execute(ON_CUSTOM_EVENT, e, event, mWorld, mGlobalEnv);
          }
        }
    }

    ScriptErrorCode EntityBehaviorManager::loadBehaviorScript(const std::string& filepath)
    {
        return mBehaviorManager.loadBehavior(filepath);
    }

    void EntityBehaviorManager::assignBehavior(Entity e, const std::string& key)
    {
        //assign a previously loaded behavior
        e.modify<EntityBehaviorComponent>().mBehavior = mBehaviorManager.makeStateBehavior(key, mBehaviorManager.makeInstanceEnvironment(e.getID()), e, mWorld, mGlobalEnv);

        //track entity if it has no transform
        if (!e.has<TransformComponent>())
            mMetaEntities.emplace(e);
    }

    void EntityBehaviorManager::assignBehavior(Entity e, const std::string& key, script::Environment param)
    {
        //assign a previously loaded behavior
        script::Environment instance = mBehaviorManager.makeInstanceEnvironment(e.getID());
        embedEnv(param, instance);
        e.modify<EntityBehaviorComponent>().mBehavior = mBehaviorManager.makeStateBehavior(key, instance, e, mWorld, mGlobalEnv);

        //track entity if it has no transform
        if (!e.has<TransformComponent>())
            mMetaEntities.emplace(e);
    }

    void EntityBehaviorManager::dissociateBehavior(Entity e)
    {
        e.modify<EntityBehaviorComponent>().mBehavior = StateBehaviorPtr<Entity, World*, script::Environment>();
    }

    void EntityBehaviorManager::setUpdateInterval(Entity e, float interval)
    {
        setUpdateInterval(e.modify<EntityUpdateTimer>(), interval);
    }

    void EntityBehaviorManager::setUpdateInterval(EntityUpdateTimer& timer, float interval)
    {
        timer.mInterval = interval;
    }

    void EntityBehaviorManager::entityCreation(Entity e) const
    {
        //nothing todo here anymore?
        //creation callback is invoked by the assignBehavior call automatically now...
    }

    void EntityBehaviorManager::entityDestruction(Entity e) const
    {
        if (e.has<EntityBehaviorComponent>() && e.modify<EntityBehaviorComponent>().valid())
            e.modify<EntityBehaviorComponent>().mBehavior->execute(ON_DESTRUCTION, e, mWorld, mGlobalEnv);
    }

    void EntityBehaviorManager::entityCollisionEnter(Entity e1, Entity e2) const
    {
        if (e1.has<EntityBehaviorComponent>() && e1.modify<EntityBehaviorComponent>().valid())
            e1.modify<EntityBehaviorComponent>().mBehavior->execute(ON_COLLISION_ENTER, e1, e2, mWorld, mGlobalEnv);
    }

    void EntityBehaviorManager::entityCollisionExit(Entity e1, Entity e2) const
    {
        if (e1.has<EntityBehaviorComponent>() && e1.modify<EntityBehaviorComponent>().valid())
            e1.modify<EntityBehaviorComponent>().mBehavior->execute(ON_COLLISION_EXIT, e1, e2, mWorld, mGlobalEnv);
    }

    void EntityBehaviorManager::entityCollision(Entity e1, Entity e2, const sf::Vector2f& mdv, const Collider& c1, const Collider& c2) const
    {
        if (e1.has<EntityBehaviorComponent>() && e1.modify<EntityBehaviorComponent>().valid())
            e1.modify<EntityBehaviorComponent>().mBehavior->execute(ON_COLLISION, e1, e2, mWorld, mGlobalEnv);
    }

    void EntityBehaviorManager::buttonPressed(const std::string& binding) const
    {
        dom::Utility<Entity>::iterate<EntityBehaviorComponent>(mWorld->getEntitiesInUpdateRange().getList(),
          [binding, this] (Entity e, EntityBehaviorComponent& behavior)
          {
              if (behavior.valid())
              {
                  behavior.mBehavior->execute(ON_BUTTON_PRESSED, e, binding, mWorld, mGlobalEnv);
              }
          });

        for (const auto& e : mMetaEntities)
        {
          EntityBehaviorComponent& behavior = e.modify<EntityBehaviorComponent>();
          if (behavior.valid())
          {
              behavior.mBehavior->execute(ON_BUTTON_PRESSED, e, binding, mWorld, mGlobalEnv);
          }
        }
    }

    void EntityBehaviorManager::buttonDown(const std::string& binding) const
    {
        dom::Utility<Entity>::iterate<EntityBehaviorComponent>(mWorld->getEntitiesInUpdateRange().getList(),
          [binding, this] (Entity e, EntityBehaviorComponent& behavior)
          {
              if (behavior.valid())
              {
                  behavior.mBehavior->execute(ON_BUTTON_DOWN, e, binding, mWorld, mGlobalEnv);
              }
          });

        for (const auto& e : mMetaEntities)
        {
          EntityBehaviorComponent& behavior = e.modify<EntityBehaviorComponent>();
          if (behavior.valid())
          {
              behavior.mBehavior->execute(ON_BUTTON_DOWN, e, binding, mWorld, mGlobalEnv);
          }
        }
    }

    void EntityBehaviorManager::buttonReleased(const std::string& binding) const
    {
        dom::Utility<Entity>::iterate<EntityBehaviorComponent>(mWorld->getEntitiesInUpdateRange().getList(),
          [binding, this] (Entity e, EntityBehaviorComponent& behavior)
          {
              if (behavior.valid())
              {
                  behavior.mBehavior->execute(ON_BUTTON_RELEASED, e, binding, mWorld, mGlobalEnv);
              }
          });

        for (const auto& e : mMetaEntities)
        {
          EntityBehaviorComponent& behavior = e.modify<EntityBehaviorComponent>();
          if (behavior.valid())
          {
              behavior.mBehavior->execute(ON_BUTTON_RELEASED, e, binding, mWorld, mGlobalEnv);
          }
        }
    }

    void EntityBehaviorManager::scriptInternalInit(script::Environment global, ungod::Application& app)
    {
        //register script functionality
        scriptRegistration::registerAssets(mBehaviorManager);
        scriptRegistration::registerWorld(mBehaviorManager, app);
        scriptRegistration::registerGameState(mBehaviorManager);
        scriptRegistration::registerApplication(mBehaviorManager, app);
        scriptRegistration::registerWater(mBehaviorManager, app);
        scriptRegistration::registerTileMap(mBehaviorManager, app);

        mGlobalEnv = global;
    }

    void EntityBehaviorManager::scriptInternalDissociate()
    {
        //reassign for entities in the world
        quad::PullResult<Entity> res;
        mWorld->getQuadTree().getContent(res);
        dom::Utility<Entity>::iterate<EntityBehaviorComponent>(res.getList(),
          [this] (Entity e, EntityBehaviorComponent& behavior)
          {
                dissociateBehavior(e);
          });

        for (const auto& e : mMetaEntities)
        {
            dissociateBehavior(e);
        }
    }

    void EntityBehaviorManager::scriptInternalReassign()
    {
        //reassign for entities in the world
        quad::PullResult<Entity> res;
        mWorld->getQuadTree().getContent(res);
        dom::Utility<Entity>::iterate<EntityBehaviorComponent>(res.getList(),
          [this] (Entity e, EntityBehaviorComponent& behavior)
          {
              if (behavior.valid())
              {
                  assignBehavior(e, behavior.getScriptName());
              }
          });

        for (const auto& e : mMetaEntities)
        {
           EntityBehaviorComponent& behavior = e.modify<EntityBehaviorComponent>();
           if (behavior.valid())
           {
              assignBehavior(e, behavior.getScriptName());
           }
        }
    }

    BehaviorManager<Entity, World*, script::Environment>& EntityBehaviorManager::getBehaviorManager()
    {
        return mBehaviorManager;
    }

    std::vector<std::pair<std::string, ScriptErrorCode>> EntityBehaviorManager::reload(ungod::Application& app)
    {
        scriptInternalDissociate();

        //reload the scripts
        auto err = mBehaviorManager.reload();

        //since the behavior now has a new state, initialize it properly
        scriptInternalInit(mBehaviorManager.createEnvironment(), app);

        //reassign
        scriptInternalReassign();

        return err;
    }

    std::vector<std::pair<std::string, ScriptErrorCode>> EntityBehaviorManager::reload(script::Environment global, ungod::Application& app, const script::SharedState& state, script::Environment main)
    {
        scriptInternalDissociate();

        //reload the scripts
        auto err = mBehaviorManager.reload(state, main);

        //since the behavior now has a new state, initialize it properly
        scriptInternalInit(global, app);

        //reassign
        scriptInternalReassign();

        return err;
    }
}
