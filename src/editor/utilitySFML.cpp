#include "utilitySFML.h"
#include "ungod/base/Logger.h"

namespace uedit
{
	void PressedKeyClickChecker::handleEvent(const sf::Event& event)
	{
		if (event.type == sf::Event::MouseButtonPressed && 
			event.mouseButton.button == sf::Mouse::Left &&
			sf::Keyboard::isKeyPressed(mKey))
				mOnClick({ event.mouseButton.x,event.mouseButton.y });
	}
}
