#include "ungod/script/EntityBehavior.h"
#include "ungod/base/World.h"

namespace ungod
{
    bool EntityBehaviorComponent::valid() const
    {
        return mBehavior.get();
    }

    bool EntityBehaviorComponent::hasValidEnvironment() const
    {
        return valid() && static_cast<bool>(mBehavior->Behavior<>::getEnvironment());
    }

    bool EntityBehaviorComponent::hasValidStateEnvironment(const std::string& stateName) const
    {
        return valid() && static_cast<bool>(mBehavior->getEnvironment(stateName));
    }

    script::Environment EntityBehaviorComponent::getEnvironment() const
    {
        return mBehavior->Behavior<>::getEnvironment().value();
    }

    script::Environment EntityBehaviorComponent::getStateEnvironment(const std::string& stateName) const
    {
        return mBehavior->getEnvironment(stateName).value();
    }

    const std::string& EntityBehaviorComponent::getScriptName() const
    {
        return mBehavior->getScriptName();
    }




    EntityBehaviorManager::EntityBehaviorManager(Application& app)
        : mBehaviorManager(app.getScriptState(), app.getGlobalScriptEnv(), main, IDENTIFIERS, ON_CREATION, ON_INIT, ON_EXIT, ON_STATIC_CONSTR, ON_STATIC_DESTR)
    {
        mScriptStateChangedLink = app.onScriptStateChanged([this, &app]()
            {
                reload(app.getScriptState(), app.getGlobalScriptEnv());
            });
    }

    ScriptErrorCode EntityBehaviorManager::loadBehaviorScript(const std::string& filepath)
    {
        auto success = mBehaviorManager.loadBehavior(filepath);
        boost::filesystem::path p = filepath;
        detail::OptionalEnvironment staticEnv = mBehaviorManager.getStaticEnvironment(p.stem().string());
        if (staticEnv) staticEnv.value()["world"] = mWorld;
        return success;
    }



    BehaviorManager<>& EntityBehaviorManager::getBehaviorManager()
    {
        return mBehaviorManager;
    }

    std::vector<std::pair<std::string, ScriptErrorCode>> EntityBehaviorManager::reload(const script::SharedState& state, script::Environment main)
    {
        ScriptQueue toReload;
        mDissociateSignal(toReload);
        auto err = mBehaviorManager.reload(state, main);
        mReloadSignal(toReload);
        return err;
    }

    owls::SignalLink<void, const ScriptQueue&> EntityBehaviorManager::onReloadSignal(const std::function<void(const ScriptQueue&)>& callback)
    {
        return mReloadSignal.connect(callback);
    }

    owls::SignalLink<void, ScriptQueue&> EntityBehaviorManager::onDissociateSignal(const std::function<void(ScriptQueue&)>& callback)
    {
        return mDissociateSignal.connect(callback);
    }

    EntityBehaviorManager::~EntityBehaviorManager()
    {
        mScriptStateChangedLink.disconnect();
    }



    EntityBehaviorHandler::EntityBehaviorHandler()
        : {}

    void EntityBehaviorHandler::init(World& world)
    {
        mWorld = &world;

        mDissociateLink = world.getBehaviorHandler().onDissociateSignal([this, &world](ScriptQueues& toReload) { scriptInternalDissociate(toReload, world); });
        mReloadLink = world.getBehaviorHandler().onReloadSignal([this](const ScriptQueues& toReload) { scriptInternalReload(toReload); });

        world.onEntityCreation([this](Entity e) { entityCreation(e); });
        world.onEntityDestruction([this](Entity e) { entityDestruction(e); });
        world.onEntitySerialized([this](Entity e, MetaNode serializer, SerializationContext& context) { callbackInvoker(ON_SERIALIZED, e, serializer, context); });
        world.onEntityDeserialized([this](Entity e, MetaNode deserializer, DeserializationContext& context) { callbackInvoker(ON_DESERIALIZED, e, deserializer, context); });

        world.getState()->getApp().getInputManager().onDown([this](const std::string& binding) { buttonDown(binding); });
        world.getState()->getApp().getInputManager().onReleased([this](const std::string& binding) { buttonReleased(binding); });
        world.getState()->getApp().getInputManager().onPressed([this](const std::string& binding) { buttonPressed(binding); });

        world.getInputEventHandler().onMouseClick([this](Entity e) { callbackInvoker(ON_MOUSE_CLICK, e); });
        world.getInputEventHandler().onMouseEnter([this](Entity e) { callbackInvoker(ON_MOUSE_ENTER, e); });
        world.getInputEventHandler().onMouseExit([this](Entity e) { callbackInvoker(ON_MOUSE_EXIT, e); });
        world.getInputEventHandler().onMouseReleased([this](Entity e) { callbackInvoker(ON_MOUSE_RELEASED, e); });

        world.getMovementHandler().onBeginMovement([this](Entity e, const sf::Vector2f& vel) { callbackInvoker(ON_MOVEMENT_BEGIN, e, vel); });
        world.getMovementHandler().onEndMovement([this](Entity e) { callbackInvoker(ON_MOVEMENT_END, e); });
        world.getMovementHandler().onDirectionChanged([this](Entity e, MovementComponent::Direction old, MovementComponent::Direction current) { callbackInvoker(ON_DIRECTION_CHANGED, e, old, current); });

        world.getVisualsHandler().onAnimationStart([this](Entity e, const std::string& key) { callbackInvoker(ON_ANIMATION_BEGIN, e, key); });
        world.getVisualsHandler().onAnimationFrame([this](Entity e, const std::string& key, int frameIndex) { callbackInvoker(ON_ANIMATION_FRAME, e, key, frameIndex); });
        world.getVisualsHandler().onAnimationStop([this](Entity e, const std::string& key) { callbackInvoker(ON_ANIMATION_END, e, key); });

        world.getSemanticsCollisionHandler().onBeginCollision([this](Entity e1, Entity e2) { entityCollisionEnter(e1, e2); });
        world.getSemanticsCollisionHandler().onCollision([this](Entity e1, Entity e2, const sf::Vector2f& mdv, const Collider& c1, const Collider& c2)
            { entityCollision(e1, e2, mdv, c1, c2); });
        world.getSemanticsCollisionHandler().onEndCollision([this](Entity e1, Entity e2) { entityCollisionExit(e1, e2); });
    }

    void EntityBehaviorHandler::update(const std::list<Entity>& entities, float delta)
    {
        dom::Utility<Entity>::iterate<EntityBehaviorComponent, EntityUpdateTimer>(entities,
            [delta, this](Entity e, EntityBehaviorComponent& behavior, EntityUpdateTimer& timer)
            {
                if (timer.mTimer.getElapsedTime().asMilliseconds() >= timer.mInterval && behavior.valid())
                {
                    behavior.mBehavior->execute(ON_UPDATE, timer.mInterval);
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
                    behavior.mBehavior->execute(ON_UPDATE, timer.mInterval);
                    timer.mTimer.restart();
                }
            }
        }

        mEventHandler.dispatchDelayed();
    }

    void EntityBehaviorHandler::handleCustomEvent(const CustomEvent& event)
    {
        mEventHandler.handleCustomEvent(event);
    }

    void EntityBehaviorHandler::assignBehavior(Entity e, const std::string& key)
    {
        //assign a previously loaded behavior
        script::Environment instance = mBehaviorManager.makeInstanceEnvironment(e.getID());
        instance["entity"] = e;
        instance["world"] = mWorld;
        e.modify<EntityBehaviorComponent>().mBehavior = mBehaviorManager.makeStateBehavior(key, instance);

        //track entity if it has no transform
        if (!e.has<TransformComponent>())
            mMetaEntities.emplace(e);
    }

    void EntityBehaviorHandler::assignBehavior(Entity e, const std::string& key, script::Environment param)
    {
        //assign a previously loaded behavior
        script::Environment instance = mBehaviorManager.makeInstanceEnvironment(e.getID());
        instance["entity"] = e;
        instance["world"] = mWorld;
        embedEnv(param, instance);
        e.modify<EntityBehaviorComponent>().mBehavior = mBehaviorManager.makeStateBehavior(key, instance);

        //track entity if it has no transform
        if (!e.has<TransformComponent>())
            mMetaEntities.emplace(e);
    }

    void EntityBehaviorHandler::dissociateBehavior(Entity e)
    {
        e.modify<EntityBehaviorComponent>().mBehavior = StateBehaviorPtr<>();
    }

    void EntityBehaviorHandler::setUpdateInterval(Entity e, float interval)
    {
        setUpdateInterval(e.modify<EntityUpdateTimer>(), interval);
    }

    void EntityBehaviorHandler::setUpdateInterval(EntityUpdateTimer& timer, float interval)
    {
        timer.mInterval = interval;
    }

    script::EventListenerLink EntityBehaviorHandler::addEventListener(Entity e, const std::string& eventType)
    {
        return mEventHandler.addListener([this, e](const CustomEvent& evt)
            {
                auto& behavior = e.modify<EntityBehaviorComponent>();
                if (behavior.valid())
                {
                    behavior.mBehavior->execute(ON_CUSTOM_EVENT, evt);
                }
            }, eventType);
    }

    script::EventListenerLink EntityBehaviorHandler::addEventListener(const script::ProtectedFunc& func, const std::string& eventType)
    {
        return mEventHandler.addListener([func](const CustomEvent& evt) { func(evt);  }, eventType);
    }

    EntityBehaviorHandler::~EntityBehaviorHandler()
    {
        mDissociateLink.disconnect();
        mReloadLink.disconnect();
    }

    void EntityBehaviorHandler::entityCreation(Entity e) const
    {
        //nothing todo here anymore?
        //creation callback is invoked by the assignBehavior call automatically now...
    }

    void EntityBehaviorHandler::entityDestruction(Entity e) const
    {
        if (e.has<EntityBehaviorComponent>() && e.modify<EntityBehaviorComponent>().valid())
            e.modify<EntityBehaviorComponent>().mBehavior->execute(ON_DESTRUCTION);
    }

    void EntityBehaviorHandler::entityCollisionEnter(Entity e1, Entity e2) const
    {
        if (e1.has<EntityBehaviorComponent>() && e1.modify<EntityBehaviorComponent>().valid())
            e1.modify<EntityBehaviorComponent>().mBehavior->execute(ON_COLLISION_ENTER, e2);
    }

    void EntityBehaviorHandler::entityCollisionExit(Entity e1, Entity e2) const
    {
        if (e1.has<EntityBehaviorComponent>() && e1.modify<EntityBehaviorComponent>().valid())
            e1.modify<EntityBehaviorComponent>().mBehavior->execute(ON_COLLISION_EXIT, e2);
    }

    void EntityBehaviorHandler::entityCollision(Entity e1, Entity e2, const sf::Vector2f& mdv, const Collider& c1, const Collider& c2) const
    {
        if (e1.has<EntityBehaviorComponent>() && e1.modify<EntityBehaviorComponent>().valid())
            e1.modify<EntityBehaviorComponent>().mBehavior->execute(ON_COLLISION, e2);
    }

    void EntityBehaviorHandler::buttonPressed(const std::string& binding) const
    {
        dom::Utility<Entity>::iterate<EntityBehaviorComponent>(mWorld->getEntitiesInUpdateRange().getList(),
            [binding, this](Entity e, EntityBehaviorComponent& behavior)
            {
                if (behavior.valid())
                {
                    behavior.mBehavior->execute(ON_BUTTON_PRESSED, binding);
                }
            });

        for (const auto& e : mMetaEntities)
        {
            EntityBehaviorComponent& behavior = e.modify<EntityBehaviorComponent>();
            if (behavior.valid())
            {
                behavior.mBehavior->execute(ON_BUTTON_PRESSED, binding);
            }
        }
    }

    void EntityBehaviorHandler::buttonDown(const std::string& binding) const
    {
        dom::Utility<Entity>::iterate<EntityBehaviorComponent>(mWorld->getEntitiesInUpdateRange().getList(),
            [binding, this](Entity e, EntityBehaviorComponent& behavior)
            {
                if (behavior.valid())
                {
                    behavior.mBehavior->execute(ON_BUTTON_DOWN, binding);
                }
            });

        for (const auto& e : mMetaEntities)
        {
            EntityBehaviorComponent& behavior = e.modify<EntityBehaviorComponent>();
            if (behavior.valid())
            {
                behavior.mBehavior->execute(ON_BUTTON_DOWN, binding);
            }
        }
    }

    void EntityBehaviorHandler::buttonReleased(const std::string& binding) const
    {
        dom::Utility<Entity>::iterate<EntityBehaviorComponent>(mWorld->getEntitiesInUpdateRange().getList(),
            [binding, this](Entity e, EntityBehaviorComponent& behavior)
            {
                if (behavior.valid())
                {
                    behavior.mBehavior->execute(ON_BUTTON_RELEASED, binding);
                }
            });

        for (const auto& e : mMetaEntities)
        {
            EntityBehaviorComponent& behavior = e.modify<EntityBehaviorComponent>();
            if (behavior.valid())
            {
                behavior.mBehavior->execute(ON_BUTTON_RELEASED, binding);
            }
        }
    }

    void EntityBehaviorHandler::scriptInternalDissociate(ScriptQueues& toReload)
    {
        auto q = toReload.emplace(this, {});
        //reassign for entities in the world
        quad::PullResult<Entity> res;
        mWorld->getQuadTree().getContent(res);
        dom::Utility<Entity>::iterate<EntityBehaviorComponent>(res.getList(),
            [this](Entity e, EntityBehaviorComponent& behavior)
            {
                if (behavior.valid())
                {
                    q.first.second.emplace_back(e, behavior.getScriptName());
                    dissociateBehavior(e);
                }
            });

        for (const auto& e : mMetaEntities)
        {
            if (behavior.valid())
            {
                q.first.second.emplace_back(e, behavior.getScriptName());
                dissociateBehavior(e);
            }
        }
    }

    void EntityBehaviorHandler::scriptInternalReload(const ScriptQueues& toReload)
    {
        auto q = toReload.find(this);
        while (!q.first->second.empty())
        {
            assignBehavior(q.first->front().first, q.first->front().second);
            q.first->pop();
        }
    }
}
