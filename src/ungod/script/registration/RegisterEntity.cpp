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
		Has hasser(Entity e) { return Has(e); }
		Get getter(Entity e) { return Get(e); }
		Add adder(Entity e) { return Add(e); }
		Rem remmer(Entity e) { return Rem(e); }

		void registerEntity(ScriptStateBase& state)
		{
			script::Usertype<Entity> entityType = state.registerUsertype<Entity>("Entity");
			entityType["valid"] = &Entity::valid;
			entityType["isSolid"] = &Entity::isSolid;
			entityType["isStatic"] = &Entity::isStatic;
			entityType["getID"] = &Entity::getID;
			entityType["has"] = hasser;
			entityType["get"] = getter;
			entityType["add"] = adder;
			entityType["remove"] = remmer;

			script::Usertype<Has> hasType = state.registerUsertype<Has>("Has");
			hasType["Transform"] = &Has::has<TransformComponent>;
			hasType["Behavior"] = &Has::has<EntityBehaviorComponent>;
			hasType["SoundEmitter"] = &Has::has<SoundEmitterComponent>;
			hasType["MovementRigidbody"] = &Has::has<RigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>;
			hasType["SemanticsRigidbody"] = &Has::has<RigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>>;
			hasType["MultiMovementRigidbody"] = &Has::has<MultiRigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>;
			hasType["MultiSemanticsRigidbody"] = &Has::has<MultiRigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>>;
			hasType["Movement"] = &Has::has<MovementComponent>;
			hasType["Steering"] = &Has::has<SteeringComponent<script::Environment>>;
			hasType["PathFinder"] = &Has::has<PathFinderComponent>;
			hasType["ShadowEmitter"] = &Has::has<ShadowEmitterComponent>;
			hasType["LightEmitter"] = &Has::has<LightEmitterComponent>;
			hasType["Visuals"] = &Has::has<VisualsComponent>;
			hasType["Sprite"] = &Has::has<SpriteComponent>;
			hasType["VertexArray"] = &Has::has<VertexArrayComponent>;
			hasType["SpriteMetadata"] = &Has::has<SpriteMetadataComponent>;
			hasType["Animation"] = &Has::has<AnimationComponent>;
			hasType["BigSprite"] = &Has::has<BigSpriteComponent>;
			hasType["MultiShadowEmitter"] = &Has::has<MultiShadowEmitter>;
			hasType["MultiLightEmitter"] = &Has::has<MultiLightEmitter>;
			hasType["MultiSprite"] = &Has::has<MultiSpriteComponent>;
			hasType["MultiAnimation"] = &Has::has<MultiAnimationComponent>;
			hasType["TileMap"] = &Has::has<TileMapComponent>;
			hasType["Water"] = &Has::has<WaterComponent>;
			hasType["ParticleSystem"] = &Has::has<ParticleSystemComponent>;


			script::Usertype<Get> getType = state.registerUsertype<Get>("Get");
			getType["Transform"] = &Get::get<TransformComponent>;
			getType["Behavior"] = &Get::get<EntityBehaviorComponent>;
			getType["SoundEmitter"] = &Get::get<SoundEmitterComponent>;
			getType["MovementRigidbody"] = &Get::get<RigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>;
			getType["SemanticsRigidbody"] = &Get::get<RigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>>;
			getType["MultiMovementRigidbody"] = &Get::get<MultiRigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>;
			getType["MultiSemanticsRigidbody"] = &Get::get<MultiRigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>>;
			getType["Movement"] = &Get::get<MovementComponent>;
			getType["Steering"] = &Get::get<SteeringComponent<script::Environment>>;
			getType["PathFinder"] = &Get::get<PathFinderComponent>;
			getType["ShadowEmitter"] = &Get::get<ShadowEmitterComponent>;
			getType["LightEmitter"] = &Get::get<LightEmitterComponent>;
			getType["Visuals"] = &Get::get<VisualsComponent>;
			getType["Sprite"] = &Get::get<SpriteComponent>;
			getType["VertexArray"] = &Get::get<VertexArrayComponent>;
			getType["SpriteMetadata"] = &Get::get<SpriteMetadataComponent>;
			getType["Animation"] = &Get::get<AnimationComponent>;
			getType["BigSprite"] = &Get::get<BigSpriteComponent>;
			getType["MultiShadowEmitter"] = &Get::get<MultiShadowEmitter>;
			getType["MultiLightEmitter"] = &Get::get<MultiLightEmitter>;
			getType["MultiSprite"] = &Get::get<MultiSpriteComponent>;
			getType["MultiAnimation"] = &Get::get<MultiAnimationComponent>;
			getType["TileMap"] = &Get::get<TileMapComponent>;
			getType["Water"] = &Get::get<WaterComponent>;
			getType["ParticleSystem"] = &Get::get<ParticleSystemComponent>;


			script::Usertype<Add> addType = state.registerUsertype<Add>("Add");
			addType["Transform"] = &Add::add<TransformComponent>;
			addType["Behavior"] = &Add::add<EntityBehaviorComponent>;
			addType["UpdateTimer"] = sol::overload(&Add::add<EntityUpdateTimer>,
				[](Add& e, float interval) { e.addWithInstantiator<EntityUpdateTimer>(e.entity.instantiate<EntityUpdateTimer>(interval)); });
			addType["SoundEmitter"] = sol::overload(&Add::add<SoundEmitterComponent>,
				[](Add& e, ProfileHandle h) { e.addWithInstantiator<SoundEmitterComponent>(e.entity.instantiate<SoundEmitterComponent>(h)); });
			addType["MovementRigidbody"] = [](Add& e) { e.add<RigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>(); };
			addType["SemanticsRigidbody"] = &Add::add<RigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>>;
			addType["MultiMovementRigidbody"] = &Add::addMulti<MultiRigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>;
			addType["MultiSemanticsRigidbody"] = &Add::addMulti<MultiRigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>>;
			addType["Movement"] = &Add::add<MovementComponent>;
			addType["Steering"] = &Add::add<SteeringComponent<script::Environment>>;
			addType["PathFinder"] = &Add::add<PathFinderComponent>;
			addType["ShadowEmitter"] = &Add::add<ShadowEmitterComponent>;
			addType["LightEmitter"] = &Add::add<LightEmitterComponent>;
			addType["LightAffector"] = &Add::add<LightAffectorComponent>;
			addType["Visuals"] = &Add::add<VisualsComponent>;
			addType["Sprite"] = &Add::add<SpriteComponent>;
			addType["VertexArray"] = &Add::add<VertexArrayComponent>;
			addType["VisualAffector"] = &Add::add<VisualAffectorComponent>;
			addType["SpriteMetadata"] = &Add::add<SpriteMetadataComponent>;
			addType["Animation"] = &Add::add<AnimationComponent>;
			addType["BigSprite"] = &Add::add<BigSpriteComponent>;
			addType["TileMap"] = &Add::add<TileMapComponent>;
			addType["Water"] = &Add::add<WaterComponent>;
			addType["ParticleSystem"] = &Add::add<ParticleSystemComponent>;
			addType["MultiShadowEmitter"] = &Add::addMulti<MultiShadowEmitter>;
			addType["MultiLightEmitter"] = &Add::addMulti<MultiLightEmitter>;
			addType["MultiLightAffector"] = &Add::addMulti<MultiLightAffector>;
			addType["MultiSprite"] = &Add::addMulti<MultiSpriteComponent>;
			addType["MultiAnimation"] = &Add::addMulti<MultiAnimationComponent>;
			addType["MultiVisualAffetor"] = &Add::addMulti<MultiVisualAffectorComponent>;


			script::Usertype<Rem> remType = state.registerUsertype<Rem>("Remove");
			remType["Transform"] = &Rem::rem<TransformComponent>;
			remType["Behavior"] = &Rem::rem<EntityBehaviorComponent>;
			remType["UpdateTimer"] = &Rem::rem<EntityUpdateTimer>;
			remType["SoundEmitter"] = &Rem::rem<SoundEmitterComponent>;
			remType["MovementRigidbody"] = &Rem::rem<RigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>;
			remType["SemanticsRigidbody"] = &Rem::rem<RigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>>;
			remType["MultiMovementRigidbody"] = &Rem::rem<MultiRigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>;
			remType["MultiSemanticsRigidbody"] = &Rem::rem<MultiRigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>>;
			remType["Movement"] = &Rem::rem<MovementComponent>;
			remType["Steering"] = &Rem::rem<SteeringComponent<script::Environment>>;
			remType["PathFinder"] = &Rem::rem<PathFinderComponent>;
			remType["ShadowEmitter"] = &Rem::rem<ShadowEmitterComponent>;
			remType["LightEmitter"] = &Rem::rem<LightEmitterComponent>;
			remType["LightAffector"] = &Rem::rem<LightAffectorComponent>;
			remType["Visuals"] = &Rem::rem<VisualsComponent>;
			remType["Sprite"] = &Rem::rem<SpriteComponent>;
			remType["VertexArray"] = &Rem::rem<VertexArrayComponent>;
			remType["VisualAffector"] = &Rem::rem<VisualAffectorComponent>;
			remType["SpriteMetadata"] = &Rem::rem<SpriteMetadataComponent>;
			remType["Animation"] = &Rem::rem<AnimationComponent>;
			remType["BigSprite"] = &Rem::rem<BigSpriteComponent>;
			remType["MultiShadowEmitter"] = &Rem::rem<MultiShadowEmitter>;
			remType["MultiLightEmitter"] = &Rem::rem<MultiLightEmitter>;
			remType["MultiLightAffector"] = &Rem::rem<MultiLightAffector>;
			remType["MultiSprite"] = &Rem::rem<MultiSpriteComponent>;
			remType["MultiAnimation"] = &Rem::rem<MultiAnimationComponent>;
			remType["MultiVisualAffetor"] = &Rem::rem<MultiVisualAffectorComponent>;
			remType["TileMap"] = &Rem::rem<TileMapComponent>;
			remType["Water"] = &Rem::rem<WaterComponent>;
			remType["ParticleSystem"] = &Rem::rem<ParticleSystemComponent>;
		}
	}
}
