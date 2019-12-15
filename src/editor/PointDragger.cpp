#include "PointDragger.h"
#include "EntityPreview.h"

namespace uedit
{
	PointDragger::PointDragger(const std::function<void(const sf::Vector2f&)>& setter, const std::function<sf::Vector2f()>& getter) :
		mSetter(setter), mGetter(getter), mLastMouse(sf::Mouse::getPosition()), mDragged(false)
	{}


	bool PointDragger::handleInput(EntityPreview& preview, const sf::Event& event) 
	{
		sf::Vector2f mousePos = preview.getMousePosition();
		sf::Vector2f mousePosLocal = preview.getEntity().get<ungod::TransformComponent>().getTransform().getInverse().transformPoint(mousePos);
		switch (event.type)
		{
			case sf::Event::MouseButtonPressed:
			{
				mLastMouse = mousePos;
				if (event.mouseButton.button == sf::Mouse::Left && ungod::distance(mousePosLocal, mGetter()) <= DRAG_DISTANCE)
					mDragged = true;
				break;
			}
			case sf::Event::MouseButtonReleased:
			{
				mDragged = false;
				break;
			}
			case sf::Event::MouseMoved:
			{
				if (mDragged)
				{
					mSetter(mGetter() + mousePos - mLastMouse);
					mLastMouse = mousePos;
				}
				break;
			}
		}
		return mDragged;
	}


	void PointDragger::render(sf::RenderWindow& window, sf::RenderStates states) const
	{
		if (!mDragged)
			return;
		static constexpr int radius = 10;
		sf::CircleShape circle(radius);
		circle.setFillColor(sf::Color::Red);
		circle.setPosition(mGetter() - sf::Vector2f{ radius / 2, radius / 2 });
		window.draw(circle, states);
	}
}