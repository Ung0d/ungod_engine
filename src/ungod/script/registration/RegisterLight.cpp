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
#include "ungod/visual/Light.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerLight(ScriptStateBase& state)
        {
			state.registerFunction("LightFlickering",
				[](float period, float strength) -> std::function<void(float, LightEmitterComponent&)>
				{ return LightFlickering(period, strength); });

			state.registerFunction("RandomizedFlickering",
				[](float baseperiod, float strength) -> std::function<void(float, LightEmitterComponent&)>
				{ return RandomizedFlickering(baseperiod, strength); });

			script::Usertype<BaseLight> baseLightType = state.registerUsertype<BaseLight>("BaseLight");
			baseLightType["setActive"] = &BaseLight::setActive;
			baseLightType["isActive"] = &BaseLight::isActive;
			baseLightType["toggleActive"] = &BaseLight::toggleActive;

			state.registerUsertype<LightCollider>("LightCollider", sol::base_classes, sol::bases<BaseLight>());

			state.registerUsertype<PointLight>("PointLight", sol::base_classes, sol::bases<BaseLight>());

			state.registerUsertype<ShadowEmitterComponent>("ShadowEmitter");

			state.registerUsertype<LightEmitterComponent>("LightEmitter");

			detail::registerMultiComponent<ShadowEmitterComponent>(state, "MultiShadowEmitter");

			detail::registerMultiComponent<LightEmitterComponent>(state, "MultiLightEmitter");

			script::Usertype<LightAffectorComponent> lightAffectorType = state.registerUsertype<LightAffectorComponent>("LightAffector");
			lightAffectorType["setActive"] = &LightAffectorComponent::setActive;
			lightAffectorType["isActive"] = &LightAffectorComponent::isActive;

			detail::registerMultiComponent<LightAffectorComponent>(state, "MultiLightAffector");

			script::Usertype<LightHandler> lightSystemType = state.registerUsertype<LightHandler>("LightSystem");
			lightSystemType["setAmbientColor"] = & LightHandler::setAmbientColor;
			lightSystemType["setLocalLightPosition"] =
				sol::overload(
					[](LightHandler& ls, Entity e, const sf::Vector2f& position) { ls.setLocalLightPosition(e, position); },
					[](LightHandler& ls, Entity e, const sf::Vector2f& position, std::size_t index) { ls.setLocalLightPosition(e, position, index); });
			lightSystemType["setLightScale"] =
				sol::overload(
					[](LightHandler& ls, Entity e, const sf::Vector2f& scale) { ls.setLightScale(e, scale); },
					[](LightHandler& ls, Entity e, const sf::Vector2f& scale, std::size_t index) { ls.setLightScale(e, scale, index); });
			lightSystemType["setLightColor"] =
				sol::overload(
					[](LightHandler& ls, Entity e, const sf::Color& color) { ls.setLightColor(e, color); },
					[](LightHandler& ls, Entity e, const sf::Color& color, std::size_t index) { ls.setLightColor(e, color, index); });
			lightSystemType["setPoint"] =
				sol::overload(
					[](LightHandler& ls, Entity e, const sf::Vector2f& point, std::size_t i) { ls.setPoint(e, point, i); },
					[](LightHandler& ls, Entity e, const sf::Vector2f& point, std::size_t pointIndex, std::size_t colliderIndex) { ls.setPoint(e, point, pointIndex, colliderIndex); });
			lightSystemType["setPointCount"] =
				sol::overload(
					[](LightHandler& ls, Entity e, std::size_t num) { ls.setPointCount(e, num); },
					[](LightHandler& ls, Entity e, std::size_t num, std::size_t colliderIndex) { ls.setPointCount(e, num, colliderIndex); });
			lightSystemType["setPoints"] =
				sol::overload(
					[](LightHandler& ls, Entity e, script::Environment points)
					{
						std::vector<sf::Vector2f> pvec;
						if (detail::table2vec(pvec, points))
							ls.setPoints(e, pvec);
					},
					[](LightHandler& ls, Entity e, script::Environment points, std::size_t colliderIndex)
					{
						std::vector<sf::Vector2f> pvec;
						if (detail::table2vec(pvec, points))
							ls.setPoints(e, pvec, colliderIndex);
					});
			lightSystemType["setLightOverShape"] =
				sol::overload(
					[](LightHandler& ls, Entity e, bool los) { ls.setLightOverShape(e, los); },
					[](LightHandler& ls, Entity e, bool los, std::size_t colliderIndex) { ls.setLightOverShape(e, los, colliderIndex); });
			lightSystemType["setAffectorCallback"] =
				sol::overload([](LightHandler& ls, Entity e, const std::function<void(float, LightEmitterComponent&)>& callback)
					{ ls.setAffectorCallback(e, callback); },
					[](LightHandler& ls, Entity e, std::size_t lightIndex, const std::function<void(float, LightEmitterComponent&)>& callback)
					{ ls.setAffectorCallback(e, lightIndex, callback); },
					[](LightHandler& ls, Entity e, std::size_t lightIndex, std::size_t affectorIndex, const std::function<void(float, LightEmitterComponent&)>& callback)
					{ ls.setAffectorCallback(e, lightIndex, affectorIndex, callback); });
			lightSystemType["getLowerBound"] = & LightHandler::getLowerBound;
			lightSystemType["getUpperBound"] = & LightHandler::getUpperBound;
        }
    }
}
