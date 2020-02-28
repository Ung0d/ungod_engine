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

#ifndef UNGOD_AI_MODEL_H
#define UNGOD_AI_MODEL_H

#include "ungod/base/Logger.h"
#include "ungod/ressource_management/Asset.h"
#include "tensorflow/lite/model.h"

namespace ungod
{
    namespace AI
    {
        using ModelBuffer = std::unique_ptr<tflite::FlatBufferModel>;

        struct Model : public Asset<ModelBuffer>
        {
            Model(const std::string& filePath, const LoadPolicy policy = LoadPolicy::SYNC);

            Model();
        };
    }

    template<>
    struct LoadBehavior<AI::ModelBuffer>
    {
        static bool loadFromFile(const std::string& filepath, AI::ModelBuffer& data);
        static std::string getIdentifier() { return "AIModel"; }
    };
}


#endif // UNGOD_AI_MODEL_H

