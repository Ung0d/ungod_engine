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


	void Collider::initCircle(const sf::Vector2f& center, float radius)
	{
		mType = ColliderType::CIRCLE;
		accomodatePoint(0, center);
		mParam[2] = radius;
		mNumParamUsed = 3;
	}


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
		case ColliderType::CIRCLE:
			return CircleConstAggregator{ *this }.getBoundingBox();
		default:
			return {};
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

	Collider makeCircle(const sf::Vector2f& center, float radius)
	{
		Collider c;
		c.initCircle(center, radius);
		return c;
	}


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

	std::tuple<std::vector<sf::Vector2f>, std::vector<sf::Vector2f>> RotatedRectConstAggregator::getAxisAndPivots(const TransformComponent& t) const
	{
		sf::Transform finalTransf;
		finalTransf *= t.getTransform();
		finalTransf.rotate(getRotation(), getCenter());

		//get the axis to test
		std::vector<sf::Vector2f> axis{ 2 };
		std::vector<sf::Vector2f> points{ 4 };

		points[0] = finalTransf.transformPoint({ getUpLeftX(), getUpLeftY() });
		points[1] = finalTransf.transformPoint({ getDownRightX(), getUpLeftY() });
		points[2] = finalTransf.transformPoint({ getDownRightX(), getDownRightY() });
		points[3] = finalTransf.transformPoint({ getUpLeftX(), getDownRightY() });

		axis[0] = points[1] - points[0];
		axis[1] = points[3] - points[0];

		return { points, axis };
	}


	RotatedRectAggregator::RotatedRectAggregator(const Collider& data) : RotatedRectConstAggregator(data) {}

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
		sf::FloatRect bounds{ t0, {} };
		for (unsigned i = 1; i < getNumberOfPoints(); i++)
		{
			auto ti = t.transformPoint({ getPointX(i), getPointY(i) });
			bounds.left = std::min(ti.x, bounds.left);
			bounds.top = std::min(ti.y, bounds.top);
			bounds.width = std::max(ti.x - bounds.left, bounds.width);
			bounds.height = std::max(ti.y - bounds.height, bounds.height);
		}
		return bounds;
	}


	/*sf::Vector2f PointSetConstAggregator::getCenter() const
	{
		switch (getNumberOfPoints() == 0)
		{
		case 1u:
			return { getPointX(0), getPointY(0) };

		case 2u:
			return getCenterPoint({ getPointX(0), getPointY(0) }, { getPointX(1), getPointY(1) });

		case 3u:
			break; //foward to actual method below

		default:
			return {}
		}


	}*/

	std::tuple<std::vector<sf::Vector2f>, std::vector<sf::Vector2f>>
		PointSetConstAggregator::getAxisAndPivots(const TransformComponent& t) const
	{
		//get the axis to test
		std::vector<sf::Vector2f> axis{ getNumberOfPoints() };
		std::vector<sf::Vector2f> points{ getNumberOfPoints() };

		for (unsigned i = 0; i < getNumberOfPoints(); i++)
			points[i] = t.getTransform().transformPoint({ getPointX(i), getPointY(i) });

		for (unsigned i = 0; i < getNumberOfPoints(); i++)
		{
			unsigned inext = (i + 1) % getNumberOfPoints();
			axis[i] = points[inext] - points[i];
		}

		return { points, axis };
	}


	PointSetAggregator::PointSetAggregator(const Collider& data) : PointSetConstAggregator(data) {}


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


	CircleConstAggregator::CircleConstAggregator(const Collider& data) : detail::AggregatorBase(data) {}

	sf::FloatRect CircleConstAggregator::getBoundingBox(const TransformComponent& t) const
	{
		return getBoundingBox(t.getTransform());
	}

	sf::FloatRect CircleConstAggregator::getBoundingBox(sf::Transform t) const
	{
		auto tcenter = t.transformPoint({ getCenterX(), getCenterY() });
		return { tcenter.x - getRadius(), tcenter.y - getRadius(), 2 * getRadius(), 2 * getRadius() };
	}


	CircleAggregator::CircleAggregator(const Collider& data) : CircleConstAggregator(data) {}

	Collider CircleAggregator::transform(const TransformComponent& t) const
	{
		auto tcenter = t.getTransform().transformPoint({ getCenterX(), getCenterY() });
		return { ColliderType::CIRCLE, {tcenter.x, tcenter.y, getRadius() * std::min(t.getScale().x, t.getScale().y) } };
	}

	void CircleAggregator::move(const sf::Vector2f& vec) const
	{
		setCenterX(getCenterX() + vec.x);
		setCenterY(getCenterY() + vec.y);
	}
}
