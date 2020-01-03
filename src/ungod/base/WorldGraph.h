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

#include "quadtree/QuadTree.h"
#include "ungod/utility/Graph.h"
#include <SFML/Graphics/Rect.hpp>
#include "ungod/serialization/Serializable.h"
#include "ungod/visual/RenderLayer.h"
#include <set>

namespace ungod
{
    class World;
    class WorldGraph;

    /** \brief A node in the world graph representing a single world to be loaded and unloaded.
    * The world is identified by a string, that is also assumed to be the name of the "main" world in the layer
    * container. The bounds of this "main" world are used to determine position and size of the node. */
    class WorldGraphNode : public Serializable<WorldGraphNode>
    {
     friend struct SerialBehavior<WorldGraphNode>;
    friend struct DeserialBehavior<WorldGraphNode>;
    public:
        WorldGraphNode(WorldGraph& wg, unsigned index, ScriptedGameState& gamestate, const std::string& identifier = {}, const std::string& datafile = {});

        /** \brief Completely loads all contents into memory. */
        void load();

        /** \brief Sets the node into a sleeping state with a very small footprint. Removes all contents from memory. */
        void unload();

		/** \brief Saves the states of the node on hard drive using the registered datafile path.*/
		void save();

        sf::FloatRect getBounds() const;

        /** \brief Adds a new world to position i of the layer stack. */
        World* addWorld(unsigned i);

        /** \brief Adds a new world to the end of the layer stack. */
        World* addWorld();

        /** \brief Returns a pointer to the world at position i of the layer stack. */
        World* getWorld(unsigned i) const;

        /** \brief Returns the number of worlds. */
        unsigned getNumWorld() const { return (unsigned)getLayers().getVector().size(); }

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

        bool isLoaded() const {return mIsLoaded;}

        unsigned getIndex() const {return mIndex;}

        const std::string& getIdentifier() const {return mIdentifier;}

        const std::string& getDataFile() const {return mDataFile;}

		/** \brief Sets the position of the node (and of all internal layers). */
		void setPosition(const sf::Vector2f& pos);

		/** \brief Moves the node (and all internal layers) along an offset. */
		void move(const sf::Vector2f& offset);

		/** \brief Resizes the node (and all internal layers). This may be a costly operation if the layers 
		* are nonempty. */
		void setSize(const sf::Vector2f& size);

		const RenderLayerContainer& getLayers() const { return mLayers; }

		void moveLayerUp(unsigned i);

		void moveLayerDown(unsigned i);

		void setActive(unsigned i, bool active = true);

    private:

        WorldGraph& mWorldGraph;
        unsigned mIndex;
        bool mIsLoaded;
        ScriptedGameState& mGamestate;
        RenderLayerContainer mLayers;
        std::string mIdentifier;
        std::string mDataFile;
    };



    template <>
    struct SerialIdentifier<WorldGraphNode>
    {
        static std::string get()  { return "WorldGraphNode"; }
    };

    template <>
    struct SerialBehavior<WorldGraphNode>
    {
        static void serialize(const WorldGraphNode& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<WorldGraphNode>
    {
        static void deserialize(WorldGraphNode& data, MetaNode deserializer, DeserializationContext& context);
    };
}

namespace quad
{
    /** \brief We have to provide the ElementTraits for the quadtree */
    template <>
    struct ElementTraits<ungod::WorldGraphNode*>
    {
        static Vector2f getPosition(ungod::WorldGraphNode const* e);
        static Vector2f getSize(ungod::WorldGraphNode const* e);
        static std::size_t getID(ungod::WorldGraphNode const* e);
    };
}


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
        WorldGraph(unsigned distance = 1) : mActive(-1), mDistance(distance) {}

        /** \brief Updates the reference position. If the active world changes, unload an load occurs. Returns true if the avtive node has changed. */
        bool updateReferencePosition(const sf::Vector2f& pos);

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

		const graph::UndirectedAdjacencyLists& getALlists() const { return mAdjacencies; }

        const quad::QuadTree<WorldGraphNode*>& getQuadTree() const { return mWorldQT; }

		/** \brief Connects a callback to the reference position changed event, which is emitted, if a new graph 
		* node is set as a new center node during some update. Parameter is the new reference position in world coodinates. */
		inline decltype(auto) onReferencePositionChanged(const std::function<void(sf::Vector2f)>& callback)
		{
			return mReferencePositionChanged.connect(callback);
		}

    private:
        int mActive;
        unsigned mDistance;
        quad::QuadTree<WorldGraphNode*> mWorldQT;
        std::vector<std::unique_ptr<WorldGraphNode>> mNodes;
        graph::UndirectedAdjacencyLists mAdjacencies;
        std::set<unsigned> mCurrentNeighborhood;
        sf::Vector2f mReferencePosition;
		owls::Signal<sf::Vector2f> mReferencePositionChanged;
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
