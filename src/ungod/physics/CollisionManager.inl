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


#ifndef COLLISION_MANAGER_IMPL_H
#define COLLISION_MANAGER_IMPL_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////setCircleCenter////////////////////////IMPLEMENTATION////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


template <std::size_t CONTEXT>
void RigidbodyManager<CONTEXT>::addCollider(Entity e, RigidbodyComponent<CONTEXT>& rcomp, const Collider& collider)
{
	rcomp.mCollider = collider;
    mContentsChangedSignal(e, collider.getBoundingBox());
}


template <std::size_t CONTEXT>
void RigidbodyManager<CONTEXT>::addRotatedRect(Entity e, RigidbodyComponent<CONTEXT>& rcomp, const sf::Vector2f& upleft, const sf::Vector2f& downright, float rotation)
{
	rcomp.mCollider.initRotatedRect(upleft, downright, rotation);
	mContentsChangedSignal(e, RotatedRectAggregator{ rcomp.mCollider }.getBoundingBox());
}


template <std::size_t CONTEXT>
void RigidbodyManager<CONTEXT>::addConvexPolygon(Entity e, RigidbodyComponent<CONTEXT>& rcomp, const std::vector<sf::Vector2f>& points)
{
	rcomp.mCollider.initConvexPolygon(points);
	mContentsChangedSignal(e, PointSetAggregator{ rcomp.mCollider }.getBoundingBox());
}


template <std::size_t CONTEXT>
void RigidbodyManager<CONTEXT>::addEdgeChain(Entity e, RigidbodyComponent<CONTEXT>& rcomp, const std::vector<sf::Vector2f>& points)
{
	rcomp.mCollider.initEdgeChain(points);
	mContentsChangedSignal(e, PointSetAggregator{ rcomp.mCollider }.getBoundingBox());
}


/*template <std::size_t CONTEXT>
void RigidbodyManager<CONTEXT>::addCircle(Entity e, RigidbodyComponent<CONTEXT> & rcomp, const sf::Vector2f& center, float radius)
{
	rcomp.mCollider.initCircle(center, radius);
	mContentsChangedSignal(e, CircleAggregator{ rcomp.mCollider }.getBoundingBox());
}*/


template <std::size_t CONTEXT>
void RigidbodyManager<CONTEXT>::clearCollider(Entity e, RigidbodyComponent<CONTEXT>& rcomp)
{
	rcomp.mCollider.reset();
	mContentRemoved(e);
}

template <std::size_t CONTEXT>
void RigidbodyManager<CONTEXT>::setRectDownRight(Entity e, RigidbodyComponent<CONTEXT>& rcomp, const sf::Vector2f& downright)
{
	RotatedRectAggregator rra{ rcomp.mCollider };
	rra.setDownRightX(downright.x);
	rra.setDownRightY(downright.y);
    mContentsChangedSignal(e, rra.getBoundingBox());
}

template <std::size_t CONTEXT>
void RigidbodyManager<CONTEXT>::setRectUpLeft(Entity e, RigidbodyComponent<CONTEXT>& rcomp, const sf::Vector2f& upleft)
{
	RotatedRectAggregator rra{ rcomp.mCollider };
	rra.setUpLeftX(upleft.x);
	rra.setUpLeftY(upleft.y);
	mContentsChangedSignal(e, rra.getBoundingBox());
}

template <std::size_t CONTEXT>
void RigidbodyManager<CONTEXT>::rotateRect(Entity e, RigidbodyComponent<CONTEXT>& rcomp, float rotation)
{
	RotatedRectAggregator rra{ rcomp.mCollider };
	rra.setRotation(rra.getRotation() + rotation);
    mContentsChangedSignal(e, rra.getBoundingBox());
}


template <std::size_t CONTEXT>
void RigidbodyManager<CONTEXT>::setRectRotation(Entity e, RigidbodyComponent<CONTEXT>& rcomp, float rotation)
{
	RotatedRectAggregator rra{ rcomp.mCollider };
	rra.setRotation(rotation);
	mContentsChangedSignal(e, rra.getBoundingBox());
}


template <std::size_t CONTEXT>
void RigidbodyManager<CONTEXT>::setPoint(Entity e, RigidbodyComponent<CONTEXT>& rcomp, unsigned i, const sf::Vector2f& point)
{
	PointSetAggregator psa{ rcomp.mCollider };
	psa.setPointX(i, point.x);
	psa.setPointY(i, point.y);
	mContentsChangedSignal(e, psa.getBoundingBox());
}


template <std::size_t CONTEXT>
void RigidbodyManager<CONTEXT>::addPoint(Entity e, RigidbodyComponent<CONTEXT>& rcomp, const sf::Vector2f& point)
{
	PointSetAggregator psa{ rcomp.mCollider };
	if (Logger::assertion(rcomp.mCollider.getNumParam()+2 > Collider::MAX_PARAM, "Maximum number of points for a collider exceeded!"))
		return;
	psa.allocatePoints(psa.getNumberOfPoints() + 1);
	psa.setPointX(psa.getNumberOfPoints()-1, point.x);
	psa.setPointY(psa.getNumberOfPoints()-1, point.y);
	mContentsChangedSignal(e, psa.getBoundingBox());
}


/*template <std::size_t CONTEXT>
void RigidbodyManager<CONTEXT>::setCircleCenter(Entity e, RigidbodyComponent<CONTEXT>& rcomp, const sf::Vector2f& center)
{
	CircleAggregator ca{ rcomp.mCollider };
	ca.setCenterX(center.x);
	ca.setCeterY(center.y);
	mContentsChangedSignal(e, ca.getBoundingBox());
}


template <std::size_t CONTEXT>
void RigidbodyManager<CONTEXT>::setRadius(Entity e, RigidbodyComponent<CONTEXT>& rcomp, float radius)
{
	CircleAggregator ca{ rcomp.mCollider };
	ca.setRadius(radius);
	mContentsChangedSignal(e, ca.getBoundingBox());
}*/

template <std::size_t CONTEXT>
void RigidbodyManager<CONTEXT>::setActive(Entity e, RigidbodyComponent<CONTEXT>& rcomp, bool active)
{
	rcomp.mActive = active;
	if (active)
		mContentsChangedSignal(e, RotatedRectAggregator{ rcomp.mCollider }.getBoundingBox());
	else
		mContentRemoved(e);
}


template <std::size_t CONTEXT>
void RigidbodyManager<CONTEXT>::moveCollider(Entity e, RigidbodyComponent<CONTEXT>& rcomp, const sf::Vector2f& vec)
{
	rcomp.mCollider.move(vec);
	mContentsChangedSignal(e, rcomp.mCollider.getBoundingBox());
}

template <std::size_t CONTEXT>
void RigidbodyManager<CONTEXT>::moveColliders(Entity e, const sf::Vector2f& vec)
{
	if (e.has<RigidbodyComponent<CONTEXT>>())
		e.modify<RigidbodyComponent<CONTEXT>>().mCollider.move(vec);
	if (e.has<MultiRigidbodyComponent<CONTEXT>>())
	{
		auto& mb = e.modify<MultiRigidbodyComponent<CONTEXT>>();
		for (unsigned i = 0; i < mb.getComponentCount(); i++)
			mb.getComponent(i).mCollider.move(vec);
	}
}

template <std::size_t CONTEXT>
sf::Vector2f RigidbodyManager<CONTEXT>::getLowerBound(Entity e) 
{
	sf::Vector2f vec;
	auto checker = [&vec](const RigidbodyComponent<CONTEXT>& body)
	{
		auto bbox = body.getCollider().getBoundingBox();
		vec.x = std::max(bbox.left + bbox.width, vec.x);
		vec.y = std::max(bbox.top + bbox.height, vec.y);
	};
	if (e.has<RigidbodyComponent<CONTEXT>>())
		checker(e.get<RigidbodyComponent<CONTEXT>>());
	if (e.has<MultiRigidbodyComponent<CONTEXT>>())
	{
		const auto& mb = e.get<MultiRigidbodyComponent<CONTEXT>>();
		for (unsigned i = 0; i < mb.getComponentCount(); i++)
			checker(mb.getComponent(i));
	}
	return vec;
}

template <std::size_t CONTEXT>
sf::Vector2f RigidbodyManager<CONTEXT>::getUpperBound(Entity e) 
{
	sf::Vector2f vec;
	auto checker = [&vec](const RigidbodyComponent<CONTEXT>& body)
	{
		auto bbox = body.getCollider().getBoundingBox();
		vec.x = std::min(bbox.left, vec.x);
		vec.y = std::min(bbox.top, vec.y);
	};
	if (e.has<RigidbodyComponent<CONTEXT>>())
		checker(e.get<RigidbodyComponent<CONTEXT>>());
	if (e.has<MultiRigidbodyComponent<CONTEXT>>())
	{
		const auto& mb = e.get<MultiRigidbodyComponent<CONTEXT>>();
		for (unsigned i = 0; i < mb.getComponentCount(); i++)
			checker(mb.getComponent(i));
	}
	return vec;
}




template<std::size_t CONTEXT>
CollisionManager<CONTEXT>::CollisionManager(quad::QuadTree<Entity>& quadtree) :
    mQuadtree(&quadtree), mBufferActive(false)
{
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
void CollisionManager<CONTEXT>::entityCollision(Entity e1, Entity e2)
{
    entityCollision(e1, e2, e1.modify<TransformComponent>(), e2.modify<TransformComponent>(), e1.modify<RigidbodyComponent<CONTEXT>>(), e2.modify<RigidbodyComponent<CONTEXT>>());
}

template<std::size_t CONTEXT>
void CollisionManager<CONTEXT>::entityCollision(Entity e1, Entity e2,
                     TransformComponent t1, TransformComponent t2,
                     RigidbodyComponent<CONTEXT>& r1,  RigidbodyComponent<CONTEXT>& r2)
{
    if (sf::FloatRect(t1.getPosition(), t1.getSize()).intersects(sf::FloatRect(t2.getPosition(), t2.getSize())))
    {
        bool collision;
        sf::Vector2f smallestOffset;
        std::tie(collision, smallestOffset) = doCollide(r1.getCollider(), r2.getCollider(), t1, t2);
        if ( collision && e1 != e2 )
        {
			notifyCollision(e1, e2, smallestOffset, r1.getCollider(), r2.getCollider());
			notifyCollision(e2, e1, smallestOffset, r2.getCollider(), r1.getCollider());
        }
    }
}

template<std::size_t CONTEXT>
void CollisionManager<CONTEXT>::notifyCollision(Entity e1, Entity e2, const sf::Vector2f& mdv, const Collider& c1, const Collider& c2)
{
	mCollisionSignal(e1, e2, mdv, c1, c2);
	auto result = mDoubleBuffers[mBufferActive].emplace(e1, std::unordered_set<Entity>());
	result.first->second.insert(e2);
}

template<std::size_t CONTEXT>
void CollisionManager<CONTEXT>::checkCollisions(const std::list<Entity>& entities)
{
    //clear active buffer
    mDoubleBuffers[mBufferActive].clear();

    dom::Utility<Entity>::iterate<TransformComponent, RigidbodyComponent<CONTEXT>>(entities,
      [this] (Entity e, TransformComponent& transf, RigidbodyComponent<CONTEXT>& body)
      {
			checkCollisions(e, transf, body);
      });
	dom::Utility<Entity>::iterate<TransformComponent, MultiRigidbodyComponent<CONTEXT>>(entities,
		[this](Entity e, TransformComponent& transf, MultiRigidbodyComponent<CONTEXT>& body)
		{
			for (unsigned i = 0; i < body.getComponentCount(); i++)
				checkCollisions(e, transf, body.getComponent(i));
		});

      for (const auto& eset : mDoubleBuffers[mBufferActive]) //for each entity in the active buffer
      {
          auto result = mDoubleBuffers[!mBufferActive].find(eset.first);  //search in the inactive buffer
          if (result == mDoubleBuffers[!mBufferActive].end())
          {
              for (const auto& other : eset.second)
                  mCollisionBeginSignal(eset.first, other);
          }
      }

      for (const auto& eset : mDoubleBuffers[!mBufferActive]) //for each entity in the inactive buffer
      {
			if (eset.first)
			{
				auto result = mDoubleBuffers[mBufferActive].find(eset.first);  //search in the active bugger
				if (result == mDoubleBuffers[mBufferActive].end())
				{
					for (const auto& other : eset.second)  //entity does not collide with anything this frame, send end signal to all
						if (other) 
							mCollisionEndSignal(eset.first, other);
				}
				else  //check for each static the entity collided with last frame, whether it is still colliding with it this frame
				{
					for (const auto& other : eset.second)
					{
						if (other)
						{
							auto staticsResult = result->second.find(other);
							if (staticsResult == result->second.end())  //entity collided with this static last frame, but not this frame
								mCollisionEndSignal(eset.first, other);
						}
					}
				}
			}
      }

      //swap buffers
      mBufferActive = !mBufferActive;
}


template<std::size_t CONTEXT>
void CollisionManager<CONTEXT>::checkCollisions(Entity e, TransformComponent transf, RigidbodyComponent<CONTEXT>& body)
{
	if (!e.isStatic() && body.isActive())
	{
		quad::PullResult<Entity> result;
		mQuadtree->retrieve(result, { transf.getPosition().x, transf.getPosition().y,
									  transf.getSize().x, transf.getSize().y });

		dom::Utility<Entity>::iterate<TransformComponent, RigidbodyComponent<CONTEXT>>(result.getList(),
			[e, &transf, &body, this](Entity other, TransformComponent& transfOther, RigidbodyComponent<CONTEXT>& bodyOther)
			{
				entityCollision(e, other, transf, transfOther, body, bodyOther);
			});
		dom::Utility<Entity>::iterate<TransformComponent, MultiRigidbodyComponent<CONTEXT>>(result.getList(),
			[e, &transf, &body, this](Entity other, TransformComponent& transfOther, MultiRigidbodyComponent<CONTEXT>& bodyOther)
			{
				for (unsigned i = 0; i < bodyOther.getComponentCount(); i++)
					entityCollision(e, other, transf, transfOther, body, bodyOther.getComponent(i));
			});
	}
}

#endif //COLLISION_MANAGER_IMPL_H
