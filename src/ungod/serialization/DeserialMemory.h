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
#ifndef UNGOD_DESERIAL_MEMORY_H
#define UNGOD_DESERIAL_MEMORY_H

#include <forward_list>
#include <pair>
#include <string>

namespace ungod
{
	class Entity;

	namespace detail
	{
		struct EntityScriptPair
		{
			EntityScriptPair(Entity e, const std::string& s) : entity(e), script(s) {}

			Entity entity;
			std::string script;
		};

		struct EntityDeserialNote
		{
			EntityDeserialNote(Entity e, MetaNode n, DeserializationContext& c) : entity(e), node(n), context(c) {}

			Entity entity;
			MetaNode node;
			DeserializationContext& context;
		};
	}

	/** \brief A class that memorizes things during the deserialization of a
	* render layer container. After deserialization, finalize can be called to 
	* perform all queued actions that can not run asycronously. */
	struct DeserialMemory
	{
		std::forward_list<detail::EntityDeserialNote> mAllDeserial;
		std::forward_list<detail::EntityScriptPair> mScriptEntities;

		void notifyDeserial(Entity e, MetaNode serializer, DeserializationContext& context);

		void notifyScriptedEntity(Entity e, const std::string& scriptname);
	};
}

#endif // !UNGOD_DESERIAL_MEMORY_H
