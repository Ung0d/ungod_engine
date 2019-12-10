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

#ifndef MULTI_COMPONENT_H
#define MULTI_COMPONENT_H

#include "dom/dom.h"
#include "ungod/serialization/Serializable.h"

namespace ungod
{
    class Entity;
    class World;

    /** \brief Just making dom::Multicomponent serializable. */
    template<typename C>
    class MultiComponent : public dom::MultiComponent<C>, public Serializable< MultiComponent<C> >
    {
    public:
        template<typename ... PARAM>
        MultiComponent(std::size_t num, dom::Universe<>& universe, PARAM&& ... param) :
            dom::MultiComponent<C>(num, universe, std::forward<PARAM>()... ) {}

        MultiComponent() {}
    };

	//shortcuts to define methods with a name and (up to 4) arguments that 
	//have to support both single, multi and raw comp calls
	#define MULTI_COMP_METHOD_0ARG(NAME,C) \
	void NAME(Entity e) \
	{ \
		NAME(e, e.modify<C>());  \
	} \
	void NAME(Entity e, unsigned multiIndex) \
	{ \
		NAME(e, e.modify<MultiComponent<C>>().getComponent(multiIndex)); \
	} \
	void NAME(Entity e, C& c); 

	#define MULTI_COMP_METHOD_1ARG(NAME,C,A,a) \
	void NAME(Entity e, A a) \
	{ \
		NAME(e, e.modify<C>(), a);  \
	} \
	void NAME(Entity e, unsigned multiIndex, A a) \
	{ \
		NAME(e, e.modify<MultiComponent<C>>().getComponent(multiIndex), a); \
	} \
	void NAME(Entity e, C& c, A a); 

#define MULTI_COMP_METHOD_2ARG(NAME,C,A1,a1,A2,a2) \
	void NAME(Entity e, A1 a1, A2 a2) \
	{ \
		NAME(e, e.modify<C>(), a1, a2); \
	} \
	void NAME(Entity e, unsigned multiIndex, A1 a1, A2 a2) \
	{ \
		NAME(e, e.modify<MultiComponent<C>>().getComponent(multiIndex), a1, a2); \
	} \
	void NAME(Entity e, C& c, A1 a1, A2 a2); 

	#define MULTI_COMP_METHOD_3ARG(NAME,C,A1,a1,A2,a2,A3,a3) \
	void NAME(Entity e,A1 a1,A2 a2,A3 a3) \
	{ \
		NAME(e, e.modify<C>(), a1, a2, a3); \
	} \
	void NAME(Entity e, unsigned multiIndex,A1 a1,A2 a2,A3 a3) \
	{ \
		NAME(e, e.modify<MultiComponent<C>>().getComponent(multiIndex), a1, a2, a3); \
	} \
	void NAME(Entity e, C& c,A1 a1,A2 a2,A3 a3); 

	#define MULTI_COMP_METHOD_4ARG(NAME,C,A1,a1,A2,a2,A3,a3,A4,a4) \
	void NAME(Entity e,A1 a1,A2 a2,A3 a3,A4 a4) \
	{ \
		NAME(e, e.modify<C>(), a1, a2, a3, a4); \
	} \
	void NAME(Entity e, unsigned multiIndex,A1 a1,A2 a2,A3 a3,A4 a4) \
	{ \
		NAME(e, e.modify<MultiComponent<C>>().getComponent(multiIndex), a1, a2, a3, a4); \
	} \
	void NAME(Entity e, C& c,A1 a1,A2 a2,A3 a3,A4 a4); 
}

#endif // MULTI_COMPONENT_H
