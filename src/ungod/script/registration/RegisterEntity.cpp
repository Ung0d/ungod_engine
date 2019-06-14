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

#include "ungod/script/registration/RegisterEntity.h"
#include "ungod/physics/CollisionContexts.h"
#include "ungod/base/World.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerEntity(ScriptStateBase& state)
        {
            state.registerUsertype<Entity>("Entity",
                                        "valid", &Entity::valid,
                                        "isSolid", &Entity::isSolid,
                                        "isStatic", &Entity::isStatic,
                                        "getID", &Entity::getID,
                                        "has", [] (Entity& e) { return Has(e); },
                                        "get", [] (Entity& e) { return Get(e); },
                                        "add", [] (Entity& e) { return Add(e); },
                                        "remove", [] (Entity& e) { return Rem(e); });

                state.registerUsertype<Has>("Has",
                                            "Transform", &Has::has<TransformComponent>,
                                            "Behavior", &Has::has<EntityBehaviorComponent>,
                                            "SoundEmitter", &Has::has<SoundEmitterComponent>,
                                            "MovementRigidbody", &Has::has<RigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>,
                                            "SemanticsRigidbody", &Has::has<RigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>>,
                                            "Movement", &Has::has<MovementComponent>,
                                            "Steering", &Has::has<SteeringComponent<script::Environment>>,
                                            "PathFinder", &Has::has<PathFinderComponent>,
                                            "ShadowEmitter", &Has::has<ShadowEmitterComponent>,
                                            "LightEmitter", &Has::has<LightEmitterComponent>,
                                            "Visuals", &Has::has<VisualsComponent>,
                                            "Sprite", &Has::has<SpriteComponent>,
                                            "VertexArray", &Has::has<VertexArrayComponent>,
                                            "SpriteMetadata", &Has::has<SpriteMetadataComponent>,
                                            "Animation", &Has::has<AnimationComponent>,
                                            "BigSprite", &Has::has<BigSpriteComponent>,
                                            "MultiShadowEmitter", &Has::has<MultiShadowEmitter>,
                                            "MultiLightEmitter", &Has::has<MultiLightEmitter>,
                                            "MultiSprite", &Has::has<MultiSpriteComponent>,
                                            "MultiAnimation", &Has::has<MultiAnimationComponent>,
                                            "TileMap", &Has::has<TileMapComponent>,
                                            "Water", &Has::has<WaterComponent>,
                                            "ParticleSystem", &Has::has<ParticleSystemComponent>);


                state.registerUsertype<Get>("Get",
                                            "Transform", &Get::get<TransformComponent>,
                                            "Behavior", &Get::get<EntityBehaviorComponent>,
                                            "SoundEmitter", &Get::get<SoundEmitterComponent>,
                                            "MovementRigidbody", &Get::get<RigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>,
                                            "SemanticsRigidbody", &Get::get<RigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>>,
                                            "Movement", &Get::get<MovementComponent>,
                                            "Steering", &Get::get<SteeringComponent<script::Environment>>,
                                            "PathFinder", &Get::get<PathFinderComponent>,
                                            "ShadowEmitter", &Get::get<ShadowEmitterComponent>,
                                            "LightEmitter", &Get::get<LightEmitterComponent>,
                                            "Visuals", &Get::get<VisualsComponent>,
                                            "Sprite", &Get::get<SpriteComponent>,
                                            "VertexArray", &Get::get<VertexArrayComponent>,
                                            "SpriteMetadata", &Get::get<SpriteMetadataComponent>,
                                            "Animation", &Get::get<AnimationComponent>,
                                            "BigSprite", &Get::get<BigSpriteComponent>,
                                            "MultiShadowEmitter", &Get::get<MultiShadowEmitter>,
                                            "MultiLightEmitter", &Get::get<MultiLightEmitter>,
                                            "MultiSprite", &Get::get<MultiSpriteComponent>,
                                            "MultiAnimation", &Get::get<MultiAnimationComponent>,
                                            "TileMap", &Get::get<TileMapComponent>,
                                            "Water", &Get::get<WaterComponent>,
                                            "ParticleSystem", &Get::get<ParticleSystemComponent>);


                state.registerUsertype<Add>("Add",
                                            "Transform", [] (Add& e) { e.add<TransformComponent>(); },
                                            "Behavior", [] (Add& e) { e.add<EntityBehaviorComponent>(); },
                                            "UpdateTimer", sol::overload( [] (Add& e) { e.add<EntityUpdateTimer>(); },
                                                                              [] (Add& e, float interval) { e.add<EntityUpdateTimer>(e.entity.instantiate<EntityUpdateTimer>(interval)); }),
                                            "SoundEmitter", sol::overload( [] (Add& e) { e.add<SoundEmitterComponent>(); },
                                                                              [] (Add& e, ProfileHandle h) { e.add<SoundEmitterComponent>(e.entity.instantiate<SoundEmitterComponent>(h)); }),
                                            "MovementRigidbody", [] (Add& e) { e.add<RigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>(); },
                                            "SemanticsRigidbody", [] (Add& e) { e.add<RigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>>(); },
                                            "Movement", [] (Add& e) { e.add<MovementComponent>(); },
                                            "Steering", [] (Add& e) { e.add<SteeringComponent<script::Environment>>(); },
                                            "PathFinder", [] (Add& e) { e.add<PathFinderComponent>(); },
                                            "ShadowEmitter", [] (Add& e) { e.add<ShadowEmitterComponent>(); },
                                            "LightEmitter", [] (Add& e) { e.add<LightEmitterComponent>(); },
                                            "LightAffector", [] (Add& e) { e.add<LightAffectorComponent>(); },
                                            "Visuals", [] (Add& e) { e.add<VisualsComponent>(); },
                                            "Sprite", [] (Add& e) { e.add<SpriteComponent>(); },
                                            "VertexArray", [] (Add& e) { e.add<VertexArrayComponent>(); },
                                            "VisualAffector", [] (Add& e) { e.add<VisualAffectorComponent>(); },
                                            "SpriteMetadata", [] (Add& e) { e.add<SpriteMetadataComponent>(); },
                                            "Animation", [] (Add& e) { e.add<AnimationComponent>(); },
                                            "BigSprite", [] (Add& e) { e.add<BigSpriteComponent>(); },
                                            "TileMap", [] (Add& e) { e.add<TileMapComponent>(); },
                                            "Water", [] (Add& e) { e.add<WaterComponent>(); },
                                            "ParticleSystem", [] (Add& e) { e.add<ParticleSystemComponent>(); },
                                            "MultiShadowEmitter", [] (Add& e, std::size_t num)
                                            {
                                                e.add<MultiShadowEmitter>();
                                                e.entity.initMulti<MultiShadowEmitter>(num);
                                            },
                                            "MultiLightEmitter", [] (Add& e, std::size_t num)
                                            {
                                                e.add<MultiLightEmitter>();
                                                e.entity.initMulti<MultiLightEmitter>(num);
                                            },
                                            "MultiLightAffector", [] (Add& e, std::size_t num)
                                            {
                                                e.add<MultiLightAffector>();
                                                e.entity.initMulti<MultiLightAffector>(num);
                                            },
                                            "MultiSprite", [] (Add& e, std::size_t num)
                                            {
                                                e.add<MultiSpriteComponent>();
                                                e.entity.initMulti<MultiSpriteComponent>(num);
                                            },
                                            "MultiAnimation", [] (Add& e, std::size_t num)
                                            {
                                                e.add<MultiAnimationComponent>();
                                                e.entity.initMulti<MultiAnimationComponent>(num);
                                            },
                                            "MultiVisualAffetor", [] (Add& e, std::size_t num)
                                            {
                                                e.add<MultiVisualAffectorComponent>();
                                                e.entity.initMulti<MultiVisualAffectorComponent>(num);
                                            });


                state.registerUsertype<Rem>("Remove",
                                            "Transform", [] (Rem& e) { e.rem<TransformComponent>(); },
                                            "Behavior", [] (Rem& e) { e.rem<EntityBehaviorComponent>(); },
                                            "UpdateTimer", [] (Rem& e) { e.rem<EntityUpdateTimer>(); },
                                            "SoundEmitter", [] (Rem& e) { e.rem<SoundEmitterComponent>(); },
                                            "MovementRigidbody", [] (Rem& e) { e.rem<RigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>(); },
                                            "SemanticsRigidbody", [] (Rem& e) { e.rem<RigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>>(); },
                                            "Movement", [] (Rem& e) { e.rem<MovementComponent>(); },
                                            "Steering", [] (Rem& e) { e.rem<SteeringComponent<script::Environment>>(); },
                                            "PathFinder", [] (Rem& e) { e.rem<PathFinderComponent>(); },
                                            "ShadowEmitter", [] (Rem& e) { e.rem<ShadowEmitterComponent>(); },
                                            "LightEmitter", [] (Rem& e) { e.rem<LightEmitterComponent>(); },
                                            "LightAffector", [] (Rem& e) { e.rem<LightAffectorComponent>(); },
                                            "Visuals", [] (Rem& e) { e.rem<VisualsComponent>(); },
                                            "Sprite", [] (Rem& e) { e.rem<SpriteComponent>(); },
                                            "VertexArray", [] (Rem& e) { e.rem<VertexArrayComponent>(); },
                                            "VisualAffector", [] (Rem& e) { e.rem<VisualAffectorComponent>(); },
                                            "SpriteMetadata", [] (Rem& e) { e.rem<SpriteMetadataComponent>(); },
                                            "Animation", [] (Rem& e) { e.rem<AnimationComponent>(); },
                                            "BigSprite", [] (Rem& e) { e.rem<BigSpriteComponent>(); },
                                            "MultiShadowEmitter", [] (Rem& e) { e.rem<MultiShadowEmitter>(); },
                                            "MultiLightEmitter", [] (Rem& e) { e.rem<MultiLightEmitter>(); },
                                            "MultiLightAffector", [] (Rem& e) { e.rem<MultiLightAffector>(); },
                                            "MultiSprite", [] (Rem& e) { e.rem<MultiSpriteComponent>(); },
                                            "MultiAnimation",[] (Rem& e) { e.rem<MultiAnimationComponent>(); },
                                            "MultiVisualAffetor",[] (Rem& e) { e.rem<MultiVisualAffectorComponent>(); },
                                            "TileMap",[] (Rem& e) { e.rem<TileMapComponent>(); },
                                            "Water",[] (Rem& e) { e.rem<WaterComponent>(); },
                                            "ParticleSystem",[] (Rem& e) { e.rem<ParticleSystemComponent>(); });
        }
    }
}
