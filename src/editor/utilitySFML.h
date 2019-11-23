#ifndef UEDIT_UTILITY_SFML
#define UEDIT_UTILITY_SFML

#include <SFML/System.hpp>
#include <SFML/Window/Event.hpp>
#include "owls/Signal.h"

namespace uedit
{
	class PressedKeyClickChecker
	{
	public:
		PressedKeyClickChecker(sf::Keyboard::Key key = sf::Keyboard::Key::LShift) : mKey(key) {}
		void handleEvent(const sf::Event& event);

		inline decltype(auto) onClick(const std::function<void(const sf::Vector2i&)>& callback)
		{
			return mOnClick.connect(callback);
		}

	private:
		owls::Signal<const sf::Vector2i&> mOnClick;
		sf::Keyboard::Key mKey;
	};
}

#endif // !UEDIT_UTILITY_SFML
