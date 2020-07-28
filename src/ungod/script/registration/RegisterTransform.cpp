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

#include "ungod/script/registration/RegisterTransform.h"
#include "ungod/base/Transform.h"

namespace ungod
{
    namespace scriptRegistration
    {
		sf::Vector2f TransformHandlerFrontEnd::getPosition() const
		{
			return mEntity.get<TransformComponent>().getPosition();
		}

		sf::Vector2f TransformHandlerFrontEnd::getSize() const
		{
			return mEntity.get<TransformComponent>().getSize();
		}

		sf::Vector2f TransformHandlerFrontEnd::getCenterPosition() const
		{
			return mEntity.get<TransformComponent>().getCenterPosition();
		}

		sf::Vector2f TransformHandlerFrontEnd::getGlobalUpperBounds() const
		{
			return mEntity.get<TransformComponent>().getGlobalUpperBounds();
		}

		const sf::Vector2f& TransformHandlerFrontEnd::getUpperBounds() const
		{
			return mEntity.get<TransformComponent>().getUpperBounds();
		}

		const sf::Vector2f& TransformHandlerFrontEnd::getLowerBounds() const
		{
			return mEntity.get<TransformComponent>().getLowerBounds();
		}

		const sf::Vector2f& TransformHandlerFrontEnd::getScale() const
		{
			return mEntity.get<TransformComponent>().getScale();
		}

		const sf::Vector2f& TransformHandlerFrontEnd::getBaseLineOffsets() const
		{
			return mEntity.get<TransformComponent>().getBaseLineOffsets();
		}

		void TransformHandlerFrontEnd::setPosition(const sf::Vector2f& pos)
		{
			mHandler.setPosition(mEntity, pos);
		}

		void TransformHandlerFrontEnd::setScale(const sf::Vector2f& scale)
		{
			mHandler.setScale(mEntity, scale);
		}

		void TransformHandlerFrontEnd::setBaseLineOffsets(const sf::Vector2f& offsets)
		{
			mHandler.setBaseLineOffsets(mEntity, offsets);
		}

		void TransformHandlerFrontEnd::move(const sf::Vector2f& offset)
		{
			mHandler.move(mEntity, offset);
		}

        void registerTransform(ScriptStateBase& state)
        {
			script::Usertype<TransformHandlerFrontEnd> transfHandlerFrontEndType = state.registerUsertype<TransformHandlerFrontEnd>("TransformHandlerFrontEnd");
			transfHandlerFrontEndType["getSize"] = &TransformHandlerFrontEnd::getSize;
			transfHandlerFrontEndType["getPosition"] = &TransformHandlerFrontEnd::getPosition;
			transfHandlerFrontEndType["getCenterPosition"] = &TransformHandlerFrontEnd::getCenterPosition;
			transfHandlerFrontEndType["getGlobalUpperBounds"] = &TransformHandlerFrontEnd::getGlobalUpperBounds;
			transfHandlerFrontEndType["getUpperBounds"] = &TransformHandlerFrontEnd::getUpperBounds;
			transfHandlerFrontEndType["getLowerBounds"] = &TransformHandlerFrontEnd::getLowerBounds;
			transfHandlerFrontEndType["getScale"] = &TransformHandlerFrontEnd::getScale;
			transfHandlerFrontEndType["getBaseLineOffsets"] = &TransformHandlerFrontEnd::getBaseLineOffsets;
			transfHandlerFrontEndType["setPosition"] = &TransformHandlerFrontEnd::setPosition;
			transfHandlerFrontEndType["setBaseLineOffsets"] = &TransformHandlerFrontEnd::setBaseLineOffsets;
			transfHandlerFrontEndType["setScale"] = &TransformHandlerFrontEnd::setScale;
			transfHandlerFrontEndType["move"] = &TransformHandlerFrontEnd::move;
        }
    }
}
