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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////IMPLEMENTATION////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<std::size_t CONTEXT>
const std::vector< Collider >& RigidbodyComponent<CONTEXT>::getColliders() const
{
    return mColliders;
}

template <std::size_t CONTEXT>
void RigidbodyManager::addCollider(Entity e, const Collider& collider, int index)
{
    auto& r = e.modify<RigidbodyComponent<CONTEXT>>();
    if (index == -1)
        r.mColliders.emplace_back(collider);
    else
        r.mColliders.emplace(r.mColliders.begin()+index, collider);
    mContentsChangedSignal(e, collider.getBoundingBox());
}

template <std::size_t CONTEXT>
void RigidbodyManager::addCollider(Entity e, const sf::Vector2f& upleft, const sf::Vector2f& downright, float rotation, int index)
{
    auto& r = e.modify<RigidbodyComponent<CONTEXT>>();
    if (index == -1)
    {
        r.mColliders.emplace_back(upleft, downright, rotation);
        mContentsChangedSignal(e, r.mColliders.back().getBoundingBox());
    }
    else
    {
        r.mColliders.emplace(r.mColliders.begin()+index, upleft, downright, rotation);
        mContentsChangedSignal(e, r.mColliders[index].getBoundingBox());
    }
}

template <std::size_t CONTEXT>
void RigidbodyManager::removeCollider(Entity e, std::size_t index)
{
    auto& r = e.modify<RigidbodyComponent<CONTEXT>>();
    if (index >= r.mColliders.size())
        return;
    r.mColliders.erase(r.mColliders.begin() + index);
    mContentRemoved(e);
}

template <std::size_t CONTEXT>
void RigidbodyManager::modifyCollider(Entity e, std::size_t index, const Collider& collider)
{
    e.modify<RigidbodyComponent<CONTEXT>>().mColliders[index] = collider;
    mContentsChangedSignal(e, collider.getBoundingBox());
}

template <std::size_t CONTEXT>
void RigidbodyManager::setColliderDownRight(Entity e, std::size_t index, const sf::Vector2f& downright)
{
    Collider& collider = e.modify<RigidbodyComponent<CONTEXT>>().mColliders[index];
    collider.mDownright = downright;
    mContentsChangedSignal(e, collider.getBoundingBox());
}

template <std::size_t CONTEXT>
void RigidbodyManager::setColliderUpLeft(Entity e, std::size_t index, const sf::Vector2f& upleft)
{
    Collider& collider = e.modify<RigidbodyComponent<CONTEXT>>().mColliders[index];
    collider.mUpleft = upleft;
    mContentsChangedSignal(e, collider.getBoundingBox());
}

template <std::size_t CONTEXT>
void RigidbodyManager::rotateCollider(Entity e, std::size_t index, float rotation)
{
    Collider& collider = e.modify<RigidbodyComponent<CONTEXT>>().mColliders[index];
    collider.mRotation += rotation;
    mContentsChangedSignal(e, collider.getBoundingBox());
}

template <std::size_t CONTEXT>
void RigidbodyManager::setColliderRotation(Entity e, std::size_t index, float rotation)
{
    Collider& collider = e.modify<RigidbodyComponent<CONTEXT>>().mColliders[index];
    collider.mRotation = rotation;
    mContentsChangedSignal(e, collider.getBoundingBox());
}

template <std::size_t N>
struct ContextIteration
{
    static void iterate(RigidbodyManager& mgr, Entity e, const sf::Vector2f& vec)
    {
        mgr.moveContext<N>(e, vec);
        ContextIteration<N-1>::iterate(mgr, e, vec);
    }

    static void getLowerBounds(RigidbodyManager& mgr, Entity e, sf::Vector2f& vec)
    {
        mgr.contextLowerBounds<N>(e, vec);
        ContextIteration<N-1>::getLowerBounds(mgr, e, vec);
    }

    static void getUpperBounds(RigidbodyManager& mgr, Entity e, sf::Vector2f& vec)
    {
        mgr.contextUpperBounds<N>(e, vec);
        ContextIteration<N-1>::getUpperBounds(mgr, e, vec);
    }
};

template <>
struct ContextIteration<0>
{
    static void iterate(RigidbodyManager& mgr, Entity e, const sf::Vector2f& vec)
    {
        mgr.moveContext<0>(e, vec);
    }

    static void getLowerBounds(RigidbodyManager& mgr, Entity e, sf::Vector2f& vec)
    {
        mgr.contextLowerBounds<0>(e, vec);
    }

    static void getUpperBounds(RigidbodyManager& mgr, Entity e, sf::Vector2f& vec)
    {
        mgr.contextUpperBounds<0>(e, vec);
    }
};

template <std::size_t CONTEXT>
void RigidbodyManager::moveContext(Entity e, const sf::Vector2f& vec)
{
    if (e.has<RigidbodyComponent<CONTEXT>>())
    {
        RigidbodyComponent<CONTEXT>& body = e.modify<RigidbodyComponent<CONTEXT>>();
        for (auto& collider : body.mColliders)
        {
            collider.move(vec);
        }
    }
}

template <std::size_t CONTEXT>
void RigidbodyManager::contextLowerBounds(Entity e, sf::Vector2f& vec) const
{
    if (e.has<RigidbodyComponent<CONTEXT>>())
    {
        const RigidbodyComponent<CONTEXT>& body = e.get<RigidbodyComponent<CONTEXT>>();
        for (auto& collider : body.mColliders)
        {
            sf::IntRect bbox = collider.getBoundingBox();
            vec.x = std::max((float)(bbox.left + bbox.width), vec.x);
            vec.y = std::max((float)(bbox.top + bbox.height), vec.y);
        }
    }
}

template <std::size_t CONTEXT>
void RigidbodyManager::contextUpperBounds(Entity e, sf::Vector2f& vec) const
{
    if (e.has<RigidbodyComponent<CONTEXT>>())
    {
        const RigidbodyComponent<CONTEXT>& body = e.get<RigidbodyComponent<CONTEXT>>();
        for (auto& collider : body.mColliders)
        {
            sf::IntRect bbox = collider.getBoundingBox();
            vec.x = std::min((float)(bbox.left), vec.x);
            vec.y = std::min((float)(bbox.top), vec.y);
        }
    }
}





template<std::size_t CONTEXT>
CollisionManager<CONTEXT>::CollisionManager(quad::QuadTree<Entity>& quadtree) :
    mQuadtree(&quadtree), mBufferActive(false)
{
    onCollision([this] (Entity e, Entity other, const sf::Vector2f&, const Collider&, const Collider&)
                {
                    auto result = mDoubleBuffers[mBufferActive].emplace(e, std::unordered_set<Entity>());
                    result.first->second.insert(other);
                });
}


template<std::size_t CONTEXT>
void CollisionManager<CONTEXT>::onCollision(const std::function<void(Entity, Entity, const sf::Vector2f&, const Collider&, const Collider&)>& callback)
{
    mCollisionSignal.connect(callback);
}

template<std::size_t CONTEXT>
void CollisionManager<CONTEXT>::onBeginCollision(const std::function<void(Entity, Entity)>& callback)
{
    mCollisionBeginSignal.connect(callback);
}

template<std::size_t CONTEXT>
void CollisionManager<CONTEXT>::onEndCollision(const std::function<void(Entity, Entity)>& callback)
{
    mCollisionEndSignal.connect(callback);
}

template<std::size_t CONTEXT>
void CollisionManager<CONTEXT>::entityCollision(Entity e1, Entity e2,
                                                owls::Signal<Entity, Entity, const sf::Vector2f&, const Collider&, const Collider&>& signal)
{
    entityCollision(e1, e2, e1.modify<TransformComponent>(), e2.modify<TransformComponent>(), e1.modify<RigidbodyComponent<CONTEXT>>(), e2.modify<RigidbodyComponent<CONTEXT>>(), signal);
}

template<std::size_t CONTEXT>
void CollisionManager<CONTEXT>::entityCollision(Entity e1, Entity e2,
                     TransformComponent t1, TransformComponent t2,
                     RigidbodyComponent<CONTEXT>& r1,  RigidbodyComponent<CONTEXT>& r2,
                    owls::Signal<Entity, Entity, const sf::Vector2f&, const Collider&, const Collider&>& signal)
{
    if (sf::FloatRect(t1.getPosition(), t1.getSize()).intersects(sf::FloatRect(t2.getPosition(), t2.getSize())))
    {
        bool collision;
        sf::Vector2f smallestOffset;

        for (const auto& collider : r1.getColliders())
            for (const auto& colliderOther : r2.getColliders())
        {
            std::tie(collision, smallestOffset) = satAlgorithm(collider, colliderOther, t1, t2);
            if ( collision && e1 != e2 )
            {
                signal(e1, e2, smallestOffset, collider, colliderOther);
            }
        }
    }
}

template<std::size_t CONTEXT>
void CollisionManager<CONTEXT>::checkCollisions(const std::list<Entity>& entities)
{
    //clear active buffer
    mDoubleBuffers[mBufferActive].clear();

    quad::PullResult<Entity> result;
    dom::Utility<Entity>::iterate<TransformComponent, RigidbodyComponent<CONTEXT>>(entities,
      [&result, this] (Entity e, TransformComponent& transf, RigidbodyComponent<CONTEXT>& body)
      {
          if (!e.isStatic())
          {
            result.getList().clear();
            mQuadtree->retrieve(result, { transf.getPosition().x, transf.getPosition().y,
                                          transf.getSize().x, transf.getSize().y });

            dom::Utility<Entity>::iterate<TransformComponent, RigidbodyComponent<CONTEXT>>(result.getList(),
              [e, &transf, &body, this] (Entity other, TransformComponent& transfOther, RigidbodyComponent<CONTEXT>& bodyOther)
              {
                    entityCollision(e, other, transf, transfOther, body, bodyOther, mCollisionSignal);
              });
          }
      });

      for (const auto& eset : mDoubleBuffers[mBufferActive]) //for each entity in the active buffer
      {
          auto result = mDoubleBuffers[!mBufferActive].find(eset.first);  //search in the inactive bugger
          if (result == mDoubleBuffers[!mBufferActive].end())
          {
              for (const auto& other : eset.second)
              {
                  mCollisionBeginSignal(eset.first, other);
              }
          }
      }

      for (const auto& eset : mDoubleBuffers[!mBufferActive]) //for each entity in the inactive buffer
      {
          auto result = mDoubleBuffers[mBufferActive].find(eset.first);  //search in the active bugger
          if (result == mDoubleBuffers[mBufferActive].end())
          {
              for (const auto& other : eset.second)  //entities does not collide with anything this frame
              {
                mCollisionEndSignal(eset.first, other);
              }
          }
          else  //check for each static that the entity collided with last frame, whether it is still colliding with it this frame
          {
              for (const auto& other : eset.second)
              {
                  auto staticsResult = result->second.find(other);
                  if (staticsResult == result->second.end())  //entity collided with this static last frame, but not this frame
                  {
                      mCollisionEndSignal(eset.first, other);
                  }
              }
          }
      }

      //swap buffers
      mBufferActive = !mBufferActive;
}
