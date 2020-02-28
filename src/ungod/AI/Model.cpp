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

#include "ungod/AI/Model.h"

namespace ungod
{
    namespace AI
    {
        Model::Model(const std::string& filePath, const LoadPolicy policy) : Asset<ModelBuffer>(filePath, policy) {}

        Model::Model() : Asset<ModelBuffer>() {}
    }

    bool LoadBehavior<AI::ModelBuffer>::loadFromFile(const std::string& filepath, AI::ModelBuffer& data)
    {
        data = tflite::FlatBufferModel::BuildFromFile(filepath.c_str());
        ungod::Logger::assertion((bool)data, "Failed to load model " + filepath);
        return (bool)data;
    }
}
