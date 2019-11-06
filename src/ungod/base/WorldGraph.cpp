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

#include "ungod/base/WorldGraph.h"
#include "ungod/base/World.h"
#include "ungod/application/Application.h"
#include "ungod/serialization/DeserialInit.h"
#include "ungod/serialization/SerialGraph.h"
#include "ungod/serialization/SerialRenderLayer.h"


namespace ungod
{
    WorldGraphNode::WorldGraphNode(WorldGraph& wg, unsigned index, ScriptedGameState& gamestate, const std::string& identifier, const std::string& datafile)
        : mWorldGraph(wg), mIndex(index), mIsLoaded(false), mGamestate(gamestate), mLayers(mGamestate.getCamera()), mIdentifier(identifier), mDataFile(datafile) {}

    void WorldGraphNode::load()
    {
        DeserializationContext context;
        initContext(context);
        if (!context.read(mDataFile))
            return;

        context.deserializeRootObject(mLayers, static_cast<const sf::RenderTarget&>(mGamestate.getApp()->getWindow()), mGamestate);
        mIsLoaded = true;
    }

    void WorldGraphNode::unload()
    {
        mLayers.clearEverything();
        mIsLoaded = false;
    }

    sf::FloatRect WorldGraphNode::getBounds() const
    {
        sf::FloatRect globalBounds{};
        for (const auto& p : mLayers.getVector())
        {
            sf::FloatRect layerBounds = p.first->getTransformedBounds();
            globalBounds.left = std::min(globalBounds.left, layerBounds.left);
            globalBounds.top = std::min(globalBounds.top, layerBounds.top);
            globalBounds.width = std::max(globalBounds.width, layerBounds.left+layerBounds.width - globalBounds.left+globalBounds.width);
            globalBounds.height = std::max(globalBounds.height, layerBounds.left+layerBounds.height - globalBounds.left+globalBounds.height);
        }
        return globalBounds;
    }

    World* WorldGraphNode::addWorld(unsigned i)
    {
        World* world = static_cast<World*>(mLayers.registerLayer(mGamestate.makeWorld(), i));
        world->onSpaceChanged([this] (const sf::FloatRect& space) {mWorldGraph.notifyBoundsChanged(this);});
        return world;
    }

    World* WorldGraphNode::addWorld()
    {
        return addWorld(mLayers.getVector().size());
    }

    World* WorldGraphNode::getWorld(unsigned i) const
    {
        if (i < mLayers.getVector().size())
            return static_cast<World*>(mLayers.getVector()[i].first.get());
        else
            return nullptr;
    }

    bool WorldGraphNode::render(sf::RenderTarget& target, sf::RenderStates states) const
    {
        return mLayers.render(target, states);
    }

    bool WorldGraphNode::renderDebug(sf::RenderTarget& target, sf::RenderStates states,
                     bool bounds, bool texrects, bool colliders, bool audioemitters, bool lightemitters) const
    {
        return mLayers.renderDebug(target, states, bounds, texrects, colliders, audioemitters, lightemitters);
    }

    void WorldGraphNode::update(float delta)
    {
        mLayers.update(delta);
    }

    void WorldGraphNode::handleInput(const sf::Event& event, const sf::RenderTarget& target)
    {
        mLayers.handleInput(event, mGamestate.getApp()->getWindow());
    }

    void WorldGraphNode::handleCustomEvent(const CustomEvent& event)
    {
        mLayers.handleCustomEvent(event);
    }


    void SerialBehavior<WorldGraphNode>::serialize(const WorldGraphNode& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("id", data.mIdentifier, serializer);
        context.serializeProperty("file", data.mDataFile, serializer);
    }

    void DeserialBehavior<WorldGraphNode>::deserialize(WorldGraphNode& data, MetaNode deserializer, DeserializationContext& context)
    {
        //world graph nodes are deserialized in sleeping state
        auto result = deserializer.getAttributes<std::string, std::string>( {"id", ""}, {"file", ""} );
        data.mIdentifier = std::get<0>(result);
        data.mDataFile = std::get<1>(result);
    }
}

namespace quad
{
    Vector2f ElementTraits<ungod::WorldGraphNode*>::getPosition(ungod::WorldGraphNode const* w)
    {
        return Vector2f(w->getBounds().left, w->getBounds().top);
    }

    Vector2f ElementTraits<ungod::WorldGraphNode*>::getSize(ungod::WorldGraphNode const* w)
    {
        return Vector2f(w->getBounds().width, w->getBounds().height);
    }

    std::size_t ElementTraits<ungod::WorldGraphNode*>::getID(ungod::WorldGraphNode const* e)
    {
        return e->getIndex();
    }
}


namespace ungod
{
     bool WorldGraph::updateReferencePosition(const sf::Vector2f& pos)
     {
        mReferencePosition = pos;
        WorldGraphNode* node = getNode(pos);

        //we assume that the new node can have any distance to the last node; they do not have to be neighbors
        //Idea: find every node with distance <= mDistance from the current node (all active)-> set A
        //find every node with distance <= mDistance from the new node (maybe inactive)-> set B
        //in A, but not in B -> unload ; in B but not in A -> load ; do nothing for the intersect

        std::set<unsigned> neighborhoodnew;
        if (node)
        {
            if (mActive != -1 && node->getIndex() == (unsigned)mActive)
                return false; //nothing to do
            mActive = node->getIndex();
            graph::BFS bfs{mAdjacencies};
            auto conn = bfs.onNodeDiscovered([&neighborhoodnew] (unsigned i) {neighborhoodnew.emplace(i);});
            bfs.run(mActive, mDistance);
            conn.disconnect();
        }
        else
        {
            if (mActive == -1)
                return false; //nothing to do
            mActive = -1;
        }

        //in A, but not in B -> unload ; in B but not in A -> load ; do nothing for the intersect
        std::set<int> AminusB;
        std::set<int> BminusA;
        std::set_difference(mCurrentNeighborhood.begin(), mCurrentNeighborhood.end(), neighborhoodnew.begin(), neighborhoodnew.end(), std::inserter(AminusB, AminusB.end()));
        std::set_difference(neighborhoodnew.begin(), neighborhoodnew.end(), mCurrentNeighborhood.begin(), mCurrentNeighborhood.end(), std::inserter(BminusA, BminusA.end()));

        for (const auto& i : AminusB)
            mNodes[i]->unload();

        for (const auto& i : BminusA)
            mNodes[i]->load();

        mCurrentNeighborhood = neighborhoodnew;

        return true;
     }

    bool WorldGraph::render(sf::RenderTarget& target, sf::RenderStates states) const
    {
        bool done = true;
        for (const auto& i : mCurrentNeighborhood)
            done = done && mNodes[i]->render(target, states);
        return done;
    }

    bool WorldGraph::renderDebug(sf::RenderTarget& target, sf::RenderStates states,
                     bool bounds, bool texrects, bool colliders, bool audioemitters, bool lightemitters) const
    {
        bool done = true;
        for (const auto& i : mCurrentNeighborhood)
            done = done && mNodes[i]->renderDebug(target, states, bounds, texrects, colliders, audioemitters, lightemitters);
        return done;
    }

    void WorldGraph::update(float delta)
    {
        for (const auto& i : mCurrentNeighborhood)
            mNodes[i]->update(delta);
    }

    void WorldGraph::handleInput(const sf::Event& event, const sf::RenderTarget& target)
    {
        for (const auto& i : mCurrentNeighborhood)
            mNodes[i]->handleInput(event, target);
    }

    void WorldGraph::handleCustomEvent(const CustomEvent& event)
    {
        for (const auto& i : mCurrentNeighborhood)
            mNodes[i]->handleCustomEvent(event);
    }

    WorldGraphNode* WorldGraph::getNode(const sf::Vector2f& pos)
    {
        quad::PullResult<WorldGraphNode*> result;
        mWorldQT.retrieve(result, {pos.x, pos.y, 1, 1});
        if (result.getList().size() > 0)
            return result.getList().front();
        else
            return nullptr;
    }

    WorldGraphNode* WorldGraph::getNode(const std::string& identifier)
    {
        for (auto& n : mNodes)
            if (n->getIdentifier() == identifier)
                return n.get();
        return nullptr;
    }

    WorldGraphNode* WorldGraph::getActiveNode()
    {
        if (mActive == -1)
            return nullptr;
        else
            return mNodes[mActive].get();
    }

    void WorldGraph::connect(WorldGraphNode& n1, WorldGraphNode& n2)
    {
        mAdjacencies.addEdge(n1.getIndex(), n2.getIndex());
    }

    void WorldGraph::disconnect(WorldGraphNode& n1, WorldGraphNode& n2)
    {
        mAdjacencies.removeEdge(n1.getIndex(), n2.getIndex());
    }

    WorldGraphNode& WorldGraph::createNode(ScriptedGameState& gamestate, const std::string& identifier, const std::string& datafile)
    {
        mNodes.emplace_back(std::make_unique<WorldGraphNode>(*this, mNodes.size(), gamestate, identifier, datafile));
        notifyBoundsChanged(mNodes.back().get());
        mAdjacencies.setVertexCount(mAdjacencies.getVertexCount()+1);
        return *mNodes.back();
    }

    void WorldGraph::save() const
    {
        for (const auto& n : mNodes)
        {
            SerializationContext context;
            context.serializeRootObject(*n);
            context.save(n->getDataFile());
        }
    }

    void WorldGraph::notifyBoundsChanged(WorldGraphNode* node)
    {
        mWorldQT.removeFromItsNode(node);
        quad::Bounds qtBounds = mWorldQT.getBoundary();
        sf::FloatRect nodeBounds = node->getBounds();
        float bx = std::min(qtBounds.position.x, nodeBounds.left);
        float by = std::min(qtBounds.position.y, nodeBounds.top);
        float bx2 = std::max(qtBounds.position.x + qtBounds.size.x, nodeBounds.left + nodeBounds.width);
        float by2 = std::max(qtBounds.position.y + qtBounds.size.y, nodeBounds.top + nodeBounds.height);
        if (bx != qtBounds.position.x || by != qtBounds.position.y || bx2 != qtBounds.position.x + qtBounds.size.x || by2 != qtBounds.position.y + qtBounds.size.y)
            mWorldQT.setBoundary({bx, by, bx2-bx, by2-by});
        mWorldQT.insert(node);
    }



    void SerialBehavior<WorldGraph>::serialize(const WorldGraph& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeObjectContainer<WorldGraphNode>("nodes", [&data] (std::size_t i) -> const WorldGraphNode& { return *data.mNodes[i]; }, data.mNodes.size(), serializer);
        context.serializeProperty("active", data.mActive, serializer);
        context.serializeObject("graph", data.mAdjacencies, serializer);
        context.serializeProperty("pos_x", data.mReferencePosition.x, serializer);
        context.serializeProperty("pos_y", data.mReferencePosition.y, serializer);
    }


    void DeserialBehavior<WorldGraph, ScriptedGameState&>::deserialize(WorldGraph& data, MetaNode deserializer, DeserializationContext& context, ScriptedGameState& gamestate)
    {
        MetaAttribute attr = context.first(context.deserializeObjectContainer<WorldGraphNode>(
                        [&data, &gamestate](std::size_t num)
                        {
                            data.mNodes.reserve(num);
                            for (unsigned i = 0; i < num; i++)
                                data.mNodes.emplace_back(std::make_unique<WorldGraphNode>(data, i, gamestate));
                        },
                        [&data](std::size_t i) ->WorldGraphNode& {return *data.mNodes[i];}),
                            "nodes", deserializer);
        attr = context.next(context.deserializeProperty(data.mActive), "active", deserializer, attr);
        attr = context.next(context.deserializeObject(data.mAdjacencies), "graph", deserializer, attr);
        attr = context.next(context.deserializeProperty(data.mReferencePosition.x, 0.0f), "pos_x", deserializer, attr);
        attr = context.next(context.deserializeProperty(data.mReferencePosition.y, 0.0f), "pos_y", deserializer, attr);
        data.updateReferencePosition({data.mReferencePosition.x, data.mReferencePosition.y});
    }
}
