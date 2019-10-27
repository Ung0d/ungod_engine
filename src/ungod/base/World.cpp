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

namespace ungod
{
    World::World(ScriptedGameState* master) :
        mMaster(master),
        mBehaviorManager(),
        mQuadTree(),
        mVisualsManager(),
        mRenderer(*this, mVisualsManager),
        mTransformManager(mQuadTree),
        mMovementCollisionManager(mQuadTree),
        mSemanticsCollisionManager(mQuadTree),
        mMovementManager(mQuadTree, mTransformManager),
        mSteeringManager(),
        mPathPlanner(),
        mInputManager(mQuadTree),
        mAudioManager(master),
        mLightSystem(),
        mTileMapRenderer(*master->getApp()),
        mParentChildManager(*this),
        mRenderLight(true)
    {
    }

    World::World(const script::SharedState& cState, script::Environment cMain, ScriptedGameState* master) :
        mMaster(master),
        mBehaviorManager(cState, cMain),
        mQuadTree(),
        mVisualsManager(),
        mRenderer(*this, mVisualsManager),
        mTransformManager(mQuadTree),
        mMovementCollisionManager(mQuadTree),
        mSemanticsCollisionManager(mQuadTree),
        mMovementManager(mQuadTree, mTransformManager),
        mSteeringManager(),
        mPathPlanner(),
        mInputManager(mQuadTree),
        mAudioManager(master),
        mLightSystem(),
        mTileMapRenderer(*master->getApp()),
        mParentChildManager(*this),
        mRenderLight(true)
    {
    }

    void World::instantiate(Application& app,
                            const std::string& unshadowVertex,
                            const std::string& unshadowFragment,
                            const std::string& lightVertex,
                            const std::string& lightFragment,
                            const std::string& penumbraTex)
    {
        instantiate(app, unshadowVertex, unshadowFragment, lightVertex, lightFragment, penumbraTex, mBehaviorManager.getBehaviorManager().createEnvironment());
    }

    void World::instantiate(Application& app,
                            const std::string& unshadowVertex,
                            const std::string& unshadowFragment,
                            const std::string& lightVertex,
                            const std::string& lightFragment,
                            const std::string& penumbraTex,
                            script::Environment global)
    {
        //connect signals
        mVisualsManager.onContentsChanged( [this] (Entity e, const sf::IntRect& rect)
                                          {
                                                mTransformManager.handleContentsChanged(e, rect);
                                          });
        mRigidbodyManager.onContentsChanged( [this] (Entity e, const sf::IntRect& rect)
                                          {
                                                mTransformManager.handleContentsChanged(e, rect);
                                          });
        mRigidbodyManager.onContentRemoved( [this] (Entity e)
                                          {
                                                mTransformManager.handleContentsRemoved(e);
                                          });
        mLightSystem.onContentsChanged( [this] (Entity e, const sf::IntRect& rect)
                                          {
                                                mTransformManager.handleContentsChanged(e, rect);
                                          });
        mParticleSystemManager.onContentsChanged( [this] (Entity e, const sf::IntRect& rect)
                                          {
                                                mTransformManager.handleContentsChanged(e, rect);
                                          });
        mMovementCollisionManager.onCollision( [this] (Entity e, Entity other, const sf::Vector2f& vec, const Collider& c1, const Collider& c2)
                                          {
                                                mMovementManager.handleCollision(e, other, vec, c1, c2);
                                          });
        mTransformManager.onMoveContents( [this] (Entity e, const sf::Vector2f& vec)
                                          {
                                                mLightSystem.moveLights(e, vec);
                                                mLightSystem.moveLightColliders(e, vec);
                                                mRigidbodyManager.moveColliders(e, vec);
                                                mVisualsManager.moveVisuals(e, vec);
                                          });


        onComponentAdded<TileMapComponent>( [this] (Entity e) { mTileMapRenderer.handleTileMapAdded(e); });
        onComponentRemoved<TileMapComponent>( [this] (Entity e) { mTileMapRenderer.handleTileMapRemoved(e); });
        onComponentAdded<WaterComponent>( [this] (Entity e) { mTileMapRenderer.handleWaterAdded(e); });
        onComponentRemoved<WaterComponent>( [this] (Entity e) { mTileMapRenderer.handleWaterRemoved(e); });


        onComponentAdded<ParticleSystemComponent>( [this] (Entity e) { mParticleSystemManager.handleParticleSystemAdded(e); });


        //connect lower bounds methods
        mTransformManager.onLowerBoundRequest( [this] (Entity e) -> sf::Vector2f { return mVisualsManager.getLowerBound(e); });
        mTransformManager.onLowerBoundRequest( [this] (Entity e) -> sf::Vector2f { return mLightSystem.getLowerBound(e); });
        mTransformManager.onLowerBoundRequest( [this] (Entity e) -> sf::Vector2f { return mRigidbodyManager.getLowerBound(e); });
        mTransformManager.onLowerBoundRequest( [this] (Entity e) -> sf::Vector2f { return mParticleSystemManager.getLowerBound(e); });

        //connect upper bounds methods
        mTransformManager.onUpperBoundRequest( [this] (Entity e) -> sf::Vector2f { return mVisualsManager.getUpperBound(e); });
        mTransformManager.onUpperBoundRequest( [this] (Entity e) -> sf::Vector2f { return mLightSystem.getUpperBound(e); });
        mTransformManager.onUpperBoundRequest( [this] (Entity e) -> sf::Vector2f { return mRigidbodyManager.getUpperBound(e); });
        mTransformManager.onUpperBoundRequest( [this] (Entity e) -> sf::Vector2f { return mParticleSystemManager.getUpperBound(e); });

        //init audio
        mAudioManager.initVolumeSettings(VolumeSettings::NUM);

        //init light
        mLightSystem.init(app, &mQuadTree, app.getWindow().getSize(), unshadowVertex, unshadowFragment, lightVertex, lightFragment, penumbraTex);

        //init script behavior
        mBehaviorManager.init(*this, global, app);
    }

    void World::initSpace(float x, float y, float width, float height)
    {
        mQuadTree.setBoundary( {x,y,width, height} );
    }

    void World::update(float delta, const sf::Vector2f& areaPosition, const sf::Vector2f& areaSize)
    {
        //clean up entities
        for (auto& e : mEntitiesToDestroy)
        {
            if (!e) continue;
            if (e.has<TransformComponent>())
            {
                mQuadTree.removeFromItsNode(e);
            }
            e.getInstantiation()->cleanup(e);
            e.mHandle.destroy();
        }
        mEntitiesToDestroy.clear();

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

        mInputManager.update();
        mRenderer.update(mInUpdateRange.getList(), delta);
        mLightSystem.update(mInUpdateRange.getList(), delta);
        mMovementManager.update(mInUpdateRange.getList(), delta);
        mSteeringManager.update(mInUpdateRange.getList(), delta, mMovementManager);
        mPathPlanner.update(mInUpdateRange.getList(), delta, mMovementManager);
        mMovementCollisionManager.checkCollisions(mInUpdateRange.getList());
        mSemanticsCollisionManager.checkCollisions(mInUpdateRange.getList());
        mAudioManager.update(delta, &mQuadTree);
        mBehaviorManager.update(mInUpdateRange.getList(), delta);
        mParticleSystemManager.update(mInUpdateRange.getList(), delta);
    }

    void World::handleInput(const sf::Event& event, const sf::RenderTarget& target)
    {
        mInputManager.handleEvent(event, target);
    }

    bool World::render(sf::RenderTarget& target, sf::RenderStates states)
    {
        mTileMapRenderer.render(target, states, *this);
        mRenderedEntities.getList().clear();
        mRenderer.renewRenderlist(target, mRenderedEntities);

        mRenderer.render(mRenderedEntities, target, states);

        if (mRenderLight)
            mLightSystem.render(mRenderedEntities, target, states);

        return true; //todo meaningful return value
    }

    bool World::renderDebug(sf::RenderTarget& target, sf::RenderStates states, bool bounds, bool texrects, bool colliders, bool audioemitters, bool lights) const
    {
        if (bounds)
            mRenderer.renderBounds(mRenderedEntities, target, states);
        if (texrects)
            mRenderer.renderTextureRects(mRenderedEntities, target, states);
        if (colliders)
        {
            mRenderer.renderColliders<MOVEMENT_COLLISION_CONTEXT>(mRenderedEntities, target, states, sf::Color::Cyan);
            mRenderer.renderColliders<SEMANTICS_COLLISION_CONTEXT>(mRenderedEntities, target, states, sf::Color::Yellow);
        }
        if (audioemitters)
            mRenderer.renderAudioEmitters(mRenderedEntities, target, states);
        if (lights)
            mRenderer.renderLightRanges(mRenderedEntities, target, states);

        return true;
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

    void World::handleCustomEvent(const CustomEvent& event)
    {
        mBehaviorManager.handleCustomEvent(event);
    }

    dom::Universe<>& World::getUniverse()
    {
        return *this;
    }

    quad::QuadTree<Entity>& World::getQuadTree()
    {
        return mQuadTree;
    }

    TransformManager& World::getTransformManager()
    {
        return mTransformManager;
    }

    MovementManager& World::getMovementManager()
    {
        return mMovementManager;
    }

    SteeringManager<script::Environment>& World::getSteeringManager()
    {
        return mSteeringManager;
    }

    PathPlanner& World::getPathPlanner()
    {
        return mPathPlanner;
    }

    VisualsManager& World::getVisualsManager()
    {
        return mVisualsManager;
    }

    CollisionManager<MOVEMENT_COLLISION_CONTEXT>& World::getMovementCollisionManager()
    {
        return mMovementCollisionManager;
    }

    CollisionManager<SEMANTICS_COLLISION_CONTEXT>& World::getSemanticsCollisionManager()
    {
        return mSemanticsCollisionManager;
    }

    RigidbodyManager& World::getRigidbodyManager()
    {
        return mRigidbodyManager;
    }

    InputManager& World::getInputManager()
    {
        return mInputManager;
    }

    AudioManager& World::getAudioManager()
    {
        return mAudioManager;
    }

    Renderer& World::getRenderer()
    {
        return mRenderer;
    }

    LightSystem& World::getLightSystem()
    {
        return mLightSystem;
    }

    const LightSystem& World::getLightSystem() const
    {
        return mLightSystem;
    }

    InitializerManager& World::getInitializerManager()
    {
        return mInitializerManager;
    }

    EntityBehaviorManager& World::getBehaviorManager()
    {
        return mBehaviorManager;
    }

    TileMapRenderer& World::getTileMapRenderer()
    {
        return mTileMapRenderer;
    }

    ParticleSystemManager& World::getParticleSystemManager()
    {
        return mParticleSystemManager;
    }

    ParentChildManager& World::getParentChildManager()
    {
        return mParentChildManager;
    }

    owls::SignalLink<void, Entity> World::onEntityCreation(const std::function<void(Entity)>& callback)
    {
        return mEntityCreationSignal.connect(callback);
    }

    owls::SignalLink<void, Entity> World::onEntityDestruction(const std::function<void(Entity)>& callback)
    {
        return mEntityDestructionSignal.connect(callback);
    }

    owls::SignalLink<void, Entity, MetaNode, SerializationContext&> World::onEntitySerialized(const std::function<void(Entity, MetaNode, SerializationContext&)>& callback)
    {
        return mEntitySerializedSignal.connect(callback);
    }

    owls::SignalLink<void, Entity, MetaNode, DeserializationContext&> World::onEntityDeserialized(const std::function<void(Entity, MetaNode, DeserializationContext&)>& callback)
    {
        return mEntityDeserializedSignal.connect(callback);
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
}
