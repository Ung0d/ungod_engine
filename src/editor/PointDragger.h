#ifndef UEDIT_POINT_DRAGGER_H
#define UEDIT_POINT_DRAGGER_H

#include <functional>
#include <SFML/Graphics.hpp>
#include "ungod/base/Utility.h"
#include "ungod/base/Entity.h"
#include "ActionManager.h"

namespace uedit
{
	class EntityPreview;

	/** \brief A reusable class that wraps a 2d position and enables mouse dragging of this position. */
	class PointDragger
	{
	public:
		PointDragger(const std::function<void(const sf::Vector2f&)>& setter, 
					const std::function<sf::Vector2f()>& getter);


		bool handleInput(EntityPreview& preview, const sf::Event& event);

		void render(sf::RenderWindow& window, sf::RenderStates states) const;

	private:
		std::function<void(const sf::Vector2f&)> mSetter;
		std::function<sf::Vector2f()> mGetter;
		sf::Vector2f mLastMouse;
		bool mDragged;

		static constexpr float DRAG_DISTANCE = 15.0f;
	};


	/** \brief Comps C used with PointDraggerSet<C> must implement this interface. */
	template<typename COMP>
	struct CompTraits
	{
		static unsigned numPoints(const COMP& c)
		{
			static_assert(ungod::AlwaysFalse<COMP>::value, "No explicit specialization for this type found!");
			return 0u;
		}

		static void setup(ungod::Entity e, COMP& c, ActionManager& actionManager, std::list<PointDragger>& draggers)
		{
			static_assert(ungod::AlwaysFalse<COMP>::value, "No explicit specialization for this type found!");
		}
	};


	/** \brief A set of point draggers for a component and additional multi-components. */
	template<typename COMP>
	class PointDraggerSet
	{
	public:
		PointDraggerSet(ActionManager& actionManager) : mActionManager(actionManager) {}

		bool handleEvent(EntityPreview& preview, const sf::Event& event)
		{
			bool drag = false;
			for (auto& d : mPointDraggers)
				drag = drag || d.handleInput(preview, event);
			return drag;
		}

		void render(sf::RenderWindow& window, sf::RenderStates states) const
		{
			for (const auto& d : mPointDraggers)
				d.render(window, states);
		}

		void setupPointDraggers(ungod::Entity e)
		{
			mPointDraggers.clear();
			if (e.has<COMP>())
				CompTraits<COMP>::setup(e, e.modify<COMP>(), mActionManager, mPointDraggers);
			if (e.has<ungod::MultiComponent<COMP>>())
				for (unsigned i = 0; i < e.get<ungod::MultiComponent<COMP>>().getComponentCount(); i++)
					CompTraits<COMP>::setup(e, e.modify<ungod::MultiComponent<COMP>>().getComponent(i), mActionManager, mPointDraggers);
		}

		/** \brief Notifies the set that a point might be added or remove for some component and triggers
		* a rebuild. This method assumes that addition and removal are distinct operations and can not
		* occur at the same timestep. */
		void notifyChange(ungod::Entity e)
		{
			unsigned numPoints = 0;
			if (e.has<COMP>())
				numPoints += CompTraits<COMP>::numPoints(e.get<COMP>());
			if (e.has<ungod::MultiComponent<COMP>>())
				for (unsigned i = 0; i < e.get<ungod::MultiComponent<COMP>>().getComponentCount(); i++)
					numPoints += CompTraits<COMP>::numPoints(e.get<ungod::MultiComponent<COMP>>().getComponent(i));
			if (mPointDraggers.size() != numPoints)
				setupPointDraggers(e);
		}
	private:
		std::list<PointDragger> mPointDraggers;
		ActionManager& mActionManager;
	};
}

#endif //UEDIT_POINT_DRAGGER_H