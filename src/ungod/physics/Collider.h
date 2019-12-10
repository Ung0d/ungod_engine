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

#ifndef UNGOD_COLLIDER_H
#define UNGOD_COLLIDER_H

#include <array>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

namespace ungod
{
	enum class ColliderType : int
	{
		UNDEFINED,
		ROTATED_RECT,
		CONVEX_POLYGON,
		EDGE_CHAIN,
		CIRCLE
	};

	class TransformComponent;

    /**
    * \brief A Collider with modular type and a set of floating point parameters.
    */
	class Collider
	{
	public:
		static constexpr unsigned MAX_PARAM = 12;

		Collider();
		Collider(ColliderType type, std::vector<float> param);

		/** \brief Initializes a rotated rectangle. */
		void initRotatedRect(const sf::Vector2f& upleft, const sf::Vector2f& downRight, float rotation);

		/** \brief Initializes a rotated rectangle. */
		void initConvexPolygon(const std::vector<sf::Vector2f>& points);

		/** \brief Initializes a line strip. */
		void initEdgeChain(const std::vector<sf::Vector2f>& points);

		/** \brief Initializes a circle. */
		void initCircle(const sf::Vector2f& center, float radius);

		/** \brief Resets the collider to an undefined state. It will be ignored in all computations. */
		void reset();

		/** \brief Returns the ith float parameter. */
		float getParam(unsigned i) const { return mParam[i]; }

		/** \brief Sets the ith float parameter. */
		void setParam(unsigned i, float p) { mParam[i]=p; }

		/** \brief Returns the number of floating point parameters in the buffer actively used. */
		unsigned getNumParam() const { return mNumParamUsed; }

		/** \brief Sets the number of parameters to use. */
		void allocateParam(unsigned n) { mNumParamUsed = n; }

		ColliderType getType() const { return mType; }

		/** \brief Returns the bounding box of the collider. Requires an internal type-check. */
		sf::FloatRect getBoundingBox() const;

		/** \brief Moves the collider. Requires an internal type-check. */
		void move(const sf::Vector2f& vec);

		/** \brief Retrieves axis end pivots to check by SAT algorithm. */
		void getAxisAndPivots(const TransformComponent& t, std::vector<sf::Vector2f>& axis, std::vector<sf::Vector2f>& pivots, unsigned i) const;

		/** \brief Returns the center point of the collider. Requires an internal type-check. */
		sf::Vector2f getCenter() const;

		/** \brief Detail: returns the number of SAT runs required to check this collider. Requires an internal type-check. */
		unsigned getNumRuns() const;

	private:
		std::array<float, MAX_PARAM> mParam; ///<floating point parameters of the collider, ordering and structure depend on the collider type
		ColliderType mType;
		unsigned mNumParamUsed;

	private:
		//stores the values of a 2d point at param[i] and param[i+1]
		void accomodatePoint(unsigned i, const sf::Vector2f& point);
	};

	/** \brief Convenience factory. */
	Collider makeRotatedRect(const sf::Vector2f& upleft, const sf::Vector2f& downRight, float rotation= 0.0f);

	/** \brief Convenience factory. */
	Collider makeConvexPolygon(const std::vector<sf::Vector2f>& points);

	/** \brief Convenience factory. */
	Collider makeEdgeChain(const std::vector<sf::Vector2f>& points);

	/** \brief Convenience factory. */
	Collider makeCircle(const sf::Vector2f& center, float radius);

	namespace detail
	{
		class AggregatorBase
		{
		public:
			AggregatorBase(const Collider& data);
		protected:
			const Collider& mData;
		};
	}


	/** \brief A class with only additional logic that can be aggregated by ColliderData in
	* order to grant convenient functionality. */
	class RotatedRectConstAggregator : public detail::AggregatorBase
	{
	public:
		RotatedRectConstAggregator(const Collider& data);

		/** \brief Returns the center of the bounding box of the polygon. */
		sf::Vector2f getCenter() const;

		/** \brief Returns the bounding box in the "default" coordinate system of the collider. */
		sf::FloatRect getBoundingBox(const TransformComponent& t) const;
		sf::FloatRect getBoundingBox(sf::Transform t = sf::Transform()) const;

		/** \brief Retrieves all axis and all pivot points to check for by SAT algorithm. */
		void getAxisAndPivots(const TransformComponent& t, std::vector<sf::Vector2f>& axis, std::vector<sf::Vector2f>& points, unsigned) const;

		inline float getRotation() const { return mData.getParam(0); }
		inline float getUpLeftX() const { return mData.getParam(1); }
		inline float getUpLeftY() const { return mData.getParam(2); }
		inline float getDownRightX() const { return mData.getParam(3); }
		inline float getDownRightY() const { return mData.getParam(4); }
	};


	/** \brief A class with only additional logic that can be aggregated by ColliderData in 
	* order to grant convenient functionality. */
	class RotatedRectAggregator : public RotatedRectConstAggregator
	{
	public:
		RotatedRectAggregator(Collider& data);

		/** \brief Returns new collider data that is transformed according to the given transform. */
		Collider transform(const TransformComponent& t) const;

		/** \brief Moves the internal position of the collider along the given vector. */
		void move(const sf::Vector2f& vec) const;

		inline void setRotation(float r) const { getData().setParam(0, r); }
		inline void setUpLeftX(float x) const { getData().setParam(1, x); }
		inline void setUpLeftY(float y) const { getData().setParam(2, y); }
		inline void setDownRightX(float x) const { getData().setParam(3, x); }
		inline void setDownRightY(float y) const { getData().setParam(4, y); }

	private:
		Collider& getData() const { return const_cast<Collider&>(mData); }
	};


	/** \brief A class with only additional logic that can be aggregated by ColliderData in
	* order to grant convenient functionality. */
	class PointSetConstAggregator : public detail::AggregatorBase
	{
	public:
		PointSetConstAggregator(const Collider& data);

		/** \brief Returns the number of points o the polygon. */
		unsigned getNumberOfPoints() const;

		/** \brief Returns the bounding box in the "default" coordinate system of the collider. */
		sf::FloatRect getBoundingBox(const TransformComponent& t) const;
		sf::FloatRect getBoundingBox(sf::Transform t = sf::Transform()) const;

		/** \brief Returns the center of the bounding box of the polygon. */
		sf::Vector2f getCenter() const;

		inline float getPointX(unsigned i) const { return mData.getParam(2 * i); }
		inline float getPointY(unsigned i) const { return mData.getParam(2 * i + 1); }
	};

	/** \brief A class with only additional logic that can be aggregated by ColliderData in
	* order to grant convenient functionality. */
	class PointSetAggregator : public PointSetConstAggregator
	{
	public:
		PointSetAggregator(Collider& data);

		/** \brief Sets the number of polygon points. Must be below MAX_PARAM/2. */
		void allocatePoints(unsigned n) const;

		/** \brief Returns new collider data that is transformed according to the given transform. */
		Collider transform(const TransformComponent& t) const;

		/** \brief Moves the internal position of the collider along the given vector. */
		void move(const sf::Vector2f& vec) const;

		inline void setPointX(unsigned i, float x) const { getData().setParam(2 * i, x); }
		inline void setPointY(unsigned i, float y) const { getData().setParam(2 * i + 1, y); }

	private:
		Collider& getData() const { return const_cast<Collider&>(mData); }
	};


	/** \brief A class with only additional logic that can be aggregated by ColliderData in
	* order to grant convenient functionality. */
	class ConvexPolygonConstAggregator : public PointSetConstAggregator
	{
	public:
		ConvexPolygonConstAggregator(const Collider& data);

		/** \brief Retrieves all axis and all pivot points to check for by SAT algorithm. */
		void getAxisAndPivots(const TransformComponent& t, std::vector<sf::Vector2f>& axis, std::vector<sf::Vector2f>& points, unsigned i) const;
	};


	/** \brief A class with only additional logic that can be aggregated by ColliderData in
	* order to grant convenient functionality. */
	class EdgeChainConstAggregator : public PointSetConstAggregator
	{
	public:
		EdgeChainConstAggregator(const Collider& data);

		/** \brief Retrieves all axis and all pivot points to check for by SAT algorithm. */
		void getAxisAndPivots(const TransformComponent& t, std::vector<sf::Vector2f>& axis, std::vector<sf::Vector2f>& points, unsigned i) const;
	};



	/** \brief A class with only additional logic that can be aggregated by ColliderData in
	* order to grant convenient functionality. */
	class CircleConstAggregator : public detail::AggregatorBase
	{
	public:
		CircleConstAggregator(const Collider& data);

		/** \brief Returns the bounding box in the "default" coordinate system of the collider. */
		sf::FloatRect getBoundingBox(const TransformComponent& t) const;
		sf::FloatRect getBoundingBox(sf::Transform t = sf::Transform()) const;

		inline float getCenterX() const { return mData.getParam(0); }
		inline float getCenterY() const { return mData.getParam(1); }
		inline float getRadius() const { return mData.getParam(2); }
	};


	/** \brief A class with only additional logic that can be aggregated by ColliderData in
	* order to grant convenient functionality. */
	class CircleAggregator : public CircleConstAggregator
	{
	public:
		CircleAggregator(Collider& data);

		/** \brief Returns new collider data that is transformed according to the given transform. */
		Collider transform(const TransformComponent& t) const;

		/** \brief Moves the internal position of the collider along the given vector. */
		void move(const sf::Vector2f& vec) const;

		inline void setCenterX(float x) const { getData().setParam(0, x); }
		inline void setCenterY(float y) const { getData().setParam(1, y); }
		inline void setRadius(float r) const { getData().setParam(2, r); }

	private:
		Collider& getData() const { return const_cast<Collider&>(mData); }
	};
}

#endif // UNGOD_COLLIDER_H
