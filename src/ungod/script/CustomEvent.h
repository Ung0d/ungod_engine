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

#ifndef CUSTOM_EVENT_H
#define CUSTOM_EVENT_H

#include "ungod/script/Behavior.h"

namespace ungod
{
    using CustomEvent = script::Environment;

    /** \brief Factory method that instantiates a custom-event with the following structure: { type, data = { data... } },
    * where type is a string identifier and data is an arbitrary blob of information depending on the context. */
    CustomEvent makeCustomEvent(const script::SharedState& state, const std::string& type, script::Environment& env, float delay);

    /** \brief Factory method that instantiates a custom-event with the following structure: { type, data = { data... } },
    * where type is a string identifier and data is an arbitrary blob of information depending on the context. */
    template<typename ... PARAM>
    CustomEvent makeCustomEvent(const script::SharedState& state, const std::string& type, float delay, PARAM&& ...param)
    {
        return state->create_table_with("type", type, "delay", delay, "data", state->create_table_with(std::forward<PARAM>(param)...));
    }
}

#endif
