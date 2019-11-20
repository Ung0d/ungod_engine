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

#ifndef UNGOD_PATH_H
#define UNGOD_PATH_H

#include <list>

#include <SFML/System/Vector2.hpp>

#include "ungod/utility/Graph.h"
#include "ungod/base/Entity.h"
#include "quadtree/QuadTree.h"

namespace ungod
{
    class World;
    class MovementManager;
    class PathPlanner;

    /** \brief Base class for different path objects. */
    class BasePath
    {
    friend struct PathPtr;
    public:
        BasePath() : mDirection(true) {}

        /** \brief Returns the current point to chase in world coordinates. */
        virtual sf::Vector2f getCurrentWaypoint(PathPlanner& pathplanner) = 0;

        /** \brief Indicates whether the path is fully explored. */
        virtual bool isFinished() = 0;

        /** \brief Advances the path to the next waypoint and marks the path as finished if the last waypoint is reached. */
        virtual void advanceWaypoint() = 0;

        /** \brief Resets the internal position pointer. */
        virtual void reset() = 0;

        void switchDirection();

    protected:
        bool mDirection;  // true = forward

    private:
        virtual BasePath* clone() const = 0;
    };

    using PointContainer = std::vector<sf::Vector2f>;

    /** \brief A explicit path consisting of coordinate-points. These kind of paths are often manually set for entities. */
    class ExplicitPath : public BasePath
    {
    public:
        ExplicitPath(const PointContainer& points) : mPoints(points), mCurrent(0) {}

        /** \brief Returns the current point to chase in world coordinates. */
        virtual sf::Vector2f getCurrentWaypoint(PathPlanner& pathplanner) override;

        /** \brief Indicates whether the path is fully explored. */
        virtual bool isFinished() override;

        /** \brief Advances the path to the next waypoint and marks the path as finished if the last waypoint is reached. */
        virtual void advanceWaypoint() override;

        /** \brief Resets the internal position pointer. */
        virtual void reset() override;

    public:
        using Container = std::vector<sf::Vector2f>;

    private:
        Container mPoints;
        std::size_t mCurrent;

    private:
        virtual BasePath* clone() const;
    };

    /** \brief A nav-graph-path is a list of edges in a navigation graph that an entity with a movement component can follow
    * in order to navigate through its environment. */
    class NavGraphPath : public BasePath
    {
    public:
        /** \brief Returns the current point to chase in world coordinates. */
        virtual sf::Vector2f getCurrentWaypoint(PathPlanner& pathplanner) override;

        /** \brief Indicates whether the path is fully explored. */
        virtual bool isFinished() override;

        /** \brief Advances the path to the next waypoint and marks the path as finished if the last waypoint is reached. */
        virtual void advanceWaypoint() override;

        /** \brief Resets the internal position pointer. */
        virtual void reset() override;

    public:
        using Container = std::vector<graph::ALEdge>;

    private:
        Container mPoints;
        std::size_t mCurrent;

    private:
        virtual BasePath* clone() const;
    };

    struct PathPtr
    {
    public:
        PathPtr() = default;
        PathPtr(std::unique_ptr<BasePath>&& p) : path(std::move(p)) {}
        PathPtr(const PathPtr& other) { path.reset(other->clone()); }  //clone the path
        PathPtr& operator=(const PathPtr & other)
        {
            path.reset(other->clone());
            return *this;
        }

    private:
        std::unique_ptr<BasePath> path;
    public:
        BasePath* operator->() const  { return path.get(); }

        operator bool() const { return path.get(); }

        void reset() { path.reset(); }
    };

    /** \brief Defines how a path is tranversed. */
    enum class PathFollowingPolicy
    {
        ONE_SHOT,  //walks the path and removes it once finished
        CYCLE, // when the path is finished, the entity starts of from the starting point again
        PATROL  // the entity is constantly moving forward and backward on the path
    };

    /**
    * \ingroup Components
    * \brief A component that allows entities to follow a path in order to navigate through the environment.
    */
    class PathFinderComponent : public Serializable<PathFinderComponent>
    {
    friend class PathPlanner;
    public:
        PathFinderComponent() : mPath(), mTimePast(0.0f), mActive(false), mPolicy(PathFollowingPolicy::ONE_SHOT), mSpeed(1.0f), mRadius(1.0f) {}

    private:
        PathPtr mPath; //< the current path to follow, may be empty
        float mTimePast; //< the time that has past since the entity left the last waypoint
        bool mActive; //< activates and deactivates path following
        PathFollowingPolicy mPolicy;
        float mSpeed; //< the speed the path is traversed with
        float mRadius; //< the reach-radius of the waypoints
    };


    /** \brief A manager class that handles path-lookup and enables path-following for entities. */
    class PathPlanner
    {
    public:
        /** \brief Default constructor for later initialization. */
        PathPlanner() : mPathLookup(mNavGraph) {}

        /** \brief Initializes the pathplanner and builds the underlying navgraph depending on the obstacles.
        * Agent radius is a value that defines how much space "left" and "right" of a path must be given, so that
        * entities that do not exceed the given radius can move along paths without intersecting any of the colliders. */
        void buildNavGraph(World& world, float agentRadius);

        /** \brief Updates all entities with PathFinder and Movement components. */
        void update(const std::list<Entity>& entities, float delta, MovementManager& mvm);

        /** \brief Sets manually a path for the given entity. */
        inline void setPath(Entity e, const PointContainer& points,
                            PathFollowingPolicy policy = PathFollowingPolicy::ONE_SHOT,
                            float speed = DEFAULT_TRAVERSING_SPEED,
                            float radius = DEFAULT_WAYPOINT_RADIUS)
                            { setPath(e.modify<PathFinderComponent>(), points, policy, speed, radius); }
        void setPath(PathFinderComponent& pathfinder, const PointContainer& points,
                     PathFollowingPolicy policy = PathFollowingPolicy::ONE_SHOT,
                      float speed = DEFAULT_TRAVERSING_SPEED,
                      float radius = DEFAULT_WAYPOINT_RADIUS);

        /** \brief Looks up a path to the given target location and makes the entity follow it. */
        void findPathTo(Entity e, const sf::Vector2f& position);

        /** \brief Returns the center of gravity for the given graph node. */
        sf::Vector2f getGravityCenter(std::size_t nodeID);

    private:
        PathPtr createPath(const sf::Vector2f& position);


    public:
        struct QuadTreeElement
        {
            QuadTreeElement(std::size_t cNodeID, PathPlanner& cPathplanner);

            std::size_t nodeID;  //< id of the graph node the element corresponds to
            sf::FloatRect bounds;
        };

        struct NodeParameters
        {
            std::size_t a, b, c;  //< corners of the underlying triangle, they map to positions in the mesh-vertices-vector
        };

    private:
        quad::QuadTree<QuadTreeElement> mNodeFinder;  //speeds up finding of graph nodes near a certain position
        graph::UndirectedAdjacencyLists mNavGraph;  //navgraph build from a triangulation, where adjacent triangles are connected by edges
        graph::AStarAlg mPathLookup;
        std::vector<NodeParameters> mNodeParameters;  //stores for each vertex-id of the navgraph, which triangles in corresponds to
        std::vector<sf::Vector2f> mMeshVertices; //vertices of the world meshes

        static constexpr float DEFAULT_WAYPOINT_RADIUS = 30.0f;
        static constexpr float DEFAULT_TRAVERSING_SPEED = 1.0f;
    };

}

namespace quad
{
    template <>
    struct ElementTraits<ungod::PathPlanner::QuadTreeElement>
    {
        static bool isStatic(const ungod::PathPlanner::QuadTreeElement& e)
        {
            return true;
        }

        static Vector2f getPosition(const ungod::PathPlanner::QuadTreeElement& e)
        {
            return {e.bounds.left, e.bounds.top};
        }

        static Vector2f getSize(const ungod::PathPlanner::QuadTreeElement& e)
        {
            return {e.bounds.width, e.bounds.height};
        }

        static std::size_t getID(const ungod::PathPlanner::QuadTreeElement& e)
        {
            return e.nodeID;
        }
    };
}

#endif // UNGOD_PATH_H
