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

#include "ungod/script/registration/RegisterLight.h"
#include "ungod/script/registration/RegistrationDetail.h"
#include "ungod/visual/LightHandler.h"

namespace ungod
{
    namespace scriptRegistration
    {
		void LightHandlerFrontEnd::setLightPosition(const sf::Vector2f& pos)
		{
			mHandler.setLocalLightPosition(mEntity, pos);
		}

		void LightHandlerFrontEnd::setLightPosition(const sf::Vector2f& pos, std::size_t index)
		{
			mHandler.setLocalLightPosition(mEntity, pos, index);
		}

		void LightHandlerFrontEnd::setLightScale(const sf::Vector2f& scale)
		{
			mHandler.setLightScale(mEntity, scale);
		}

		void LightHandlerFrontEnd::setLightScale(const sf::Vector2f& scale, std::size_t index)
		{
			mHandler.setLightScale(mEntity, scale, index);
		}

		void LightHandlerFrontEnd::setLightColor(const sf::Color& color)
		{
			mHandler.setLightColor(mEntity, color);
		}

		void LightHandlerFrontEnd::setLightColor(const sf::Color& color, std::size_t index)
		{
			mHandler.setLightColor(mEntity, color, index);
		}

		void LightHandlerFrontEnd::setColliderPoint(const sf::Vector2f& point, std::size_t pointIndex)
		{
			mHandler.setPoint(mEntity, point, pointIndex);
		}

		void LightHandlerFrontEnd::setColliderPoint(const sf::Vector2f& point, std::size_t pointIndex, std::size_t colliderIndex)
		{
			mHandler.setPoint(mEntity, point, pointIndex, colliderIndex);
		}

		void LightHandlerFrontEnd::setColliderPointCount(std::size_t num)
		{
			mHandler.setPointCount(mEntity, num);
		}

		void LightHandlerFrontEnd::setColliderPointCount(std::size_t num, std::size_t colliderIndex)
		{
			mHandler.setPointCount(mEntity, num, colliderIndex);
		}

		void LightHandlerFrontEnd::setColliderPoints(script::Environment points)
		{
			std::vector<sf::Vector2f> pvec;
			if (detail::table2vec(pvec, points))
				mHandler.setPoints(mEntity, pvec);
		}

		void LightHandlerFrontEnd::setColliderPoints(script::Environment points, std::size_t index)
		{
			std::vector<sf::Vector2f> pvec;
			if (detail::table2vec(pvec, points))
				mHandler.setPoints(mEntity, pvec, index);
		}

		void LightHandlerFrontEnd::setLightOverShape(bool los)
		{
			mHandler.setLightOverShape(mEntity, los);
		}

		void LightHandlerFrontEnd::setLightOverShape(bool los, std::size_t index)
		{
			mHandler.setLightOverShape(mEntity, los, index);
		}

		void LightHandlerFrontEnd::setLightActive(bool active)
		{
			mHandler.setLightActive(mEntity, active);
		}

		void LightHandlerFrontEnd::setLightActive(bool active, std::size_t index)
		{
			mHandler.setLightActive(mEntity, active, index);
		}

		bool LightHandlerFrontEnd::isLightActive()
		{
			return mEntity.get<LightEmitterComponent>().getLight().isActive();
		}

		bool LightHandlerFrontEnd::isLightActive(std::size_t index)
		{
			return mEntity.get<MultiLightEmitter>().getComponent(index).getLight().isActive();
		}

		void LightHandlerFrontEnd::setAffectorCallback(const std::function<void(float, LightEmitterComponent&)>& callback)
		{
			mHandler.setAffectorCallback(mEntity, callback);
		}

		void LightHandlerFrontEnd::setAffectorCallback(const std::function<void(float, LightEmitterComponent&)>& callback, std::size_t lightIndex)
		{
			mHandler.setAffectorCallback(mEntity, lightIndex, callback);
		}

		void LightHandlerFrontEnd::setAffectorCallback(const std::function<void(float, LightEmitterComponent&)>& callback, std::size_t lightIndex, std::size_t affectorIndex)
		{
			mHandler.setAffectorCallback(mEntity, lightIndex, affectorIndex, callback);
		}

		void LightHandlerFrontEnd::setAffectorActive(bool active)
		{
			mEntity.modify<LightAffectorComponent>().setActive(active);
		}

		void LightHandlerFrontEnd::setAffectorActive(bool active, std::size_t affectorIndex)
		{
			mEntity.modify<MultiLightAffector>().getComponent(affectorIndex).setActive(active);
		}

		bool LightHandlerFrontEnd::isAffectorActive()
		{
			return mEntity.get<LightAffectorComponent>().isActive();
		}

		bool LightHandlerFrontEnd::isAffectorActive(std::size_t affectorIndex)
		{
			return mEntity.get<MultiLightAffector>().getComponent(affectorIndex).isActive();
		}


        void registerLight(ScriptStateBase& state)
        {
			state.registerFunction("LightFlickering",
				[](float period, float strength) -> std::function<void(float, LightEmitterComponent&)>
				{ return LightFlickering(period, strength); });

			state.registerFunction("RandomizedFlickering",
				[](float baseperiod, float strength) -> std::function<void(float, LightEmitterComponent&)>
				{ return RandomizedFlickering(baseperiod, strength); });

			state.registerUsertype<LightEmitterComponent>("LightEmitter");

			script::Usertype<LightHandler> lightHandlerType = state.registerUsertype<LightHandler>("LightHandler");
			lightHandlerType["setAmbientColor"] = & LightHandler::setAmbientColor;
			lightHandlerType["getAmbientColor"] = &LightHandler::getAmbientColor;

			script::Usertype<LightHandlerFrontEnd> lightHandlerFrontEndType = state.registerUsertype<LightHandlerFrontEnd>("LightHandlerFrontEnd");
			lightHandlerFrontEndType["setLightPosition"] =
				sol::overload(
					[](LightHandlerFrontEnd& lhfe, const sf::Vector2f& position) { lhfe.setLightPosition(position); },
					[](LightHandlerFrontEnd& lhfe, const sf::Vector2f& position, std::size_t index) { lhfe.setLightPosition(position, index); });
			lightHandlerFrontEndType["setLightScale"] =
				sol::overload(
					[](LightHandlerFrontEnd& lhfe, const sf::Vector2f& scale) { lhfe.setLightScale(scale); },
					[](LightHandlerFrontEnd& lhfe, const sf::Vector2f& scale, std::size_t index) { lhfe.setLightScale(scale, index); });
			lightHandlerFrontEndType["setLightColor"] =
				sol::overload(
					[](LightHandlerFrontEnd& lhfe, const sf::Color& color) { lhfe.setLightColor(color); },
					[](LightHandlerFrontEnd& lhfe, const sf::Color& color, std::size_t index) { lhfe.setLightColor(color, index); });
			lightHandlerFrontEndType["setColliderPoint"] =
				sol::overload(
					[](LightHandlerFrontEnd& lhfe, const sf::Vector2f& point, std::size_t i) { lhfe.setColliderPoint(point, i); },
					[](LightHandlerFrontEnd& lhfe, const sf::Vector2f& point, std::size_t pointIndex, std::size_t colliderIndex) { lhfe.setColliderPoint(point, pointIndex, colliderIndex); });
			lightHandlerFrontEndType["setColliderPointCount"] =
				sol::overload(
					[](LightHandlerFrontEnd& lhfe, std::size_t num) { lhfe.setColliderPointCount(num); },
					[](LightHandlerFrontEnd& lhfe, std::size_t num, std::size_t colliderIndex) { lhfe.setColliderPointCount(num, colliderIndex); });
			lightHandlerFrontEndType["setColliderPoints"] =
				sol::overload(
					[](LightHandlerFrontEnd& lhfe, script::Environment points) { lhfe.setColliderPoints(points); },
					[](LightHandlerFrontEnd& lhfe, script::Environment points, std::size_t colliderIndex) { lhfe.setColliderPoints(points, colliderIndex); });
			lightHandlerFrontEndType["setLightOverShape"] =
				sol::overload(
					[](LightHandlerFrontEnd& lhfe, bool los) { lhfe.setLightOverShape(los); },
					[](LightHandlerFrontEnd& lhfe, bool los, std::size_t colliderIndex) { lhfe.setLightOverShape(los, colliderIndex); });
			lightHandlerFrontEndType["setLightActive"] =
				sol::overload(
					[](LightHandlerFrontEnd& lhfe, bool active) { lhfe.setLightActive(active); },
					[](LightHandlerFrontEnd& lhfe, bool active, std::size_t colliderIndex) { lhfe.setLightActive(active, colliderIndex); });
			lightHandlerFrontEndType["isLightActive"] =
				sol::overload(
					[](LightHandlerFrontEnd& lhfe) { return lhfe.isLightActive(); },
					[](LightHandlerFrontEnd& lhfe, std::size_t colliderIndex) { return lhfe.isLightActive(colliderIndex); });
			lightHandlerFrontEndType["setAffectorCallback"] =
				sol::overload([](LightHandlerFrontEnd& lhfe, const std::function<void(float, LightEmitterComponent&)>& callback)
					{ lhfe.setAffectorCallback(callback); },
					[](LightHandlerFrontEnd& lhfe, const std::function<void(float, LightEmitterComponent&)>& callback, std::size_t lightIndex)
					{ lhfe.setAffectorCallback(callback, lightIndex); },
					[](LightHandlerFrontEnd& lhfe, const std::function<void(float, LightEmitterComponent&)>& callback, std::size_t lightIndex, std::size_t affectorIndex)
					{ lhfe.setAffectorCallback(callback, lightIndex, affectorIndex); });
			lightHandlerFrontEndType["setAffectorActive"] =
				sol::overload(
					[](LightHandlerFrontEnd& lhfe, bool active) { lhfe.setAffectorActive(active); },
					[](LightHandlerFrontEnd& lhfe, bool active, std::size_t colliderIndex) { lhfe.setAffectorActive(active, colliderIndex); });
			lightHandlerFrontEndType["isAffectorActive"] =
				sol::overload(
					[](LightHandlerFrontEnd& lhfe) { return lhfe.isAffectorActive(); },
					[](LightHandlerFrontEnd& lhfe, std::size_t colliderIndex) { return lhfe.isAffectorActive(colliderIndex); });
        }
    }
}
