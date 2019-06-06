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

#include "ungod/serialization/SerialCamera.h"
#include "ungod/visual/Camera.h"

namespace ungod
{
    void SerialBehavior<Camera>::serialize(const Camera& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("deadzone", data.mDeadZoneRadius, serializer);
        context.serializeProperty("scroll_speed", data.mScrollSpeed, serializer);
        context.serializeProperty("zoom", data.mCurZoom, serializer);
        context.serializeProperty("center_x", data.mView.getCenter().x, serializer);
        context.serializeProperty("center_y", data.mView.getCenter().y, serializer);
    }

    void DeserialBehavior<Camera, sf::RenderTarget&>::deserialize(Camera& data, MetaNode deserializer, DeserializationContext& context, sf::RenderTarget& target)
    {
        data.init(target);
        auto res = deserializer.getAttributes<float, float, float, float, float>({"deadzone", Camera::DEFAULT_DEAD_ZONE_RADIUS},
                                              {"scroll_speed", Camera::DEFAULT_SCROLL_SPEED},
                                              {"zoom", 1.0f},
                                              {"center_x", 0.0f},
                                              {"center_y", 0.0f} );
        data.setDeadZoneRadius(std::get<0>(res));
        data.setScrollSpeed(std::get<1>(res));
        data.setZoom(std::get<2>(res));
        data.lookAt( {std::get<3>(res), std::get<4>(res)} );
    }
}
