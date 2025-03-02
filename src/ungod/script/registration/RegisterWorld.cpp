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

#include "ungod/script/registration/RegisterWorld.h"
#include "ungod/content/EntityTypes.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerWorld(ScriptStateBase& state)
        {
			script::Usertype<World> worldType = state.registerUsertype<World>("World");
            // Factory methods to produce single entities.
			worldType["createEntity"] = sol::overload([](World& world) { return world.create(EntityBaseComponents(), EntityOptionalComponents()); },
				[](World& world, const std::string& script) { return world.create(EntityBaseComponents(), EntityOptionalComponents(), script); },
				[](World& world, const std::string& script, script::Environment param) { return world.create(EntityBaseComponents(), EntityOptionalComponents(), script, param); });
			worldType["createActor"] = sol::overload([](World& world) { return world.create(ActorBaseComponents(), ActorOptionalComponents()); },
				[](World& world, const std::string& script) { return world.create(ActorBaseComponents(), ActorOptionalComponents(), script); },
				[](World& world, const std::string& script, script::Environment param) { return world.create(ActorBaseComponents(), ActorOptionalComponents(), script, param); });
			worldType["createMetaObject"] = sol::overload([](World& world) { return world.create(MetaObjectBaseComponents(), MetaObjectOptionalComponents()); },
				[](World& world, const std::string& script)
				{ return world.create(MetaObjectBaseComponents(), MetaObjectOptionalComponents(), script); },
				[](World& world, const std::string& script, script::Environment param)
				{ return world.create(MetaObjectBaseComponents(), MetaObjectOptionalComponents(), script, param); });
			worldType["createLight"] = [](World& world) { return world.create(LightBaseComponents(), LightOptionalComponents()); },
			worldType["createTrigger"] = sol::overload([](World& world) { return world.create(TriggerBaseComponents(), TriggerOptionalComponents()); },
				[](World& world, const std::string& script)
				{ return world.create(TriggerBaseComponents(), TriggerOptionalComponents(), script); },
				[](World& world, const std::string& script, script::Environment param)
				{ return world.create(TriggerBaseComponents(), TriggerOptionalComponents(), script, param); });
			worldType["createWorldObject"] = sol::overload([](World& world) { return world.create(WorldObjectBaseComponents(), WorldObjectOptionalComponents()); },
				[](World& world, const std::string& script)
				{ return world.create(WorldObjectBaseComponents(), WorldObjectOptionalComponents(), script); },
				[](World& world, const std::string& script, script::Environment param)
				{ return world.create(WorldObjectBaseComponents(), WorldObjectOptionalComponents(), script, param); });
			worldType["createParticleSystem"] = [](World& world) { return world.create(ParticleSystemBaseComponents(), ParticleSystemOptionalComponents()); };
			worldType["createAudioEmitter"] = [](World& world) { return world.create(AudioEmitterBaseComponents(), AudioEmitterOptionalComponents()); };
			// Factory methods to produce sets of entities.
			worldType["createEntitys"] = &detail::entityCreator<EntityBaseComponents, EntityOptionalComponents>;
			worldType["createActors"] = &detail::entityCreator<ActorBaseComponents, ActorOptionalComponents>;
			worldType["createMetaObjects"] = &detail::entityCreator<MetaObjectBaseComponents, MetaObjectOptionalComponents>;
			worldType["createLights"] = &detail::entityCreator<LightBaseComponents, LightOptionalComponents>;
			worldType["createTriggers"] = &detail::entityCreator<TriggerBaseComponents, TriggerOptionalComponents>;
			worldType["createWorldObjects"] = &detail::entityCreator<WorldObjectBaseComponents, WorldObjectOptionalComponents>;
			worldType["createParticleSystems"] = &detail::entityCreator<ParticleSystemBaseComponents, ParticleSystemOptionalComponents>;
			worldType["createAudioEmitters"] = &detail::entityCreator<AudioEmitterBaseComponents, AudioEmitterOptionalComponents>;
			// Entity control methods.
			worldType["destroy"] = &World::destroy;
			worldType["remove"] = &World::remove;
			worldType["destroyNamed"] = &World::destroyNamed;
			worldType["makeCopy"] = &World::makeCopy;
			worldType["accomodateForeign"] = &World::accomodateForeign;
			worldType["add"] = &World::addEntity;
			worldType["addNearby"] = &World::addEntityNearby;
			worldType["setRenderDepth"] = &World::setRenderDepth;
			worldType["setName"] = &World::setName;
			worldType["getName"] = &World::getName;
			worldType["gamestate"] = &World::getState;
			worldType["tagWithName"] = &World::tagWithName;
			worldType["getEntityByName"] = &World::getEntityByName;
			worldType["getGraph"] = &World::getGraph;
			worldType["getNode"] = &World::getNode;
			worldType["queryNearby"] = sol::overload([](World& world, Entity e)
				{
					quad::PullResult<Entity> pull = world.getEntitiesNearby(e);
					return pull.getList();
				}, [](World& world, const sf::Vector2f& pos)
				{
					quad::PullResult<Entity> pull = world.getEntitiesNearby(pos);
					return pull.getList();
				});
			//handlers
			worldType["getLightHandler"] = [](World& w) -> LightHandler& { return w.getLightHandler();  };
			worldType["getParentChildHandler"] = [](World& w) -> ParentChildHandler& { return w.getParentChildHandler();  };
        }
    }
}

