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

#include "ungod/physics/Path.h"
#include "ungod/physics/Physics.h"
#include "ungod/physics/Movement.h"
#include "ungod/base/World.h"
#include "ungod/utility/Vec2fTraits.h"
#include "ungod/utility/DelaunayTriangulation.h"
#include "ungod/utility/DisjointSets.h"

namespace ungod
{
    void BasePath::switchDirection() { mDirection = !mDirection; }


    sf::Vector2f ExplicitPath::getCurrentWaypoint(PathPlanner& pathplanner)
    {
        return mPoints[mCurrent];
    }

    bool ExplicitPath::isFinished()
    {
        return (mDirection && mCurrent == mPoints.size()-1) || (!mDirection && mCurrent == 0);
    }

    void ExplicitPath::advanceWaypoint()
    {
        (mDirection) ? ++mCurrent : --mCurrent;
    }

    void ExplicitPath::reset()
    {
        (mDirection) ? mCurrent = 0 : mCurrent = mPoints.size()-1;
    }

    BasePath* ExplicitPath::clone() const
    {
        return new ExplicitPath(*this);
    }


    sf::Vector2f NavGraphPath::getCurrentWaypoint(PathPlanner& pathplanner)
    {
        return pathplanner.getGravityCenter(mPoints[mCurrent].destination);
    }

    bool NavGraphPath::isFinished()
    {
        return (mDirection && mCurrent == mPoints.size()-1) || (!mDirection && mCurrent == 0);
    }

    void NavGraphPath::advanceWaypoint()
    {
        (mDirection) ? ++mCurrent : --mCurrent;
    }

    void NavGraphPath::reset()
    {
        (mDirection) ? mCurrent = 0 : mCurrent = mPoints.size()-1;
    }

    BasePath* NavGraphPath::clone() const
    {
        return new NavGraphPath(*this);
    }


    constexpr float PathPlanner::DEFAULT_WAYPOINT_RADIUS;

    PathPlanner::QuadTreeElement::QuadTreeElement(std::size_t cNodeID, PathPlanner& cPathplanner) :
        nodeID(cNodeID)
    {
        bounds = triangleBounds(cPathplanner.mMeshVertices[cPathplanner.mNodeParameters[nodeID].a],
                                cPathplanner.mMeshVertices[cPathplanner.mNodeParameters[nodeID].b],
                                cPathplanner.mMeshVertices[cPathplanner.mNodeParameters[nodeID].c]);
    }

    void PathPlanner::buildNavGraph(World& world, float agentRadius)
    {
        //build a vector of the cornerpoints of all obstacles in the world
        //extend the geometry by the given radius
        std::vector<sf::Vector2f> points;
        DisjointSets<const Collider*> colliderClusters; //bundles together intersecting colliders

        /*world.forAll<TransformComponent, RigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>>([&] (Entity e, TransformComponent& t, RigidbodyComponent<MOVEMENT_COLLISION_CONTEXT>& rb)
          {
              if (e.isStatic())
              {
                  for (const auto& collider : rb.getColliders())
                  {
                        colliderClusters.insertElement(&collider);
                  }

                  for (std::size_t i = 0; i < rb.getColliders().size(); ++i)
                  {
                        //extend geometry and add points to the pointlist
                        sf::Transform localtrans;
                        localtrans *= t.getTransform();
                        localtrans.rotate(rb.getColliders()[i].getRotation(), rb.getColliders()[i].getCenter());
                        points.emplace_back( localtrans.transformPoint( {rb.getColliders()[i].getUpleft().x - agentRadius, rb.getColliders()[i].getUpleft().y - agentRadius} ) );
                        points.emplace_back( localtrans.transformPoint( {rb.getColliders()[i].getDownright().x + agentRadius, rb.getColliders()[i].getUpleft().y - agentRadius} ) );
                        points.emplace_back( localtrans.transformPoint( {rb.getColliders()[i].getDownright().x + agentRadius, rb.getColliders()[i].getDownright().y + agentRadius}  ) );
                        points.emplace_back( localtrans.transformPoint( {rb.getColliders()[i].getUpleft().x - agentRadius, rb.getColliders()[i].getDownright().y + agentRadius} ) );

                        //find other colliders that collide with the current collider
                        for (std::size_t j = i+1; j < rb.getColliders().size(); ++j)
                        {
                            if (satAlgorithm(rb.getColliders()[i], rb.getColliders()[j], t, t).first)
                            {
                                colliderClusters.merge(&rb.getColliders()[i], &rb.getColliders()[j]);
                            }
                        }
                  }

                  //find other entities that collide with the current entity, if found, merge the colliders that overlap
                  owls::Signal<Entity, Entity, const sf::Vector2f&, const Collider&, const Collider&> collisionSignal;
                  collisionSignal.connect([&] (Entity e1, Entity e2, const sf::Vector2f& mdv, const Collider& c1, const Collider& c2)
                      {
                        colliderClusters.merge(&c1, &c2);
                      });
                  quad::PullResult<Entity> pull = world.getEntitiesNearby(e);
                  for (const auto& other : pull.getList())
                  {
                      CollisionManager<MOVEMENT_COLLISION_CONTEXT>::entityCollision(e, other, collisionSignal);
                  }

                  //now remove for each collider cluster all triangles, that have edges between 2 points of the cluster,
              }
          });*/

          //triangulate the points
          DelaunayTriangulation<sf::Vector2f> triag;
          triag.run(points, sf::Vector2f(world.getQuadTree().getBoundary().position.x, world.getQuadTree().getBoundary().position.y),
                            sf::Vector2f(world.getQuadTree().getBoundary().position.x + world.getQuadTree().getBoundary().size.x,
                                         world.getQuadTree().getBoundary().position.y + world.getQuadTree().getBoundary().size.y));

          //cleanup diagonal edges of each collider and their adjacent triangles
          for (std::size_t i = 0; i < points.size()/4; ++i)
          {
              triag.getData().removeAdjacentTriangles(Edge(4*i, 4*i+2));
              triag.getData().removeAdjacentTriangles(Edge(4*i+1, 4*i+3));
          }

          //for each "cluster" of intersecting obstacles, remove triangles that intersect any colliders


          //associate triangles with graph nodes, ignore triangles that lie within colliders

    }

    void PathPlanner::update(const std::list<Entity>& entities, float delta, MovementManager& mvm)
    {
        dom::Utility<Entity>::iterate< PathFinderComponent, MovementComponent, TransformComponent >(entities,
          [delta, &mvm, this] (Entity e, PathFinderComponent& pathfinder, MovementComponent& mv, TransformComponent& transf)
          {
              if (pathfinder.mPath && pathfinder.mActive)
              {
                  sf::Vector2f waypoint = pathfinder.mPath->getCurrentWaypoint(*this);

                  pathfinder.mTimePast += delta;
                  mvm.seek(e, waypoint, pathfinder.mSpeed);

                  if (distance(transf.getCenterPosition(), waypoint) < pathfinder.mRadius)
                  {
                      if (pathfinder.mPath->isFinished())
                      {
                          switch (pathfinder.mPolicy)
                          {
                              case PathFollowingPolicy::ONE_SHOT:
                              {
                                  pathfinder.mPath.reset();
                                  pathfinder.mActive = false;
                                  break;
                              }
                              case PathFollowingPolicy::CYCLE:
                              {
                                  pathfinder.mPath->reset();
                                  pathfinder.mTimePast = 0.0f;
                                  break;
                              }
                              case PathFollowingPolicy::PATROL:
                              {
                                  pathfinder.mPath->switchDirection();
                                  pathfinder.mPath->advanceWaypoint();
                                  pathfinder.mTimePast = 0.0f;
                                  break;
                              }
                          }
                      }
                      else
                      {
                          //consider waypoint as reached
                          pathfinder.mPath->advanceWaypoint();
                          pathfinder.mTimePast = 0.0f;
                      }
                  }
              }
          });
    }

    void PathPlanner::setPath(PathFinderComponent& pathfinder, const PointContainer& points, PathFollowingPolicy policy, float speed, float radius)
    {
        pathfinder.mPath = PathPtr{std::unique_ptr<BasePath>(new ExplicitPath(points))};
        pathfinder.mActive = true;
        pathfinder.mTimePast = 0.0f;
        pathfinder.mPolicy = policy;
        pathfinder.mSpeed = speed;
        pathfinder.mRadius = radius;
    }

    void PathPlanner::findPathTo(Entity e, const sf::Vector2f& position)
    {
        /*Pathfinder& pf = e.modify<Pathfinder>();
        pf.mActive = true;
        pf.mPath = createPath(position);*/
    }

    sf::Vector2f PathPlanner::getGravityCenter(std::size_t nodeID)
    {
        return triangleCenterOfGravity(mMeshVertices[mNodeParameters[nodeID].a], mMeshVertices[mNodeParameters[nodeID].b], mMeshVertices[mNodeParameters[nodeID].c]);
    }

    PathPtr PathPlanner::createPath(const sf::Vector2f& position)
    {
        /*//find the graph node that is closest to the target position, by querying the quad tree, which stores bounding boxes of the
        //triangles, the graph nodes correspond to.
        //That means we look for the triangle, that contains the requested position
        std::size_t targetNode;
        bool nodeLinked = false;
        quad::PullResult<PathPlanner::QuadTreeElement> result;
        mNodeFinder.retrieve(result, quad::Bounds(position.x, position.y, 1, 1));
        for (const auto& e : result.getList())
        {
            if (triangleContains(mMeshVertices[mNodeParameters[e.nodeID].a],
                                 mMeshVertices[mNodeParameters[e.nodeID].b],
                                 mMeshVertices[mNodeParameters[e.nodeID].c],
                                 position))
             {
                 targetNode = e.nodeID;
                 nodeLinked = true;
                 break;
             }
        }

        if (!nodeLinked)  //very bad case, that may occurs under rare conditions
        {
            return {};  //todo maybe resolve this by a linear search of the nearest graph node?
        }*/
		return PathPtr{};
    }
}
