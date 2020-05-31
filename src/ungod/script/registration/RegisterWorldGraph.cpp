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

#include "ungod/script/registration/RegisterWorldGraph.h"
#include "ungod/base/World.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerWorldGraph(ScriptStateBase& state)
        {
			script::Usertype<WorldGraphNode> worldGraphNodeType = state.registerUsertype<WorldGraphNode>("WorldGraphNode");
			worldGraphNodeType["addWorld"] = [](WorldGraphNode& node) { return node.addWorld(); };
			worldGraphNodeType["getWorld"] = &WorldGraphNode::getWorld;
			worldGraphNodeType["setSize"] = &WorldGraphNode::setSize;
			worldGraphNodeType["setPosition"] = &WorldGraphNode::setPosition;
			worldGraphNodeType["getBounds"] = &WorldGraphNode::getBounds;
			worldGraphNodeType["getPosition"] = &WorldGraphNode::getPosition;
			worldGraphNodeType["moveLayerDown"] = &WorldGraphNode::moveLayerDown;
			worldGraphNodeType["moveLayerUp"] = &WorldGraphNode::moveLayerUp;
			worldGraphNodeType["isLoaded"] = &WorldGraphNode::isLoaded;
			worldGraphNodeType["getNumWorld"] = &WorldGraphNode::getNumWorld;
			worldGraphNodeType["wait"] = &WorldGraphNode::wait;
			worldGraphNodeType["save"] = &WorldGraphNode::save;
			worldGraphNodeType["getIdentifier"] = &WorldGraphNode::getIdentifier;
			worldGraphNodeType["mapToGlobalPosition"] = &WorldGraphNode::mapToGlobalPosition;
			worldGraphNodeType["mapToLocalPosition"] = &WorldGraphNode::mapToLocalPosition;

			script::Usertype<WorldGraph> worldGraphType = state.registerUsertype<WorldGraph>("WorldGraph");
			worldGraphType["createNode"] = &WorldGraph::createNode;
			worldGraphType["activateNode"] = &WorldGraph::activateNode;
			worldGraphType["getActiveNode"] = &WorldGraph::getActiveNode;
			worldGraphType["getNodeByIdentifier"] = [](WorldGraph& wg, const std::string& id) { return wg.getNode(id); };
			worldGraphType["getNodeByPosition"] = [](WorldGraph& wg, const sf::Vector2f& p) { return wg.getNode(p); };
			worldGraphType["connect"] = &WorldGraph::connect;
			worldGraphType["disconnect"] = &WorldGraph::disconnect;
			worldGraphType["setDistance"] = &WorldGraph::setDistance;
			worldGraphType["getDistance"] = &WorldGraph::getDistance;
			worldGraphType["getNumberOfNodes"] = &WorldGraph::getNumberOfNodes;
			worldGraphType["updateReferencePosition"] = [](WorldGraph& wg, const sf::Vector2f& pos) { wg.updateReferencePosition(pos); };
			worldGraphType["getCamera"] = [](WorldGraph& wg) -> Camera& { return wg.getCamera(); };
        }
    }
}

