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
#include "ungod/base/WorldGraphNode.h"

namespace ungod
{
    constexpr std::size_t WaterComponent::MAX_REFLECTION_WORLDS;

    std::vector<World*> WaterComponent::getReflectionWorlds() const
    {
        std::vector<World*> reflworlds;
        for (unsigned i = 0; i < MAX_REFLECTION_WORLDS; i++)
        {
            if (mReflectionWorlds[i].first)
            {
                auto* world = mReflectionWorlds[i].first->getWorld(mReflectionWorlds[i].second);
                if (world)
                    reflworlds.emplace_back(world);
            }
        }
        return reflworlds;
    }



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

    void WaterHandler::update(const std::list<Entity>& entities, const Camera& cam)
    {
        dom::Utility<Entity>::iterate<WaterComponent>(entities,
            [&cam](Entity e, WaterComponent& wc)
            {
                wc.mWater.update(cam);
            });
    }

    void WaterHandler::initWater(WaterComponent& water, const std::string& distortionMap, const std::string& fragmentShader, const std::string& vertexShader)
   {
      water.mWater.init(distortionMap, fragmentShader, vertexShader);
   }

   bool WaterHandler::addReflectionWorld(WaterComponent& water, WorldGraphNode* node, const std::string& world)
   {
       for (int i = 0; i < WaterComponent::MAX_REFLECTION_WORLDS; i++)
           if (water.mReflectionWorlds[i].first == node &&
               water.mReflectionWorlds[i].second == world)
           {
               Logger::warning("Attempt to add a reflection world twice. Omitting second one.");
               return false;
           }
       for (int i = 0; i < WaterComponent::MAX_REFLECTION_WORLDS; i++)
           if (!water.mReflectionWorlds[i].first)
           {
               water.mReflectionWorlds[i].first = node;
               water.mReflectionWorlds[i].second = world;
               return true;
           }
       Logger::warning("Maximum number of worlds for water reflection (", WaterComponent::MAX_REFLECTION_WORLDS, ") exceeded.");
       return false;
   }

   bool WaterHandler::removeReflectionWorld(WaterComponent& water, WorldGraphNode* node, const std::string& world)
   {
       for (int i = 0; i < WaterComponent::MAX_REFLECTION_WORLDS; i++)
           if (water.mReflectionWorlds[i].first == node &&
               water.mReflectionWorlds[i].second == world)
           {
               water.mReflectionWorlds[i].first = nullptr;
               water.mReflectionWorlds[i].second.clear();
               return true;
           }
       return false;
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

