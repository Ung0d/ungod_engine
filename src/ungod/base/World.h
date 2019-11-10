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

#ifndef WORLD_H
#define WORLD_H

#include "ungod/base/Entity.h"
#include "ungod/base/Transform.h"
#include "ungod/base/ParentChild.h"
#include "ungod/visual/Visual.h"
#include "ungod/visual/Renderer.h"
#include "ungod/physics/Movement.h"
#include "ungod/physics/Path.h"
#include "ungod/physics/Collision.h"
#include "ungod/physics/Steering.h"
#include "ungod/base/Input.h"
#include "ungod/base/EntityUtility.h"
#include "ungod/audio/Audio.h"
#include "ungod/visual/Light.h"
#include "ungod/serialization/SerialWorld.h"
#include "ungod/physics/CollisionContexts.h"
#include "ungod/script/EntityBehavior.h"
#include "ungod/visual/RenderLayer.h"
#include "ungod/application/ScriptedGameState.h"
#include "ungod/visual/TileMapRenderer.h"
#include "ungod/base/ComponentSignalBase.h"
#include "ungod/content/EntityTypes.h"
#include "ungod/content/particle_system/ParticleComponent.h"
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <optional>

namespace ungod
{
    class Camera;

    /**
    * \brief A master-class based on dom::Universe that produces and destroys entities and combined
    * with a quad-tree for efficient retrieval of selections of entities.
    */
    class World : public RenderLayer, public ComponentSignalBase
    {
    friend class SerialBehavior<World, const sf::RenderTarget&>;
    friend class DeserialBehavior<World, const sf::RenderTarget&>;

    public:
        /** \brief Default constructs a "void". */
        World(ScriptedGameState* master);

        /** \brief Constructs a "void" and shares access with an existing script state. */
        World(const script::SharedState& cState, script::Environment cMain, ScriptedGameState* master);

        /** \brief Must be called to instantiate the world. */
        void instantiate(Application& app,
                            const std::string& unshadowVertex,
                            const std::string& unshadowFragment,
                            const std::string& lightVertex,
                            const std::string& lightFragment,
                            const std::string& penumbraTex);
        void instantiate(Application& app,
                            const std::string& unshadowVertex,
                            const std::string& unshadowFragment,
                            const std::string& lightVertex,
                            const std::string& lightFragment,
                            const std::string& penumbraTex,
                            script::Environment global);

        /** \brief Must be called to give the world "space". */
        void initSpace(float x, float y, float width, float height);

        /** \brief Updates the world for the given delta-time amount. */
        virtual void update(float delta, const sf::Vector2f& areaPosition, const sf::Vector2f& areaSize) override;

        /** \brief Evaluates the given input event. */
        virtual void handleInput(const sf::Event& event, const sf::RenderTarget& target) override;

        /** \brief Renders the world content. */
        virtual bool render(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates()) override;

        /** \brief Renders debug information. Must be called after the normal render call. */
        virtual bool renderDebug(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates(),
                                 bool bounds = true, bool texrects = true, bool colliders = true, bool audioemitters = true, bool lights = true) const override;

        /** \brief Factory method based on dom::Universe<>::create, but constructs ungod::Entitys instead.
        * You have to specify the base- and optional components for this entity. */
        template<typename... BASE, typename... OPTIONAL>
        void create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co, std::size_t num, const std::function<void(Entity)>& f);

        /** \brief Same, but attaches the given script to the entity and runs its "onCreation" method. The script must be loaded before. */
        template<typename... BASE, typename... OPTIONAL>
        void create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co, std::size_t num, const std::function<void(Entity)>& f, const std::string& script);
        template<typename... BASE, typename... OPTIONAL>
        void create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co, std::size_t num, const std::function<void(Entity)>& f, const std::string& script, script::Environment param);

        /** \brief Creates a new entity. You have to specify its base- and its optional-components. */
        template<typename... BASE, typename... OPTIONAL>
        Entity create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co);

        /** \brief Same, but attaches the given script to the entity and runs its "onCreation" method. The script must be loaded before. */
        template<typename... BASE, typename... OPTIONAL>
        Entity create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co, const std::string& script);
        template<typename... BASE, typename... OPTIONAL>
        Entity create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co, const std::string& script, script::Environment param);

        /** \brief Creates an entity with just base components. */
        template<typename... BASE>
        Entity create(BaseComponents<BASE...> cb);

        /** \brief Same, but attaches the given script to the entity and runs its "onCreation" method. The script must be loaded before. */
        template<typename... BASE>
        Entity create(BaseComponents<BASE...> cb, const std::string& script);
        template<typename... BASE>
        Entity create(BaseComponents<BASE...> cb, const std::string& script, script::Environment param);

        /** \brief Creates an entity with just optional components.
        * Emits entity creation event. */
        template<typename... OPTIONAL>
        Entity create(OptionalComponents<OPTIONAL...> co);

        /** \brief Same, but attaches the given script to the entity and runs its "onCreation" method. The script must be loaded before. */
        template<typename... OPTIONAL>
        Entity create(OptionalComponents<OPTIONAL...> co, const std::string& script);
        template<typename... OPTIONAL>
        Entity create(OptionalComponents<OPTIONAL...> co, const std::string& script, script::Environment param);

		/** \brief adds an entity to the world. */
		void addEntity(Entity e);

		/** \brief adds an entity to the world using an existing entity in this world as a hint for fast insertion. */
		void addEntityNearby(Entity e, Entity hint);

        /** \brief Destroys the given entity. Automatically invalidates all other handles.
        * Emits entity destruction event. */
        void destroy(Entity e);

        /** \brief Does the same as destroy but additionally removes the entity from the name list.
        * If the entity was names before, if has to destroyed through this method to keep the name
        * map clean. */
        void destroyNamed(Entity e);

        /** \brief Creates a new entity and constructs its components with instantiators. */
        template<typename... BASE, typename... OPTIONAL>
        Entity create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co, dom::ComponentInstantiator<BASE>... ci);

        /** \brief Creates a new entity and constructs its components with instantiators. */
        template<typename... BASE>
        Entity create(BaseComponents<BASE...> cb, dom::ComponentInstantiator<BASE>... ci);

        /** \brief Same, but attaches the given script to the entity and runs its "onCreation" method. The script must be loaded before. */
        template<typename... BASE, typename... OPTIONAL>
        Entity create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co, dom::ComponentInstantiator<BASE>... ci, const std::string& script);
        template<typename... BASE, typename... OPTIONAL>
        Entity create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co, dom::ComponentInstantiator<BASE>... ci, const std::string& script, script::Environment param);

        /** \brief Same, but attaches the given script to the entity and runs its "onCreation" method. The script must be loaded before. */
        template<typename... BASE>
        Entity create(BaseComponents<BASE...> cb, dom::ComponentInstantiator<BASE>... ci, const std::string& script);
        template<typename... BASE>
        Entity create(BaseComponents<BASE...> cb, dom::ComponentInstantiator<BASE>... ci, const std::string& script, script::Environment param);

        /** \brief Create a complete copy of the entity with all it components. */
        Entity makeCopy(Entity e);

        /** \brief Tells the world that there exists a instantiation with the given components.
        * This call is mandatory for all instantiations that shall be deserialized. */
        template<typename... BASE, typename... OPTIONAL>
        void registerInstantiation(BaseComponents<BASE...>, OptionalComponents<OPTIONAL...>);

        /** \brief Handles the given custom event. Forwards it to all entities with script components. */
        virtual void handleCustomEvent(const CustomEvent& event) override;


        /** \brief Returns the boundaries of the render layer. */
        virtual sf::FloatRect getBounds() const override;

        /** \brief Returns a reference to the internal universe. */
        dom::Universe<>& getUniverse();

        /** \brief Returns a reference to the internal quad tree. */
        quad::QuadTree<Entity>& getQuadTree();

        /** \brief Returns a reference to the transform manager. */
        TransformManager& getTransformManager();

        /** \brief Returns a reference to the movement manager. */
        MovementManager& getMovementManager();

        /** \brief Returns a reference to the steering manager. */
        SteeringManager<script::Environment>& getSteeringManager();

        /** \brief Returns a reference to the pathplanner. */
        PathPlanner& getPathPlanner();

        /** \brief Returns a reference to the visuals manager. */
        VisualsManager& getVisualsManager();

        /** \brief Returns a reference to the movement collision manager. */
        CollisionManager<MOVEMENT_COLLISION_CONTEXT>& getMovementCollisionManager();

        /** \brief Returns a reference to the collision manager. */
        CollisionManager<SEMANTICS_COLLISION_CONTEXT>& getSemanticsCollisionManager();

        /** \brief Returns a reference to the rigidbody manager. */
        RigidbodyManager& getRigidbodyManager();

        /** \brief Returns a reference to the inout manager. */
        InputManager& getInputManager();

        /** \brief Returns a reference to the audio manager. */
        AudioManager& getAudioManager();

        /** \brief Returns a reference to the renderer. */
        Renderer& getRenderer();

        /** \brief Returns a reference to the light system. */
        LightSystem& getLightSystem();
        const LightSystem& getLightSystem() const;

        /** \brief Returns a reference to the initializer system. */
        InitializerManager& getInitializerManager();

        /** \brief Returns a reference to the behavior manager. */
        EntityBehaviorManager& getBehaviorManager();

        /** \brief Returns a reference to the tilemap renderer. */
        TileMapRenderer& getTileMapRenderer();

        /** \brief Returns a reference to the particle-system-manager. */
        ParticleSystemManager& getParticleSystemManager();

        /** \brief Returns a reference to the parent-child-manager. */
        ParentChildManager& getParentChildManager();

        /** \brief Registers new callback for the EntityCreation-signal. */
        owls::SignalLink<void, Entity> onEntityCreation(const std::function<void(Entity)>& callback);

        /** \brief Registers new callback for the EntityDestruction-signal. */
        owls::SignalLink<void, Entity> onEntityDestruction(const std::function<void(Entity)>& callback);

        /** \brief Registers new callback for the Entity-serialized-signal. */
        owls::SignalLink<void, Entity, MetaNode, SerializationContext&> onEntitySerialized(const std::function<void(Entity, MetaNode, SerializationContext&)>& callback);

        /** \brief Registers new callback for the Entity-deserialized-signal. */
        owls::SignalLink<void, Entity, MetaNode, DeserializationContext&> onEntityDeserialized(const std::function<void(Entity, MetaNode, DeserializationContext&)>& callback);

        /** \brief Returns a list of entities that are in update range during the current frame. */
        const quad::PullResult<Entity>& getEntitiesInUpdateRange() const;

        /** \brief Returns a list of entities that are in render range during the current frame. */
        const quad::PullResult<Entity>& getEntitiesInRenderRange() const;

        /** \brief Returns the entities relativly neraby the given entity. */
        quad::PullResult<Entity> getEntitiesNearby(Entity e) const;

        /** \brief Notifies the world that the given entity was serialized. */
        void notifySerialized(Entity e, MetaNode serializer, SerializationContext& context);

        /** \brief Notifies the world that the given entity was deserialized. */
        void notifyDeserialized(Entity e, MetaNode serializer, DeserializationContext& context);

        /** \brief Performs the given functor for all entities in the world with the given set of components. */
        template<typename ... C, typename F>
        void forAll(const F& func) const;

        /** \brief Performs the given functor for all entities in update range in the world with the given set of components. */
        template<typename ... C, typename F>
        void forAllInRange(const F& func) const;

        /** \brief Returns a ptr to the game state that owns this world. */
        ScriptedGameState* getState() const { return mMaster; }

        /** \brief Tags the given entity with a name. */
        void tagWithName(Entity e, const std::string& name);

        /** \brief Retrieves an entity by name. Returns an "empty" entity if not found. */
        Entity getEntityByName(const std::string& name) const;

        /** \brief Retrieves the name of an entity. Returns empty string if not found. */
        std::string getName(Entity e) const;

        /** \brief Toggles the rendering of light on and off. */
        void toggleLight(bool on);
        bool isLightToggled() const;

    private:
        ScriptedGameState* mMaster;
        EntityBehaviorManager mBehaviorManager;
        quad::QuadTree<Entity> mQuadTree;
        VisualsManager mVisualsManager;
        Renderer mRenderer;
        TransformManager mTransformManager;
        CollisionManager<MOVEMENT_COLLISION_CONTEXT> mMovementCollisionManager;
        CollisionManager<SEMANTICS_COLLISION_CONTEXT> mSemanticsCollisionManager;
        MovementManager mMovementManager;
        SteeringManager<script::Environment> mSteeringManager;
        PathPlanner mPathPlanner;
        RigidbodyManager mRigidbodyManager;
        InputManager mInputManager;
        AudioManager mAudioManager;
        LightSystem mLightSystem;
        InitializerManager mInitializerManager;
        TileMapRenderer mTileMapRenderer;
        ParticleSystemManager mParticleSystemManager;
        ParentChildManager mParentChildManager;
        std::unordered_map<std::string, std::function<void(DeserializationContext&, MetaNode)>> mDeserialMap;
        owls::Signal<Entity> mEntityCreationSignal;
        owls::Signal<Entity> mEntityDestructionSignal;
        owls::Signal<Entity, MetaNode, SerializationContext&> mEntitySerializedSignal;
        owls::Signal<Entity, MetaNode, DeserializationContext&> mEntityDeserializedSignal;

        quad::PullResult< Entity > mInUpdateRange;
        quad::PullResult< Entity > mRenderedEntities;

        std::forward_list<Entity> mEntitiesToDestroy;

        bool mRenderLight;


        struct EntityTag   {};
        struct NameTag {};
        typedef boost::bimap< boost::bimaps::unordered_set_of< boost::bimaps::tagged<std::string, NameTag> >,
                                boost::bimaps::unordered_set_of<boost::bimaps::tagged<Entity, EntityTag>, std::hash<Entity>> > NameBimap;
        NameBimap mEntityNames;

    public:
        virtual void serialize(ungod::MetaNode serializer, ungod::SerializationContext& context, const sf::RenderTarget& target) const override
        {
            deferredSerialize<World>(*this, serializer, context, target);
        }
        virtual std::string getSerialIdentifier() const override
        {
            return deferredGetIdentifier<World>();
        }
    };



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////IMPLEMENTATION////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    //#include "ungod/base/Entity.inl"
    #include "ungod/physics/Collision.inl"



    template<typename... BASE, typename... OPTIONAL>
    void World::create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co, std::size_t num, const std::function<void(Entity)>& f)
    {
        dom::Universe<>::create<BASE...>(num, [this, f, cb, co] (dom::EntityHandle<> e)
        {
            Entity en(std::move(e), cb, co);
            mEntityCreationSignal(en);
            f( en );
        });
    }

    template<typename... BASE, typename... OPTIONAL>
    void World::create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co, std::size_t num, const std::function<void(Entity)>& f, const std::string& script)
    {
        dom::Universe<>::create<BASE...>(num, [this, f, cb, co, script] (dom::EntityHandle<> e)
        {
            Entity en(std::move(e), cb, co);
            mBehaviorManager.assignBehavior(en, script);
            mEntityCreationSignal(en);
            f( en );
        });
    }

    template<typename... BASE, typename... OPTIONAL>
    void World::create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co, std::size_t num, const std::function<void(Entity)>& f, const std::string& script, script::Environment param)
    {
        dom::Universe<>::create<BASE...>(num, [this, f, cb, co, script, param] (dom::EntityHandle<> e)
        {
            Entity en(std::move(e), cb, co);
            mBehaviorManager.assignBehavior(en, script, param);
            mEntityCreationSignal(en);
            f( en );
        });
    }


    template<typename... BASE, typename... OPTIONAL>
    Entity World::create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co)
    {
        Entity e(dom::Universe<>::create<BASE...>(), cb, co);
        mEntityCreationSignal(e);
        return e;
    }

    template<typename... BASE, typename... OPTIONAL>
    Entity World::create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co, const std::string& script)
    {
        Entity e(dom::Universe<>::create<BASE...>(), cb, co);
        mBehaviorManager.assignBehavior(e, script);
        mEntityCreationSignal(e);
        return e;
    }

    template<typename... BASE, typename... OPTIONAL>
    Entity World::create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co, const std::string& script, script::Environment param)
    {
        Entity e(dom::Universe<>::create<BASE...>(), cb, co);
        mBehaviorManager.assignBehavior(e, script, param);
        mEntityCreationSignal(e);
        return e;
    }


    template<typename... BASE>
    Entity World::create(BaseComponents<BASE...> cb)
    {
        Entity e(dom::Universe<>::create<BASE...>(), cb, OptionalComponents<>());
        mEntityCreationSignal(e);
        return e;
    }

    template<typename... BASE>
    Entity World::create(BaseComponents<BASE...> cb, const std::string& script)
    {
        Entity e(dom::Universe<>::create<BASE...>(), cb, OptionalComponents<>());
        mBehaviorManager.assignBehavior(e, script);
        mEntityCreationSignal(e);
        return e;
    }

    template<typename... BASE>
    Entity World::create(BaseComponents<BASE...> cb, const std::string& script, script::Environment param)
    {
        Entity e(dom::Universe<>::create<BASE...>(), cb, OptionalComponents<>());
        mBehaviorManager.assignBehavior(e, script, param);
        mEntityCreationSignal(e);
        return e;
    }


    template<typename... OPTIONAL>
    Entity World::create(OptionalComponents<OPTIONAL...> co)
    {
        Entity e(dom::Universe<>::create(), BaseComponents<>(), co);
        mEntityCreationSignal(e);
        return e;
    }

    template<typename... OPTIONAL>
    Entity World::create(OptionalComponents<OPTIONAL...> co, const std::string& script)
    {
        Entity e(dom::Universe<>::create(), BaseComponents<>(), co);
        mBehaviorManager.assignBehavior(e, script);
        mEntityCreationSignal(e);
        return e;
    }

    template<typename... OPTIONAL>
    Entity World::create(OptionalComponents<OPTIONAL...> co, const std::string& script, script::Environment param)
    {
        Entity e(dom::Universe<>::create(), BaseComponents<>(), co);
        mBehaviorManager.assignBehavior(e, script, param);
        mEntityCreationSignal(e);
        return e;
    }


    template<typename... BASE, typename... OPTIONAL>
    Entity World::create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co, dom::ComponentInstantiator<BASE>... ci)
    {
        Entity e(dom::Universe<>::create<BASE...>(ci...), cb, co);
        mEntityCreationSignal(e);
        return e;
    }

    template<typename... BASE, typename... OPTIONAL>
    Entity World::create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co, dom::ComponentInstantiator<BASE>... ci,  const std::string& script)
    {
        Entity e(dom::Universe<>::create<BASE...>(ci...), cb, co);
        mBehaviorManager.assignBehavior(e, script);
        mEntityCreationSignal(e);
        return e;
    }

    template<typename... BASE, typename... OPTIONAL>
    Entity World::create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co, dom::ComponentInstantiator<BASE>... ci,  const std::string& script, script::Environment param)
    {
        Entity e(dom::Universe<>::create<BASE...>(ci...), cb, co);
        mBehaviorManager.assignBehavior(e, script, param);
        mEntityCreationSignal(e);
        return e;
    }


    template<typename... BASE>
    Entity World::create(BaseComponents<BASE...> cb, dom::ComponentInstantiator<BASE>... ci)
    {
        Entity e(dom::Universe<>::create<BASE...>(ci...), cb, OptionalComponents<>());
        mEntityCreationSignal(e);
        return e;
    }

    template<typename... BASE>
    Entity World::create(BaseComponents<BASE...> cb, dom::ComponentInstantiator<BASE>... ci,  const std::string& script)
    {
        Entity e(dom::Universe<>::create<BASE...>(ci...), cb, OptionalComponents<>());
        mBehaviorManager.assignBehavior(e, script);
        mEntityCreationSignal(e);
        return e;
    }

    template<typename... BASE>
    Entity World::create(BaseComponents<BASE...> cb, dom::ComponentInstantiator<BASE>... ci,  const std::string& script, script::Environment param)
    {
        Entity e(dom::Universe<>::create<BASE...>(ci...), cb, OptionalComponents<>());
        mBehaviorManager.assignBehavior(e, script, param);
        mEntityCreationSignal(e);
        return e;
    }


    template<typename... BASE, typename... OPTIONAL>
    void World::registerInstantiation(BaseComponents<BASE...>, OptionalComponents<OPTIONAL...>)
    {
        typedef EntityInstantiation< BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> > Instantiation;

        mDeserialMap.emplace( SerialIdentifier<Instantiation>::get(), [this] (DeserializationContext& context, MetaNode deserializer)
        {
            std::vector<Entity> entities;
            context.first( context.deserializeObjectContainer<Entity, World&, const Application&>(
                                [&entities, this] (std::size_t init)
                                {
                                    entities.reserve(init);
                                    create(BaseComponents<BASE...>(), OptionalComponents<OPTIONAL...>(), init, [&entities] (Entity e) { entities.emplace_back(std::move(e)); });
                                },
                                [&entities] (std::size_t i) -> Entity& { return entities[i]; }, *this, *mMaster->getApp(), BaseComponents<BASE...>(), OptionalComponents<OPTIONAL...>()),
                            SerialIdentifier<Instantiation>::get(), deserializer );


            for (auto& e : entities)
            {
                mQuadTree.insert(e);
            }
        } );
    }

    template<>
    inline void World::registerInstantiation(TileMapBaseComponents, TileMapOptionalComponents)
    {
        typedef EntityInstantiation< TileMapBaseComponents, TileMapOptionalComponents > Instantiation;

        mDeserialMap.emplace( SerialIdentifier<Instantiation>::get(), [this] (DeserializationContext& context, MetaNode deserializer)
        {
            std::vector<Entity> entities;

            context.first( context.deserializeObjectContainer<Entity, World&, const Application&>(
                                [&entities, this] (std::size_t init)
                                {
                                    entities.reserve(init);
                                    create(TileMapBaseComponents(), TileMapOptionalComponents(), init, [&entities] (Entity e) { entities.emplace_back(std::move(e)); });
                                },
                                [&entities] (std::size_t i) -> Entity& { return entities[i]; }, *this, *mMaster->getApp(), TileMapBaseComponents(), TileMapOptionalComponents()),
                            SerialIdentifier<Instantiation>::get(), deserializer );
        } );
    }

    template<>
    inline void World::registerInstantiation(WaterBaseComponents, WaterOptionalComponents)
    {
        typedef EntityInstantiation< WaterBaseComponents, WaterOptionalComponents > Instantiation;

        mDeserialMap.emplace( SerialIdentifier<Instantiation>::get(), [this] (DeserializationContext& context, MetaNode deserializer)
        {
            std::vector<Entity> entities;

            context.first( context.deserializeObjectContainer<Entity, World&, const Application&>(
                                [&entities, this] (std::size_t init)
                                {
                                    entities.reserve(init);
                                    create(WaterBaseComponents(), WaterOptionalComponents(), init, [&entities] (Entity e) { entities.emplace_back(std::move(e)); });
                                },
                                [&entities] (std::size_t i) -> Entity& { return entities[i]; }, *this, *mMaster->getApp(), WaterBaseComponents(), WaterOptionalComponents()),
                            SerialIdentifier<Instantiation>::get(), deserializer );
        } );
    }

    template<typename ... C, typename F>
    void World::forAll(const F& func) const
    {
        quad::PullResult< Entity > fullpull;
        mQuadTree.getContent(fullpull);
        dom::Utility<Entity>::iterate<C...>(fullpull.getList(), func);
    }

    template<typename ... C, typename F>
    void World::forAllInRange(const F& func) const
    {
        dom::Utility<Entity>::iterate<C...>(mInUpdateRange.getList(), func);
    }
}

#include "ungod/serialization/EntitySerial.inl"
#include "ungod/serialization/CollisionSerial.inl"

#endif // WORLD_H
