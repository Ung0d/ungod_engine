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

#ifndef UNGOD_WORLD_H
#define UNGOD_WORLD_H

#include "ungod/base/Entity.h"
#include "ungod/audio/Audio.h"
#include "ungod/base/ParentChild.h"
#include "ungod/visual/RenderLayer.h"
#include "ungod/base/ComponentSignalBase.h"
#include "ungod/audio/MusicEmitter.h"
#include "ungod/base/InputEvents.h"
#include "ungod/script/EntityBehaviorHandler.h"
#include "ungod/visual/Visual.h"
#include "ungod/physics/Movement.h"
#include "ungod/physics/Path.h"
#include "ungod/physics/CollisionHandler.h"
#include "ungod/physics/Steering.h"
#include "ungod/base/Input.h"
#include "ungod/base/EntityUtility.h"
#include "ungod/visual/LightHandler.h"
#include "ungod/serialization/SerialWorld.h"
#include "ungod/physics/CollisionContexts.h"
#include "ungod/script/EntityBehavior.h"
#include "ungod/visual/RenderLayer.h"
#include "ungod/application/ScriptedGameState.h"
#include "ungod/content/tilemap/TileMapHandler.h"
#include "ungod/content/water/WaterHandler.h"
#include "ungod/base/ComponentSignalBase.h"
#include "ungod/content/EntityTypes.h"
#include "ungod/content/particle_system/ParticleComponent.h"
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <optional>

namespace ungod
{
    class Camera;
    class WorldGraphNode;

    /**
    * \brief A renderlayer with a quadtree representing a world of entities.
    */
    class World : public RenderLayer, public ComponentSignalBase
    {
    friend struct SerialBehavior<World>;
    friend struct DeserialBehavior<World, DeserialMemory&>;

    public:
        /** \brief Creates an empty world. */
        World(WorldGraphNode& node);

        /** \brief Must be called to instantiate the world. 
        * Using an uninitialized world by any means is undefined behavior. 
        * If the world was deserialized and not created "hard" in code, a deserial memory
        * must be provided. */
        void init(ScriptedGameState& master, const DeserialMemory* deserialMemory = nullptr);

		/** \brief Returns width and height of the world. */
		virtual sf::Vector2f getSize() const override;

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

        /** \brief Removes the entity from the world, but keeps the handle valid. Can be reverted by calling add. */
        void remove(Entity e);

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

		/** \brief Copies an entity from another world to this world 
		* i.e. returns a new entity which is an exact copy of e but now attached to this world. */
		Entity makeForeignCopy(Entity e);

        /** \brief Tells the world that there exists an instantiation with the given components.
        * This call is mandatory for all instantiations that shall be deserialized. */
        template<typename... BASE, typename... OPTIONAL>
        void registerInstantiation(BaseComponents<BASE...>, OptionalComponents<OPTIONAL...>);

        /** \brief Handles the given custom event. Forwards it to all entities with script components. */
        virtual void handleCustomEvent(const CustomEvent& event) override;

        /** \brief For low level entity management. */
        dom::Universe<>& getUniverse() { return *this; }
        const dom::Universe<>& getUniverse() const { return *this; }

        /** \brief For spartial layout. */
        quad::QuadTree<Entity>& getQuadTree() { return mQuadTree; }
        const quad::QuadTree<Entity>& getQuadTree() const { return mQuadTree; }

        /** \brief For binding scripts to entities and handling event calls. */
        EntityBehaviorHandler& getBehaviorHandler() { return mEntityBehaviorHandler; }
        const EntityBehaviorHandler& getBehaviorHandler() const { return mEntityBehaviorHandler; }

        /** \brief For altering entity transforms. */
        TransformHandler& getTransformHandler() { return mTransformHandler; }
        const TransformHandler& getTransformHandler() const { return mTransformHandler; }

        /** \brief For handling mouse interactions with entities. */
        InputEventHandler& getInputEventHandler() { return mInputEventHandler; }
        const InputEventHandler& getInputEventHandler() const { return mInputEventHandler; }

        /** \brief For entity movement. */
        MovementHandler& getMovementHandler() { return mMovementHandler; }
        const MovementHandler& getMovementHandler() const { return mMovementHandler; }

        /** \brief For connecting steering behaviors to entities. */
        SteeringHandler<script::Environment>& getSteeringHandler() { return mSteeringHandler; }
        const SteeringHandler<script::Environment>& getSteeringHandler() const { return mSteeringHandler; }

        /** \brief For settings static paths or computing shortest ones. */
        PathPlanner& getPathPlanner() { return mPathPlanner; }
        const PathPlanner& getPathPlanner() const { return mPathPlanner; }

        /** \brief Returns a reference to the visuals manager. */
        VisualsHandler& getVisualsHandler() { return mVisualsHandler; }
        const VisualsHandler& getVisualsHandler() const { return mVisualsHandler; }

        /** \brief For handling movement collisions. */
        CollisionHandler<MOVEMENT_COLLISION_CONTEXT>& getMovementCollisionHandler() { return mMovementCollisionHandler; }

        /** \brief For handling semantics collisions. */
        CollisionHandler<SEMANTICS_COLLISION_CONTEXT>& getSemanticsCollisionHandler() { return mSemanticsCollisionHandler; }

        /** \brief For defining movement rigidbodies. */
        RigidbodyHandler<MOVEMENT_COLLISION_CONTEXT>& getMovementRigidbodyHandler() { return mMovementRigidbodyHandler; }

        /** \brief For definining semantics rigidbodies. */
        RigidbodyHandler<SEMANTICS_COLLISION_CONTEXT>& getSemanticsRigidbodyHandler() { return mSemanticsRigidbodyHandler; }

        /** \brief For music tracks and continous sounds embedded in the world. */
        MusicEmitterMixer& getMusicEmitterMixer() { return mMusicEmitterMixer; }
        const MusicEmitterMixer& getMusicEmitterMixer() const { return mMusicEmitterMixer; }

        /** \brief For connecting entities to sound profiles and playing sounds. */
        SoundHandler& getSoundHandler() { return mSoundHandler; }
        const SoundHandler& getSoundHandler() const { return mSoundHandler; }

        /** \brief For rendering and setting up lights. */
        LightHandler& getLightHandler() { return mLightHandler; }
        const LightHandler& getLightHandler() const { return mLightHandler; }

        /** \brief For handling tilemaps. */
        TileMapHandler& getTileMapHandler() { return mTileMapHandler; }
        const TileMapHandler& getTileMapHandler() const { return mTileMapHandler; }

        /** \brief For handling water. */
        WaterHandler& getWaterHandler() { return mWaterHandler; }
        const WaterHandler& getWaterHandler() const { return mWaterHandler; }

        /** \briefFor handling particle systems. */
        ParticleSystemHandler& getParticleSystemHandler() { return mParticleSystemHandler; }
        const ParticleSystemHandler& getParticleSystemHandler() const { return mParticleSystemHandler; }

        /** \brief For defining child-parent connections of entities. */
        ParentChildHandler& getParentChildHandler() { return mParentChildHandler; }
        const ParentChildHandler& getParentChildHandler() const { return mParentChildHandler; }

        /** \brief Registers new callback for the Entity-serialized-signal. */
        owls::SignalLink<void, Entity, MetaNode, SerializationContext&> onEntitySerialized(const std::function<void(Entity, MetaNode, SerializationContext&)>& callback);

        /** \brief Registers new callback for the Entity-deserialized-signal. */
        owls::SignalLink<void, Entity, MetaNode, DeserializationContext&> onEntityDeserialized(const std::function<void(Entity, MetaNode, DeserializationContext&)>& callback);

        /** \brief Registers new callback for the EntityCreation-signal. */
        owls::SignalLink<void, Entity> onEntityCreation(const std::function<void(Entity)>& callback);

        /** \brief Registers new callback for the EntityDestruction-signal. */
        owls::SignalLink<void, Entity> onEntityDestruction(const std::function<void(Entity)>& callback);

        /** \brief Returns a list of entities that are in update range during the current frame. */
        const quad::PullResult<Entity>& getEntitiesInUpdateRange() const;

        /** \brief Returns a list of entities that are in render range during the current frame. */
        const quad::PullResult<Entity>& getEntitiesInRenderRange() const;

        /** \brief Returns the entities relativly neraby the given entity. */
        quad::PullResult<Entity> getEntitiesNearby(Entity e) const;

        /** \brief Notifies the world that the given entity was serialized. */
        void notifySerialized(Entity e, MetaNode serializer, SerializationContext& context);

        /** \brief Performs the given functor for all entities in the world with the given set of components. */
        template<typename ... C, typename F>
        void forAll(const F& func) const;

        /** \brief Performs the given functor for all entities in update range in the world with the given set of components. */
        template<typename ... C, typename F>
        void forAllInRange(const F& func) const;

        /** \brief Returns a ptr to the game state that owns this world. */
        ScriptedGameState* getState() const { return mMaster; }

        /** \brief Returns a reference to the world graph this world is part of. */
        WorldGraph& getGraph() const { return mMaster->getWorldGraph(); }

        /** \brief Returns a reference to the world graph node this world belongs to. */
        WorldGraphNode& getNode() const { return mNode; }

        /** \brief Tags the given entity with a name. */
        void tagWithName(Entity e, const std::string& name);

        /** \brief Retrieves an entity by name. Returns an "empty" entity if not found. */
        Entity getEntityByName(const std::string& name) const;

        /** \brief Retrieves the name of an entity. Returns empty string if not found. */
        std::string getName(Entity e) const;

        /** \brief Toggles the rendering of light on and off. */
        void toggleLight(bool on);
        bool isLightToggled() const;

		~World() override;

    private:
        WorldGraphNode& mNode;
        ScriptedGameState* mMaster;
        quad::QuadTree<Entity> mQuadTree;
        EntityBehaviorHandler mEntityBehaviorHandler;
        TransformHandler mTransformHandler;
        InputEventHandler mInputEventHandler;
        MovementHandler mMovementHandler;
        SteeringHandler<script::Environment> mSteeringHandler;
        PathPlanner mPathPlanner;
        VisualsHandler mVisualsHandler;
        CollisionHandler<MOVEMENT_COLLISION_CONTEXT> mMovementCollisionHandler;
        CollisionHandler<SEMANTICS_COLLISION_CONTEXT> mSemanticsCollisionHandler;
        RigidbodyHandler<MOVEMENT_COLLISION_CONTEXT> mMovementRigidbodyHandler;
        RigidbodyHandler<SEMANTICS_COLLISION_CONTEXT> mSemanticsRigidbodyHandler;
        std::unique_ptr<AudioListener> mListener;
        MusicEmitterMixer mMusicEmitterMixer;
        SoundHandler mSoundHandler;
        LightHandler mLightHandler;
        TileMapHandler mTileMapHandler;
        WaterHandler mWaterHandler;
        ParticleSystemHandler mParticleSystemHandler;
        ParentChildHandler mParentChildHandler;

        std::unordered_map<std::string, std::function<void(DeserializationContext&, MetaNode, DeserialMemory&)>> mDeserialMap;

        owls::Signal<Entity> mEntityCreationSignal;
        owls::Signal<Entity> mEntityDestructionSignal;
        owls::Signal<Entity, MetaNode, SerializationContext&> mEntitySerializedSignal;
        owls::Signal<Entity, MetaNode, DeserializationContext&> mEntityDeserializedSignal;

        std::forward_list<Entity> mEntitiesToDestroy;


        bool mRenderLight;


        quad::PullResult< Entity > mInUpdateRange;
        quad::PullResult< Entity > mRenderedEntities;

        struct EntityTag   {};
        struct NameTag {};
        typedef boost::bimap< boost::bimaps::unordered_set_of< boost::bimaps::tagged<std::string, NameTag> >,
                                boost::bimaps::unordered_set_of<boost::bimaps::tagged<Entity, EntityTag>, std::hash<Entity>> > NameBimap;
        NameBimap mEntityNames;

    public:
        virtual void serialize(ungod::MetaNode serializer, ungod::SerializationContext& context) const override
        {
            deferredSerialize<World>(*this, serializer, context);
        }
        virtual std::string getSerialIdentifier() const override
        {
            return deferredGetIdentifier<World>();
        }

	private:
		/** \brief Sets the world to a new size. */
		virtual void setSize(const sf::Vector2f& layersize) override;

        //destroys all entities queued for destruction, automatically called during update and on destruction of the world
        void destroyQueued();
    };



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////IMPLEMENTATION////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    #include "ungod/physics/CollisionHandler.inl"



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
            mMaster->getEntityBehaviorManager().assignBehavior(en, script);
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
            mMaster->getEntityBehaviorManager().assignBehavior(en, script, param);
            mEntityCreationSignal(en);
            mMaster->getEntityBehaviorManager().initBehavior(en);
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
        mEntityBehaviorHandler.assignBehavior(e, script);
        mEntityCreationSignal(e);
        mEntityBehaviorHandler.initBehavior(e);
        return e;
    }

    template<typename... BASE, typename... OPTIONAL>
    Entity World::create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co, const std::string& script, script::Environment param)
    {
        Entity e(dom::Universe<>::create<BASE...>(), cb, co);
        mEntityBehaviorHandler.assignBehavior(e, script, param);
        mEntityCreationSignal(e);
        mEntityBehaviorHandler.initBehavior(e);
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
        mEntityBehaviorHandler.assignBehavior(e, script);
        mEntityCreationSignal(e);
        mEntityBehaviorHandler.initBehavior(e);
        return e;
    }

    template<typename... BASE>
    Entity World::create(BaseComponents<BASE...> cb, const std::string& script, script::Environment param)
    {
        Entity e(dom::Universe<>::create<BASE...>(), cb, OptionalComponents<>());
        mEntityBehaviorHandler.assignBehavior(e, script, param);
        mEntityCreationSignal(e);
        mEntityBehaviorHandler.initBehavior(e);
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
        mEntityBehaviorHandler.assignBehavior(e, script);
        mEntityCreationSignal(e);
        mEntityBehaviorHandler.initBehavior(e);
        return e;
    }

    template<typename... OPTIONAL>
    Entity World::create(OptionalComponents<OPTIONAL...> co, const std::string& script, script::Environment param)
    {
        Entity e(dom::Universe<>::create(), BaseComponents<>(), co);
        mEntityBehaviorHandler.assignBehavior(e, script, param);
        mEntityCreationSignal(e);
        mEntityBehaviorHandler.initBehavior(e);
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
        mEntityBehaviorHandler.assignBehavior(e, script);
        mEntityCreationSignal(e);
        mEntityBehaviorHandler.initBehavior(e);
        return e;
    }

    template<typename... BASE, typename... OPTIONAL>
    Entity World::create(BaseComponents<BASE...> cb, OptionalComponents<OPTIONAL...> co, dom::ComponentInstantiator<BASE>... ci,  const std::string& script, script::Environment param)
    {
        Entity e(dom::Universe<>::create<BASE...>(ci...), cb, co);
        mEntityBehaviorHandler.assignBehavior(e, script, param);
        mEntityCreationSignal(e);
        mEntityBehaviorHandler.initBehavior(e);
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
        mEntityBehaviorHandler.assignBehavior(e, script);
        mEntityCreationSignal(e);
        mEntityBehaviorHandler.initBehavior(e);
        return e;
    }

    template<typename... BASE>
    Entity World::create(BaseComponents<BASE...> cb, dom::ComponentInstantiator<BASE>... ci,  const std::string& script, script::Environment param)
    {
        Entity e(dom::Universe<>::create<BASE...>(ci...), cb, OptionalComponents<>());
        mEntityBehaviorHandler.assignBehavior(e, script, param);
        mEntityCreationSignal(e);
        mEntityBehaviorHandler.initBehavior(e);
        return e;
    }


    template<typename... BASE, typename... OPTIONAL>
    void World::registerInstantiation(BaseComponents<BASE...>, OptionalComponents<OPTIONAL...>)
    {
        typedef EntityInstantiation< BaseComponents<BASE...>, OptionalComponents<OPTIONAL...> > Instantiation;

        mDeserialMap.emplace( SerialIdentifier<Instantiation>::get(), [this] (DeserializationContext& context, MetaNode deserializer, DeserialMemory& deserialMemory)
        {
            std::vector<Entity> entities;
            context.first( context.deserializeObjectContainer<Entity, DeserialMemory&>(
                                [&entities, this] (std::size_t init)
                                {
                                    entities.reserve(init);
                                    dom::Universe<>::create<BASE...>(init, [this, &entities](dom::EntityHandle<> e)
                                        {
                                            entities.emplace_back(std::move(std::move(e)), BaseComponents<BASE...>(), OptionalComponents<OPTIONAL...>());
                                        });
                                },
                                [&entities] (std::size_t i) -> Entity& { return entities[i]; }, deserialMemory, BaseComponents<BASE...>(), OptionalComponents<OPTIONAL...>()),
                            SerialIdentifier<Instantiation>::get(), deserializer );


            for (auto& e : entities)
            {
                mQuadTree.insert(e);
            }
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
