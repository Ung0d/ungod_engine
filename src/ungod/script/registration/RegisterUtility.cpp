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

#include "ungod/script/registration/RegisterUtility.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>
#include "ungod/physics/Physics.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerUtility(ScriptStateBase& state)
        {
            state.registerUsertype<sf::Vector2f>("Vec2f",
                                                                sol::constructors<sf::Vector2f(), sf::Vector2f(float x, float y)>(),
                                                                    "x", &sf::Vector2f::x,
                                                                    "y", &sf::Vector2f::y);

            state.registerUsertype<sf::Vector2i>("Vec2i",
                                                                sol::constructors<sf::Vector2i(), sf::Vector2i(int x, int y)>(),
                                                                    "x", &sf::Vector2i::x,
                                                                    "y", &sf::Vector2i::y);

            state.registerUsertype<sf::IntRect>("IntRect",
                                                                sol::constructors<sf::IntRect(), sf::IntRect(const sf::Vector2i& pos, const sf::Vector2i& size), sf::IntRect(int left, int top, int width, int height)>(),
                                                                    "left", &sf::IntRect::left,
                                                                    "top", &sf::IntRect::top,
                                                                    "width", &sf::IntRect::width,
                                                                    "height", &sf::IntRect::height);

            state.registerUsertype<sf::FloatRect>("FloatRect",
                                                                sol::constructors<sf::FloatRect(), sf::FloatRect(const sf::Vector2f& pos, const sf::Vector2f& size), sf::FloatRect(float left, float top, float width, float height)>(),
                                                                    "left", &sf::FloatRect::left,
                                                                    "top", &sf::FloatRect::top,
                                                                    "width", &sf::FloatRect::width,
                                                                    "height", &sf::FloatRect::height);

            state.registerUsertype<sf::Color>("Color",
                                                                sol::constructors<sf::Color(), sf::Color(uint8_t r, uint8_t g, uint8_t b), sf::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)>(),
                                                                    "r", &sf::Color::r, "g", &sf::Color::g, "b", &sf::Color::b, "a", &sf::Color::a);

            state.registerFunction("distance", [] (const sf::Vector2f& p1, const sf::Vector2f& p2) { return distance(p1, p2); });

            state.registerFunction("randProb", []() { return NumberGenerator::getFloatRandBetw(0,1); });

            script::Usertype<sf::Clock> clockType = state.registerUsertype<sf::Clock>("Clock");
            clockType["restart"] = &sf::Clock::restart;
            clockType["elapsedSeconds"] = [](sf::Clock& clock) -> float { return clock.getElapsedTime().asSeconds(); };

            state.registerFunction("makeClock", []() { return sf::Clock{}; });


            //register a function, that enumerates all files in a directory
            state.registerFunction("directoryFiles", &ungod::enumerateDirectory);
        }
    }
}
