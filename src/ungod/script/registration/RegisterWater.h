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

#ifndef REGISTER_WATER_H
#define REGISTER_WATER_H

#include "ungod/script/Behavior.h"

namespace ungod
{
    class Application;
    class Entity;
    class WaterHandler;
    class World;
    class WorldGraphNode;

    namespace scriptRegistration
    {
        class WaterHandlerFrontEnd
        {
        public:
            WaterHandlerFrontEnd(Entity& e, WaterHandler& h) : mEntity(e), mHandler(h) {}
            void initWater(const std::string& distortionTex, const std::string& fragmentShader, const std::string& vertexShader);
            void addReflectionWorld(WorldGraphNode* node, const std::string& world);
            void removeReflectionWorld(WorldGraphNode* node, const std::string& world);
            void setWaterShaders(bool set);
            void setWaterDistortionFactor(float distortion);
            void setWaterFlowFactor(float flow);
            void setWaterReflectionOpacity(float opac);
        private:
            Entity& mEntity;
            WaterHandler& mHandler;
        };


        /** \brief Registers application functionality for scripts. */
        void registerWater(ScriptStateBase& state);
    }
}

#endif //REGISTER_WATER_H

