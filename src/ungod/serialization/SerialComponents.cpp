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

#include "ungod/Serialization/SerialComponents.h"
#include "ungod/base/World.h"
#include "ungod/application/Application.h"

namespace ungod
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// transform //////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SerialBehavior<TransformComponent, Entity, const World&, const Application&>::serialize(const TransformComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
        context.serializeProperty("t_x", data.mTransform.getPosition().x, serializer);
        context.serializeProperty("t_y", data.mTransform.getPosition().y, serializer);
        if (data.mTransform.getScale().x != 1.0)
            context.serializeProperty("scale", data.mTransform.getScale().x, serializer);
        if (data.mBaseLineOffsets.x != 0.0f || data.mBaseLineOffsets.y != 0.0f)
        {
            context.serializeProperty("baseline_x", data.mBaseLineOffsets.x, serializer);
            context.serializeProperty("baseline_y", data.mBaseLineOffsets.y, serializer);
        }
        //we dont need to serialize the lower and upper bounds, since it is just an internal value that is automatically set depending on attached contents
    }

    void DeserialBehavior<TransformComponent, Entity, World&, const Application&>::deserialize(TransformComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
        auto result = deserializer.getAttributes<float, float, float, float, float>
                        ( {"t_x", 0.0f}, {"t_y", 0.0f}, {"scale", 1.0f}, {"baseline_x", 1.0f}, {"baseline_y", 0.0f});
        data.mTransform.setPosition(std::get<0>(result), std::get<1>(result));
        data.mTransform.setScale(std::get<2>(result), std::get<2>(result));
        data.mBaseLineOffsets.x = std::get<3>(result);
        data.mBaseLineOffsets.y = std::get<4>(result);
        //we dont need to emit signals or notify the quadtree
        //entities are inserted AFTER deserialization
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// movement ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SerialBehavior<MovementComponent, Entity, const World&, const Application&>::serialize(const MovementComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
        if (data.getVelocity().x != 0.0f)
            context.serializeProperty("vel_x", data.getVelocity().x, serializer);
        if (data.getVelocity().y != 0.0f)
            context.serializeProperty("vel_y", data.getVelocity().y, serializer);
        if (data.getAcceleration().x != 0.0f)
            context.serializeProperty("acc_x", data.getAcceleration().x, serializer);
        if (data.getAcceleration().y != 0.0f)
            context.serializeProperty("acc_y", data.getAcceleration().y, serializer);
        if (data.mDirection != MovementComponent::Direction::IDLE)
            context.serializeProperty("direction", static_cast<std::underlying_type<MovementComponent::Direction>::type>(data.mDirection), serializer);
    }

    void DeserialBehavior<MovementComponent, Entity, World&, const Application&>::deserialize(MovementComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&)
    {
        auto result = deserializer.getAttributes<float, float, float, float, std::underlying_type<MovementComponent::Direction>::type>
                        ( {"vel_x", 0.0f}, {"vel_y", 0.0f},
                         {"acc_x", 0.0f}, {"acc_y", 0.0f}, {"direction", static_cast<std::underlying_type<MovementComponent::Direction>::type>(MovementComponent::Direction::IDLE)});
        data.mMobilityUnit.velocity.x = std::get<0>(result);
        data.mMobilityUnit.velocity.y = std::get<1>(result);
        data.mMobilityUnit.acceleration.x = std::get<2>(result);
        data.mMobilityUnit.acceleration.y = std::get<3>(result);
        data.mDirection = static_cast<MovementComponent::Direction>(std::get<4>(result));
    }


    void SerialBehavior<SteeringComponent<script::Environment>, Entity, const World&, const Application&>::serialize(const SteeringComponent<script::Environment>& data, MetaNode serializer, SerializationContext& context, Entity, const World& world, const Application&)
    {
        context.serializeProperty("active", data.mActive, serializer);
        context.serializeProperty("pattern", data.mSteeringPattern->getIdentifier(), serializer);
    }

    void DeserialBehavior<SteeringComponent<script::Environment>, Entity, World&, const Application&>::deserialize(SteeringComponent<script::Environment>& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&)
    {
         auto result = deserializer.getAttributes<bool, std::string>
                        ( {"active", false}, {"pattern", ""});
        //world.getSteeringManager().connectPattern()
        //data.mActive = std::get<0>(result);
    }


    void SerialBehavior<PathFinderComponent, Entity, const World&, const Application&>::serialize(const PathFinderComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World& world, const Application&)
    {
    }

    void DeserialBehavior<PathFinderComponent, Entity, World&, const Application&>::deserialize(PathFinderComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&)
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// audio ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SerialBehavior<AudioManager>::serialize(const AudioManager& data, MetaNode serializer, SerializationContext& context)
    {
        //TODO deserial and serial of music... not really important at the moment since music is usually loaded scriptwise anyway
        //serialize music
        /*MetaNode musicNode = context.appendSubnode(serializer, "music");
        context.serializeProperty("count", data.mMusic.size(), musicNode);
        for (unsigned i = 0; i < data.mMusic.size(); ++i)
        {
            MetaNode musicEntry = context.appendSubnode(musicNode, "m");
            context.serializeProperty("filepath", data.mMusic[i].mMusic.getFilePath(), musicEntry);
            context.serializeProperty("volume", data.mMusicVolumes[i], musicEntry);
        }*/

        //serializing sound profile
        MetaNode profilesNode = context.appendSubnode(serializer, "sound_profiles");
        for (const auto& entry : data.mSoundProfiles)
        {
            MetaNode profile = context.appendSubnode(profilesNode, entry.first);
            context.serializeProperty("count", entry.second.mSounds.size(), profile);
            for (const auto& bundle : entry.second.mSounds)
            {
                MetaNode soundNode = context.appendSubnode(profile, "s");
                context.serializeProperty("filepath", bundle->sound.getFilePath(), soundNode);
            }
        }

        //serialize sound volumes
        MetaNode volumesNode = context.appendSubnode(serializer, "sound_volumes");
        context.serializeProperty("count", data.mVolumeSettings.size(), volumesNode);
        for (const auto& volume : data.mVolumeSettings)
        {
            MetaNode volNode = context.appendSubnode(volumesNode, "v");
            context.serializeProperty("volume", volume, volNode);
        }
    }

    void DeserialBehavior<AudioManager>::deserialize(AudioManager& data, MetaNode deserializer, DeserializationContext& context)
    {
        /*MetaNode musicNode = deserializer.firstNode("music");
        if (musicNode)
        {
            data.initMusic(musicNode.getAttribute<std::size_t>("count", 0u));
            std::size_t i = 0;
            forEachSubnode(musicNode, [&data, &i] (MetaNode sub)
            {
                auto result = sub.getAttributes<std::string, float>({"filepath", ""}, {"volume",1.0f});
                data.loadMusic(std::get<0>(result), i);
                data.setMusicVolume(std::get<1>(result), i);
                ++i;
            }, "m");
        }*/
        MetaNode profilesNode = deserializer.firstNode("sound_profiles");
        if (profilesNode)
        {
            forEachSubnode(profilesNode, [&data] (MetaNode sub)
            {
                auto handle = data.initSoundProfile(sub.name());
                data.initSounds(handle, sub.getAttribute<std::size_t>("count", 0u));
                std::size_t i = 0;
                forEachSubnode(sub, [&data, &handle, &i] (MetaNode soundNode)
                {
                    data.loadSound(handle, soundNode.getAttribute<std::string>("filepath", ""), i);
                    ++i;
                });
            });
        }
        MetaNode volumesNode = deserializer.firstNode("sound_volumes");
        if (volumesNode)
        {
            data.initVolumeSettings(volumesNode.getAttribute<std::size_t>("count", 0u));
            std::size_t i = 0;
            forEachSubnode(volumesNode, [&data, &i] (MetaNode sub)
            {
                data.setVolume(i, sub.getAttribute<float>("volume", 1.0f));
                ++i;
            }, "m");
        }
    }



    void SerialBehavior<SoundEmitterComponent, Entity, const World&, const Application&>::serialize(const SoundEmitterComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
        context.serializeProperty("key", data.mProfile.getKey(), serializer);
    }

    void DeserialBehavior<SoundEmitterComponent, Entity, World&, const Application&>::deserialize(SoundEmitterComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
        std::string key = deserializer.getAttribute<std::string>("key", "");
        world.getAudioManager().connectProfile(data, key);
    }


    void SerialBehavior<MusicEmitterComponent, Entity, const World&, const Application&>::serialize(const MusicEmitterComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
        if (data.mMusicData.loaded)
            context.serializeProperty("file", data.mMusicData.filepath, serializer);

        if (data.mDistanceCap != MusicEmitterComponent::DEFAULT_DISTANCE_CAP)
            context.serializeProperty("dc", data.mDistanceCap, serializer);

        if (data.mVolume < 100.0f)
            context.serializeProperty("vol", data.mVolume, serializer);

        if (!data.mActive)
            context.serializeProperty("a", data.mActive, serializer);
    }

    void DeserialBehavior<MusicEmitterComponent, Entity, World&, const Application&>::deserialize(MusicEmitterComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
        auto result = deserializer.getAttributes<std::string, float, float, bool>(
            {"file", ""}, {"dc", MusicEmitterComponent::DEFAULT_DISTANCE_CAP}, {"vol", 100.0f}, {"a", true} );
        if (std::get<0>(result) != "")
            world.getAudioManager().getMusicEmitterMixer().loadMusic(e, std::get<0>(result));
        world.getAudioManager().getMusicEmitterMixer().setDistanceCap(e, std::get<1>(result));
        world.getAudioManager().getMusicEmitterMixer().setEmitterVolume(e, std::get<2>(result));
        world.getAudioManager().getMusicEmitterMixer().setEmitterActive(e, std::get<3>(result));
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// visuals ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SerialBehavior<VisualsComponent, Entity, const World&, const Application&>::serialize(const VisualsComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
        context.serializeProperty("filepath", data.mImage.getFilePath(), serializer);
        if (!data.mVisible)
            context.serializeProperty("visible", data.mVisible, serializer);
        if (data.mHideForCamera)
            context.serializeProperty("hide4cam", data.mVisible, serializer);
    }

    void DeserialBehavior<VisualsComponent, Entity, World&, const Application&>::deserialize(VisualsComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
        auto result = deserializer.getAttributes<std::string, bool, bool>({"filepath", ""}, {"visible", true}, {"hide4cam", false});
        world.getVisualsManager().loadTexture(data, std::get<0>(result), LoadPolicy::ASYNC);
        world.getVisualsManager().setVisible(e, std::get<1>(result));
        world.getVisualsManager().setHideForCamera(e, std::get<2>(result));
    }

    void SerialBehavior<VertexArrayComponent, Entity, const World&, const Application&>::serialize(const VertexArrayComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
        context.serializeProperty("rect_count", data.mVertices.textureRectCount(), serializer);

        for (unsigned i = 0; i < data.mVertices.textureRectCount(); ++i)
        {
            MetaNode rectNode = context.appendSubnode(serializer, "texrect");

            if (i < data.mKeys.size() && !data.mKeys[i].empty())
            {
                context.serializeProperty("key", data.mKeys[i], rectNode);
            }
            else
            {
                sf::FloatRect texRect = data.mVertices.getTextureRect(i);
                context.serializeProperty("left", texRect.left, rectNode);
                context.serializeProperty("top", texRect.top, rectNode);
                context.serializeProperty("width", texRect.width, rectNode);
                context.serializeProperty("height", texRect.height, rectNode);
            }
            //positions may have rectangular form, or may not... serialization can be optimzed, if rectangular form is given
            if (data.mVertices.getPoint(i, 0).y == data.mVertices.getPoint(i, 1).y &&
                data.mVertices.getPoint(i, 2).y == data.mVertices.getPoint(i, 3).y &&
                data.mVertices.getPoint(i, 0).x == data.mVertices.getPoint(i, 3).x &&
                data.mVertices.getPoint(i, 2).x == data.mVertices.getPoint(i, 1).x)
            {
                context.serializeProperty("pos_1_x", data.mVertices.getPosition(i).x, rectNode);
                context.serializeProperty("pos_1_y", data.mVertices.getPosition(i).y, rectNode);
                //test if the rect is flipped
                if (data.mVertices.isFlippedX(i)) 
                    context.serializeProperty("flip_x", true, rectNode);
                if (data.mVertices.isFlippedY(i))
                    context.serializeProperty("flip_y", true, rectNode);
            }
            else
            {
                context.serializeProperty("pos_1_x", data.mVertices.getPoint(i, 0).x, rectNode);
                context.serializeProperty("pos_1_y", data.mVertices.getPoint(i, 0).y, rectNode);
                context.serializeProperty("pos_2_x", data.mVertices.getPoint(i, 1).x, rectNode);
                context.serializeProperty("pos_2_y", data.mVertices.getPoint(i, 1).y, rectNode);
                context.serializeProperty("pos_3_x", data.mVertices.getPoint(i, 2).x, rectNode);
                context.serializeProperty("pos_3_y", data.mVertices.getPoint(i, 2).y, rectNode);
                context.serializeProperty("pos_4_x", data.mVertices.getPoint(i, 3).x, rectNode);
                context.serializeProperty("pos_4_y", data.mVertices.getPoint(i, 3).y, rectNode);
            }
            //serialize color, if we got a color unequal to white... ommit serial, if its white
            if (data.mVertices.getRectColor(i) != sf::Color::White)
            {
                context.serializeProperty("r", data.mVertices.getRectColor(i).r, rectNode);
                context.serializeProperty("g", data.mVertices.getRectColor(i).g, rectNode);
                context.serializeProperty("b", data.mVertices.getRectColor(i).b, rectNode);
                context.serializeProperty("a", data.mVertices.getRectColor(i).a, rectNode);
            }
        }
    }

    void DeserialBehavior<VertexArrayComponent, Entity, World&, const Application&>::deserialize(VertexArrayComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
        auto result = deserializer.getAttributes<unsigned>({"rect_count", 0});

        world.getVisualsManager().initTextureRects(e, std::get<0>(result));

        unsigned i = 0;
        forEachSubnode(deserializer, [&] (MetaNode texRectNode)
        {
            auto keyAttr = texRectNode.firstAttribute("key");

            sf::IntRect texrect;

            if (keyAttr && e.has<SpriteMetadataComponent>())
            {
                std::string key = keyAttr.value();
                sf::IntRect texrect = e.get<SpriteMetadataComponent>().getTexrect(key);
                if (texrect.left == 0 && texrect.top == 0 && texrect.width == 0 && texrect.height == 0)
                {
                    ungod::Logger::warning("Got texrect with dimensions {0,0,0,0} from metadata ");
                    ungod::Logger::warning(e.get<SpriteMetadataComponent>().getFilePath());
                    ungod::Logger::endl();
                }
                auto rectData = texRectNode.getAttributes<float, float, bool, bool>({"pos_1_x",0.0f}, {"pos_1_y",0.0f}, { "flip_x", false }, { "flip_y", false });
                auto rectData2 = texRectNode.getAttributes<float, float, float, float, float, float, uint8_t, uint8_t, uint8_t, uint8_t>
                    ({"pos_2_x", std::get<0>(rectData) + texrect.width},
                     {"pos_2_y", std::get<1>(rectData)},
                     {"pos_3_x", std::get<0>(rectData) + texrect.width},
                     {"pos_3_y", std::get<1>(rectData) + texrect.height},
                     {"pos_4_x", std::get<0>(rectData)},
                     {"pos_4_y", std::get<1>(rectData) + texrect.height},
                     {"r", sf::Color::White.r},
                     {"g", sf::Color::White.g},
                     {"b", sf::Color::White.b},
                     {"a", sf::Color::White.a});

                world.getVisualsManager().setArrayTextureRect(e, i, key);
                world.getVisualsManager().setPoints(e, data, i,
                                                    { std::get<0>(rectData), std::get<1>(rectData) },
                                                    { std::get<0>(rectData2), std::get<1>(rectData2) },
                                                    { std::get<2>(rectData2), std::get<3>(rectData2) },
                                                    { std::get<4>(rectData2), std::get<5>(rectData2) });
                world.getVisualsManager().setArrayRectColor(e, sf::Color{std::get<6>(rectData2), std::get<7>(rectData2), std::get<8>(rectData2), std::get<9>(rectData2)}, i);
                if (std::get<2>(rectData))
                    world.getVisualsManager().flipVertexX(data, i);
                if (std::get<3>(rectData))
                    world.getVisualsManager().flipVertexY(data, i);
            }
            else
            {
                auto rectData = texRectNode.getAttributes<int, int, int, int, float, float, bool, bool>
                    ({"left",0}, {"top",0}, {"width",0}, {"height",0}, {"pos_1_x",0.0f}, {"pos_1_y",0.0f}, { "flip_x", false }, { "flip_y", false });

                auto rectData2 = texRectNode.getAttributes<float, float, float, float, float, float, uint8_t, uint8_t, uint8_t, uint8_t>
                    ({"pos_2_x", std::get<4>(rectData) + std::get<2>(rectData)},
                     {"pos_2_y", std::get<5>(rectData)},
                     {"pos_3_x", std::get<4>(rectData) + std::get<2>(rectData)},
                     {"pos_3_y", std::get<5>(rectData) + std::get<3>(rectData)},
                     {"pos_4_x", std::get<4>(rectData)},
                     {"pos_4_y", std::get<5>(rectData) + std::get<3>(rectData)},
                     {"r", sf::Color::White.r},
                     {"g", sf::Color::White.g},
                     {"b", sf::Color::White.b},
                     {"a", sf::Color::White.a});

                world.getVisualsManager().setArrayTextureRect(data, { (float)std::get<0>(rectData), (float)std::get<1>(rectData), (float)std::get<2>(rectData), (float)std::get<3>(rectData) }, i);
                world.getVisualsManager().setPoints(e, data, i,
                                                    { std::get<4>(rectData), std::get<5>(rectData) },
                                                    { std::get<0>(rectData2), std::get<1>(rectData2) },
                                                    { std::get<2>(rectData2), std::get<3>(rectData2) },
                                                    { std::get<4>(rectData2), std::get<5>(rectData2) });
                world.getVisualsManager().setArrayRectColor(e, sf::Color{std::get<6>(rectData2), std::get<7>(rectData2), std::get<8>(rectData2), std::get<9>(rectData2)}, i);
                if (std::get<2>(rectData))
                    world.getVisualsManager().flipVertexX(data, i);
                if (std::get<3>(rectData))
                    world.getVisualsManager().flipVertexY(data, i);
            }

            //dont forget to increment!
            ++i;

        }, "texrect");
    }

    void SerialBehavior<SpriteComponent, Entity, const World&, const Application&>::serialize(const SpriteComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application& world)
    {
        if (!data.mKey.empty())
        {
            context.serializeProperty("key", data.mKey, serializer);
        }
        else
        {
            context.serializeProperty("tex_x", (int)(data.mSprite.getTextureRect().left), serializer);
            context.serializeProperty("tex_y", (int)(data.mSprite.getTextureRect().top), serializer);
            context.serializeProperty("size_x", (int)(data.mSprite.getTextureRect().width), serializer);
            context.serializeProperty("size_y", (int)(data.mSprite.getTextureRect().height), serializer);
        }

        if (data.mSprite.getPosition().x != 0.0f)
            context.serializeProperty("pos_x", data.mSprite.getPosition().x, serializer);
        if (data.mSprite.getPosition().y != 0.0f)
            context.serializeProperty("pos_y", data.mSprite.getPosition().y, serializer);

        if (data.mSprite.getScale().x != 1.0f)
            context.serializeProperty("scale_x", data.mSprite.getScale().x, serializer);
        if (data.mSprite.getScale().y != 1.0f)
            context.serializeProperty("scale_y", data.mSprite.getScale().y, serializer);
        if (data.mSprite.getRotation() != 0.0f)
            context.serializeProperty("rotation", data.mSprite.getRotation(), serializer);

        if (data.mSprite.getOrigin().x != 0.0f)
            context.serializeProperty("origin_x", data.mSprite.getOrigin().x, serializer);
        if (data.mSprite.getOrigin().y != 0.0f)
            context.serializeProperty("origin_y", data.mSprite.getOrigin().y, serializer);

        if (data.mSprite.isFlippedX())
            context.serializeProperty("flip_x", true, serializer);
        if (data.mSprite.isFlippedY())
            context.serializeProperty("flip_y", true, serializer);
    }

    void DeserialBehavior<SpriteComponent, Entity, World&, const Application&>::deserialize(SpriteComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
        auto keyAttr = deserializer.firstAttribute("key");

        if (keyAttr && e.has<SpriteMetadataComponent>())
        {
            world.getVisualsManager().setSpriteTextureRect(e, data, e.get<SpriteMetadataComponent>(), keyAttr.value() );
        }
        else
        {
            auto result = deserializer.getAttributes<float, float, float, float>(
                     {"tex_x", 0},  {"tex_y", 0}, {"size_x", 0},  {"size_y", 0} );
            if (std::get<0>(result) == 0 && std::get<1>(result) == 0 && std::get<2>(result) == 0 && std::get<3>(result) == 0)
            {
                ungod::Logger::warning("Got texrect with dimensions {0,0,0,0} when deserializing a sprite.");
                ungod::Logger::endl();
            }
            world.getVisualsManager().setSpriteTextureRect(e, data, sf::FloatRect( std::get<0>(result), std::get<1>(result),
                                        std::get<2>(result), std::get<3>(result)) );
        }

        auto result = deserializer.getAttributes<float, float, float, float, float,float, float,bool,bool>(
                     {"pos_x", 0.0f},  {"pos_y", 0.0f}, {"scale_x", 1.0f}, {"scale_y", 1.0f}, {"rotation", 0.0f},
                     {"origin_x", 0.0f}, {"origin_y", 0.0f}, {"flip_x", false}, {"flip_y", false}  );
        world.getVisualsManager().setSpritePosition(e, data, sf::Vector2f(std::get<0>(result), std::get<1>(result)));
        world.getVisualsManager().setScale(e, data, {std::get<2>(result), std::get<3>(result)});
        world.getVisualsManager().setRotation(e, data, std::get<4>(result));
        world.getVisualsManager().setOrigin(e, data, sf::Vector2f(std::get<5>(result), std::get<6>(result)));
        if (std::get<7>(result))
            world.getVisualsManager().flipSpriteX(data);
        if (std::get<8>(result))
            world.getVisualsManager().flipSpriteY(data);
    }

    void SerialBehavior<VisualAffectorComponent, Entity, const World&, const Application&>::serialize(const VisualAffectorComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application& world)
    {
        context.serializeProperty("active", data.isActive(), serializer);
    }

    void DeserialBehavior<VisualAffectorComponent, Entity, World&, const Application&>::deserialize(VisualAffectorComponent& data, MetaNode deserializer, DeserializationContext& context, Entity, World& world, const Application&)
    {
        data.setActive(deserializer.getAttribute<bool>("active", false));
    }

    void SerialBehavior<SpriteMetadataComponent, Entity, const World&, const Application&>::serialize(const SpriteMetadataComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
        context.serializeProperty("filepath", data.mMeta.getFilePath(), serializer);
    }

    void DeserialBehavior<SpriteMetadataComponent, Entity, World&, const Application&>::deserialize(SpriteMetadataComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
        auto filepath = deserializer.getAttribute<std::string>("filepath", "");
        world.getVisualsManager().loadMetadata(e, filepath);
    }

    void SerialBehavior<AnimationComponent, Entity, const World&, const Application&>::serialize(const AnimationComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
        context.serializeProperty("running", data.mAnimation.isRunning(), serializer);
        context.serializeProperty("key", data.mAnimation.getKey(), serializer);
    }

    void DeserialBehavior<AnimationComponent, Entity, World&, const Application&>::deserialize(AnimationComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
        auto result = deserializer.getAttributes<bool, std::string>
                ({ "running", false }, {"key", ""});
        world.getVisualsManager().setAnimationState(e, std::get<1>(result));
        world.getVisualsManager().setRunning(e, std::get<0>(result));
    }

    void SerialBehavior<BigSpriteComponent, Entity, const World&, const Application&>::serialize(const BigSpriteComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
        context.serializeProperty("filepath", data.mBigImage.getFilePath(), serializer);
        if (!data.mVisible)
            context.serializeProperty("visible", data.mVisible, serializer);


        if (data.mBigSprite.getPosition().x != 0.0f)
            context.serializeProperty("pos_x", data.mBigSprite.getPosition().x, serializer);
        if (data.mBigSprite.getPosition().y != 0.0f)
            context.serializeProperty("pos_y", data.mBigSprite.getPosition().y, serializer);

        if (data.mBigSprite.getScale().x != 1.0f)
            context.serializeProperty("scale_x", data.mBigSprite.getScale().x, serializer);
        if (data.mBigSprite.getScale().y != 1.0f)
            context.serializeProperty("scale_y", data.mBigSprite.getScale().y, serializer);
        if (data.mBigSprite.getRotation() != 0.0f)
            context.serializeProperty("rotation", data.mBigSprite.getRotation(), serializer);

        if (data.mBigSprite.getOrigin().x != 0.0f)
            context.serializeProperty("origin_x", data.mBigSprite.getOrigin().x, serializer);
        if (data.mBigSprite.getOrigin().y != 0.0f)
            context.serializeProperty("origin_y", data.mBigSprite.getOrigin().y, serializer);

        //serialize color, if we got a color unequal to white... ommit serial, if its white
        if (data.mBigSprite.getColor() != sf::Color::White)
        {
            context.serializeProperty("r", data.mBigSprite.getColor().r, serializer);
            context.serializeProperty("g", data.mBigSprite.getColor().g, serializer);
            context.serializeProperty("b", data.mBigSprite.getColor().b, serializer);
            context.serializeProperty("a", data.mBigSprite.getColor().a, serializer);
        }
    }

    void DeserialBehavior<BigSpriteComponent, Entity, World&, const Application&>::deserialize(BigSpriteComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
        auto result = deserializer.getAttributes<std::string, bool, float, float, float, float, float,float, float>(
                    {"filepath", ""}, {"visible", true},
                     {"pos_x", 0.0f},  {"pos_y", 0.0f}, {"scale_x", 1.0f}, {"scale_y", 1.0f}, {"rotation", 0.0f},
                     {"origin_x", 0.0f}, {"origin_y", 0.0f}  );
        world.getVisualsManager().loadBigTexture(e, data, std::get<0>(result), LoadPolicy::ASYNC);
        world.getVisualsManager().setBigSpriteVisibility(data, std::get<1>(result));
        world.getVisualsManager().setBigSpritePosition(e, data, sf::Vector2f(std::get<2>(result), std::get<3>(result)));
        world.getVisualsManager().setBigSpriteScale(e, data, {std::get<4>(result), std::get<5>(result)});
        world.getVisualsManager().setBigSpriteRotation(e, data, std::get<6>(result));
        world.getVisualsManager().setBigSpriteOrigin(e, data, sf::Vector2f(std::get<7>(result), std::get<8>(result)));
    }


    void SerialBehavior<TileMapComponent, Entity, const World&, const Application&>::serialize(const TileMapComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application& app)
    {
        context.serializeObject("tilemap", data.getTileMap(), serializer);
    }

    void DeserialBehavior<TileMapComponent, Entity, World&, const Application&>::deserialize(TileMapComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application& app)
    {
        auto deserial = [&](TileMap& tm) mutable
        {
            context.first(context.deserializeObject(tm), "tilemap", deserializer);
        };
        world.getTileMapRenderer().tilemapCallback(e, data, deserial);
    }


    void SerialBehavior<WaterComponent, Entity, const World&, const Application&>::serialize(const WaterComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application& app)
    {
        context.serializeObject("water", data.getWater(), serializer, static_cast<const sf::RenderTarget&>(app.getWindow()));
    }

    void DeserialBehavior<WaterComponent, Entity, World&, const Application&>::deserialize(WaterComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application& app)
    {
        context.first(context.deserializeObject(data.mWater, static_cast<const sf::RenderTarget&>(app.getWindow())), "water", deserializer);

        auto deserial = [&](Water& w) mutable
        {
            context.first(context.deserializeObject(w, static_cast<const sf::RenderTarget&>(app.getWindow())), "water", deserializer);
        };
        world.getTileMapRenderer().waterCallback(e, data, deserial);
    }



    void SerialBehavior<ParticleSystemComponent, Entity, const World&, const Application&>::serialize(const ParticleSystemComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application& app)
    {
        context.serializeObject("ps", data.mParticleSystem.value(), serializer);
    }

    void DeserialBehavior<ParticleSystemComponent, Entity, World&, const Application&>::deserialize(ParticleSystemComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application& app)
    {
        context.first(context.deserializeObject(data.mParticleSystem.value()), "ps", deserializer);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// light ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SerialBehavior<ShadowEmitterComponent, Entity, const World&, const Application&>::serialize(const ShadowEmitterComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
        context.serializeProperty("light_over_shape", data.mLightCollider.getLightOverShape(), serializer);
        context.serializeProperty("point_count", data.mLightCollider.getPointCount(), serializer);
        for (unsigned i = 0; i < data.mLightCollider.getPointCount(); ++i)
        {
            MetaNode pointNode = context.appendSubnode(serializer, "point");
            context.serializeProperty("x", data.mLightCollider.getPoint(i).x, pointNode);
            context.serializeProperty("y", data.mLightCollider.getPoint(i).y, pointNode);
        }
    }

    void DeserialBehavior<ShadowEmitterComponent, Entity, World&, const Application&>::deserialize(ShadowEmitterComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
        auto result = deserializer.getAttributes<bool, std::size_t>( {"light_over_shape", false}, {"point_count", 0u});
        world.getLightSystem().setLightOverShape(data, std::get<0>(result));
        world.getLightSystem().setPointCount(e, data, std::get<1>(result));
        std::size_t i = 0;
        forEachSubnode(deserializer, [&] (MetaNode sub)
        {
            auto coords = sub.getAttributes<float, float>( {"x", 0.0f}, {"y", 0.0f});
            world.getLightSystem().setPoint(data, e, {std::get<0>(coords),std::get<1>(coords)}, i);
            ++i;
        }, "point");
    }

    void SerialBehavior<LightEmitterComponent, Entity, const World&, const Application&>::serialize(const LightEmitterComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
        context.serializeProperty("r", data.mLight.getColor().r, serializer);
        context.serializeProperty("g", data.mLight.getColor().g, serializer);
        context.serializeProperty("b", data.mLight.getColor().b, serializer);
        context.serializeProperty("tex", data.mLight.getImage().getFilePath(), serializer);
        context.serializeProperty("active", data.mLight.isActive(), serializer);
        context.serializeProperty("scale_x", data.mLight.getScale().x, serializer);
        context.serializeProperty("scale_y", data.mLight.getScale().y, serializer);
        context.serializeProperty("pos_x", data.mLight.getPosition().x, serializer);
        context.serializeProperty("pos_y", data.mLight.getPosition().y, serializer);
        context.serializeProperty("source_x", data.mLight.getSourcePoint().x, serializer);
        context.serializeProperty("source_y", data.mLight.getSourcePoint().y, serializer);
        context.serializeProperty("radius", data.mLight.getRadius(), serializer);
        context.serializeProperty("shadow_ext_mult", data.mLight.getShadowExtendMultiplier(), serializer);
    }

    void DeserialBehavior<LightEmitterComponent, Entity, World&, const Application&>::deserialize(LightEmitterComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
        auto result = deserializer.getAttributes<uint8_t, uint8_t, uint8_t, std::string, bool,
                                                 float, float, float, float, float, float, float, float>
                                    ( {"r", 255u}, {"g", 255u}, {"b", 255u},
                                      {"tex", PointLight::DEFAULT_TEXTURE_PATH},
                                      {"active", true},
                                      {"scale_x", 1.0f}, {"scale_y", 1.0f},
                                      {"pos_x", 0.0f}, {"pos_y", 0.0f},
                                      {"source_x", 0.0f}, {"source_y", PointLight::DEFAULT_RADIUS},
                                      {"radius", 1.0f}, {"shadow_ext_mult", PointLight::DEFAULT_SHADOW_EXTEND_MULTIPLIER});
        world.getLightSystem().setLightColor(data, { std::get<0>(result), std::get<1>(result), std::get<2>(result) });
        world.getLightSystem().loadLightTexture(data, std::get<3>(result));
        world.getLightSystem().setLightActive(data, std::get<4>(result));
        world.getLightSystem().setLightScale(data, e, sf::Vector2f{ std::get<5>(result), std::get<6>(result) });
        world.getLightSystem().setLocalLightPosition(data, e, sf::Vector2f{ std::get<7>(result), std::get<8>(result) });
        world.getLightSystem().setLightOrigin(data, e, sf::Vector2f{ std::get<9>(result), std::get<10>(result) });
        world.getLightSystem().setLightRadius(data, std::get<11>(result));
        world.getLightSystem().setShadowExtendMultiplier(data, std::get<12>(result));
    }

    void SerialBehavior<LightAffectorComponent, Entity, const World&, const Application&>::serialize(const LightAffectorComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {

    }

    void DeserialBehavior<LightAffectorComponent, Entity, World&, const Application&>::deserialize(LightAffectorComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {

    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// entity behavior ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SerialBehavior<Initializer, Entity, const World&, const Application&>::serialize(const Initializer& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
        context.serializeProperty("active", data.mActive, serializer);
        context.serializeProperty("id", data.mInitializerID, serializer);
    }

    void DeserialBehavior<Initializer, Entity, World&, const Application&>::deserialize(Initializer& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
        auto result = deserializer.getAttributes<bool, std::size_t>( {"active", false}, {"id", 0u} );
        data.mActive = std::get<0>(result);
        if (data.mActive)
            world.getInitializerManager().initialize(e, std::get<1>(result), world);
    }


    void SerialBehavior<EntityBehaviorManager, Entity, const World&, const Application&>::serialize(const EntityBehaviorManager& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
    }

    void DeserialBehavior<EntityBehaviorManager, Entity, World&, const Application&>::deserialize(EntityBehaviorManager& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
    }

    void SerialBehavior<EntityBehaviorComponent, Entity, const World&, const Application&>::serialize(const EntityBehaviorComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
    }

    void DeserialBehavior<EntityBehaviorComponent, Entity, World&, const Application&>::deserialize(EntityBehaviorComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
    }

    void SerialBehavior<EntityUpdateTimer, Entity, const World&, const Application&>::serialize(const EntityUpdateTimer& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
    }

    void DeserialBehavior<EntityUpdateTimer, Entity, World&, const Application&>::deserialize(EntityUpdateTimer& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
    }





    void SerialBehavior<ParentComponent, Entity, const World&, const Application&>::serialize(const ParentComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
    }

    void DeserialBehavior<ParentComponent, Entity, World&, const Application&>::deserialize(ParentComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
    }


    void SerialBehavior<ChildComponent, Entity, const World&, const Application&>::serialize(const ChildComponent& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
    }

    void DeserialBehavior<ChildComponent, Entity, World&, const Application&>::deserialize(ChildComponent& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
    }
}
