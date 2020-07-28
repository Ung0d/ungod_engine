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

#include "ungod/script/registration/RegisterWater.h"
#include "ungod/content/water/WaterHandler.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void WaterHandlerFrontEnd::initWater(const std::string& distortionTex, const std::string& fragmentShader, const std::string& vertexShader)
        {
            mHandler.initWater(mEntity, distortionTex, fragmentShader, vertexShader);
        }

        void WaterHandlerFrontEnd::addReflectionWorld(World* world)
        {
            mHandler.addReflectionWorld(mEntity, world);
        }

        void WaterHandlerFrontEnd::removeReflectionWorld(World* world)
        {
            mHandler.removeReflectionWorld(mEntity, world);
        }

        void WaterHandlerFrontEnd::setWaterShaders(bool set)
        {
            mHandler.setWaterShaders(mEntity, set);
        }

        void WaterHandlerFrontEnd::setWaterDistortionFactor(float distortion)
        {
            mHandler.setWaterDistortionFactor(mEntity, distortion);
        }

        void WaterHandlerFrontEnd::setWaterFlowFactor(float flow)
        {
            mHandler.setWaterFlowFactor(mEntity, flow);
        }

        void WaterHandlerFrontEnd::setWaterReflectionOpacity(float opac)
        {
            mHandler.setWaterReflectionOpacity(mEntity, opac);
        }


        void registerWater(ScriptStateBase& state)
        {
            script::Usertype<WaterHandlerFrontEnd> waterHandlerType = state.registerUsertype<WaterHandlerFrontEnd>("WaterHandlerFrontEnd");
            waterHandlerType["initWater"] = &WaterHandlerFrontEnd::initWater;
            waterHandlerType["addReflectionWorld"] = &WaterHandlerFrontEnd::addReflectionWorld;
            waterHandlerType["removeReflectionWorld"] = &WaterHandlerFrontEnd::removeReflectionWorld;
            waterHandlerType["setWaterShaders"] = &WaterHandlerFrontEnd::setWaterShaders;
            waterHandlerType["setWaterDistortionFactor"] = &WaterHandlerFrontEnd::setWaterDistortionFactor;
            waterHandlerType["setWaterFlowFactor"] = &WaterHandlerFrontEnd::setWaterFlowFactor;
            waterHandlerType["setWaterReflectionOpacity"] = &WaterHandlerFrontEnd::setWaterReflectionOpacity;
        }
    }
}
