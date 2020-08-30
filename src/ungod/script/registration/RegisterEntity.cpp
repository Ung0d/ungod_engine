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

#include "ungod/script/Registration.h"
#include "ungod/physics/CollisionContexts.h"
#include "ungod/base/World.h"
#include "ungod/base/Utility.h"

namespace ungod
{
	namespace scriptRegistration
	{
		Has hasser(Entity e) { return Has(e); }
		Add adder(Entity e) { return Add(e); }
		Rem remmer(Entity e) { return Rem(e); }

		void registerEntity(ScriptStateBase& state)
		{
			script::Usertype<Entity> entityType = state.registerUsertype<Entity>("Entity");
			entityType["valid"] = &Entity::valid;
			entityType["isSolid"] = &Entity::isSolid;
			entityType["isStatic"] = &Entity::isStatic;
			entityType["getWorld"] = &Entity::getWorld;
			entityType["has"] = hasser;
			entityType["add"] = adder;
			entityType["remove"] = remmer;
			entityType["transform"] = [](Entity& e) { return TransformHandlerFrontEnd{ e, e.getWorld().getTransformHandler() }; };
			entityType["behavior"] = [](Entity& e) { return EntityBehaviorHandlerFrontEnd{ e, e.getWorld().getBehaviorHandler() }; };
			entityType["sound"] = [](Entity& e) { return SoundHandlerFrontEnd{ e, e.getWorld().getSoundHandler() }; };
			entityType["music"] = [](Entity& e) { return MusicEmitterFrontEnd{ e, e.getWorld().getMusicEmitterMixer() }; };
			entityType["semanticsRigidbody"] = [](Entity& e) { return RigidbodyHandlerFrontEnd<SEMANTICS_COLLISION_CONTEXT>{ e, e.getWorld().getSemanticsRigidbodyHandler() }; };
			entityType["movementRigidbody"] = [](Entity& e) { return RigidbodyHandlerFrontEnd<MOVEMENT_COLLISION_CONTEXT>{ e, e.getWorld().getMovementRigidbodyHandler() }; };
			entityType["movement"] = [](Entity& e) { return MovementHandlerFrontEnd{ e, e.getWorld().getMovementHandler() }; };
			entityType["steering"] = [](Entity& e) { return SteeringHandlerFrontEnd<script::Environment>{ e, e.getWorld().getSteeringHandler() }; };
			entityType["path"] = [](Entity& e) { return PathPlannerFrontEnd{ e, e.getWorld().getPathPlanner() }; };
			entityType["light"] = [](Entity& e) { return LightHandlerFrontEnd{ e, e.getWorld().getLightHandler() }; };
			entityType["visuals"] = [](Entity& e) { return VisualHandlerFrontEnd{ e, e.getWorld().getVisualsHandler() }; };
			entityType["tilemap"] = [](Entity& e) { return TileMapHandlerFrontEnd{ e, e.getWorld().getTileMapHandler() }; };
			entityType["water"] = [](Entity& e) { return WaterHandlerFrontEnd{ e, e.getWorld().getWaterHandler() }; };
			entityType["particleSystem"] = [](Entity& e) { return ParticleSystemHandlerFrontEnd{ e, e.getWorld().getParticleSystemHandler() }; };

			script::Usertype<Has> hasType = state.registerUsertype<Has>("Has");
			hasType["transform"] = &Has::has<TransformComponent>;
			hasType["behavior"] = &Has::has<EntityBehaviorComponent>;
			hasType["updateTimer"] = &Has::has<EntityUpdateTimer>;
			hasType["sound"] = &Has::has<SoundEmitterComponent>;
			hasType["music"] = &Has::has<MusicEmitterComponent>;
			hasType["movementRigidbody"] = &Has::has<RigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>;
			hasType["semanticsRigidbody"] = &Has::has<RigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>>;
			hasType["multiMovementRigidbody"] = &Has::has<MultiRigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>;
			hasType["multiSemanticsRigidbody"] = &Has::has<MultiRigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>>;
			hasType["movement"] = &Has::has<MovementComponent>;
			hasType["steering"] = &Has::has<SteeringComponent<script::Environment>>;
			hasType["path"] = &Has::has<PathFinderComponent>;
			hasType["shadows"] = &Has::has<ShadowEmitterComponent>;
			hasType["light"] = &Has::has<LightEmitterComponent>;
			hasType["lightAffector"] = &Has::has<LightAffectorComponent>;
			hasType["multiShadows"] = &Has::has<MultiShadowEmitter>;
			hasType["multiLight"] = &Has::has<MultiLightEmitter>;
			hasType["multiLightAffector"] = &Has::has<MultiLightAffector>;
			hasType["visuals"] = &Has::has<VisualsComponent>;
			hasType["sprite"] = &Has::has<SpriteComponent>;
			hasType["vertexArray"] = &Has::has<VertexArrayComponent>;
			hasType["spriteMetadata"] = &Has::has<SpriteMetadataComponent>;
			hasType["animation"] = &Has::has<AnimationComponent>;
			hasType["bigSprite"] = &Has::has<BigSpriteComponent>;
			hasType["multiSprite"] = &Has::has<MultiSpriteComponent>;
			hasType["multiAnimation"] = &Has::has<MultiAnimationComponent>;
			hasType["multiVisualAffetor"] = &Has::has<MultiVisualAffectorComponent>;
			hasType["tilemap"] = &Has::has<TileMapComponent>;
			hasType["water"] = &Has::has<WaterComponent>;
			hasType["particleSystem"] = &Has::has<ParticleSystemComponent>;

			script::Usertype<Add> addType = state.registerUsertype<Add>("Add");
			addType["transform"] = &Add::add<TransformComponent>;
			addType["behavior"] = &Add::add<EntityBehaviorComponent>;
			addType["updateTimer"] = sol::overload(&Add::add<EntityUpdateTimer>,
				[](Add& e, float interval) { e.addWithInstantiator<EntityUpdateTimer>(e.entity.instantiate<EntityUpdateTimer>(interval)); });
			addType["sound"] = sol::overload(&Add::add<SoundEmitterComponent>,
				[](Add& e, ProfileHandle h) { e.addWithInstantiator<SoundEmitterComponent>(e.entity.instantiate<SoundEmitterComponent>(h)); });
			addType["music"] = &Add::add<MusicEmitterComponent>;
			addType["movementRigidbody"] = [](Add& e) { e.add<RigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>(); };
			addType["semanticsRigidbody"] = &Add::add<RigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>>;
			addType["multiMovementRigidbody"] = &Add::addMulti<MultiRigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>;
			addType["multiSemanticsRigidbody"] = &Add::addMulti<MultiRigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>>;
			addType["movement"] = &Add::add<MovementComponent>;
			addType["steering"] = &Add::add<SteeringComponent<script::Environment>>;
			addType["path"] = &Add::add<PathFinderComponent>;
			addType["shadows"] = &Add::add<ShadowEmitterComponent>;
			addType["light"] = &Add::add<LightEmitterComponent>;
			addType["lightAffector"] = &Add::add<LightAffectorComponent>;
			addType["multiShadowEmitter"] = &Add::addMulti<MultiShadowEmitter>;
			addType["multiLightEmitter"] = &Add::addMulti<MultiLightEmitter>;
			addType["multiLightAffector"] = &Add::addMulti<MultiLightAffector>;
			addType["visuals"] = &Add::add<VisualsComponent>;
			addType["sprite"] = &Add::add<SpriteComponent>;
			addType["vertexArray"] = &Add::add<VertexArrayComponent>;
			addType["visualAffector"] = &Add::add<VisualAffectorComponent>;
			addType["spriteMetadata"] = &Add::add<SpriteMetadataComponent>;
			addType["animation"] = &Add::add<AnimationComponent>;
			addType["bigSprite"] = &Add::add<BigSpriteComponent>;
			addType["multiSprite"] = &Add::addMulti<MultiSpriteComponent>;
			addType["multiAnimation"] = &Add::addMulti<MultiAnimationComponent>;
			addType["multiVisualAffetor"] = &Add::addMulti<MultiVisualAffectorComponent>;
			addType["tilemap"] = &Add::add<TileMapComponent>;
			addType["water"] = &Add::add<WaterComponent>;
			addType["particleSystem"] = &Add::add<ParticleSystemComponent>;

			script::Usertype<Rem> remType = state.registerUsertype<Rem>("Remove");
			remType["transform"] = &Rem::rem<TransformComponent>;
			remType["behavior"] = &Rem::rem<EntityBehaviorComponent>;
			remType["updateTimer"] = &Rem::rem<EntityUpdateTimer>;
			remType["sound"] = &Rem::rem<SoundEmitterComponent>;
			remType["music"] = &Rem::rem<MusicEmitterComponent>;
			remType["movementRigidbody"] = &Rem::rem<RigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>;
			remType["semanticsRigidbody"] = &Rem::rem<RigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>>;
			remType["multiMovementRigidbody"] = &Rem::rem<MultiRigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>;
			remType["multiSemanticsRigidbody"] = &Rem::rem<MultiRigidbodyComponent<SEMANTICS_COLLISION_CONTEXT>>;
			remType["movement"] = &Rem::rem<MovementComponent>;
			remType["steering"] = &Rem::rem<SteeringComponent<script::Environment>>;
			remType["path"] = &Rem::rem<PathFinderComponent>;
			remType["shadows"] = &Rem::rem<ShadowEmitterComponent>;
			remType["light"] = &Rem::rem<LightEmitterComponent>;
			remType["lightAffector"] = &Rem::rem<LightAffectorComponent>;
			remType["multiShadowEmitter"] = &Rem::rem<MultiShadowEmitter>;
			remType["multiLightEmitter"] = &Rem::rem<MultiLightEmitter>;
			remType["multiLightAffector"] = &Rem::rem<MultiLightAffector>;
			remType["visuals"] = &Rem::rem<VisualsComponent>;
			remType["sprite"] = &Rem::rem<SpriteComponent>;
			remType["vertexArray"] = &Rem::rem<VertexArrayComponent>;
			remType["visualAffector"] = &Rem::rem<VisualAffectorComponent>;
			remType["spriteMetadata"] = &Rem::rem<SpriteMetadataComponent>;
			remType["animation"] = &Rem::rem<AnimationComponent>;
			remType["bigSprite"] = &Rem::rem<BigSpriteComponent>;
			remType["multiSprite"] = &Rem::rem<MultiSpriteComponent>;
			remType["multiAnimation"] = &Rem::rem<MultiAnimationComponent>;
			remType["multiVisualAffetor"] = &Rem::rem<MultiVisualAffectorComponent>;
			remType["tilemap"] = &Rem::rem<TileMapComponent>;
			remType["water"] = &Rem::rem<WaterComponent>;
			remType["particleSystem"] = &Rem::rem<ParticleSystemComponent>;

			state.registerFunction("isBelow", &ungod::isBelow);
		}
	}
}
