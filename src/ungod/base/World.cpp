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

#include "ungod/base/World.h"
#include "ungod/audio/VolumeSettings.h"
#include "ungod/serialization/CollisionSerial.h"
#include "ungod/serialization/SerialMultiComponent.h"
#include "ungod/serialization/SerialComponents.h"
#include "ungod/application/Application.h"
#include "ungod/application/ScriptedGameState.h"
#include "ungod/content/EntityTypes.h"

namespace ungod
{
    World::World() :
        mMaster(nullptr),
        mQuadTree(),
        mEntityBehaviorHandler(),
        mTransformHandler(mQuadTree),
        mInputEventHandler(mQuadTree, this),
        mMovementHandler(mQuadTree, mTransformHandler),
        mSteeringHandler(),
        mPathPlanner(),
        mVisualsHandler(),
        mMovementCollisionHandler(mQuadTree),
        mSemanticsCollisionHandler(mQuadTree),
        mMovementRigidbodyHandler(),
        mSemanticsRigidbodyHandler(),
        mListener(),
        mMusicEmitterMixer(),
        mSoundHandler(),
        mLightHandler(),
        mTileMapHandler(),
        WaterHandler(),
        mParentChildManager(*this),
        mRenderLight(true)
    {
    }


    void init(ScriptedGameState& master)
    {
        mMaster = &master;
        mEntityBehaviorHandler.init(*this);
        mSteeringHandler.init(master.getSteeringManager());
        mLightHandler.init(master.getLightManager());
        mListener = std::unique_ptr<AudioListener>(new CameraListener(master.getCamera(), world*this));
        mMusicEmitterMixer.init(mListener.get());
        mSoundHandler.init(master.getApp().getSoundProfileManager(), mListener.get());
        mTileMapHandler.init(*this);
        mWaterHandler.init(*this);

        //set up signal callbacks
        world->getTransformManager().onPositionChanged([this](Entity e, const sf::Vector2f& position) { mScriptCallbacks.execute(ON_POSITION_CHANGED, this, e, position); });
        world->getTransformManager().onSizeChanged([this](Entity e, const sf::Vector2f& size) { mScriptCallbacks.execute(ON_SIZE_CHANGED, this, e, size); });
        world->getMovementManager().onBeginMovement([this](Entity e, const sf::Vector2f& vel) { mScriptCallbacks.execute(ON_BEGIN_MOVEMENT, this, e, vel); });
        world->getMovementManager().onEndMovement([this](Entity e) { mScriptCallbacks.execute(ON_END_MOVEMENT, this, e); });
        world->getMovementManager().onDirectionChanged([this](Entity e, MovementComponent::Direction oldDirec, MovementComponent::Direction newDirec) { mScriptCallbacks.execute(ON_DIRECTION_CHANGED, this, e, oldDirec, newDirec); });
        world->getVisualsManager().onVisibilityChanged([this](Entity e, bool visibility) { mScriptCallbacks.execute(ON_VISIBILITY_CHANGED, this, e, visibility); });
        world->getVisualsManager().onAnimationStart([this](Entity e, const std::string& key) { mScriptCallbacks.execute(ON_ANIMATION_START, this, e, key); });
        world->getVisualsManager().onAnimationStop([this](Entity e, const std::string& key) { mScriptCallbacks.execute(ON_ANIMATION_STOP, this, e, key); });
        world->getMovementCollisionManager().onCollision([this](Entity e1, Entity e2, const sf::Vector2f& mdv, const Collider& c1, const Collider& c2) { mScriptCallbacks.execute(ON_MOV_COLLISION, this, e1, e2, mdv, c1, c2); });
        world->getMovementCollisionManager().onBeginCollision([this](Entity e1, Entity e2) { mScriptCallbacks.execute(ON_BEGIN_MOV_COLLISION, this, e1, e2); });
        world->getMovementCollisionManager().onEndCollision([this](Entity e1, Entity e2) { mScriptCallbacks.execute(ON_END_MOVE_COLLISION, this, e1, e2); });
        world->getSemanticsCollisionManager().onCollision([this](Entity e1, Entity e2, const sf::Vector2f& mdv, const Collider& c1, const Collider& c2) { mScriptCallbacks.execute(ON_SEM_COLLISION, this, e1, e2, mdv, c1, c2); });
        world->getSemanticsCollisionManager().onBeginCollision([this](Entity e1, Entity e2) { mScriptCallbacks.execute(ON_BEGIN_SEM_COLLISION, this, e1, e2); });
        world->getSemanticsCollisionManager().onEndCollision([this](Entity e1, Entity e2) { mScriptCallbacks.execute(ON_END_SEM_COLLISION, this, e1, e2); });
        world->getInputManager().onPressed([this](const std::string& binding) { mScriptCallbacks.execute(ON_PRESSED, this, binding); });
        world->getInputManager().onDown([this](const std::string& binding) { mScriptCallbacks.execute(ON_DOWN, this, binding); });
        world->getInputManager().onReleased([this](const std::string& binding) { mScriptCallbacks.execute(ON_RELEASED, this, binding); });
        world->getInputManager().onMouseEnter([this](Entity e) { mScriptCallbacks.execute(ON_MOUSE_ENTER, this, e); });
        world->getInputManager().onMouseClick([this](Entity e) { mScriptCallbacks.execute(ON_MOUSE_CLICK, this, e); });
        world->getInputManager().onMouseExit([this](Entity e) { mScriptCallbacks.execute(ON_MOUSE_EXIT, this, e); });
        world->getInputManager().onMouseReleased([this](Entity e) { mScriptCallbacks.execute(ON_MOUSE_RELEASED, this, e); });


        //register instantiations for deserialization
        registerTypes(*this);


        //connect signals
        mVisualsManager.onContentsChanged( [this] (Entity e, const sf::FloatRect& rect)
                                          {
                                                mTransformManager.handleContentsChanged(e, rect);
                                          });
        mMovementRigidbodyManager.onContentsChanged( [this] (Entity e, const sf::FloatRect& rect)
                                          {
                                                mTransformManager.handleContentsChanged(e, rect);
                                          });
		mSemanticsRigidbodyManager.onContentsChanged([this](Entity e, const sf::FloatRect& rect)
											{
												mTransformManager.handleContentsChanged(e, rect);
											});
        mMovementRigidbodyManager.onContentRemoved( [this] (Entity e)
                                          {
                                                mTransformManager.handleContentsRemoved(e);
                                          });
		mSemanticsRigidbodyManager.onContentRemoved([this](Entity e)
											{
												mTransformManager.handleContentsRemoved(e);
											});
        mLightSystem.onContentsChanged( [this] (Entity e, const sf::FloatRect& rect)
                                          {
                                                mTransformManager.handleContentsChanged(e, rect);
                                          });
        mParticleSystemManager.onContentsChanged( [this] (Entity e, const sf::FloatRect& rect)
                                          {
                                                mTransformManager.handleContentsChanged(e, rect);
                                          });
        mMovementCollisionManager.onCollision( [this] (Entity e, Entity other, const sf::Vector2f& vec, const Collider& c1, const Collider& c2)
                                          {
                                                mMovementManager.handleCollision(e, other, vec, c1, c2);
                                          });
		mTileMapRenderer.onContentsChanged([this](Entity e, const sf::FloatRect& rect)
			{
				mTransformManager.handleContentsChanged(e, rect);
			});
        mTransformManager.onMoveContents( [this] (Entity e, const sf::Vector2f& vec)
                                          {
                                                mLightSystem.moveLights(e, vec);
                                                mLightSystem.moveLightColliders(e, vec);
												mMovementRigidbodyManager.moveColliders(e, vec);
												mSemanticsRigidbodyManager.moveColliders(e, vec);
                                                mVisualsManager.moveVisuals(e, vec);
												mTileMapRenderer.moveMaps(e, vec);
                                          });


        onComponentAdded<ParticleSystemComponent>( [this] (Entity e) { mParticleSystemManager.handleParticleSystemAdded(e); });


        //connect lower bounds methods
        mTransformManager.onLowerBoundRequest( [this] (Entity e) -> sf::Vector2f { return mVisualsManager.getLowerBound(e); });
        mTransformManager.onLowerBoundRequest( [this] (Entity e) -> sf::Vector2f { return mLightSystem.getLowerBound(e); });
        mTransformManager.onLowerBoundRequest( [this] (Entity e) -> sf::Vector2f { return mMovementRigidbodyManager.getLowerBound(e); });
		mTransformManager.onLowerBoundRequest([this](Entity e) -> sf::Vector2f { return mSemanticsRigidbodyManager.getLowerBound(e); });
        mTransformManager.onLowerBoundRequest( [this] (Entity e) -> sf::Vector2f { return mParticleSystemManager.getLowerBound(e); });

        //connect upper bounds methods
        mTransformManager.onUpperBoundRequest( [this] (Entity e) -> sf::Vector2f { return mVisualsManager.getUpperBound(e); });
        mTransformManager.onUpperBoundRequest( [this] (Entity e) -> sf::Vector2f { return mLightSystem.getUpperBound(e); });
        mTransformManager.onUpperBoundRequest( [this] (Entity e) -> sf::Vector2f { return mMovementRigidbodyManager.getUpperBound(e); });
		mTransformManager.onUpperBoundRequest([this](Entity e) -> sf::Vector2f { return mSemanticsRigidbodyManager.getUpperBound(e); });
        mTransformManager.onUpperBoundRequest( [this] (Entity e) -> sf::Vector2f { return mParticleSystemManager.getUpperBound(e); });

        //init audio
        mAudioManager.initVolumeSettings(VolumeSettings::NUM);

        //init light
        mLightSystem.init(app, &mQuadTree, app.getWindow().getSize(), unshadowVertex, unshadowFragment, lightVertex, lightFragment, penumbraTex);

        //init script behavior
        mBehaviorManager.init(*this, app);
    }

	sf::Vector2f World::getSize() const
	{
		auto bounds = mQuadTree.getBoundary();
		return { bounds.size.x, bounds.size.y };
	}

	void World::setSize(const sf::Vector2f& layersize) 
	{
        if (getSize() != layersize)
		    mQuadTree.setBoundary({ 0.0f,0.0f,layersize.x,layersize.y });
	}

    void World::update(float delta, const sf::Vector2f& areaPosition, const sf::Vector2f& areaSize)
    {
		destroyQueued();

        //first step: retrieve the entities that may collide with the window
        mInUpdateRange.getList().clear();
        mQuadTree.retrieve(mInUpdateRange, {areaPosition.x, areaPosition.y, areaSize.x, areaSize.y} );

        //second step: sort out the entities, that are not on the screen
        auto removalCondition = [areaPosition, areaSize](Entity entity)
        {
             const TransformComponent& t = entity.get<TransformComponent>();

             return !(t.getPosition().x <= areaPosition.x + areaSize.x &&
                      t.getPosition().x + t.getSize().x >= areaPosition.x &&
                      t.getPosition().y <= areaPosition.y + areaSize.y &&
                      t.getPosition().y + t.getSize().y >= areaPosition.y);
        };
        mInUpdateRange.getList().erase(std::remove_if( mInUpdateRange.getList().begin(),
                                               mInUpdateRange.getList().end(),
                                               removalCondition), mInUpdateRange.getList().end());

        mEntityBehaviorHandler.update(mInUpdateRange.getList(), delta);
        mMovementHandler.update(mInUpdateRange.getList(), delta);
        mSteeringManager.update(mInUpdateRange.getList(), delta, mMovementManager);
        mPathPlanner.update(mInUpdateRange.getList(), delta, mMovementManager);
        mMovementCollisionManager.checkCollisions(mInUpdateRange.getList());
        mSemanticsCollisionManager.checkCollisions(mInUpdateRange.getList());
        mMusicEmitterMixer.update(delta, &mQuadTree);
        mSoundHandler.update(delta);
        mLightHandler.update(mInUpdateRange.getList(), delta);

        mParticleSystemManager.update(mInUpdateRange.getList(), delta);

        if (!mMuteSound) //todo where to get the quadtree from??
            mMusicEmitterMixer.update(delta, static_cast<AudioListener*>(mListener.get()), mQuadTree);

        mMaster->getRenderer().update(mInUpdateRange.getList(), delta, mVisualsHandler);
    }

    void World::handleInput(const sf::Event& event, const sf::RenderTarget& target)
    {
        mInputEventHandler.handleEvent(event, target, mMaster->getCamera());
    }

    bool World::render(sf::RenderTarget& target, sf::RenderStates states)
    {
        mRenderedEntities.getList().clear();
        mMaster->getRenderer().renewRenderlist(target, mRenderedEntities);

        mMaster->getRenderer().render(mRenderedEntities, target, states);

        if (mRenderLight)
            mLightHandler.render(mRenderedEntities, mQuadTree, target, states);

        return true; //todo meaningful return value
    }

    bool World::renderDebug(sf::RenderTarget& target, sf::RenderStates states, bool bounds, bool texrects, bool colliders, bool audioemitters, bool lights) const
    {
        if (bounds)
            mMaster->getRenderer().renderBounds(mRenderedEntities, target, states);
        if (texrects)
            mMaster->getRenderer().renderTextureRects(mRenderedEntities, target, states);
        if (colliders)
        {
            mMaster->getRenderer().renderColliders<MOVEMENT_COLLISION_CONTEXT>(mRenderedEntities, target, states, sf::Color::Cyan);
            mMaster->getRenderer().renderColliders<SEMANTICS_COLLISION_CONTEXT>(mRenderedEntities, target, states, sf::Color::Yellow);
        }
        if (audioemitters)
            mMaster->getRenderer().renderAudioEmitters(mRenderedEntities, target, states);
        if (lights)
            mMaster->getRenderer().renderLightRanges(mRenderedEntities, target, states);

        return true;
    }

	void World::addEntity(Entity e)
	{
		mQuadTree.insert(e);
	}

	void World::addEntityNearby(Entity e, Entity hint)
	{
		mQuadTree.insertNearby(e, hint);
	}

    void World::destroy(Entity e)
    {
        mEntitiesToDestroy.push_front(e);
    }

    void World::destroyNamed(Entity e)
    {
        destroy(e);
        mEntityNames.right.erase(e);
    }

    Entity World::makeCopy(Entity e)
    {
        Entity cpy = e.getInstantiation()->makeCopy(e.mHandle);
        mEntityCreationSignal(cpy);
        return cpy;
    }

	Entity World::makeForeignCopy(Entity e)
	{
		Entity fcpy = e.getInstantiation()->makeForeignCopy(e.mHandle, *this);
		mEntityCreationSignal(fcpy);
		return fcpy;
	}

	void moveFromOtherWorld(Entity e);

    void World::handleCustomEvent(const CustomEvent& event)
    {
        mEntityBehaviorHandler.handleCustomEvent(event);
    }

    owls::SignalLink<void, Entity, MetaNode, SerializationContext&> World::onEntitySerialized(const std::function<void(Entity, MetaNode, SerializationContext&)>& callback)
    {
        return mEntitySerializedSignal.connect(callback);
    }

    owls::SignalLink<void, Entity, MetaNode, DeserializationContext&> World::onEntityDeserialized(const std::function<void(Entity, MetaNode, DeserializationContext&)>& callback)
    {
        return mEntityDeserializedSignal.connect(callback);
    }

    owls::SignalLink<void, Entity> World::onEntityCreation(const std::function<void(Entity)>& callback)
    {
        return mEntityCreationSignal.connect(callback);
    }

    owls::SignalLink<void, Entity> World::onEntityDestruction(const std::function<void(Entity)>& callback)
    {
        return mEntityDestructionSignal.connect(callback);
    }

    const quad::PullResult<Entity>& World::getEntitiesInUpdateRange() const
    {
        return mInUpdateRange;
    }

    const quad::PullResult<Entity>& World::getEntitiesInRenderRange() const
    {
        return mRenderedEntities;
    }

    quad::PullResult<Entity> World::getEntitiesNearby(Entity e) const
    {
        quad::PullResult<Entity> pull;
        TransformComponent& t = e.modify<TransformComponent>();
        mQuadTree.retrieve(pull, {t.getPosition().x, t.getPosition().y, t.getSize().x, t.getSize().y} );
        return pull;
    }

    void World::notifySerialized(Entity e, MetaNode serializer, SerializationContext& context)
    {
        mEntitySerializedSignal(e, serializer, context);
    }

    void World::notifyDeserialized(Entity e, MetaNode serializer, DeserializationContext& context)
    {
        mEntityDeserializedSignal(e, serializer, context);
    }


    void World::tagWithName(Entity e, const std::string& name)
    {
        mEntityNames.insert( NameBimap::value_type{name, e} );
        Logger::info(e.getID());
        Logger::info(" INSERT ");
        Logger::info(name);
        Logger::endl();

        for (const auto& nameEntityPair : mEntityNames)
        {
            Logger::info(nameEntityPair.get<World::EntityTag>().getID());
            Logger::info(" MAP ");
            Logger::info(nameEntityPair.get<World::NameTag>());
            Logger::endl();
        }
        for (const auto& pair : mEntityNames.by<NameTag>())
        {
            Logger::info(pair.second.getID());
            Logger::info(" MAPBN ");
            Logger::info(pair.first);
            Logger::endl();
        }
    }


    Entity World::getEntityByName(const std::string& name) const
    {
        auto result = mEntityNames.by<NameTag>().find(name);
        if (result != mEntityNames.by<NameTag>().end())
        {
            return result->get<EntityTag>();
        }
        else
            return {};
    }


    std::string World::getName(Entity e) const
    {
        auto result = mEntityNames.by<EntityTag>().find(e);
        if (result != mEntityNames.by<EntityTag>().end())
            return result->get<NameTag>();
        else
            return "";
    }


    void World::toggleLight(bool on)
    {
        mRenderLight = on;
    }


    bool World::isLightToggled() const
    {
        return mRenderLight;
    }
	
	World::~World()
	{
		quad::PullResult< Entity > fullpull;
		mQuadTree.getContent(fullpull);
		for (const auto& e : fullpull.getList())
			if (e)
				destroy(e);
	}

    void World::destroyQueued()
    {
        for (auto& e : mEntitiesToDestroy)
        {
            if (!e) continue;
            if (e.has<TransformComponent>())
            {
                mQuadTree.removeFromItsNode(e);
            }
            mEntityDestructionSignal(e);
            e.getInstantiation()->cleanup(e);
            e.mHandle.destroy();
        }
        mEntitiesToDestroy.clear();
    }
}
