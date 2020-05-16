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

#ifndef UNGOD_WORLD_GRAPH_H
#define UNGOD_WORLD_GRAPH_H

#include "ungod/audio/Audio.h"
#include "quadtree/QuadTree.h"
#include "ungod/utility/Graph.h"
#include <SFML/Graphics/Rect.hpp>
#include "ungod/serialization/Serializable.h"
#include "ungod/visual/RenderLayer.h"
#include "ungod/base/WorldGraphNode.h"
#include <set>

namespace ungod
{

    /** \brief A graph datastructure, that connects several worlds together. One world out of the
    * pool can be active at a time. The pool automatically loads all worlds up to a certain distance
    * (default 1) into memory and unloads worlds accordingly, if the active world has changed. */
    class WorldGraph : public Serializable<WorldGraph>
    {
     friend struct SerialBehavior<WorldGraph>;
    friend struct DeserialBehavior<WorldGraph, ScriptedGameState&>;
    public:
        WorldGraph(unsigned distance = 1);

        /** \brief Updates the reference position. If the active world changes, new nodes are loaded and old ones unloaded as needed. Returns true if the active node has changed. */
        bool updateReferencePosition(const sf::Vector2f& pos, bool ignoreIdentity = true);

        /** \brief Render the world scene. */
        bool render(sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Render debug information. */
        bool renderDebug(sf::RenderTarget& target, sf::RenderStates states,
                         bool bounds = true, bool texrects = true, bool colliders = true, bool audioemitters = true, bool lightemitters = true) const;

        /** \brief Updates the world scene for the given amount of delta time. */
        void update(float delta);

        /** \brief Handles the input. */
        void handleInput(const sf::Event& event, const sf::RenderTarget& target);

        /** \brief Forwards a custom event. */
        void handleCustomEvent(const CustomEvent& event);

        /** \brief Accesses a graph node by position; nullptr if not found. */
        WorldGraphNode* getNode(const sf::Vector2f& pos);

        /** \brief Slow retrieval by name (O(#nodes)); nullptr if not found. */
        WorldGraphNode* getNode(const std::string& identifier);

		/** \brief Slow retrieval by index. */
		WorldGraphNode* getNode(unsigned i);

        /** \brief Activates a node by name. */
        void activateNode(const std::string& identifier);

        /** \brief Returns the number of world graph nodes. */
        unsigned getNumberOfNodes() const { return (unsigned)mAdjacencies.getVertexCount(); }

        /** \brief Get active. */
        WorldGraphNode* getActiveNode();

        /** \brief Connects two world graph nodes. */
        void connect(WorldGraphNode& n1, WorldGraphNode& n2);

        /** \brief Removes the connection between n1 and n2 if it exists. */
        void disconnect(WorldGraphNode& n1, WorldGraphNode& n2);

        /** \brief Creates a new, empty node and returns a reference to it. */
        WorldGraphNode& createNode(ScriptedGameState& gamestate, const std::string& identifier, const std::string& datafile);

        unsigned getDistance() const { return mDistance; }

        void setDistance(unsigned distance) { mDistance = distance; }

        /** \brief Saves state of all loaded graph nodes to memory using their respective file IDs. */
        void save(const ScriptedGameState& gamestate) const;

        /** \brief Update the internals if the bounds of a given node have changed. */
        void notifyBoundsChanged(WorldGraphNode* node);
          
        /** \brief Checks in all active nodes and their attached worlds for out of bounds cases and may transfers these entities to new nodes. */
        void checkOutOfBounds();

		const graph::UndirectedAdjacencyLists& getALlists() const { return mAdjacencies; }

        const quad::QuadTree<WorldGraphNode*>& getQuadTree() const { return mWorldQT; }

		/** \brief Connects a callback to the reference position changed event, which is emitted, if a new graph 
		* node is set as a new center node during some update. Parameter is the new reference position in world coodinates. */
		inline decltype(auto) onActiveNodeChanged(const std::function<void(WorldGraph&, WorldGraphNode&, WorldGraphNode&)>& callback)
		{
			return mActiveNodeChanged.connect(callback);
		}

    private:
        int mActive;
        unsigned mDistance;
        quad::QuadTree<WorldGraphNode*> mWorldQT;
        std::vector<std::unique_ptr<WorldGraphNode>> mNodes;
        graph::UndirectedAdjacencyLists mAdjacencies;
        std::set<unsigned> mCurrentNeighborhood;
        sf::Vector2f mReferencePosition;
        std::map<Entity, sf::Clock> mJustLeft;
		owls::Signal<WorldGraph&, WorldGraphNode&, WorldGraphNode&> mActiveNodeChanged;
        constexpr static float NODE_TRANSITION_TIMER_S = 10.0f;
    };



    template <>
    struct SerialIdentifier<WorldGraph>
    {
        static std::string get()  { return "WorldGraph"; }
    };

    template <>
    struct SerialBehavior<WorldGraph>
    {
        static void serialize(const WorldGraph& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<WorldGraph, ScriptedGameState&>
    {
        static void deserialize(WorldGraph& data, MetaNode deserializer, DeserializationContext& context, ScriptedGameState& gamestate);
    };
}

#endif // UNGOD_WORLD_GRAPH_H
