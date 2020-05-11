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

#ifndef SERIAL_COMPONENTS_H
#define SERIAL_COMPONENTS_H

#include "ungod/serialization/Serializable.h"
#include "ungod/script/Script.h"

namespace ungod
{
    class Application;
    class Entity;
    class World;
    class TransformComponent;
    class SoundEmitterComponent;
    class AudioManager;
    class MovementComponent;
    template<typename GETTER > class SteeringComponent;
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
    class ParticleSystemComponent;
    class MusicEmitterComponent;
    class ParentComponent;
    class ChildComponent;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// transform //////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <>
    struct SerialIdentifier<TransformComponent>
    {
        static std::string get()  { return "T"; }
    };

    template <>
    struct SerialBehavior<TransformComponent, Entity, const World&, const Application&>
    {
        static void serialize(const TransformComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&);
    };

    template <>
    struct DeserialBehavior<TransformComponent, Entity, World&, const Application&>
    {
        static void deserialize(TransformComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&);
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// audio ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    template <>
    struct SerialIdentifier<SoundEmitterComponent>
    {
        static std::string get()  { return "SE"; }
    };

    template <>
    struct SerialBehavior<SoundEmitterComponent, Entity, const World&, const Application&>
    {
        static void serialize(const SoundEmitterComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&);
    };

    template <>
    struct DeserialBehavior<SoundEmitterComponent, Entity, World&, const Application&>
    {
        static void deserialize(SoundEmitterComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&);
    };


    template <>
    struct SerialIdentifier<MusicEmitterComponent>
    {
        static std::string get()  { return "MusE"; }
    };

    template <>
    struct SerialBehavior<MusicEmitterComponent, Entity, const World&, const Application&>
    {
        static void serialize(const MusicEmitterComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&);
    };

    template <>
    struct DeserialBehavior<MusicEmitterComponent, Entity, World&, const Application&>
    {
        static void deserialize(MusicEmitterComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&);
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// movement ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <>
    struct SerialIdentifier<MovementComponent>
    {
        static std::string get()  { return "M"; }
    };

    template <>
    struct SerialBehavior<MovementComponent, Entity, const World&, const Application&>
    {
        static void serialize(const MovementComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World& world, const Application&);
    };

    template <>
    struct DeserialBehavior<MovementComponent, Entity, World&, const Application&>
    {
        static void deserialize(MovementComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };


    template <>
    struct SerialIdentifier<SteeringComponent<script::Environment>>
    {
        static std::string get()  { return "SC"; }
    };

    template <>
    struct SerialBehavior<SteeringComponent<script::Environment>, Entity, const World&, const Application&>
    {
        static void serialize(const SteeringComponent<script::Environment>& data, MetaNode serializer, SerializationContext& context, Entity, const World& world, const Application&);
    };

    template <>
    struct DeserialBehavior<SteeringComponent<script::Environment>, Entity, World&, const Application&>
    {
        static void deserialize(SteeringComponent<script::Environment>& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };


    template <>
    struct SerialIdentifier<PathFinderComponent>
    {
        static std::string get()  { return "PF"; }
    };

    template <>
    struct SerialBehavior<PathFinderComponent, Entity, const World&, const Application&>
    {
        static void serialize(const PathFinderComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World& world, const Application&);
    };

    template <>
    struct DeserialBehavior<PathFinderComponent, Entity, World&, const Application&>
    {
        static void deserialize(PathFinderComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// visuals ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <>
    struct SerialIdentifier<VisualsComponent>
    {
        static std::string get()  { return "V"; }
    };
    template <>
    struct SerialBehavior<VisualsComponent, Entity, const World&, const Application&>
    {
        static void serialize(const VisualsComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World& world, const Application&);
    };
    template <>
    struct DeserialBehavior<VisualsComponent, Entity, World&, const Application&>
    {
        static void deserialize(VisualsComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };

    template <>
    struct SerialIdentifier<VertexArrayComponent>
    {
        static std::string get()  { return "VA"; }
    };
    template <>
    struct SerialBehavior<VertexArrayComponent, Entity, const World&, const Application&>
    {
        static void serialize(const VertexArrayComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World& world, const Application&);
    };
    template <>
    struct DeserialBehavior<VertexArrayComponent, Entity, World&, const Application&>
    {
        static void deserialize(VertexArrayComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };

    template <>
    struct SerialIdentifier<SpriteComponent>
    {
        static std::string get()  { return "SP"; }
    };
    template <>
    struct SerialBehavior<SpriteComponent, Entity, const World&, const Application&>
    {
        static void serialize(const SpriteComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World& world, const Application&);
    };
    template <>
    struct DeserialBehavior<SpriteComponent, Entity, World&, const Application&>
    {
        static void deserialize(SpriteComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };


    template <>
    struct SerialIdentifier<VisualAffectorComponent>
    {
        static std::string get()  { return "VisA"; }
    };
    template <>
    struct SerialBehavior<VisualAffectorComponent, Entity, const World&, const Application&>
    {
        static void serialize(const VisualAffectorComponent& data, MetaNode serializer, SerializationContext& context, Entity,const  World& world, const Application&);
    };
    template <>
    struct DeserialBehavior<VisualAffectorComponent, Entity, World&, const Application&>
    {
        static void deserialize(VisualAffectorComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };


    template <>
    struct SerialIdentifier<SpriteMetadataComponent>
    {
        static std::string get()  { return "SPM"; }
    };
    template <>
    struct SerialBehavior<SpriteMetadataComponent, Entity, const World&, const Application&>
    {
        static void serialize(const SpriteMetadataComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&);
    };
    template <>
    struct DeserialBehavior<SpriteMetadataComponent, Entity, World&, const Application&>
    {
        static void deserialize(SpriteMetadataComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };


    template <>
    struct SerialIdentifier<AnimationComponent>
    {
        static std::string get()  { return "A"; }
    };
    template <>
    struct SerialBehavior<AnimationComponent, Entity, const World&, const Application&>
    {
        static void serialize(const AnimationComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&);
    };
    template <>
    struct DeserialBehavior<AnimationComponent, Entity, World&, const Application&>
    {
        static void deserialize(AnimationComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };


    template <>
    struct SerialIdentifier<BigSpriteComponent>
    {
        static std::string get()  { return "BigS"; }
    };
    template <>
    struct SerialBehavior<BigSpriteComponent, Entity, const World&, const Application&>
    {
        static void serialize(const BigSpriteComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&);
    };
    template <>
    struct DeserialBehavior<BigSpriteComponent, Entity, World&, const Application&>
    {
        static void deserialize(BigSpriteComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };


    template <>
    struct SerialIdentifier<TileMapComponent>
    {
        static std::string get()  { return "TM"; }
    };
    template <>
    struct SerialBehavior<TileMapComponent, Entity, const World&, const Application&>
    {
        static void serialize(const TileMapComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application& app);
    };
    template <>
    struct DeserialBehavior<TileMapComponent, Entity, World&, const Application&>
    {
        static void deserialize(TileMapComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application& app);
    };



    template <>
    struct SerialIdentifier<WaterComponent>
    {
        static std::string get()  { return "W"; }
    };
    template <>
    struct SerialBehavior<WaterComponent, Entity, const World&, const Application&>
    {
        static void serialize(const WaterComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application& app);
    };
    template <>
    struct DeserialBehavior<WaterComponent, Entity, World&, const Application&>
    {
        static void deserialize(WaterComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application& app);
    };



    template <>
    struct SerialIdentifier<ParticleSystemComponent>
    {
        static std::string get()  { return "PS"; }
    };
    template <>
    struct SerialBehavior<ParticleSystemComponent, Entity, const World&, const Application&>
    {
        static void serialize(const ParticleSystemComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application& app);
    };
    template <>
    struct DeserialBehavior<ParticleSystemComponent, Entity, World&, const Application&>
    {
        static void deserialize(ParticleSystemComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application& app);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// light ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <>
    struct SerialIdentifier<ShadowEmitterComponent>
    {
        static std::string get()  { return "SHE"; }
    };
    template <>
    struct SerialBehavior<ShadowEmitterComponent, Entity, const World&, const Application&>
    {
        static void serialize(const ShadowEmitterComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&);
    };
    template <>
    struct DeserialBehavior<ShadowEmitterComponent, Entity, World&, const Application&>
    {
        static void deserialize(ShadowEmitterComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };

    template <>
    struct SerialIdentifier<LightEmitterComponent>
    {
        static std::string get()  { return "LE"; }
    };
    template <>
    struct SerialBehavior<LightEmitterComponent, Entity, const World&, const Application&>
    {
        static void serialize(const LightEmitterComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&);
    };
    template <>
    struct DeserialBehavior<LightEmitterComponent, Entity, World&, const Application&>
    {
        static void deserialize(LightEmitterComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };

    template <>
    struct SerialIdentifier<LightAffectorComponent>
    {
        static std::string get()  { return "LA"; }
    };
    template <>
    struct SerialBehavior<LightAffectorComponent, Entity, const World&, const Application&>
    {
        static void serialize(const LightAffectorComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&);
    };
    template <>
    struct DeserialBehavior<LightAffectorComponent, Entity, World&, const Application&>
    {
        static void deserialize(LightAffectorComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// entity behavior ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <>
    struct SerialIdentifier<Initializer>
    {
        static std::string get()  { return "IN"; }
    };
    template <>
    struct SerialBehavior<Initializer, Entity, const World&, const Application&>
    {
        static void serialize(const Initializer& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&);
    };
    template <>
    struct DeserialBehavior<Initializer, Entity, World&, const Application&>
    {
        static void deserialize(Initializer& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };



    template <>
    struct SerialIdentifier<EntityBehaviorManager>
    {
        static std::string get()  { return "EntityBehaviorManager"; }
    };
    template <>
    struct SerialBehavior<EntityBehaviorManager, Entity, const World&, const Application&>
    {
        static void serialize(const EntityBehaviorManager& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&);
    };
    template <>
    struct DeserialBehavior<EntityBehaviorManager, Entity, World&, const Application&>
    {
        static void deserialize(EntityBehaviorManager& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };


    template <>
    struct SerialIdentifier<EntityBehaviorComponent>
    {
        static std::string get()  { return "EB"; }
    };
    template <>
    struct SerialBehavior<EntityBehaviorComponent, Entity, const World&, const Application&>
    {
        static void serialize(const EntityBehaviorComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&);
    };
    template <>
    struct DeserialBehavior<EntityBehaviorComponent, Entity, World&, const Application&>
    {
        static void deserialize(EntityBehaviorComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };


    template <>
    struct SerialIdentifier<EntityUpdateTimer>
    {
        static std::string get()  { return "EUP"; }
    };
    template <>
    struct SerialBehavior<EntityUpdateTimer, Entity, const World&, const Application&>
    {
        static void serialize(const EntityUpdateTimer& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&);
    };
    template <>
    struct DeserialBehavior<EntityUpdateTimer, Entity, World&, const Application&>
    {
        static void deserialize(EntityUpdateTimer& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// parent child ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <>
    struct SerialIdentifier<ParentComponent>
    {
        static std::string get()  { return "PRNT"; }
    };
    template <>
    struct SerialBehavior<ParentComponent, Entity, const World&, const Application&>
    {
        static void serialize(const ParentComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&);
    };
    template <>
    struct DeserialBehavior<ParentComponent, Entity, World&, const Application&>
    {
        static void deserialize(ParentComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };


    template <>
    struct SerialIdentifier<ChildComponent>
    {
        static std::string get()  { return "CHLD"; }
    };
    template <>
    struct SerialBehavior<ChildComponent, Entity, const World&, const Application&>
    {
        static void serialize(const ChildComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&);
    };
    template <>
    struct DeserialBehavior<ChildComponent, Entity, World&, const Application&>
    {
        static void deserialize(ChildComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&);
    };
}

#endif // SERIAL_COMPONENTS_H
