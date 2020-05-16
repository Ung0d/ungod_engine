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

#include "ungod/script/registration/RegisterTransform.h"
#include "ungod/base/Transform.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerTransform(ScriptStateBase& state)
        {
			script::Usertype<TransformComponent> transfType = state.registerUsertype<TransformComponent>("Transform"); 
			transfType["getSize"] = &TransformComponent::getSize;
			transfType["getPosition"] = &TransformComponent::getPosition;
			transfType["getCenterPosition"] = &TransformComponent::getCenterPosition;
			transfType["getOriginPosition"] = &TransformComponent::getOriginPosition;
			transfType["getUpperBounds"] = &TransformComponent::getUpperBounds;
			transfType["getLowerBounds"] = &TransformComponent::getLowerBounds;
			transfType["getScale"] = &TransformComponent::getScale;
			transfType["getBaseLineOffsets"] = &TransformComponent::getBaseLineOffsets;

			script::Usertype<TransformHandler> transfManagerType = state.registerUsertype<TransformHandler>("TransformManager");
			transfManagerType["setPosition"] = sol::overload([](TransformHandler& tm, Entity e, float x, float y) { tm.setPosition(e, { x,y }); },
				[](TransformHandler& tm, Entity e, const sf::Vector2f& pos) { tm.setPosition(e, pos); });
			transfManagerType["setBaseLineOffsets"] = sol::overload([](TransformHandler& tm, Entity e, float x, float y) { tm.setBaseLineOffsets(e, { x,y }); },
				[](TransformHandler& tm, Entity e, const sf::Vector2f& pos) { tm.setBaseLineOffsets(e, pos); });
			transfManagerType["setScale"] = sol::overload([](TransformHandler& tm, Entity e, float scale) { tm.setScale(e, scale); },
				[](TransformHandler& tm, Entity e, const sf::Vector2f& scale) { tm.setScale(e, scale); });
			transfManagerType["move"] = sol::overload( [] (TransformHandler& tm, Entity e, float x, float y) { tm.move(e, {x,y}); },
                                                      [] (TransformHandler& tm, Entity e, const sf::Vector2f& pos) { tm.move(e, pos); });
        }
    }
}
