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
            /*state.registerUsertype<TransformComponent>("Transform",
                                                    "isPlane", &TransformComponent::isPlane,
                                                    "getSize", &TransformComponent::getSize,
                                                    "getPosition", &TransformComponent::getPosition,
                                                    "getCenterPosition", &TransformComponent::getCenterPosition,
                                                    "getUpperBounds", &TransformComponent::getUpperBounds,
                                                    "getLowerBounds", &TransformComponent::getLowerBounds,
                                                    "getScale", &TransformComponent::getScale);

            state.registerUsertype<TransformManager>("TransformManager",
                                                           "setPlane", [] (TransformManager& tm, Entity e, bool plane) {tm.setPlane(e, plane);},
                                                           "setPosition", sol::overload( [] (TransformManager& tm, Entity e, float x, float y) { tm.setPosition(e, {x,y}); },
                                                                                         [] (TransformManager& tm, Entity e, const sf::Vector2f& pos) { tm.setPosition(e, pos); }),
                                                           "setBaseLineOffsets", sol::overload( [] (TransformManager& tm, Entity e, float x, float y) { tm.setBaseLineOffsets(e, {x,y}); },
                                                                                         [] (TransformManager& tm, Entity e, const sf::Vector2f& pos) { tm.setBaseLineOffsets(e, pos); }),
                                                           "setScale", sol::overload( [] (TransformManager& tm, Entity e, float scale) { tm.setScale(e, scale); },
                                                                                      [] (TransformManager& tm, Entity e, const sf::Vector2f& scale) { tm.setScale(e, scale); }),
                                                           "move", sol::overload( [] (TransformManager& tm, Entity e, float x, float y) { tm.move(e, {x,y}); },
                                                                                  [] (TransformManager& tm, Entity e, const sf::Vector2f& pos) { tm.move(e, pos); })); */
        }
    }
}
