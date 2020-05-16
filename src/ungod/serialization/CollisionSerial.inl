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

#ifndef COLLISION_SERIAL_IMPL_H
#define COLLISION_SERIAL_IMPL_H

namespace ungod
{
    //define serialization behavior
    template <std::size_t CONTEXT>
    std::string SerialIdentifier<RigidbodyComponent<CONTEXT>>::get()  { return std::string("RB") + std::to_string(CONTEXT); }

    template <std::size_t CONTEXT>
    void SerialBehavior<RigidbodyComponent<CONTEXT>, Entity, const World&, const Application&>::serialize
		(const RigidbodyComponent<CONTEXT>& data, MetaNode serializer, SerializationContext& context, Entity, const World&, const Application&)
    {
		context.serializeProperty("a", data.isActive(), serializer);
		context.serializeProperty("t", static_cast<std::underlying_type_t<ColliderType>>(data.getCollider().getType()), serializer);
		for (unsigned i = 0; i < data.getCollider().getNumParam(); i++)
			context.serializeProperty("p"+std::to_string(i), data.getCollider().getParam(i), serializer);
    }

    template <std::size_t CONTEXT>
    void DeserialBehavior<RigidbodyComponent<CONTEXT>, Entity, World&, const Application&>::deserialize(RigidbodyComponent<CONTEXT>& data, MetaNode deserializer, DeserializationContext& context, Entity e, World& world, const Application&)
    {
		std::vector<float> param;
		param.reserve(Collider::MAX_PARAM);
		bool active;
		auto attr = context.first(context.deserializeProperty(active, false), "a", deserializer);
		std::underlying_type_t<ColliderType> type;
		attr = context.next(context.deserializeProperty(type, (std::underlying_type_t<ColliderType>)ColliderType::UNDEFINED), "t", deserializer, attr);
		float curp;
		int i = 0;
		attr = context.next(context.deserializeProperty(curp), "p0", deserializer, attr);
		while (attr)
		{
			param.emplace_back(curp);
			i++;
			attr = context.next(context.deserializeProperty(curp), "p"+std::to_string(i), deserializer, attr);
		}
		if constexpr (CONTEXT == MOVEMENT_COLLISION_CONTEXT)
		{
			world.getMovementRigidbodyHandler().setActive(e, active);
			world.getMovementRigidbodyHandler().addCollider(e, Collider{ static_cast<ColliderType>(type), param });
		}
		else if constexpr (CONTEXT == SEMANTICS_COLLISION_CONTEXT)
		{
			world.getSemanticsRigidbodyHandler().setActive(e, active);
			world.getSemanticsRigidbodyHandler().addCollider(e, Collider{ static_cast<ColliderType>(type), param });
		}
    }
}

#endif // COLLISION_SERIAL_IMPL_H
