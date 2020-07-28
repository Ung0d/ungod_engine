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

#include "ungod/content/water/WaterHandler.h"
#include "ungod/application/Application.h"
#include "ungod/base/World.h"

namespace ungod
{
    void WaterHandler::init(World& world)
    {
        targetSizeChanged(world, world.getState()->getApp().getWindow().getSize());
        world.onComponentAdded<WaterComponent>([&world](Entity e)
            {
                e.modify<WaterComponent>().mWater.targetsizeChanged(world.getState()->getApp().getWindow().getSize());
            });
        mTargetSizeLink.disconnect();
        mTargetSizeLink = world.getState()->getApp().onTargetSizeChanged([&world, this](const sf::Vector2u& targetsize)
            {
                targetSizeChanged(world, targetsize);
            });
    }

    void WaterHandler::update(const std::list<Entity>& entities)
    {
        dom::Utility<Entity>::iterate<WaterComponent>(entities,
            [](Entity e, WaterComponent& wc)
            {
                wc.mWater.update();
            });
    }

    void WaterHandler::initWater(WaterComponent& water, const std::string& distortionMap, const std::string& fragmentShader, const std::string& vertexShader)
   {
      water.mWater.init(distortionMap, fragmentShader, vertexShader);
   }

   bool WaterHandler::addReflectionWorld(WaterComponent& water, World* world)
   {
        return water.mWater.addReflectionWorld(world);
   }

   bool WaterHandler::removeReflectionWorld(WaterComponent& water, World* world)
   {
       return water.mWater.removeReflectionWorld(world);
   }

    void WaterHandler::setWaterShaders(WaterComponent& water, bool flag)
    {
        water.mWater.setShaders(flag);
    }

    void WaterHandler::setWaterDistortionFactor(WaterComponent& water, float distortion)
    {
        water.mWater.setDistortionFactor(distortion);
    }

    void WaterHandler::setWaterFlowFactor(WaterComponent& water, float flow)
    {
        water.mWater.setFlowFactor(flow);
    }

    void WaterHandler::setWaterReflectionOpacity(WaterComponent& water, float opacity)
    {
        water.mWater.setReflectionOpacity(opacity);
    }

    void WaterHandler::targetSizeChanged(const World& world, const sf::Vector2u& targetsize)
    {
        world.getUniverse().iterateOverComponents<WaterComponent>([targetsize](WaterComponent& water)
            {
                water.mWater.targetsizeChanged(targetsize);
            });
    }

    WaterHandler::~WaterHandler()
    {
        mTargetSizeLink.disconnect();
    }
}

