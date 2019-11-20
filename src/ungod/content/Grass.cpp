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

#include "ungod/content/Grass.h"

namespace ungod
{
    void GrassManager::update(const std::list<Entity>& entities, float delta, float windForce)
    {
        //calculate the "sway-range" that depends on the current windForce
        float swayRange = windForce*10.0f;

        //calculate the "sway-offset" that determines how wide the grass sways in this frame
        float swayOffset = windForce * 0.1f * delta;

        VisualsManager& vm = mWorld->getVisualsManager();

        dom::Utility<Entity>::iterate< GrassPatch, VertexArray >(entities,
          [delta, &vm, swayRange, swayOffset, this] (Entity e, GrassPatch& grass, VertexArray& vertices)
          {
              for (auto& data : grass.mGrassData)
             {
                 //flip sway direction?
                 if (vm.getPoint(e, data.rectIndex, 0).x <= data.position.x - swayRange)
                 {
                     data.swayDirection = true;  /// -->>>>
                 }
                 else if (vm.getPoint(e, data.rectIndex, 0).x >= data.position.x + swayRange)
                 {
                     data.swayDirection = false;  /// <<<<--
                 }

                 if (data.swayDirection)
                 {
                     vm.setPoints(e, data.rectIndex,
                                  vm.getPoint(e, data.rectIndex, 0) + sf::Vector2f(swayOffset, 0),
                                   vm.getPoint(e, data.rectIndex, 1) + sf::Vector2f(swayOffset, 0),
                                  vm.getPoint(e, data.rectIndex, 2),
                                  vm.getPoint(e, data.rectIndex, 3));
                 }
                 else
                 {
                     vm.setPoints(e, data.rectIndex,
                                  vm.getPoint(e, data.rectIndex, 0) - sf::Vector2f(swayOffset, 0),
                                  vm.getPoint(e, data.rectIndex, 1) - sf::Vector2f(swayOffset, 0),
                                  vm.getPoint(e, data.rectIndex, 2),
                                  vm.getPoint(e, data.rectIndex, 3));
                 }
             }
          });
    }

    void GrassManager::handleCollision(Entity trampling, Entity grass, const sf::Vector2f& vec, const Collider& collider, const Collider&)
    {
        if (grass.has<GrassPatch>())
        {
            const GrassPatch& grasspatch = grass.get<GrassPatch>();
            for (const auto& data : grasspatch.mGrassData)
            {
                if (containsPoint(collider, trampling.get<TransformComponent>(), grass.get<TransformComponent>().getTransform().transformPoint(data.position)))
                {
                    mWorld->getVisualsManager().setArrayTextureRect(grass, data.rectIndex, grasspatch.mTrampledKey);
                }
                else
                {
                    mWorld->getVisualsManager().setArrayTextureRect(grass, data.rectIndex, data.sheetKey);
                }
            }
        }
    }

    void GrassManager::handleCollisionEnd(Entity trampling, Entity grass)
    {
        if (grass.has<GrassPatch>())
        {
            const GrassPatch& grasspatch = grass.get<GrassPatch>();
            for (const auto& data : grasspatch.mGrassData)
            {
                mWorld->getVisualsManager().setArrayTextureRect(grass, data.rectIndex, data.sheetKey);
            }
        }
    }


    void GrassManager::initUniform(Entity grass, std::size_t grassDensity, float patchWidth, float patchHeight,
                     const std::string& imagePath, const std::vector<std::string>& grassKeys, const std::string& trampeledKey)
    {
        if (grassKeys.size() == 0)
            return;

        init(grass, imagePath, grassKeys, trampeledKey, grassDensity);

        for (std::size_t i = 0; i < grassDensity; ++i)
        {
            grass.modify<GrassPatch>().mGrassData.emplace_back( i, sf::Vector2f( NumberGenerator::getFloatRandBetw(0, patchWidth), NumberGenerator::getFloatRandBetw(0, patchHeight) ),
                                    grassKeys[NumberGenerator::getRandBetw(0, grassKeys.size()-1)]);
        }
    }


    void GrassManager::initNormal(Entity grass, std::size_t grassDensity, float distance,
                     const std::string& imagePath, const std::vector<std::string>& grassKeys, const std::string& trampeledKey)
    {
        if (grassKeys.size() == 0)
            return;

        init(grass, imagePath, grassKeys, trampeledKey, grassDensity);

        for (std::size_t i = 0; i < grassDensity; ++i)
        {
            grass.modify<GrassPatch>().mGrassData.emplace_back( i, sf::Vector2f( std::max(-2.0f*distance, std::min(2.0f*distance, NumberGenerator::getNormRand(0,distance))), std::max(-2.0f*distance, std::min(2.0f*distance, NumberGenerator::getNormRand(0,distance))) ),
                                    grassKeys[NumberGenerator::getRandBetw(0, grassKeys.size()-1)]);
        }
    }

    void GrassManager::init(Entity grass, const std::string& imagePath,
                            const std::vector<std::string>& grassKeys, const std::string& trampeledKey, std::size_t grassDensity)
    {
        GrassPatch& gp = grass.modify<GrassPatch>();

        gp.mGrassData.reserve(grassDensity);
        gp.mTrampledKey = trampeledKey;

        //init texture rects and visuals
        mWorld->getVisualsManager().loadTexture(grass, imagePath, LoadPolicy::ASYNC);
        mWorld->getVisualsManager().initTextureRects(grass, grassDensity);
    }
}
