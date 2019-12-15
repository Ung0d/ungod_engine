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

#include "ungod/physics/Collider.h"
#include "ungod/base/Transform.h"
#include "ungod/base/Logger.h"
#include "ungod/physics/Physics.h"

namespace ungod
{
	Collider::Collider() :
        mType(ColliderType::UNDEFINED), mNumParamUsed(0u) {}

	Collider::Collider(ColliderType type, std::vector<float> param) :
		mType(type), mNumParamUsed((unsigned)param.size()) 
	{
		std::copy(param.begin(), param.end(), mParam.begin());
	}

	void Collider::initRotatedRect(const sf::Vector2f& upleft, const sf::Vector2f& downRight, float rotation)
	{
		mType = ColliderType::ROTATED_RECT;
		mParam[0] = rotation;
		accomodatePoint(1, upleft);
		accomodatePoint(3, downRight);
		mNumParamUsed = 5;
	}


	void Collider::initConvexPolygon(const std::vector<sf::Vector2f>& points)
	{
		Logger::assertion(points.size() * 2 <= MAX_PARAM, "Too many points for collider.");
		mType = ColliderType::CONVEX_POLYGON;
		for (unsigned i = 0; i < points.size(); i++)
			accomodatePoint(2 * i, points[i]);
		mNumParamUsed = unsigned(2* points.size());
	}


	void Collider::initEdgeChain(const std::vector<sf::Vector2f>& points)
	{
		initConvexPolygon(points);
		mType = ColliderType::EDGE_CHAIN;
	}


	/*void Collider::initCircle(const sf::Vector2f& center, float radius)
	{
		mType = ColliderType::CIRCLE;
		accomodatePoint(0, center);
		mParam[2] = radius;
		mNumParamUsed = 3;
	}*/


	void Collider::reset()
	{
		mType = ColliderType::UNDEFINED;
	}


	sf::FloatRect Collider::getBoundingBox() const
	{
		switch (mType)
		{
		case ColliderType::ROTATED_RECT:
			return RotatedRectConstAggregator{ *this }.getBoundingBox();
		case ColliderType::CONVEX_POLYGON: case ColliderType::EDGE_CHAIN:
			return PointSetConstAggregator{ *this }.getBoundingBox();
		/*case ColliderType::CIRCLE:
			return CircleConstAggregator{ *this }.getBoundingBox();*/
		default:
			return {};
		}
	}


	void Collider::move(const sf::Vector2f& vec)
	{
		switch (mType)
		{
		case ColliderType::ROTATED_RECT:
			RotatedRectAggregator{ *this }.move(vec); 
			break;
		case ColliderType::CONVEX_POLYGON: case ColliderType::EDGE_CHAIN:
			PointSetAggregator{ *this }.move(vec);
			break;
		/*case ColliderType::CIRCLE:
			CircleAggregator{ *this }.move(vec);
			break;*/
		default:
			break;
		}
	}


	void Collider::getAxisAndPivots(const TransformComponent& t, std::vector<sf::Vector2f>& axis, std::vector<sf::Vector2f>& pivots, unsigned i) const
	{
		switch (mType)
		{
		case ColliderType::ROTATED_RECT:
			RotatedRectConstAggregator{ *this }.getAxisAndPivots(t, axis, pivots, i);
			break;
		case ColliderType::CONVEX_POLYGON:
			ConvexPolygonConstAggregator{ *this }.getAxisAndPivots(t, axis, pivots, i);
			break;
		case ColliderType::EDGE_CHAIN:
			EdgeChainConstAggregator{ *this }.getAxisAndPivots(t, axis, pivots, i);
			break;
		/*case ColliderType::CIRCLE:
			break;*/
		default:
			break;
		}
	}


	sf::Vector2f Collider::getCenter() const
	{
		switch (mType)
		{
		case ColliderType::ROTATED_RECT:
			return RotatedRectConstAggregator{ *this }.getCenter();
		case ColliderType::CONVEX_POLYGON:
			return PointSetConstAggregator{ *this }.getCenter();
		case ColliderType::EDGE_CHAIN:
			return {};
		/*case ColliderType::CIRCLE:
			return {};*/
		default:
			return {};
		}
	}


	unsigned Collider::getNumRuns() const
	{
		switch (mType)
		{
		case ColliderType::ROTATED_RECT:
			return 1u;
		case ColliderType::CONVEX_POLYGON:
			return 1u;
		case ColliderType::EDGE_CHAIN:
		{
			auto nump = PointSetConstAggregator{ *this }.getNumberOfPoints();
			return nump > 0 ? nump - 1 : 0u;
		}
		/*case ColliderType::CIRCLE:
			return {};*/
		default:
			return 0u;
		}
	}


	void Collider::accomodatePoint(unsigned i, const sf::Vector2f& point) 
	{
		mParam[i] = point.x;
		mParam[(std::size_t)i+1] = point.y;
	}


	Collider makeRotatedRect(const sf::Vector2f& upleft, const sf::Vector2f& downRight, float rotation)
	{
		Collider c;
		c.initRotatedRect(upleft, downRight, rotation);
		return c;
	}

	Collider makeConvexPolygon(const std::vector<sf::Vector2f>& points)
	{
		Collider c;
		c.initConvexPolygon(points);
		return c;
	}

	Collider makeEdgeChain(const std::vector<sf::Vector2f>& points)
	{
		Collider c;
		c.initEdgeChain(points);
		return c;
	}

	/*Collider makeCircle(const sf::Vector2f& center, float radius)
	{
		Collider c;
		c.initCircle(center, radius);
		return c;
	}*/


	namespace detail
	{
		AggregatorBase::AggregatorBase(const Collider& data) : mData(data)
		{}
	}


	RotatedRectConstAggregator::RotatedRectConstAggregator(const Collider& data) : detail::AggregatorBase(data) {}

	sf::Vector2f RotatedRectConstAggregator::getCenter() const
	{
		return sf::Vector2f{ getUpLeftX() + 0.5f * (getDownRightX() - getUpLeftX()),
							 getUpLeftY() + 0.5f * (getDownRightY() - getUpLeftY()) };
	}

	sf::FloatRect RotatedRectConstAggregator::getBoundingBox(const TransformComponent& t) const
	{
		return getBoundingBox(t.getTransform());
	}

	sf::FloatRect RotatedRectConstAggregator::getBoundingBox(sf::Transform transf) const
	{
		transf.rotate(getRotation(), getCenter());
		sf::Vector2f p1 = transf.transformPoint({ getUpLeftX(), getUpLeftY() });
		sf::Vector2f p2 = transf.transformPoint({ getDownRightX(), getUpLeftY() });
		sf::Vector2f p3 = transf.transformPoint({ getDownRightX(), getDownRightY() });
		sf::Vector2f p4 = transf.transformPoint({ getUpLeftX(), getDownRightY() });

		float minx = std::numeric_limits<float>::infinity();
		float maxx = -std::numeric_limits<float>::infinity();
		float miny = std::numeric_limits<float>::infinity();
		float maxy = -std::numeric_limits<float>::infinity();

		minx = std::min(minx, p1.x);
		maxx = std::max(maxx, p1.x);
		miny = std::min(miny, p1.y);
		maxy = std::max(maxy, p1.y);

		minx = std::min(minx, p2.x);
		maxx = std::max(maxx, p2.x);
		miny = std::min(miny, p2.y);
		maxy = std::max(maxy, p2.y);

		minx = std::min(minx, p3.x);
		maxx = std::max(maxx, p3.x);
		miny = std::min(miny, p3.y);
		maxy = std::max(maxy, p3.y);

		minx = std::min(minx, p4.x);
		maxx = std::max(maxx, p4.x);
		miny = std::min(miny, p4.y);
		maxy = std::max(maxy, p4.y);

		return { minx, miny, maxx - minx, maxy - miny };
	}

	void RotatedRectConstAggregator::getAxisAndPivots(const TransformComponent& t, std::vector<sf::Vector2f>& axis, std::vector<sf::Vector2f>& points, unsigned) const
	{
		sf::Transform finalTransf;
		finalTransf *= t.getTransform();
		finalTransf.rotate(getRotation(), getCenter());

		points.emplace_back(finalTransf.transformPoint({ getUpLeftX(), getUpLeftY() }));
		points.emplace_back(finalTransf.transformPoint({ getDownRightX(), getUpLeftY() }));
		points.emplace_back(finalTransf.transformPoint({ getDownRightX(), getDownRightY() }));
		points.emplace_back(finalTransf.transformPoint({ getUpLeftX(), getDownRightY() }));

		auto n = points.size();

		auto v1 = points[n - 3] - points[n - 4];
		normalize(v1);
		axis.emplace_back(perpendicularVector(v1));
		auto v2 = points[n - 1] - points[n - 4];
		normalize(v2);
		axis.emplace_back(perpendicularVector(v2));
	}


	RotatedRectAggregator::RotatedRectAggregator(Collider& data) : RotatedRectConstAggregator(data) {}

	Collider RotatedRectAggregator::transform(const TransformComponent& t) const
	{
		auto tUL = t.getTransform().transformPoint({getUpLeftX(), getUpLeftY() });
		auto tDR = t.getTransform().transformPoint({ getDownRightX(), getDownRightY() });
		return { ColliderType::ROTATED_RECT, {getRotation(), tUL.x, tUL.y, tDR.x, tDR.y} };
	}

	void RotatedRectAggregator::move(const sf::Vector2f& vec) const
	{
		setUpLeftX(getUpLeftX() + vec.x);
		setUpLeftY(getUpLeftY() + vec.y);
		setDownRightX(getDownRightX() + vec.x);
		setDownRightY(getDownRightY() + vec.y);
	}


	PointSetConstAggregator::PointSetConstAggregator(const Collider& data) : detail::AggregatorBase(data) {}

	unsigned PointSetConstAggregator::getNumberOfPoints() const
	{
		return mData.getNumParam() / 2;
	}


	sf::FloatRect PointSetConstAggregator::getBoundingBox(const TransformComponent& t) const
	{
		return getBoundingBox(t.getTransform());
	}


	sf::FloatRect PointSetConstAggregator::getBoundingBox(sf::Transform t) const
	{
		if (mData.getNumParam() == 0) return {};
		auto t0 = t.transformPoint({ getPointX(0), getPointY(0) });
		sf::FloatRect bounds{ t0, t0 };
		for (unsigned i = 1; i < getNumberOfPoints(); i++)
		{
			auto ti = t.transformPoint({ getPointX(i), getPointY(i) });
			bounds.left = std::min(ti.x, bounds.left);
			bounds.top = std::min(ti.y, bounds.top);
			bounds.width = std::max(ti.x, bounds.width);
			bounds.height = std::max(ti.y, bounds.height);
		}
		bounds.width -= bounds.left;
		bounds.height -= bounds.top;
		return bounds;
	}


	sf::Vector2f PointSetConstAggregator::getCenter() const
	{
		switch (getNumberOfPoints())
		{
		case 1u:
			return { getPointX(0), getPointY(0) };

		case 2u:
			return centerPoint({ getPointX(0), getPointY(0) }, { getPointX(1), getPointY(1) });

		case 0u:
			return {};

		default:
			break; //foward to >2 case below
		}

		float area = 0.0f;
		sf::Vector2f center;
		for (unsigned i = 0; i < getNumberOfPoints(); i++)
		{
			unsigned inext = (i + 1) % getNumberOfPoints();
			float adiff = getPointX(i) * getPointY(inext) - getPointY(i) * getPointX(inext);
			area += adiff;
			center.x += (getPointX(i) + getPointX(inext)) * adiff;
			center.y += (getPointY(i) + getPointY(inext)) * adiff;
		}
		area /= 2;
		return (1/(6*area))*center;
	}


	ColliderType PointSetConstAggregator::getType() const
	{
		return mData.getType();
	}


	PointSetAggregator::PointSetAggregator(Collider& data) : PointSetConstAggregator(data) {}


	void PointSetAggregator::allocatePoints(unsigned n) const
	{
		Logger::assertion(2 * n <= Collider::MAX_PARAM, "Can not allocate that many points.");
		getData().allocateParam(2 * n);
	}


	Collider PointSetAggregator::transform(const TransformComponent& t) const
	{
		std::vector<float> data;
		for (unsigned i = 0; i < getNumberOfPoints(); i++)
		{
			auto ti = t.getTransform().transformPoint({ getPointX(i), getPointY(i) });
			data[2 * (std::size_t)i] = ti.x;
			data[2*(std::size_t)i+1] = ti.y;
		}
		return { getData().getType(), std::move(data)};
	}


	void PointSetAggregator::move(const sf::Vector2f& vec) const
	{
		for (unsigned i = 0; i < getNumberOfPoints(); i++)
		{
			setPointX(i, getPointX(i) + vec.x);
			setPointY(i, getPointY(i) + vec.y);
		}
	}

	ConvexPolygonConstAggregator::ConvexPolygonConstAggregator(const Collider& data) : PointSetConstAggregator(data) {}

	void ConvexPolygonConstAggregator::getAxisAndPivots(const TransformComponent& t, std::vector<sf::Vector2f>& axis, std::vector<sf::Vector2f>& points, unsigned) const
	{
		for (unsigned i = 0; i < getNumberOfPoints(); i++)
			points.emplace_back(t.getTransform().transformPoint({ getPointX(i), getPointY(i) }));
		auto n = points.size();
		for (unsigned i = 0; i < getNumberOfPoints(); i++)
		{
			unsigned inext = (i + 1) % getNumberOfPoints();
			auto v = points[n - getNumberOfPoints() + inext] - points[n - getNumberOfPoints() + i];
			normalize(v);
			axis.emplace_back(perpendicularVector(v));
		}
	}


	EdgeChainConstAggregator::EdgeChainConstAggregator(const Collider& data) : PointSetConstAggregator(data) {}

	void EdgeChainConstAggregator::getAxisAndPivots(const TransformComponent& t, std::vector<sf::Vector2f>& axis, std::vector<sf::Vector2f>& points, unsigned i) const
	{
		points.emplace_back(t.getTransform().transformPoint({ getPointX(i), getPointY(i) }));
		points.emplace_back(t.getTransform().transformPoint({ getPointX(i+1), getPointY(i+1) }));

		auto n = points.size();
		auto v = points[n - 1] - points[n - 2];
		normalize(v);
		axis.emplace_back(perpendicularVector(v));
	}

	/*CircleConstAggregator::CircleConstAggregator(const Collider& data) : detail::AggregatorBase(data) {}

	sf::FloatRect CircleConstAggregator::getBoundingBox(const TransformComponent& t) const
	{
		return getBoundingBox(t.getTransform());
	}

	sf::FloatRect CircleConstAggregator::getBoundingBox(sf::Transform t) const
	{
		auto tcenter = t.transformPoint({ getCenterX(), getCenterY() });
		return { tcenter.x - getRadius(), tcenter.y - getRadius(), 2 * getRadius(), 2 * getRadius() };
	}

	void CircleConstAggregator::getAxisAndPivots(
		const TransformComponent& t, std::vector<sf::Vector2f>& axis, std::vector<sf::Vector2f>& points, unsigned i) const
	{
		//no additional axis for
	}


	CircleAggregator::CircleAggregator(Collider& data) : CircleConstAggregator(data) {}

	Collider CircleAggregator::transform(const TransformComponent& t) const
	{
		auto tcenter = t.getTransform().transformPoint({ getCenterX(), getCenterY() });
		return { ColliderType::CIRCLE, {tcenter.x, tcenter.y, getRadius() * std::min(t.getScale().x, t.getScale().y) } };
	}

	void CircleAggregator::move(const sf::Vector2f& vec) const
	{
		setCenterX(getCenterX() + vec.x);
		setCenterY(getCenterY() + vec.y);
	}*/
}
