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

#ifndef ENTITY_TYPES_H
#define ENTITY_TYPES_H

#include <cstddef>
#include "ungod/physics/CollisionContexts.h"
#include "ungod/script/Script.h"

namespace ungod
{
    template<typename... C > struct BaseComponents;
    template<typename... C > struct OptionalComponents;
    class World;
    class TransformComponent;
    class SoundEmitterComponent;
    class AudioManager;
    class MovementComponent;
    template<typename GETTER > class SteeringComponent;
    template<std::size_t i> class RigidbodyComponent;
    class PathFinderComponent;
    class VisualsComponent;
    class VertexArrayComponent;
    class SpriteComponent;
    class VisualAffectorComponent;
    class SpriteMetadataComponent;
    class AnimationComponent;
    class BigSpriteComponent;
    class TileMapComponent;
    class WaterComponent;
    class LightEmitterComponent;
    class ShadowEmitterComponent;
    class LightAffectorComponent;
    class Initializer;
    class EntityBehaviorManager;
    class EntityBehaviorComponent;
    class EntityUpdateTimer;
    template<typename C > class MultiComponent;
    class ParticleSystemComponent;
    class MusicEmitterComponent;
    class ParentComponent;
    class ChildComponent;

    /** \defgroup EntityTypes
     * @{
    */

    /*
    * IMPORTANT RULES!!!
    * Deserialization requires a certain order of the components, make sure:
    * 1) SpriteMetaDataComponent is ALWAYS listed before Sprite-, MultiSPrite- and Vertexarray components.
    */

    /** \brief A basic entity with just a transform by default. */
    using EntityBaseComponents = BaseComponents<TransformComponent, EntityBehaviorComponent>;
    using EntityOptionalComponents = OptionalComponents<VisualsComponent, RigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>, MovementComponent, SpriteMetadataComponent,
                                                        SpriteComponent, MultiComponent<SpriteComponent>, VertexArrayComponent, VisualAffectorComponent, MultiComponent<VisualAffectorComponent>,
                                                        AnimationComponent, MultiComponent<AnimationComponent>, BigSpriteComponent, RigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>,
                                                        EntityUpdateTimer, SoundEmitterComponent, SteeringComponent<script::Environment>, PathFinderComponent, ShadowEmitterComponent, LightEmitterComponent, LightAffectorComponent,
                                                        MultiComponent<LightAffectorComponent>, MultiComponent<LightEmitterComponent>, MultiComponent<ShadowEmitterComponent>, ParticleSystemComponent,
                                                        ParentComponent, ChildComponent>;

    /** \brief An Actor is an entity with a transform, a visual appearence, a rigidbody, movement and scriptable-behavior. */
    using ActorBaseComponents = BaseComponents<TransformComponent, VisualsComponent, RigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>, MovementComponent, EntityBehaviorComponent>;
    using ActorOptionalComponents = OptionalComponents<SpriteMetadataComponent, SpriteComponent, MultiComponent<SpriteComponent>, VertexArrayComponent, VisualAffectorComponent, MultiComponent<VisualAffectorComponent>,
                                                       AnimationComponent, MultiComponent<AnimationComponent>, RigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>,
                                                       EntityUpdateTimer, SoundEmitterComponent, SteeringComponent<script::Environment>, PathFinderComponent, ShadowEmitterComponent, LightEmitterComponent, LightAffectorComponent,
                                                       MultiComponent<LightAffectorComponent>, MultiComponent<LightEmitterComponent>, MultiComponent<ShadowEmitterComponent>, ParticleSystemComponent,
                                                       ParentComponent, ChildComponent>;

    /** \brief An MetaObject is a scriptable entity with no transform, that has no representation in the world. */
    using MetaObjectBaseComponents = BaseComponents<EntityBehaviorComponent>;
    using MetaObjectOptionalComponents = OptionalComponents<EntityUpdateTimer, ParentComponent, ChildComponent>;

    /** \brief A Light that illuminates dark places. */
    using LightBaseComponents = BaseComponents<TransformComponent, LightEmitterComponent>;
    using LightOptionalComponents = OptionalComponents<LightAffectorComponent, ParentComponent, ChildComponent>;

    /** \brief A trigger is an entity in the world that has a transform, a rigidbody and a script-behavior coupled to that rigidbody.. */
    using TriggerBaseComponents = BaseComponents<TransformComponent, EntityBehaviorComponent>;
    using TriggerOptionalComponents = OptionalComponents<RigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>, RigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>, ParentComponent, ChildComponent>;

    /** \brief A world object is stationary entity in the world, that has a visual appearence and a transform by default. */
    using WorldObjectBaseComponents = BaseComponents<TransformComponent, VisualsComponent>;
    using WorldObjectOptionalComponents = OptionalComponents<SpriteMetadataComponent, RigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>, EntityBehaviorComponent,
                                                             SpriteComponent, MultiComponent<SpriteComponent>, VertexArrayComponent, VisualAffectorComponent, MultiComponent<VisualAffectorComponent>,
                                                             AnimationComponent, MultiComponent<AnimationComponent>, RigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>,
                                                             EntityUpdateTimer, SoundEmitterComponent, ShadowEmitterComponent, LightEmitterComponent, LightAffectorComponent,
                                                             MultiComponent<LightAffectorComponent>, MultiComponent<LightEmitterComponent>, MultiComponent<ShadowEmitterComponent>, ParticleSystemComponent,
                                                             ParentComponent, ChildComponent, TileMapComponent, WaterComponent>;

    /** \brief A particle system entity. */
    using ParticleSystemBaseComponents = BaseComponents<TransformComponent, VisualsComponent, ParticleSystemComponent>;
    using ParticleSystemOptionalComponents = OptionalComponents<LightEmitterComponent, LightAffectorComponent, MultiComponent<LightAffectorComponent>, MultiComponent<LightEmitterComponent>,
                                                                ParentComponent, ChildComponent>;

    /** \brief A audio emitter entity. */
    using AudioEmitterBaseComponents = BaseComponents<TransformComponent, MusicEmitterComponent>;
    using AudioEmitterOptionalComponents = OptionalComponents<EntityBehaviorComponent, EntityUpdateTimer, ParentComponent, ChildComponent>;

    /** @} */

    void registerTypes(World& world);


    enum EntityType
    {
        ENTITY,
        ACTOR,
        META_OBJECT,
        LIGHT,
        TRIGGER,
        WORLD_OBJECT,
        TILEMAP,
        WATER,
        PARTICLE_SYSTEM,
        AUDIO_EMITTER
    };

    /** \brief Type traits for instantiations. */
    template<typename BASE, typename OPTIONAL>
    struct InstantiationCode
    {
        static int get() { return -1; }
    };

    template<>
    struct InstantiationCode<EntityBaseComponents, EntityOptionalComponents>
    {
        static int get() { return ENTITY; }
    };

    template<>
    struct InstantiationCode<ActorBaseComponents, ActorOptionalComponents>
    {
        static int get() { return ACTOR; }
    };

    template<>
    struct InstantiationCode<MetaObjectBaseComponents, MetaObjectOptionalComponents>
    {
        static int get() { return META_OBJECT; }
    };

    template<>
    struct InstantiationCode<LightBaseComponents, LightOptionalComponents>
    {
        static int get() { return LIGHT; }
    };

    template<>
    struct InstantiationCode<TriggerBaseComponents, TriggerOptionalComponents>
    {
        static int get() { return TRIGGER; }
    };

    template<>
    struct InstantiationCode<WorldObjectBaseComponents, WorldObjectOptionalComponents>
    {
        static int get() { return WORLD_OBJECT; }
    };

    template<>
    struct InstantiationCode<ParticleSystemBaseComponents, ParticleSystemOptionalComponents>
    {
        static int get() { return PARTICLE_SYSTEM; }
    };

    template<>
    struct InstantiationCode<AudioEmitterBaseComponents, AudioEmitterOptionalComponents>
    {
        static int get() { return AUDIO_EMITTER; }
    };
}

#endif // ENTITY_TYPES_H
