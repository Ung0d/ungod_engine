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

		Logger::info("Node loaded: ");
		Logger::info(getIdentifier());
		Logger::endl();
    }

    void WorldGraphNode::unload()
    {
        mLayers.clearEverything();
        mIsLoaded = false;

		Logger::info("Node unloaded: ");
		Logger::info(getIdentifier());
		Logger::endl();
    }

	void WorldGraphNode::save()
	{
		SerializationContext context;
		context.serializeRootObject(mLayers, static_cast<const sf::RenderTarget&>(mGamestate.getApp()->getWindow()));
		context.save(mDataFile);
	}

	sf::FloatRect WorldGraphNode::getBounds() const
	{
		return {mLayers.getPosition().x, mLayers.getPosition().y , mLayers.getSize().x , mLayers.getSize().y };
	}

    World* WorldGraphNode::addWorld(unsigned i)
    {
        return static_cast<World*>(mLayers.registerLayer(mGamestate.makeWorld(), i));
    }

    World* WorldGraphNode::addWorld()
    {
        return addWorld((unsigned)mLayers.getVector().size());
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

	void WorldGraphNode::setPosition(const sf::Vector2f& pos)
	{
		mLayers.setPosition(pos);
		mWorldGraph.notifyBoundsChanged(this);
	}

	void WorldGraphNode::move(const sf::Vector2f& offset)
	{
		mLayers.setPosition(mLayers.getPosition() + offset);
		mWorldGraph.notifyBoundsChanged(this);
	}

	void WorldGraphNode::setSize(const sf::Vector2f& size)
	{
		mLayers.setSize(size);
		mWorldGraph.notifyBoundsChanged(this);
	}


	void WorldGraphNode::moveLayerUp(unsigned i)
	{
		mLayers.moveLayerUp(i);
	}

	void WorldGraphNode::moveLayerDown(unsigned i)
	{
		mLayers.moveLayerDown(i);
	}

	void WorldGraphNode::setActive(unsigned i, bool active)
	{
		mLayers.setActive(i, active);
	}

    void SerialBehavior<WorldGraphNode>::serialize(const WorldGraphNode& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("id", data.mIdentifier, serializer);
        context.serializeProperty("file", data.mDataFile, serializer);
		context.serializeProperty("x", data.mLayers.getPosition().x, serializer);
		context.serializeProperty("y", data.mLayers.getPosition().y, serializer);
		context.serializeProperty("w", data.mLayers.getSize().x, serializer);
		context.serializeProperty("h", data.mLayers.getSize().y, serializer);

    }

    void DeserialBehavior<WorldGraphNode>::deserialize(WorldGraphNode& data, MetaNode deserializer, DeserializationContext& context)
    {
        //world graph nodes are deserialized in sleeping state
        auto result = deserializer.getAttributes<std::string, std::string, float, float, float, float>(
			{"id", ""}, {"file", ""}, { "x", 0.0f }, { "y", 0.0f }, { "w", 0.0f }, { "h", 0.0f });
        data.mIdentifier = std::get<0>(result);
        data.mDataFile = std::get<1>(result);
		data.setPosition({ std::get<2>(result) , std::get<3>(result) });
		data.setSize({ std::get<4>(result) , std::get<5>(result) });
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

	uint64_t ElementTraits<ungod::WorldGraphNode*>::getID(ungod::WorldGraphNode const* e)
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

		mReferencePositionChanged(pos);

		Logger::info("New active node: ");
		Logger::info(node->getIdentifier());
		Logger::endl();

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
		for (const auto& node : result.getList())
			if (node->getBounds().contains(pos))
				return node;
        return nullptr;
    }

    WorldGraphNode* WorldGraph::getNode(const std::string& identifier)
    {
        for (auto& n : mNodes)
            if (n->getIdentifier() == identifier)
                return n.get();
        return nullptr;
    }

	WorldGraphNode* WorldGraph::getNode(unsigned i)
	{
		return mNodes[i].get();
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
        mNodes.emplace_back(std::make_unique<WorldGraphNode>(*this, (unsigned)mNodes.size(), gamestate, identifier, datafile));
        notifyBoundsChanged(mNodes.back().get());
        mAdjacencies.setVertexCount((unsigned)mAdjacencies.getVertexCount()+1);
        return *mNodes.back();
    }

    void WorldGraph::save(const ScriptedGameState& gamestate) const
    {
		for (const auto& i : mCurrentNeighborhood)
			mNodes[i]->save();
    }

    void WorldGraph::notifyBoundsChanged(WorldGraphNode* node)
    {
		//bounds are overextended by this factor if a resize occurs
		static constexpr float RESCALE_FACTOR = 1.5f;

		mWorldQT.removeFromItsNode(node);
        quad::Bounds qtBounds = mWorldQT.getBoundary();
        sf::FloatRect nodeBounds = node->getBounds();

		//def values may be altered below
		float left = qtBounds.position.x;
		float top = qtBounds.position.y;
		float width = qtBounds.size.x;
		float height = qtBounds.size.y;

		//left or top overextend?
		float leftDiff = nodeBounds.left - left;
		float topDiff = nodeBounds.top - top;
		if (leftDiff < 0)
		{
			left += leftDiff * RESCALE_FACTOR;
			width -= leftDiff * RESCALE_FACTOR;
		}
		if (topDiff < 0)
		{
			top += topDiff * RESCALE_FACTOR;
			height -= leftDiff * RESCALE_FACTOR;
		}

		//bot or right overextend?
		float rightDiff = nodeBounds.left + nodeBounds.width - left-width;
		float botDiff = nodeBounds.top + nodeBounds.height - top - height;
		if (rightDiff > 0)
		{
			width += rightDiff * RESCALE_FACTOR;
		}
		if (botDiff > 0)
		{
			height += botDiff * RESCALE_FACTOR;
		}

        if (left != qtBounds.position.x || top != qtBounds.position.y || width != qtBounds.size.x || height != qtBounds.size.y)
            mWorldQT.setBoundary({left, top, width, height});
		mWorldQT.insert(node);
    }



    void SerialBehavior<WorldGraph>::serialize(const WorldGraph& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeObjectContainer<WorldGraphNode>("nodes", [&data] (std::size_t i) -> const WorldGraphNode& { return *data.mNodes[i]; }, data.mNodes.size(), serializer);
        context.serializeObject("graph", data.mAdjacencies, serializer);
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
		for (const auto& n : data.mNodes)
			data.notifyBoundsChanged(n.get());
        attr = context.next(context.deserializeObject(data.mAdjacencies), "graph", deserializer, attr);
    }
}
