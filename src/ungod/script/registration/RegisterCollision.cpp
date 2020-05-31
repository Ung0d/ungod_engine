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

#include "ungod/script/registration/RegisterCollision.h"
#include "ungod/base/World.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerRigidbody(ScriptStateBase& state)
        {
            script::Usertype<RotatedRectConstAggregator> rrcaType = state.registerUsertype<RotatedRectConstAggregator>("RotatedRectConstAggregator");
            rrcaType["getRotation"] = &RotatedRectConstAggregator::getRotation;
            rrcaType["getUpLeft"] = [](RotatedRectConstAggregator& rrca)
                     {return sf::Vector2f{rrca.getUpLeftX(), rrca.getUpLeftY()}; };
            rrcaType["getDownRight"] = [](RotatedRectConstAggregator& rrca)
                     {return sf::Vector2f{ rrca.getDownRightX(), rrca.getDownRightY() }; };

            script::Usertype<PointSetConstAggregator> pscaType = state.registerUsertype<PointSetConstAggregator>("PointSetConstAggregator");
            rrcaType["getPoint"] = [](PointSetConstAggregator& psca, unsigned i)
            {return sf::Vector2f{ psca.getPointX(i), psca.getPointY(i) }; };

            state.registerFunction("makeRotatedRect", [](const sf::Vector2f& upleft, const sf::Vector2f& downRight, float rotation)
                { return makeRotatedRect(upleft, downRight, rotation); });
            state.registerFunction("makeConvexPolygon", [] (script::Environment points)
                { return makeConvexPolygon(env2vec<sf::Vector2f>(points)); });
            state.registerFunction("makeEdgeChain", [](script::Environment points)
                { return makeEdgeChain(env2vec<sf::Vector2f>(points)); });
        }
    }
}
