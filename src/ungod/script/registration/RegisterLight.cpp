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

			state.registerUsertype<BaseLight>("BaseLight",
				"setActive", &BaseLight::setActive,
				"isActive", &BaseLight::isActive,
				"toggleActive", &BaseLight::toggleActive);

			state.registerUsertype<LightCollider>("LightCollider",
				sol::base_classes, sol::bases<BaseLight>());

			state.registerUsertype<PointLight>("PointLight",
				sol::base_classes, sol::bases<BaseLight>());

			state.registerUsertype<ShadowEmitterComponent>("ShadowEmitter");

			state.registerUsertype<LightEmitterComponent>("LightEmitter");

			detail::registerMultiComponent<ShadowEmitterComponent>(state, "MultiShadowEmitter");

			detail::registerMultiComponent<LightEmitterComponent>(state, "MultiLightEmitter");

			state.registerUsertype<LightAffectorComponent>("LightAffector",
				"setActive", &LightAffectorComponent::setActive,
				"isActive", &LightAffectorComponent::isActive);

			detail::registerMultiComponent<LightAffectorComponent>(state, "MultiLightAffector");

            state.registerUsertype<LightSystem>("LightSystem",
                                                      "setAmbientColor", &LightSystem::setAmbientColor/*,
                                                      "setLocalLightPosition",
                                                      sol::overload(
                                                        [] (LightSystem& ls, Entity e, const sf::Vector2f& position) { ls.setLocalLightPosition(e, position); },
                                                        [] (LightSystem& ls, Entity e, const sf::Vector2f& position, std::size_t index) { ls.setLocalLightPosition(e, position ,index); } ),
                                                      "setLightScale",
                                                      sol::overload(
                                                        [] (LightSystem& ls, Entity e, const sf::Vector2f& scale) { ls.setLightScale(e, scale); },
                                                        [] (LightSystem& ls, Entity e, const sf::Vector2f& scale, std::size_t index) { ls.setLightScale(e, scale ,index); } ),
                                                      "setLightColor",
                                                      sol::overload(
                                                        [] (LightSystem& ls, Entity e, const sf::Color& color) { ls.setLightColor(e, color); },
                                                        [] (LightSystem& ls, Entity e, const sf::Color& color, std::size_t index) { ls.setLightColor(e, color ,index); } ),
                                                      "setPoint",
                                                      sol::overload(
                                                        [] (LightSystem& ls, Entity e, const sf::Vector2f& point, std::size_t i) { ls.setPoint(e, point, i); },
                                                        [] (LightSystem& ls, Entity e, const sf::Vector2f& point, std::size_t pointIndex, std::size_t colliderIndex) { ls.setPoint(e, point, pointIndex, colliderIndex); } ),
                                                      "setPointCount",
                                                      sol::overload(
                                                        [] (LightSystem& ls, Entity e, std::size_t num) { ls.setPointCount(e, num); },
                                                        [] (LightSystem& ls, Entity e, std::size_t num, std::size_t colliderIndex) { ls.setPointCount(e, num, colliderIndex); } ),
                                                      "setPoints",
                                                      sol::overload(
                                                        [] (LightSystem& ls, Entity e, script::Environment points)
                                                        {
                                                            std::vector<sf::Vector2f> pvec;
                                                            if (detail::table2vec(pvec, points))
                                                                ls.setPoints(e, pvec);
                                                        },
                                                        [] (LightSystem& ls, Entity e, script::Environment points, std::size_t colliderIndex)
                                                        {
                                                            std::vector<sf::Vector2f> pvec;
                                                            if (detail::table2vec(pvec, points))
                                                                ls.setPoints(e, pvec, colliderIndex);
                                                        }),
                                                        "setLightOverShape",
                                                      sol::overload(
                                                        [] (LightSystem& ls, Entity e, bool los) { ls.setLightOverShape(e,los); },
                                                        [] (LightSystem& ls, Entity e, bool los, std::size_t colliderIndex) { ls.setLightOverShape(e, los, colliderIndex); } ),
                                                        "setAffectorCallback",
                                                      sol::overload( [] (LightSystem& ls, Entity e, const std::function<void(float, LightEmitterComponent&)>& callback)
                                                                    { ls.setAffectorCallback(e, callback); },
                                                                     [] (LightSystem& ls, Entity e, std::size_t lightIndex, const std::function<void(float, LightEmitterComponent&)>& callback)
                                                                    { ls.setAffectorCallback(e, lightIndex, callback); },
                                                                     [] (LightSystem& ls, Entity e, std::size_t lightIndex, std::size_t affectorIndex, const std::function<void(float, LightEmitterComponent&)>& callback)
                                                                    { ls.setAffectorCallback(e, lightIndex, affectorIndex, callback); } ),
                                                        "getLowerBound", &LightSystem::getLowerBound,
                                                        "getUpperBound", &LightSystem::getUpperBound*/
                                                      );
        }
    }
}
