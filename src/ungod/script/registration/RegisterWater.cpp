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
#include "ungod/content/Water.h"
#include "ungod/application/Application.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerWater(ScriptStateBase& state, Application& app)
        {
             state.registerUsertype<Water>("Water",
                                          "setTiles", sol::overload(
                                           [] (Water& water, script::Environment tiles, script::Environment materials, std::size_t mapX, std::size_t mapY)
                                           {
                                                water.getTileMap().setTiles(env2vec<int>(tiles), env2vec<unsigned>(materials), mapX, mapY);
                                           },
                                           [] (Water& water, script::Environment tiles, script::Environment materials, script::Environment active, std::size_t mapX, std::size_t mapY)
                                           {
                                                water.getTileMap().setTiles(env2vec<int>(tiles), env2vec<unsigned>(materials), env2vec<bool>(active), mapX, mapY);
                                           }),
                                           "reserveTileCount", [] (Water& water, std::size_t num, const unsigned mapSizeX, const unsigned mapSizeY)
                                           {
                                               water.getTileMap().reserveTileCount(num, mapSizeX, mapSizeY);
                                           },
                                           "addTile", sol::overload(
                                           [] (Water& water, int id, unsigned material)
                                           {
                                               water.getTileMap().addTile(id, material);
                                           },
                                           [] (Water& water, int id, unsigned material, bool active)
                                           {
                                               water.getTileMap().addTile(id, material, active);
                                           }),
                                           "loadTiles", [] (Water& water,
                                                           const std::string& texFilepath, const std::string& metaFilepath,
                                                           std::size_t tileWidth, std::size_t tileHeight,
                                                           script::Environment keymap)
                                           {
                                                water.loadTiles(texFilepath, metaFilepath, tileWidth, tileHeight, env2vec<std::string>(keymap));
                                           },
                                           "loadShaders", [&app] (Water& water,
                                                           const std::string& distortionMap,
                                                           const std::string& fragmentShader, const std::string& vertexShader)
                                           {
                                                water.loadShaders(distortionMap, fragmentShader, vertexShader, app.getWindow());
                                           },
                                           "setShaders", &Water::setShaders,
                                           "setReflections", &Water::setReflections,
                                           "setDistortionFactor", &Water::setDistortionFactor,
                                           "setFlowFactor", &Water::setFlowFactor,
                                           "setReflectionOpacity", &Water::setReflectionOpacity);
        }
    }
}
