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
    struct DeserialMemory;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// transform //////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <>
    struct SerialIdentifier<TransformComponent>
    {
        static std::string get()  { return "T"; }
    };

    template <>
    struct SerialBehavior<TransformComponent, Entity>
    {
        static void serialize(const TransformComponent& data, MetaNode serializer, SerializationContext& context, Entity);
    };

    template <>
    struct DeserialBehavior<TransformComponent, Entity, DeserialMemory&>
    {
        static void deserialize(TransformComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
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
    struct SerialBehavior<SoundEmitterComponent, Entity>
    {
        static void serialize(const SoundEmitterComponent& data, MetaNode serializer, SerializationContext& context, Entity);
    };

    template <>
    struct DeserialBehavior<SoundEmitterComponent, Entity, DeserialMemory&>
    {
        static void deserialize(SoundEmitterComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
    };


    template <>
    struct SerialIdentifier<MusicEmitterComponent>
    {
        static std::string get()  { return "MusE"; }
    };

    template <>
    struct SerialBehavior<MusicEmitterComponent, Entity>
    {
        static void serialize(const MusicEmitterComponent& data, MetaNode serializer, SerializationContext& context, Entity);
    };

    template <>
    struct DeserialBehavior<MusicEmitterComponent, Entity, DeserialMemory&>
    {
        static void deserialize(MusicEmitterComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
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
    struct SerialBehavior<MovementComponent, Entity>
    {
        static void serialize(const MovementComponent& data, MetaNode serializer, SerializationContext& context, Entity e);
    };

    template <>
    struct DeserialBehavior<MovementComponent, Entity, DeserialMemory&>
    {
        static void deserialize(MovementComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
    };


    template <>
    struct SerialIdentifier<SteeringComponent<script::Environment>>
    {
        static std::string get()  { return "SC"; }
    };

    template <>
    struct SerialBehavior<SteeringComponent<script::Environment>, Entity>
    {
        static void serialize(const SteeringComponent<script::Environment>& data, MetaNode serializer, SerializationContext& context, Entity e);
    };

    template <>
    struct DeserialBehavior<SteeringComponent<script::Environment>, Entity, DeserialMemory&>
    {
        static void deserialize(SteeringComponent<script::Environment>& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
    };


    template <>
    struct SerialIdentifier<PathFinderComponent>
    {
        static std::string get()  { return "PF"; }
    };

    template <>
    struct SerialBehavior<PathFinderComponent, Entity>
    {
        static void serialize(const PathFinderComponent& data, MetaNode serializer, SerializationContext& context, Entity e);
    };

    template <>
    struct DeserialBehavior<PathFinderComponent, Entity, DeserialMemory&>
    {
        static void deserialize(PathFinderComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
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
    struct SerialBehavior<VisualsComponent, Entity>
    {
        static void serialize(const VisualsComponent& data, MetaNode serializer, SerializationContext& context, Entity e);
    };
    template <>
    struct DeserialBehavior<VisualsComponent, Entity, DeserialMemory&>
    {
        static void deserialize(VisualsComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
    };

    template <>
    struct SerialIdentifier<VertexArrayComponent>
    {
        static std::string get()  { return "VA"; }
    };
    template <>
    struct SerialBehavior<VertexArrayComponent, Entity>
    {
        static void serialize(const VertexArrayComponent& data, MetaNode serializer, SerializationContext& context, Entity e);
    };
    template <>
    struct DeserialBehavior<VertexArrayComponent, Entity, DeserialMemory&>
    {
        static void deserialize(VertexArrayComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
    };

    template <>
    struct SerialIdentifier<SpriteComponent>
    {
        static std::string get()  { return "SP"; }
    };
    template <>
    struct SerialBehavior<SpriteComponent, Entity>
    {
        static void serialize(const SpriteComponent& data, MetaNode serializer, SerializationContext& context, Entity e);
    };
    template <>
    struct DeserialBehavior<SpriteComponent, Entity, DeserialMemory&>
    {
        static void deserialize(SpriteComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
    };


    template <>
    struct SerialIdentifier<VisualAffectorComponent>
    {
        static std::string get()  { return "VisA"; }
    };
    template <>
    struct SerialBehavior<VisualAffectorComponent, Entity>
    {
        static void serialize(const VisualAffectorComponent& data, MetaNode serializer, SerializationContext& context, Entity e);
    };
    template <>
    struct DeserialBehavior<VisualAffectorComponent, Entity, DeserialMemory&>
    {
        static void deserialize(VisualAffectorComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
    };


    template <>
    struct SerialIdentifier<SpriteMetadataComponent>
    {
        static std::string get()  { return "SPM"; }
    };
    template <>
    struct SerialBehavior<SpriteMetadataComponent, Entity>
    {
        static void serialize(const SpriteMetadataComponent& data, MetaNode serializer, SerializationContext& context, Entity e);
    };
    template <>
    struct DeserialBehavior<SpriteMetadataComponent, Entity, DeserialMemory&>
    {
        static void deserialize(SpriteMetadataComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
    };


    template <>
    struct SerialIdentifier<AnimationComponent>
    {
        static std::string get()  { return "A"; }
    };
    template <>
    struct SerialBehavior<AnimationComponent, Entity>
    {
        static void serialize(const AnimationComponent& data, MetaNode serializer, SerializationContext& context, Entity e);
    };
    template <>
    struct DeserialBehavior<AnimationComponent, Entity, DeserialMemory&>
    {
        static void deserialize(AnimationComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
    };


    template <>
    struct SerialIdentifier<BigSpriteComponent>
    {
        static std::string get()  { return "BigS"; }
    };
    template <>
    struct SerialBehavior<BigSpriteComponent, Entity>
    {
        static void serialize(const BigSpriteComponent& data, MetaNode serializer, SerializationContext& context, Entity e);
    };
    template <>
    struct DeserialBehavior<BigSpriteComponent, Entity, DeserialMemory&>
    {
        static void deserialize(BigSpriteComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
    };


    template <>
    struct SerialIdentifier<TileMapComponent>
    {
        static std::string get()  { return "TM"; }
    };
    template <>
    struct SerialBehavior<TileMapComponent, Entity>
    {
        static void serialize(const TileMapComponent& data, MetaNode serializer, SerializationContext& context, Entity e);
    };
    template <>
    struct DeserialBehavior<TileMapComponent, Entity, DeserialMemory&>
    {
        static void deserialize(TileMapComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
    };



    template <>
    struct SerialIdentifier<WaterComponent>
    {
        static std::string get()  { return "W"; }
    };
    template <>
    struct SerialBehavior<WaterComponent, Entity>
    {
        static void serialize(const WaterComponent& data, MetaNode serializer, SerializationContext& context, Entity e);
    };
    template <>
    struct DeserialBehavior<WaterComponent, Entity, DeserialMemory&>
    {
        static void deserialize(WaterComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
    };



    template <>
    struct SerialIdentifier<ParticleSystemComponent>
    {
        static std::string get()  { return "PS"; }
    };
    template <>
    struct SerialBehavior<ParticleSystemComponent, Entity>
    {
        static void serialize(const ParticleSystemComponent& data, MetaNode serializer, SerializationContext& context, Entity e);
    };
    template <>
    struct DeserialBehavior<ParticleSystemComponent, Entity, DeserialMemory&>
    {
        static void deserialize(ParticleSystemComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
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
    struct SerialBehavior<ShadowEmitterComponent, Entity>
    {
        static void serialize(const ShadowEmitterComponent& data, MetaNode serializer, SerializationContext& context, Entity e);
    };
    template <>
    struct DeserialBehavior<ShadowEmitterComponent, Entity, DeserialMemory&>
    {
        static void deserialize(ShadowEmitterComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
    };

    template <>
    struct SerialIdentifier<LightEmitterComponent>
    {
        static std::string get()  { return "LE"; }
    };
    template <>
    struct SerialBehavior<LightEmitterComponent, Entity>
    {
        static void serialize(const LightEmitterComponent& data, MetaNode serializer, SerializationContext& context, Entity e);
    };
    template <>
    struct DeserialBehavior<LightEmitterComponent, Entity, DeserialMemory&>
    {
        static void deserialize(LightEmitterComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
    };

    template <>
    struct SerialIdentifier<LightAffectorComponent>
    {
        static std::string get()  { return "LA"; }
    };
    template <>
    struct SerialBehavior<LightAffectorComponent, Entity>
    {
        static void serialize(const LightAffectorComponent& data, MetaNode serializer, SerializationContext& context, Entity e);
    };
    template <>
    struct DeserialBehavior<LightAffectorComponent, Entity, DeserialMemory&>
    {
        static void deserialize(LightAffectorComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// entity behavior ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    template <>
    struct SerialIdentifier<EntityBehaviorComponent>
    {
        static std::string get()  { return "EB"; }
    };
    template <>
    struct SerialBehavior<EntityBehaviorComponent, Entity>
    {
        static void serialize(const EntityBehaviorComponent& data, MetaNode serializer, SerializationContext& context, Entity e);
    };
    template <>
    struct DeserialBehavior<EntityBehaviorComponent, Entity, DeserialMemory&>
    {
        static void deserialize(EntityBehaviorComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
    };


    template <>
    struct SerialIdentifier<EntityUpdateTimer>
    {
        static std::string get()  { return "EUP"; }
    };
    template <>
    struct SerialBehavior<EntityUpdateTimer, Entity>
    {
        static void serialize(const EntityUpdateTimer& data, MetaNode serializer, SerializationContext& context, Entity e);
    };
    template <>
    struct DeserialBehavior<EntityUpdateTimer, Entity, DeserialMemory&>
    {
        static void deserialize(EntityUpdateTimer& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
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
    struct SerialBehavior<ParentComponent, Entity>
    {
        static void serialize(const ParentComponent& data, MetaNode serializer, SerializationContext& context, Entity e);
    };
    template <>
    struct DeserialBehavior<ParentComponent, Entity, DeserialMemory&>
    {
        static void deserialize(ParentComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
    };


    template <>
    struct SerialIdentifier<ChildComponent>
    {
        static std::string get()  { return "CHLD"; }
    };
    template <>
    struct SerialBehavior<ChildComponent, Entity>
    {
        static void serialize(const ChildComponent& data, MetaNode serializer, SerializationContext& context, Entity e);
    };
    template <>
    struct DeserialBehavior<ChildComponent, Entity, DeserialMemory&>
    {
        static void deserialize(ChildComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, DeserialMemory& deserialmemory);
    };
}

#endif // SERIAL_COMPONENTS_H
