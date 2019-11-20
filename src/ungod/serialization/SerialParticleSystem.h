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

#ifndef SERIAL_PARTICLE_SYSTEM_H
#define SERIAL_PARTICLE_SYSTEM_H

#include "ungod/serialization/Serializable.h"

namespace ungod
{
    class ParticleSystem;
    class ParticleFunctorMaster;
    struct DirectionalForce;
	struct DisplaceForce;
	struct FadeOut;
	struct ColorShift;
	struct RotateParticle;
	struct ScaleParticle;
	struct VelocityBasedRotation;
	struct AnimatedParticles;
	struct ExplicitTexrect;
	struct TexrectByKey;
	struct MultipleTexrectsByKey;
	struct AnimationTexrectData;
	struct FixedPosition;
	struct EllipseDist;
	struct RectangleDist;
	struct LineSegmentDist;
	struct FixedVelocity;
	struct ConeDist;
	struct IntervalTick;
	struct OneShotTick;
	struct IntervalLifetime;
    class UniversalEmitter;
    struct UniversalEstimator;

    template <>
    struct SerialIdentifier<ParticleSystem>
    {
        static std::string get()  { return "ParticleSystem"; }
    };

    template <>
    struct SerialBehavior<ParticleSystem>
    {
        static void serialize(const ParticleSystem& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<ParticleSystem>
    {
        static void deserialize(ParticleSystem& data, MetaNode deserializer, DeserializationContext& context);
    };

    template <>
    struct SerialIdentifier<DirectionalForce>
    {
        static std::string get()  { return "DirectionalForce"; }
    };

    template <>
    struct SerialBehavior<DirectionalForce>
    {
        static void serialize(const DirectionalForce& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<DirectionalForce>
    {
        static void deserialize(DirectionalForce& data, MetaNode deserializer, DeserializationContext& context);
    };


    template <>
    struct SerialIdentifier<DisplaceForce>
    {
        static std::string get()  { return "DisplaceForce"; }
    };

    template <>
    struct SerialBehavior<DisplaceForce>
    {
        static void serialize(const DisplaceForce& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<DisplaceForce>
    {
        static void deserialize(DisplaceForce& data, MetaNode deserializer, DeserializationContext& context);
    };


    template <>
    struct SerialIdentifier<FadeOut>
    {
        static std::string get()  { return "FadeOut"; }
    };

    template <>
    struct SerialBehavior<FadeOut>
    {
        static void serialize(const FadeOut& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<FadeOut>
    {
        static void deserialize(FadeOut& data, MetaNode deserializer, DeserializationContext& context);
    };



    template <>
    struct SerialIdentifier<AnimatedParticles>
    {
        static std::string get()  { return "AnimatedParticles"; }
    };

    template <>
    struct SerialBehavior<AnimatedParticles>
    {
        static void serialize(const AnimatedParticles& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<AnimatedParticles>
    {
        static void deserialize(AnimatedParticles& data, MetaNode deserializer, DeserializationContext& context);
    };



    template <>
    struct SerialIdentifier<ColorShift>
    {
        static std::string get()  { return "ColorShift"; }
    };

    template <>
    struct SerialBehavior<ColorShift>
    {
        static void serialize(const ColorShift& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<ColorShift>
    {
        static void deserialize(ColorShift& data, MetaNode deserializer, DeserializationContext& context);
    };



    template <>
    struct SerialIdentifier<RotateParticle>
    {
        static std::string get()  { return "RotateParticle"; }
    };

    template <>
    struct SerialBehavior<RotateParticle>
    {
        static void serialize(const RotateParticle& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<RotateParticle>
    {
        static void deserialize(RotateParticle& data, MetaNode deserializer, DeserializationContext& context);
    };



    template <>
    struct SerialIdentifier<ScaleParticle>
    {
        static std::string get()  { return "ScaleParticle"; }
    };

    template <>
    struct SerialBehavior<ScaleParticle>
    {
        static void serialize(const ScaleParticle& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<ScaleParticle>
    {
        static void deserialize(ScaleParticle& data, MetaNode deserializer, DeserializationContext& context);
    };



    template <>
    struct SerialIdentifier<VelocityBasedRotation>
    {
        static std::string get()  { return "VelocityBasedRotation"; }
    };

    template <>
    struct SerialBehavior<VelocityBasedRotation>
    {
        static void serialize(const VelocityBasedRotation& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<VelocityBasedRotation>
    {
        static void deserialize(VelocityBasedRotation& data, MetaNode deserializer, DeserializationContext& context);
    };



    template <>
    struct SerialIdentifier<ExplicitTexrect>
    {
        static std::string get()  { return "ExplicitTexrect"; }
    };

    template <>
    struct SerialBehavior<ExplicitTexrect>
    {
        static void serialize(const ExplicitTexrect& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<ExplicitTexrect>
    {
        static void deserialize(ExplicitTexrect& data, MetaNode deserializer, DeserializationContext& context);
    };



    template <>
    struct SerialIdentifier<TexrectByKey>
    {
        static std::string get()  { return "TexrectByKey"; }
    };

    template <>
    struct SerialBehavior<TexrectByKey>
    {
        static void serialize(const TexrectByKey& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<TexrectByKey>
    {
        static void deserialize(TexrectByKey& data, MetaNode deserializer, DeserializationContext& context);
    };



    template <>
    struct SerialIdentifier<MultipleTexrectsByKey>
    {
        static std::string get()  { return "MultipleTexrectsByKey"; }
    };

    template <>
    struct SerialBehavior<MultipleTexrectsByKey>
    {
        static void serialize(const MultipleTexrectsByKey& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<MultipleTexrectsByKey>
    {
        static void deserialize(MultipleTexrectsByKey& data, MetaNode deserializer, DeserializationContext& context);
    };



    template <>
    struct SerialIdentifier<FixedPosition>
    {
        static std::string get()  { return "FixedPosition"; }
    };

    template <>
    struct SerialBehavior<FixedPosition>
    {
        static void serialize(const FixedPosition& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<FixedPosition>
    {
        static void deserialize(FixedPosition& data, MetaNode deserializer, DeserializationContext& context);
    };



    template <>
    struct SerialIdentifier<EllipseDist>
    {
        static std::string get()  { return "EllipseDist"; }
    };

    template <>
    struct SerialBehavior<EllipseDist>
    {
        static void serialize(const EllipseDist& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<EllipseDist>
    {
        static void deserialize(EllipseDist& data, MetaNode deserializer, DeserializationContext& context);
    };



    template <>
    struct SerialIdentifier<RectangleDist>
    {
        static std::string get()  { return "RectangleDist"; }
    };

    template <>
    struct SerialBehavior<RectangleDist>
    {
        static void serialize(const RectangleDist& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<RectangleDist>
    {
        static void deserialize(RectangleDist& data, MetaNode deserializer, DeserializationContext& context);
    };



    template <>
    struct SerialIdentifier<LineSegmentDist>
    {
        static std::string get()  { return "LineSegmentDist"; }
    };

    template <>
    struct SerialBehavior<LineSegmentDist>
    {
        static void serialize(const LineSegmentDist& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<LineSegmentDist>
    {
        static void deserialize(LineSegmentDist& data, MetaNode deserializer, DeserializationContext& context);
    };



    template <>
    struct SerialIdentifier<FixedVelocity>
    {
        static std::string get()  { return "FixedVelocity"; }
    };

    template <>
    struct SerialBehavior<FixedVelocity>
    {
        static void serialize(const FixedVelocity& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<FixedVelocity>
    {
        static void deserialize(FixedVelocity& data, MetaNode deserializer, DeserializationContext& context);
    };



    template <>
    struct SerialIdentifier<ConeDist>
    {
        static std::string get()  { return "ConeDist"; }
    };

    template <>
    struct SerialBehavior<ConeDist>
    {
        static void serialize(const ConeDist& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<ConeDist>
    {
        static void deserialize(ConeDist& data, MetaNode deserializer, DeserializationContext& context);
    };



    template <>
    struct SerialIdentifier<IntervalTick>
    {
        static std::string get()  { return "IntervalTick"; }
    };

    template <>
    struct SerialBehavior<IntervalTick>
    {
        static void serialize(const IntervalTick& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<IntervalTick>
    {
        static void deserialize(IntervalTick& data, MetaNode deserializer, DeserializationContext& context);
    };


    template <>
    struct SerialIdentifier<OneShotTick>
    {
        static std::string get()  { return "OneShotTick"; }
    };

    template <>
    struct SerialBehavior<OneShotTick>
    {
        static void serialize(const OneShotTick& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<OneShotTick>
    {
        static void deserialize(OneShotTick& data, MetaNode deserializer, DeserializationContext& context);
    };



    template <>
    struct SerialIdentifier<IntervalLifetime>
    {
        static std::string get()  { return "IntervalLifetime"; }
    };

    template <>
    struct SerialBehavior<IntervalLifetime>
    {
        static void serialize(const IntervalLifetime& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<IntervalLifetime>
    {
        static void deserialize(IntervalLifetime& data, MetaNode deserializer, DeserializationContext& context);
    };



    template <>
    struct SerialIdentifier<UniversalEmitter>
    {
        static std::string get()  { return "UniversalEmitter"; }
    };

    template <>
    struct SerialBehavior<UniversalEmitter, ParticleFunctorMaster*>
    {
        static void serialize(const UniversalEmitter& data, MetaNode serializer, SerializationContext& context, ParticleFunctorMaster* master);
    };

    template <>
    struct DeserialBehavior<UniversalEmitter, ParticleFunctorMaster*>
    {
        static void deserialize(UniversalEmitter& data, MetaNode deserializer, DeserializationContext& context, ParticleFunctorMaster* master);
    };



    template <>
    struct SerialIdentifier<UniversalEstimator>
    {
        static std::string get()  { return "UniversalEstimator"; }
    };

    template <>
    struct SerialBehavior<UniversalEstimator>
    {
        static void serialize(const UniversalEstimator& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<UniversalEstimator>
    {
        static void deserialize(UniversalEstimator& data, MetaNode deserializer, DeserializationContext& context);
    };

    /** \brief A method that must be called for the deserial context in question when running adeserial-session
    * where particle-systems are (or may be) involved. */
    void prepareParticleSystemDeserial(DeserializationContext& context, ParticleFunctorMaster* master);
}

#endif // SERIAL_COMPONENTS_H
