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

#include "ungod/serialization/SerialComponents.h"
#include "ungod/content/particle_system/ParticleSystem.h"
#include "ungod/content/particle_system/Affector.h"
#include "ungod/content/particle_system/Distributions.h"

namespace ungod
{
    void SerialBehavior<ParticleSystem>::serialize(const ParticleSystem& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("mf", data.getMaxForce(), serializer);
        context.serializeProperty("mv", data.getMaxVelocity(), serializer);
        context.serializeProperty("s", data.getParticleSpeed(), serializer);
        context.serializeObject("em", data.mEmitter, serializer);
        context.serializeObject("es", data.mCountEstimator, serializer);
        context.serializeObject("texr", data.mTexrectInit, serializer);
        context.serializeObjectContainer("aff", data.mAffectors, serializer);
    }


    void DeserialBehavior<ParticleSystem>::deserialize(ParticleSystem& data, MetaNode deserializer, DeserializationContext& context)
    {
        auto attr = context.first(context.deserializeProperty([&data] (float mf) { data.setMaxForce(mf); }, ParticleSystem::MAX_FORCE_DEFAULT), "mf", deserializer);
        attr = context.next(context.deserializeProperty([&data] (float mv) { data.setMaxVelocity(mv); }, ParticleSystem::MAX_VELOCITY_DEFAULT), "mv", deserializer, attr);
        attr = context.next(context.deserializeProperty([&data] (float s) { data.setParticleSpeed(s); }, ParticleSystem::PARTICLE_SPEED_DEFAULT), "s", deserializer, attr);

        attr = context.next(context.deserializeObject(data.mEmitter), "em", deserializer, attr);
        attr = context.next(context.deserializeObject(data.mCountEstimator), "es", deserializer, attr);
        attr = context.next(context.deserializeObject(data.mTexrectInit), "texr", deserializer, attr);
        attr = context.next(context.deserializeObjectContainer<FunctorHandle>(
                            [&data] (std::size_t init)
                                {
                                    data.mAffectors.resize(init);
                                },
                            [&data] (std::size_t i) -> FunctorHandle& { return data.mAffectors[i]; }),
                    "aff", deserializer, attr);
    }

    void SerialBehavior<DirectionalForce>::serialize(const DirectionalForce& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("fx", data.force.x, serializer);
        context.serializeProperty("fy", data.force.y, serializer);
    }


    void DeserialBehavior<DirectionalForce>::deserialize(DirectionalForce& data, MetaNode deserializer, DeserializationContext& context)
    {
        sf::Vector2f force;
        auto attr = context.first(context.deserializeProperty(force.x, 0.0f), "fx", deserializer);
        attr = context.next(context.deserializeProperty(force.y, 0.0f), "fy", deserializer, attr);
        data.init(force);
    }


    void SerialBehavior<DisplaceForce>::serialize(const DisplaceForce& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("s", data.speed, serializer);
        context.serializeProperty("c", data.circle, serializer);
        context.serializeProperty("a", data.angle, serializer);
    }


    void DeserialBehavior<DisplaceForce>::deserialize(DisplaceForce& data, MetaNode deserializer, DeserializationContext& context)
    {
        float speed, circle, angle;
        auto attr = context.first(context.deserializeProperty(speed, 1.0f), "s", deserializer);
        attr = context.next(context.deserializeProperty(circle, 20.0f), "c", deserializer, attr);
        attr = context.next(context.deserializeProperty(angle, 0.0f), "a", deserializer, attr);
        data.init(speed, circle, angle);
    }

    void SerialBehavior<FadeOut>::serialize(const FadeOut& data, MetaNode serializer, SerializationContext& context)
    {
        //nothing to do here...
    }


    void DeserialBehavior<FadeOut>::deserialize(FadeOut& data, MetaNode deserializer, DeserializationContext& context)
    {
        data.init(); //returns immediatly, but still xD
    }


    void SerialBehavior<AnimatedParticles>::serialize(const AnimatedParticles& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("metaID", data.meta.getFilePath(), serializer);
        context.serializeProperty("num", data.animations.size(), serializer);
        if (!data.animations.empty())
            context.serializeProperty("key", data.animations[0].animation.getKey(), serializer);
    }


    void DeserialBehavior<AnimatedParticles>::deserialize(AnimatedParticles& data, MetaNode deserializer, DeserializationContext& context)
    {
        std::string metaID;
        std::string key;
        std::size_t num;
        auto attr = context.first(context.deserializeProperty(metaID), "metaID", deserializer);
        attr = context.next(context.deserializeProperty(num), "num", deserializer, attr);
        attr = context.next(context.deserializeProperty(key), "key", deserializer, attr);
        data.init(metaID, key, num);
    }


    void SerialBehavior<ColorShift>::serialize(const ColorShift& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("rb", data.colorBegin.r, serializer);
        context.serializeProperty("gb", data.colorBegin.g, serializer);
        context.serializeProperty("bb", data.colorBegin.b, serializer);
        context.serializeProperty("re", data.colorEnd.r, serializer);
        context.serializeProperty("ge", data.colorEnd.g, serializer);
        context.serializeProperty("be", data.colorEnd.b, serializer);
    }


    void DeserialBehavior<ColorShift>::deserialize(ColorShift& data, MetaNode deserializer, DeserializationContext& context)
    {
        sf::Color colorBegin, colorEnd;
        auto attr = context.first(context.deserializeProperty(colorBegin.r, sf::Color::White.r), "rb", deserializer);
        attr = context.next(context.deserializeProperty(colorBegin.g, sf::Color::White.g), "gb", deserializer, attr);
        attr = context.next(context.deserializeProperty(colorBegin.b, sf::Color::White.b), "bb", deserializer, attr);
        attr = context.next(context.deserializeProperty(colorEnd.r, sf::Color::White.r), "re", deserializer, attr);
        attr = context.next(context.deserializeProperty(colorEnd.g, sf::Color::White.g), "ge", deserializer, attr);
        attr = context.next(context.deserializeProperty(colorEnd.b, sf::Color::White.b), "be", deserializer, attr);
        data.init(colorBegin, colorEnd);
    }


    void SerialBehavior<RotateParticle>::serialize(const RotateParticle& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("s", data.speed, serializer);
    }


    void DeserialBehavior<RotateParticle>::deserialize(RotateParticle& data, MetaNode deserializer, DeserializationContext& context)
    {
        float speed;
        context.first(context.deserializeProperty(speed, 0.0f), "s", deserializer);
        data.init(speed);
    }



    void SerialBehavior<ScaleParticle>::serialize(const ScaleParticle& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("sbx", data.scalesBegin.x, serializer);
        context.serializeProperty("sby", data.scalesBegin.y, serializer);
        context.serializeProperty("sex", data.scalesEnd.x, serializer);
        context.serializeProperty("sey", data.scalesEnd.y, serializer);
    }


    void DeserialBehavior<ScaleParticle>::deserialize(ScaleParticle& data, MetaNode deserializer, DeserializationContext& context)
    {
        sf::Vector2f scalesBegin, scalesEnd;
        auto attr = context.first(context.deserializeProperty(scalesBegin.x, 1.0f), "sbx", deserializer);
        attr = context.next(context.deserializeProperty(scalesBegin.y, 1.0f), "sby", deserializer, attr);
        attr = context.next(context.deserializeProperty(scalesEnd.x, 1.0f), "sex", deserializer, attr);
        attr = context.next(context.deserializeProperty(scalesEnd.y, 1.0f), "sey", deserializer, attr);
        data.init(scalesBegin, scalesEnd);
    }

    void SerialBehavior<VelocityBasedRotation>::serialize(const VelocityBasedRotation& data, MetaNode serializer, SerializationContext& context)
    {
        //nothing to do here
    }


    void DeserialBehavior<VelocityBasedRotation>::deserialize(VelocityBasedRotation& data, MetaNode deserializer, DeserializationContext& context)
    {
        data.init(); //returns immediatly, but still xD
    }

    void SerialBehavior<ExplicitTexrect>::serialize(const ExplicitTexrect& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("l", data.rect.left, serializer);
        context.serializeProperty("t", data.rect.top, serializer);
        context.serializeProperty("w", data.rect.width, serializer);
        context.serializeProperty("h", data.rect.height, serializer);
    }


    void DeserialBehavior<ExplicitTexrect>::deserialize(ExplicitTexrect& data, MetaNode deserializer, DeserializationContext& context)
    {
        sf::IntRect rect;
        auto attr = context.first(context.deserializeProperty(rect.left, 0), "l", deserializer);
        attr = context.next(context.deserializeProperty(rect.top, 0), "t", deserializer, attr);
        attr = context.next(context.deserializeProperty(rect.width, 0), "w", deserializer, attr);
        attr = context.next(context.deserializeProperty(rect.height, 0), "h", deserializer, attr);
        data.init(rect);
    }


    void SerialBehavior<TexrectByKey>::serialize(const TexrectByKey& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("metaID", data.meta.getFilePath(), serializer);
        context.serializeProperty("key", data.key, serializer);
    }


    void DeserialBehavior<TexrectByKey>::deserialize(TexrectByKey& data, MetaNode deserializer, DeserializationContext& context)
    {
        std::string metaID;
        std::string key;
        auto attr = context.first(context.deserializeProperty(metaID), "metaID", deserializer);
        attr = context.next(context.deserializeProperty(key), "key", deserializer, attr);
        data.init(metaID, key);
    }


    void SerialBehavior<MultipleTexrectsByKey>::serialize(const MultipleTexrectsByKey& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("metaID", data.meta.getFilePath(), serializer);
        context.serializePropertyContainer("keys", data.keys, serializer);
    }


    void DeserialBehavior<MultipleTexrectsByKey>::deserialize(MultipleTexrectsByKey& data, MetaNode deserializer, DeserializationContext& context)
    {
        std::string metaID;
        std::vector<std::string> keys;
        auto attr = context.first(context.deserializeProperty(metaID), "metaID", deserializer);
        attr = context.next(context.deserializeContainer<std::string>([&keys](std::size_t s) { keys.reserve(s); },
                                                         [&keys](const std::string& key) { keys.emplace_back(key); }),
                            "key", deserializer, attr);
        data.init(metaID, keys);
    }


    void SerialBehavior<FixedPosition>::serialize(const FixedPosition& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("px", data.position.x, serializer);
        context.serializeProperty("py", data.position.y, serializer);
    }


    void DeserialBehavior<FixedPosition>::deserialize(FixedPosition& data, MetaNode deserializer, DeserializationContext& context)
    {
        sf::Vector2f position;
        auto attr = context.first(context.deserializeProperty(position.x, 0.0f), "px", deserializer);
        attr = context.next(context.deserializeProperty(position.y, 0.0f), "py", deserializer, attr);
        data.init(position);
    }


    void SerialBehavior<EllipseDist>::serialize(const EllipseDist& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("cx", data.center.x, serializer);
        context.serializeProperty("cy", data.center.y, serializer);
        context.serializeProperty("rx", data.radius.x, serializer);
        context.serializeProperty("ry", data.radius.y, serializer);
    }


    void DeserialBehavior<EllipseDist>::deserialize(EllipseDist& data, MetaNode deserializer, DeserializationContext& context)
    {
        sf::Vector2f center;
        sf::Vector2f radius;
        auto attr = context.first(context.deserializeProperty(center.x, 0.0f), "cx", deserializer);
        attr = context.next(context.deserializeProperty(center.y, 0.0f), "cy", deserializer, attr);
        attr = context.next(context.deserializeProperty(radius.x, 0.0f), "rx", deserializer, attr);
        attr = context.next(context.deserializeProperty(radius.y, 0.0f), "ry", deserializer, attr);
        data.init(center, radius);
    }


    void SerialBehavior<RectangleDist>::serialize(const RectangleDist& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("tlx", data.topleft.x, serializer);
        context.serializeProperty("tly", data.topleft.y, serializer);
        context.serializeProperty("sx", data.sizes.x, serializer);
        context.serializeProperty("sy", data.sizes.y, serializer);
    }


    void DeserialBehavior<RectangleDist>::deserialize(RectangleDist& data, MetaNode deserializer, DeserializationContext& context)
    {
        sf::Vector2f topleft;
        sf::Vector2f sizes;
        auto attr = context.first(context.deserializeProperty(topleft.x, 0.0f), "tlx", deserializer);
        attr = context.next(context.deserializeProperty(topleft.y, 0.0f), "tly", deserializer, attr);
        attr = context.next(context.deserializeProperty(sizes.x, 0.0f), "sx", deserializer, attr);
        attr = context.next(context.deserializeProperty(sizes.y, 0.0f), "sy", deserializer, attr);
        data.init(topleft, sizes);
    }


    void SerialBehavior<LineSegmentDist>::serialize(const LineSegmentDist& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("p1x", data.point1.x, serializer);
        context.serializeProperty("p1y", data.point1.y, serializer);
        context.serializeProperty("p2x", data.point2.x, serializer);
        context.serializeProperty("p2y", data.point2.y, serializer);
    }


    void DeserialBehavior<LineSegmentDist>::deserialize(LineSegmentDist& data, MetaNode deserializer, DeserializationContext& context)
    {
        sf::Vector2f p1, p2;
        auto attr = context.first(context.deserializeProperty(p1.x, 0.0f), "p1x", deserializer);
        attr = context.next(context.deserializeProperty(p1.y, 0.0f), "p1y", deserializer, attr);
        attr = context.next(context.deserializeProperty(p2.x, 0.0f), "p2x", deserializer, attr);
        attr = context.next(context.deserializeProperty(p2.y, 0.0f), "p2y", deserializer, attr);
        data.init(p1, p2);
    }



    void SerialBehavior<FixedVelocity>::serialize(const FixedVelocity& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("vx", data.velocity.x, serializer);
        context.serializeProperty("vy", data.velocity.y, serializer);
    }


    void DeserialBehavior<FixedVelocity>::deserialize(FixedVelocity& data, MetaNode deserializer, DeserializationContext& context)
    {
        sf::Vector2f vel;
        auto attr = context.first(context.deserializeProperty(vel.x, 0.0f), "vx", deserializer);
        attr = context.next(context.deserializeProperty(vel.y, 0.0f), "vy", deserializer, attr);
        data.init(vel);
    }


    void SerialBehavior<ConeDist>::serialize(const ConeDist& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("d1x", data.direction1.x, serializer);
        context.serializeProperty("d1y", data.direction1.y, serializer);
        context.serializeProperty("d2x", data.direction2.x, serializer);
        context.serializeProperty("d2y", data.direction2.y, serializer);
    }


    void DeserialBehavior<ConeDist>::deserialize(ConeDist& data, MetaNode deserializer, DeserializationContext& context)
    {
        sf::Vector2f d1;
        sf::Vector2f d2;
        auto attr = context.first(context.deserializeProperty(d1.x, 0.0f), "d1x", deserializer);
        attr = context.next(context.deserializeProperty(d1.y, 0.0f), "d1y", deserializer, attr);
        attr = context.next(context.deserializeProperty(d2.x, 0.0f), "d2x", deserializer, attr);
        attr = context.next(context.deserializeProperty(d2.y, 0.0f), "d2y", deserializer, attr);
        data.init(d1, d2);
    }


    void SerialBehavior<IntervalTick>::serialize(const IntervalTick& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("min", data.msmin, serializer);
        context.serializeProperty("max", data.msmax, serializer);
        context.serializeProperty("num", data.numparticle, serializer);
    }


    void DeserialBehavior<IntervalTick>::deserialize(IntervalTick& data, MetaNode deserializer, DeserializationContext& context)
    {
		float msmin, msmax;
		int num;
        auto attr = context.first(context.deserializeProperty<float>(msmin, 0.0f), "min", deserializer);
        attr = context.next(context.deserializeProperty<float>(msmax, 0.0f), "max", deserializer, attr);
        attr = context.next(context.deserializeProperty<int>(num, 0), "num", deserializer, attr);
        data.init(msmin, msmax, num);
    }


    void SerialBehavior<OneShotTick>::serialize(const OneShotTick& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("num", data.numparticle, serializer);
    }


    void DeserialBehavior<OneShotTick>::deserialize(OneShotTick& data, MetaNode deserializer, DeserializationContext& context)
    {
		int num;
        context.first(context.deserializeProperty<int>(num, 0u), "num", deserializer);
        data.init(num);
        data.shot = true;
    }


    void SerialBehavior<IntervalLifetime>::serialize(const IntervalLifetime& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("min", data.msmin, serializer);
        context.serializeProperty("max", data.msmax, serializer);
    }


    void DeserialBehavior<IntervalLifetime>::deserialize(IntervalLifetime& data, MetaNode deserializer, DeserializationContext& context)
    {
        float msmin, msmax;
        auto attr = context.first(context.deserializeProperty<float>(msmin, 0u), "min", deserializer);
        attr = context.next(context.deserializeProperty<float>(msmax, 0u), "max", deserializer, attr);
        data.init(msmin, msmax);
    }


    void SerialBehavior<UniversalEmitter, ParticleFunctorMaster*>::serialize(const UniversalEmitter& data, MetaNode serializer, SerializationContext& context, ParticleFunctorMaster* master)
    {
        context.serializeObject("pos", data.mPosInit, serializer);
        context.serializeObject("vel", data.mVelInit, serializer);
        context.serializeObject("lt", data.mLifetimeInit, serializer);
        context.serializeObject("si", data.mSpawnInterval, serializer);
    }


    void DeserialBehavior<UniversalEmitter, ParticleFunctorMaster*>::deserialize(UniversalEmitter& data, MetaNode deserializer, DeserializationContext& context, ParticleFunctorMaster* master)
    {
        FunctorHandle posinit, velinit, lifetimeinit, spawninit;
        auto attr = context.first(context.deserializeObject(posinit), "pos", deserializer);
        attr = context.next(context.deserializeObject(velinit), "vel", deserializer, attr);
        attr = context.next(context.deserializeObject(lifetimeinit), "lt", deserializer, attr);
        attr = context.next(context.deserializeObject(spawninit), "si", deserializer, attr);
        data.init(master);
        data.mPosInit = posinit;
        data.mVelInit = velinit;
        data.mLifetimeInit = lifetimeinit;
        data.mSpawnInterval = spawninit;
    }


    void SerialBehavior<UniversalEstimator>::serialize(const UniversalEstimator& data, MetaNode serializer, SerializationContext& context)
    {
    }


    void DeserialBehavior<UniversalEstimator>::deserialize(UniversalEstimator& data, MetaNode deserializer, DeserializationContext& context)
    {
    }



    void prepareParticleSystemDeserial(DeserializationContext& context, ParticleFunctorMaster* master)
    {
        context.instantiate<DirectionalForce>([] () { return new DirectionalForce(); });
        context.instantiate<DisplaceForce>([] () { return new DisplaceForce(); });
        context.instantiate<FadeOut>([] () { return new FadeOut(); });
        context.instantiate<AnimatedParticles>([] () { return new AnimatedParticles(); });
        context.instantiate<ColorShift>([] () { return new ColorShift(); });
        context.instantiate<RotateParticle>([] () { return new RotateParticle(); });
        context.instantiate<ScaleParticle>([] () { return new ScaleParticle(); });
        context.instantiate<VelocityBasedRotation>([] () { return new VelocityBasedRotation(); });
        context.instantiate<ExplicitTexrect>([] () { return new ExplicitTexrect(); });
        context.instantiate<TexrectByKey>([] () { return new TexrectByKey(); });
        context.instantiate<MultipleTexrectsByKey>([] () { return new MultipleTexrectsByKey(); });
        context.instantiate<FixedPosition>([] () { return new FixedPosition(); });
        context.instantiate<EllipseDist>([] () { return new EllipseDist(); });
        context.instantiate<RectangleDist>([] () { return new RectangleDist(); });
        context.instantiate<LineSegmentDist>([] () { return new LineSegmentDist(); });
        context.instantiate<FixedVelocity>([] () { return new FixedVelocity(); });
        context.instantiate<ConeDist>([] () { return new ConeDist(); });
        context.instantiate<IntervalTick>([] () { return new IntervalTick(); });
        context.instantiate<IntervalLifetime>([] () { return new IntervalLifetime(); });
        context.instantiate<UniversalEmitter, ParticleFunctorMaster*>([] () { return new UniversalEmitter(); }, std::move(master));
        context.instantiate<UniversalEstimator>([] () { return new UniversalEstimator(); });
    }
}
