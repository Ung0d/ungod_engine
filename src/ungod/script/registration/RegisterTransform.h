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

#ifndef REGISTER_TRANSFORM_H
#define REGISTER_TRANSFORM_H

#include <SFML/System/Vector2.hpp>
#include "ungod/script/Behavior.h"

namespace ungod
{
	class Entity;
	class TransformHandler;

    namespace scriptRegistration
    {
		class TransformHandlerFrontEnd
		{
		public:
			TransformHandlerFrontEnd(Entity& e, TransformHandler& h) : mEntity(e), mHandler(h) {}
			sf::Vector2f getPosition() const;
			sf::Vector2f getSize() const;
			sf::Vector2f getCenterPosition() const;
			sf::Vector2f getGlobalUpperBounds() const;
			sf::Vector2f getUpperBounds() const;
			sf::Vector2f getLowerBounds() const;
			sf::Vector2f getScale() const;
			sf::Vector2f getBaseLineOffsets() const;
			void setPosition(const sf::Vector2f& pos);
			void setScale(const sf::Vector2f& scale);
			void setBaseLineOffsets(const sf::Vector2f& offsets);
			void move(const sf::Vector2f& offset);

		private:
			Entity& mEntity;
			TransformHandler& mHandler;
		};

        /** \brief Registers transform functionality for scripts. */
        void registerTransform(ScriptStateBase& state);
    }
}


#endif
