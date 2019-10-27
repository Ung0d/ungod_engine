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

#include "ungod/serialization/DeserialInit.h"
#include "ungod/serialization/SerialParticleSystem.h"
#include "ungod/base/World.h"
#include "ungod/content/EntityTypes.h"

namespace ungod
{
    void initDeserial(DeserializationContext& context, World& world)
    {
        prepareParticleSystemDeserial(context, &world.getParticleSystemManager().getFunctorMaster());
    }


    void initContext(DeserializationContext& context)
    {
        context.changeStorageSemantics<deserial_ref_semantics::ByValue<Entity>>( SerialIdentifier< EntityInstantiation<EntityBaseComponents, EntityOptionalComponents> >::get() );
        context.changeStorageSemantics<deserial_ref_semantics::ByValue<Entity>>( SerialIdentifier< EntityInstantiation<ActorBaseComponents, ActorOptionalComponents> >::get() );
        context.changeStorageSemantics<deserial_ref_semantics::ByValue<Entity>>( SerialIdentifier< EntityInstantiation<MetaObjectBaseComponents, MetaObjectOptionalComponents> >::get() );
        context.changeStorageSemantics<deserial_ref_semantics::ByValue<Entity>>( SerialIdentifier< EntityInstantiation<LightBaseComponents, LightOptionalComponents> >::get() );
        context.changeStorageSemantics<deserial_ref_semantics::ByValue<Entity>>( SerialIdentifier< EntityInstantiation<TriggerBaseComponents, TriggerOptionalComponents> >::get() );
        context.changeStorageSemantics<deserial_ref_semantics::ByValue<Entity>>( SerialIdentifier< EntityInstantiation<WorldObjectBaseComponents, WorldObjectOptionalComponents> >::get() );
        context.changeStorageSemantics<deserial_ref_semantics::ByValue<Entity>>( SerialIdentifier< EntityInstantiation<TileMapBaseComponents,TileMapOptionalComponents> >::get() );
        context.changeStorageSemantics<deserial_ref_semantics::ByValue<Entity>>( SerialIdentifier< EntityInstantiation<WaterBaseComponents, WaterOptionalComponents> >::get() );
        context.changeStorageSemantics<deserial_ref_semantics::ByValue<Entity>>( SerialIdentifier< EntityInstantiation<ParticleSystemBaseComponents, ParticleSystemOptionalComponents> >::get() );
        context.changeStorageSemantics<deserial_ref_semantics::ByValue<Entity>>( SerialIdentifier< EntityInstantiation<AudioEmitterBaseComponents, AudioEmitterOptionalComponents> >::get() );
    }
}
