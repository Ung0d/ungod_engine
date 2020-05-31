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

#ifndef UNGOD_NODE_DATA_H
#define UNGOD_NODE_DATA_H

#include "ungod/ressource_management/Asset.h"
#include "ungod/visual/RenderLayer.h"
#include "ungod/serialization/DeserialMemory.h"

namespace ungod
{
    class Entity; 
    class WorldGraphNode;

    struct NodeDataStruct
    {
        DeserializationContext context;
        RenderLayerContainer container;
        DeserialMemory memory;
    };

    using NodeData = Asset<NodeDataStruct, WorldGraphNode*>;

    template<>
    struct LoadBehavior<NodeDataStruct, WorldGraphNode*>
    {
        static bool loadFromFile(const std::string& filepath, NodeDataStruct& data, WorldGraphNode* node);
        static std::string getIdentifier() { return "NodeData"; }
    };
}

#endif // UNGOD_NODE_DATA_H