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

#include "ungod/serialization/SerialWater.h"
#include "ungod/content/Water.h"
#include "ungod/base/World.h"

namespace ungod
{
    void SerialBehavior<Water, const sf::RenderTarget&>::serialize(const Water& data, MetaNode serializer, SerializationContext& context, const sf::RenderTarget& target)
    {
        context.serializeObject("grd", data.mGround, serializer);

        context.serializeProperty("df", data.mDistortionFactor, serializer);
        context.serializeProperty("ff", data.mFlowFactor, serializer);
        context.serializeProperty("ro", data.mReflectionOpacity, serializer);

        context.serializeProperty("dtex", data.mDistortionTexID, serializer);
        context.serializeProperty("vert", data.mVertexShaderID, serializer);
        context.serializeProperty("frag", data.mFragmentShaderID, serializer);
    }

    void DeserialBehavior<Water, const sf::RenderTarget&>::deserialize(Water& data, MetaNode deserializer, DeserializationContext& context, const sf::RenderTarget& target)
    {
        auto attr = context.first(context.deserializeObject(data.mGround), "grd", deserializer);

        attr = context.next(context.deserializeProperty([&data] (float v) { data.setDistortionFactor(v); }, Water::DEFAULT_DISTORTION ), "df", deserializer, attr);
        attr = context.next(context.deserializeProperty([&data] (float v) { data.setFlowFactor(v); }, Water::DEFAULT_FLOW ), "ff", deserializer, attr);
        attr = context.next(context.deserializeProperty([&data] (float v) { data.setReflectionOpacity(v); }, Water::DEFAULT_REFLECTION_OPACITY ), "ro", deserializer, attr);

        std::string distex, vert, frag;

        attr = context.next(context.deserializeProperty(distex), "dtex", deserializer, attr);
        attr = context.next(context.deserializeProperty(vert), "vert", deserializer, attr);
        attr = context.next(context.deserializeProperty(frag), "frag", deserializer, attr);

        data.loadShaders(distex, frag, vert, target);
    }
}
