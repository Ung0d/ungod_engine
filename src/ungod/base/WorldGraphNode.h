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

#include "quadtree/QuadTree.h"
#include "ungod/utility/Graph.h"
#include <SFML/Graphics/Rect.hpp>
#include "ungod/serialization/Serializable.h"
#include "ungod/visual/RenderLayer.h"
#include "ungod/base/ComponentSignalBase.h"
#include <set>

namespace ungod
{
    class World;
    class WorldGraph;

    /** \brief A node in the world graph representing a single world to be loaded and unloaded.
    * The world is identified by a string, that is also assumed to be the name of the "main" world in the layer
    * container. The bounds of this "main" world are used to determine position and size of the node.
    * Each node keeps its own disjoint dom::Universe for allocations of entities and components that can be independently loaded 
    * and unloaded. */
    class WorldGraphNode : public Serializable<WorldGraphNode>, public ComponentSignalBase
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

#endif // UNGOD_WORLD_GRAPH_NODE_H
