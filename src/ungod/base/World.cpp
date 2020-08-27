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
#include "ungod/serialization/CollisionSerial.h"
#include "ungod/serialization/SerialMultiComponent.h"
#include "ungod/serialization/SerialComponents.h"
#include "ungod/application/Application.h"
#include "ungod/application/ScriptedGameState.h"
#include "ungod/content/EntityTypes.h"

namespace ungod
{
    World::World(WorldGraphNode& node) :
        mNode(node),
        mMaster(nullptr),
        mQuadTree(),
        mEntityBehaviorHandler(),
        mTransformHandler(mQuadTree),
        mInputEventHandler(mQuadTree, node),
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
        mWaterHandler(),
        mParentChildHandler(),
        mRenderLight(true)
    {
        //register instantiations for deserialization
        registerTypes(*this);

        //connect signals
        mVisualsHandler.onContentsChanged([this](Entity e, const sf::FloatRect& rect)
            {
                mTransformHandler.handleContentsChanged(e, rect);
            });
        mMovementRigidbodyHandler.onContentsChanged([this](Entity e, const sf::FloatRect& rect)
            {
                mTransformHandler.handleContentsChanged(e, rect);
            });
        mSemanticsRigidbodyHandler.onContentsChanged([this](Entity e, const sf::FloatRect& rect)
            {
                mTransformHandler.handleContentsChanged(e, rect);
            });
        mMovementRigidbodyHandler.onContentRemoved([this](Entity e)
            {
                mTransformHandler.handleContentsRemoved(e);
            });
        mSemanticsRigidbodyHandler.onContentRemoved([this](Entity e)
            {
                mTransformHandler.handleContentsRemoved(e);
            });
        mLightHandler.onContentsChanged([this](Entity e, const sf::FloatRect& rect)
            {
                mTransformHandler.handleContentsChanged(e, rect);
            });
        mParticleSystemHandler.onContentsChanged([this](Entity e, const sf::FloatRect& rect)
            {
                mTransformHandler.handleContentsChanged(e, rect);
            });
        mMovementCollisionHandler.onCollision([this](Entity e, Entity other, const sf::Vector2f& vec, const Collider& c1, const Collider& c2)
            {
                if (!e.isStatic())
                    mMovementHandler.handleCollision(e, other, vec, c1, c2);
            });
        mTileMapHandler.onContentsChanged([this](Entity e, const sf::FloatRect& rect)
            {
                mTransformHandler.handleContentsChanged(e, rect);
            });
        mTransformHandler.onMoveContents([this](Entity e, const sf::Vector2f& vec)
            {
                mLightHandler.moveLights(e, vec);
                mLightHandler.moveLightColliders(e, vec);
                mMovementRigidbodyHandler.moveColliders(e, vec);
                mSemanticsRigidbodyHandler.moveColliders(e, vec);
                mVisualsHandler.moveVisuals(e, vec);
                mTileMapHandler.moveTilemap(e, vec);
            });


        onComponentAdded<ParticleSystemComponent>([this](Entity e) { mParticleSystemHandler.handleParticleSystemAdded(e); });


        //connect lower bounds methods
        mTransformHandler.onLowerBoundRequest([this](Entity e) -> sf::Vector2f { return mVisualsHandler.getLowerBound(e); });
        mTransformHandler.onLowerBoundRequest([this](Entity e) -> sf::Vector2f { return mLightHandler.getLowerBound(e); });
        mTransformHandler.onLowerBoundRequest([this](Entity e) -> sf::Vector2f { return mMovementRigidbodyHandler.getLowerBound(e); });
        mTransformHandler.onLowerBoundRequest([this](Entity e) -> sf::Vector2f { return mSemanticsRigidbodyHandler.getLowerBound(e); });
        mTransformHandler.onLowerBoundRequest([this](Entity e) -> sf::Vector2f { return mParticleSystemHandler.getLowerBound(e); });
        //mTransformHandler.onLowerBoundRequest([this](Entity e) -> sf::Vector2f { return mTileMapHandler.getLowerBound(e); });

        //connect upper bounds methods
        mTransformHandler.onUpperBoundRequest([this](Entity e) -> sf::Vector2f { return mVisualsHandler.getUpperBound(e); });
        mTransformHandler.onUpperBoundRequest([this](Entity e) -> sf::Vector2f { return mLightHandler.getUpperBound(e); });
        mTransformHandler.onUpperBoundRequest([this](Entity e) -> sf::Vector2f { return mMovementRigidbodyHandler.getUpperBound(e); });
        mTransformHandler.onUpperBoundRequest([this](Entity e) -> sf::Vector2f { return mSemanticsRigidbodyHandler.getUpperBound(e); });
        mTransformHandler.onUpperBoundRequest([this](Entity e) -> sf::Vector2f { return mParticleSystemHandler.getUpperBound(e); });
        //mTransformHandler.onLowerBoundRequest([this](Entity e) -> sf::Vector2f { return mTileMapHandler.getUpperBound(e); });
    }


    void World::init(ScriptedGameState& master, const DeserialMemory* deserialMemory)
    {
        mMaster = &master;
        mEntityBehaviorHandler.init(*this);
        mSteeringHandler.init(master.getSteeringManager());
        mLightHandler.init(master.getLightManager());
        mListener = std::unique_ptr<AudioListener>(new CameraListener(master.getWorldGraph().getCamera(), *this));
        mMusicEmitterMixer.init(mListener.get());
        mSoundHandler.init(master.getApp().getSoundProfileManager(), mListener.get());
        mTileMapHandler.init(*this);
        mWaterHandler.init(*this);
        mParentChildHandler.init(*this);

        if (deserialMemory)
        {
            for (const auto& entry : deserialMemory->scriptEntities) //assign scripts first!
            {
                mEntityBehaviorHandler.assignBehavior(entry.entity, entry.script, entry.initParam);
                mEntityDeserializedSignal(entry.entity, entry.node, entry.context);
            }
            /*for (const auto& entity : deserialMemory->all) //signals may invoke script calls!
            {
                mEntityCreationSignal(entity);
            } */
            for (const auto& entry : deserialMemory->scriptEntities) 
            {
                mEntityBehaviorHandler.initBehavior(entry.entity);
            }
            for (const auto& entry : deserialMemory->waterEntities)
            {
                for (const auto& s : entry.keys)
                {
                    std::size_t sep = s.find('/');
                    if (sep != std::string::npos)
                    {
                        std::string nodeID = s.substr(0, sep);
                        mWaterHandler.addReflectionWorld(entry.entity, getGraph().getNode(nodeID), s.substr(sep + 1, s.size()));
                    }
                }
            }
        }
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

    void World::extend(const sf::Vector2f& leftTopExtensions, const sf::Vector2f& rightBotExtensions)
    {
        if (leftTopExtensions.x != 0.0f ||
            leftTopExtensions.y != 0.0f ||
            rightBotExtensions.x != 0.0f ||
            rightBotExtensions.y != 0.0f)
        {
            auto bounds = mQuadTree.getBoundary();
            bounds.size.x += leftTopExtensions.x + rightBotExtensions.x;
            bounds.size.y += leftTopExtensions.y + rightBotExtensions.y;
            quad::PullResult<Entity> pull;
            mQuadTree.getContent(pull);
            mQuadTree.clear();
            mQuadTree.setBoundary(bounds);
            while (!pull.done())
            {
                Entity e = pull.poll();
                mTransformHandler.move(e, leftTopExtensions);
                mQuadTree.insert(e);
            }
        }
    }

    void World::update(float delta, const sf::Vector2f& areaPosition, const sf::Vector2f& areaSize)
    {
		destroyQueued();

        //first step: retrieve the entities that may collide with the window
        mInUpdateRange.getList().clear();
        mQuadTree.retrieve(mInUpdateRange, {areaPosition.x, areaPosition.y, areaSize.x, areaSize.y} );

        //second step: sort out the entities, that are not in the update area
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
        mSteeringHandler.update(mInUpdateRange.getList(), delta, mMovementHandler);
        mPathPlanner.update(mInUpdateRange.getList(), delta, mMovementHandler);
        mMovementCollisionHandler.checkCollisions(mInUpdateRange.getList());
        mSemanticsCollisionHandler.checkCollisions(mInUpdateRange.getList());
        mMusicEmitterMixer.update(delta, mQuadTree);
        mSoundHandler.update(delta);
        mLightHandler.update(mInUpdateRange.getList(), delta);
        mParticleSystemHandler.update(mInUpdateRange.getList(), delta);
        mTileMapHandler.update(mInUpdateRange.getList(), *this);
        mWaterHandler.update(mInUpdateRange.getList(), mNode.getGraph().getCamera());
        mMusicEmitterMixer.update(delta, mQuadTree);

        mMaster->getRenderer().update(mInUpdateRange.getList(), delta, mVisualsHandler);
    }

    void World::handleInput(const sf::Event& event, const sf::RenderTarget& target)
    {
        mInputEventHandler.handleEvent(event, target, getGraph().getCamera());
    }

    bool World::render(sf::RenderTarget& target, sf::RenderStates states)
    {
        mMaster->getRenderer().renewRenderlist(mQuadTree, mRenderedEntities, target, states);

        mMaster->getRenderer().render(mRenderedEntities, target, states, mVisualsHandler);

        if (mRenderLight)
            mLightHandler.render(mRenderedEntities, *this, target, states);

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

    void World::remove(Entity e)
    {
        if (e.has<TransformComponent>())
            mQuadTree.removeFromItsNode(e);
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

	Entity World::accomodateForeign(Entity e)
	{
        if (&e.getWorld() != this)
        {
            Entity fcpy = e.getInstantiation()->makeForeignCopy(e.mHandle, *this);
            if (fcpy.has<EntityBehaviorComponent>())
            {
                const auto& eb = fcpy.get<EntityBehaviorComponent>();
                if (eb.hasValidEnvironment())
                {
                    eb.getEnvironment()["entity"] = fcpy;
                    e.getWorld().getBehaviorHandler().dissociateBehavior(e);
                    mEntityBehaviorHandler.initBehavior(fcpy);
                }
            }
            if (e.has<ParentComponent>())
            {
                for (const auto& child : e.get<ParentComponent>().getChildren())
                    mParentChildHandler.addChild(fcpy, accomodateForeign(child));
            }
            mTransformHandler.setPosition(fcpy,
                getNode().mapToLocalPosition(
                    e.getWorld().getNode().mapToGlobalPosition(e.get<TransformComponent>().getPosition())));
            e.getWorld().destroy(e);
            addEntity(fcpy);
            return fcpy;
        }
        else
        {
            Logger::error("Attempt to foreign copy an entity to it's home world.");
            return {};
        }
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


    void World::tagWithName(Entity e, const std::string& name)
    {
        mEntityNames.insert( NameBimap::value_type{name, e} );
        /*Logger::info(e.getID());
        Logger::info(" INSERT ");
        Logger::info(name);

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
        }*/
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
        destroyQueued();
	}

    void World::destroyQueued()
    {
        for (auto& e : mEntitiesToDestroy)
        {
            if (!e) continue;
            remove(e);
            mEntityDestructionSignal(e);
            e.getInstantiation()->cleanup(e);
            e.mHandle.destroy();
        }
        mEntitiesToDestroy.clear();
    }
}
