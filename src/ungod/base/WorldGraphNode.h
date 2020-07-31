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

#ifndef UNGOD_WORLD_GRAPH_NODE_H
#define UNGOD_WORLD_GRAPH_NODE_H

#include "owls/Signal.h"
#include "quadtree/QuadTree.h"
#include "ungod/base/Transform.h"
#include "ungod/base/NodeData.h"
#include "ungod/utility/Graph.h"
#include <SFML/Graphics/Rect.hpp>
#include "ungod/serialization/Serializable.h"
#include <set>

namespace ungod
{
    class World;
    class WorldGraph;

    /** \brief A node in the world graph representing a single chunk of the game to be loaded and unloaded 
    * independently of the rest of the graph.
    * The node is identified by a string, that is also assumed to be the name of the "main" world in the layer
    * container. The bounds of this "main" world are used to determine position and size of the node. */
    class WorldGraphNode : public Serializable<WorldGraphNode>
    {
     friend struct SerialBehavior<WorldGraphNode>;
    friend struct DeserialBehavior<WorldGraphNode>;
    public:
        constexpr static float DEFAULT_SIZE = 1000.0f;
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
        World* addWorld(unsigned i, bool init = true);

        /** \brief Adds a new world to the end of the layer stack. */
        World* addWorld(bool init = true);

        /** \brief Returns a pointer to the world at position i of the layer stack. */
        World* getWorld(unsigned i) const;

        /** \brief Returns a pointer to the (a) world with identifier on the layer stack. */
        World* getWorld(const std::string& identifier) const;

        /** \brief Access the world graph object this node belongs to. */
        WorldGraph& getGraph() { return mWorldGraph; }

        /** \brief Returns the number of worlds. */
        unsigned getNumWorld() const { return (unsigned)getLayers().getVector().size(); }

        /** \brief Updates the world scene for the given amount of delta time. */
        void update(float delta);

        /** \brief Handles the input. */
        void handleInput(const sf::Event& event, const sf::RenderTarget& target);

        /** \brief Forwards a custom event. */
        void handleCustomEvent(const CustomEvent& event);

        bool isLoaded() const {return mIsLoaded;}

        unsigned getIndex() const {return mIndex;}

        const std::string& getIdentifier() const {return mIdentifier;}

        void setIdentifier(const std::string& id) { mIdentifier = id; }

        const std::string& getDataFile() const {return mDataFile;}

		/** \brief Sets the position of the node (and of all internal layers). */
		void setPosition(const sf::Vector2f& pos);

		/** \brief Moves the node (and all internal layers) along an offset. */
		void move(const sf::Vector2f& offset);

        void setSize(const sf::Vector2f& size);

		/** \brief Resizes the node (and all internal layers). 
        * This may be a costly operation if the layers are nonempty. 
        * If leftTop is nonzero, positions of all transforms are adjusted such that  */
		void extend(const sf::Vector2f& leftTopExtensions, const sf::Vector2f& rightBotExtensions);

		const RenderLayerContainer& getLayers() const { return mLayers; }

        void activateLayer(unsigned i, bool active = true);

		void moveLayerUp(unsigned i);

		void moveLayerDown(unsigned i);

		void setActive(unsigned i, bool active = true);

        /** \brief Maps a position in local coordinates to a position in the global world. */
        sf::Vector2f mapToGlobalPosition(const sf::Vector2f& position) const;

        /** \brief Maps a position in global coordinates to a position relative to the layer. */
        sf::Vector2f mapToLocalPosition(const sf::Vector2f& position) const;

        inline sf::Vector2f getPosition() const { return{ mBounds.left, mBounds.top }; }

        inline sf::Vector2f getSize() const { return{ mBounds.width, mBounds.height }; }

        /**  \brief Waits for the node to load, if loading is currently in process. */
        void wait();

        void setSaveContents(bool save) { mSaveContents = save; }

        owls::SignalLink<void> onNodeChanged(const std::function<void()>& callback);

        int getPriority() const { return mPriority; }

        void setPriority(int priority) { mPriority = priority; }

        ~WorldGraphNode();

    private:
        WorldGraph& mWorldGraph;
        unsigned mIndex;
        bool mLoadingInProcess;
        bool mIsLoaded;
        ScriptedGameState& mGamestate;
        NodeData mData;
        RenderLayerContainer mLayers;
        std::string mIdentifier;
        std::string mDataFile;
        sf::FloatRect mBounds;
        bool mSaveContents;
        owls::Signal<> mNodeChangedSignal;
        int mPriority;

    private:
        // if loading is currently in progress, attempts to init the loaded render layers if ready returning success
        bool tryInit();
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
        static uint64_t getID(ungod::WorldGraphNode const* e);
    };
}

#endif // UNGOD_WORLD_GRAPH_NODE_H
