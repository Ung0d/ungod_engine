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

#include "ungod/script/registration/RegisterVisuals.h"
#include "ungod/visual/Visual.h"
#include "ungod/visual/Camera.h"
#include "ungod/script/registration/RegistrationDetail.h"
#include "ungod/application/Application.h"
#include "ungod/base/World.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerVisuals(ScriptStateBase& state, Application& app)
        {
			script::Usertype<VisualsComponent> visualsCompType = state.registerUsertype<VisualsComponent>("Visuals");
			visualsCompType["isLoaded"] = & VisualsComponent::isLoaded;
			visualsCompType["isVisible"] = & VisualsComponent::isVisible;
			visualsCompType["getOpacity"] = & VisualsComponent::getOpacity;

			script::Usertype < VertexArrayComponent> vertexArrayCompType = state.registerUsertype<VertexArrayComponent>("VertexArrayComponent");
			vertexArrayCompType["textureRectCount"] = [](const VertexArrayComponent& vert) {return vert.getVertices().textureRectCount(); };
			vertexArrayCompType["getPosition"] = [](const VertexArrayComponent& vert, std::size_t index) {return vert.getVertices().getPosition(index); };
			vertexArrayCompType["getTextureRect"] = [](const VertexArrayComponent& vert, std::size_t index) {return vert.getVertices().getTextureRect(index); };
			vertexArrayCompType["getBounds"] = [](const VertexArrayComponent& vert) {return vert.getVertices().getBounds(); };

			script::Usertype < SpriteComponent> spriteCompType = state.registerUsertype<SpriteComponent>("SpriteComponent");
			spriteCompType["getPosition"] = [](const SpriteComponent& sprite) {return sprite.getSprite().getPosition(); };
			spriteCompType["getScale"] = [](const SpriteComponent& sprite) {return sprite.getSprite().getScale(); };
			spriteCompType["getRotation"] = [](const SpriteComponent& sprite) {return sprite.getSprite().getRotation(); };
			spriteCompType["getOrigin"] = [](const SpriteComponent& sprite) {return sprite.getSprite().getOrigin(); };
			spriteCompType["getTextureRect"] = [](const SpriteComponent& sprite) {return sprite.getSprite().getTextureRect(); };
			spriteCompType["getBounds"] = [](const SpriteComponent& sprite) {return sprite.getSprite().getBounds(); };
			spriteCompType["getUntransformedBounds"] = [](const SpriteComponent& sprite) {return sprite.getSprite().getUntransformedBounds(); };

            detail::registerMultiComponent<SpriteComponent>(state, "MultiSpriteComponent");

			script::Usertype < VisualAffectorComponent> affectorCompType = state.registerUsertype<VisualAffectorComponent>("VisualAffectorComponent");
			affectorCompType["setActive"] = &VisualAffectorComponent::setActive;
			affectorCompType["isActive"] = & VisualAffectorComponent::isActive;

            detail::registerMultiComponent<VisualAffectorComponent>(state, "MultiVisualAffectorComponent");

            state.registerUsertype<SpriteMetadataComponent>("SpriteMetadataComponent");

			script::Usertype < AnimationComponent> animCompType = state.registerUsertype<AnimationComponent>("AnimationComponent");
			animCompType["isLooping"] = [](const AnimationComponent& anim) {return anim.getAnimation().isLooping(); };
			animCompType["isRunning"] = [](const AnimationComponent& anim) {return anim.getAnimation().isRunning(); };
			animCompType["getKey"] = [](const AnimationComponent& anim) {return anim.getAnimation().getKey(); };
			animCompType["getCurrentIndex"] = [](const AnimationComponent& anim) {return anim.getAnimation().getCurrentIndex(); };
			animCompType["getSpeed"] = [](const AnimationComponent& anim) {return anim.getAnimation().getSpeed(); };

            detail::registerMultiComponent<AnimationComponent>(state, "MultiAnimationComponent");

			script::Usertype <BigSpriteComponent> bigspriteCompType = state.registerUsertype<BigSpriteComponent>("BigSpriteComponent");
			bigspriteCompType["isLoaded"] = & BigSpriteComponent::isLoaded;
			bigspriteCompType["isVisible"] = & BigSpriteComponent::isVisible;
			bigspriteCompType["getFilepath"] = & BigSpriteComponent::getFilePath;
			bigspriteCompType["getPosition"] = [](const BigSpriteComponent& sprite) {return sprite.getBigSprite().getPosition(); };
			bigspriteCompType["getScale"] = [](const BigSpriteComponent& sprite) {return sprite.getBigSprite().getScale(); };
			bigspriteCompType["getRotation"] = [](const BigSpriteComponent& sprite) {return sprite.getBigSprite().getRotation(); };
			bigspriteCompType["getOrigin"] = [](const BigSpriteComponent& sprite) {return sprite.getBigSprite().getOrigin(); };
			bigspriteCompType["getBounds"] = [](const BigSpriteComponent& sprite) {return sprite.getBigSprite().getGlobalBounds(); };
			bigspriteCompType["getColor"] = [](const BigSpriteComponent& sprite) {return sprite.getBigSprite().getColor(); };

			script::Usertype <VisualsHandler> visualsmngrType = state.registerUsertype<VisualsHandler>("VisualsManager");
			visualsmngrType["initTextureRects"] = & VisualsHandler::initTextureRects;
			visualsmngrType["setSpriteTextureRect"] = sol::overload([](VisualsHandler& vm, Entity e, const sf::FloatRect& rect) { vm.setSpriteTextureRect(e, rect); },
				[](VisualsHandler& vm, Entity e, const sf::FloatRect& rect, std::size_t index) { vm.setSpriteTextureRect(e, rect, index); },
				[](VisualsHandler& vm, Entity e, const std::string& key) { vm.setSpriteTextureRect(e, key); },
				[](VisualsHandler& vm, Entity e, const std::string& key, std::size_t index) { vm.setSpriteTextureRect(e, key, index); });
			visualsmngrType["setSpritePosition"] = sol::overload([](VisualsHandler& vm, Entity e, const sf::Vector2f& pos) { vm.setSpritePosition(e, pos); },
				[](VisualsHandler& vm, Entity e, const sf::Vector2f& pos, std::size_t index) { vm.setSpritePosition(e, pos, index); });
			visualsmngrType["setArrayTextureRect"] = sol::overload([](VisualsHandler& vm, Entity e, const sf::FloatRect& rect, std::size_t index) { vm.setArrayTextureRect(e, rect, index); },
				[](VisualsHandler& vm, Entity e, std::size_t index, const std::string& key) { vm.setArrayTextureRect(e, index, key); });
			visualsmngrType["setTextureRectPosition"] = & VisualsHandler::setTextureRectPosition;
			visualsmngrType["setPoints"] = [](VisualsHandler& vm, Entity e, std::size_t index, const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3, const sf::Vector2f& p4)
			{ vm.setPoints(e, index, p1, p2, p3, p4); };
			visualsmngrType["getPoint"] = & VisualsHandler::getPoint;
			visualsmngrType["newTextureRect"] = sol::overload([](VisualsHandler& vm, Entity e, const sf::FloatRect& rect) { return vm.newTextureRect(e, rect); },
				[](VisualsHandler& vm, Entity e) { return vm.newTextureRect(e); },
				[](VisualsHandler& vm, Entity e, const std::string& key) { return vm.newTextureRect(e, key); });
			visualsmngrType["loadTexture"] = sol::overload([](VisualsHandler& vm, Entity e, const std::string& imageID, const script::ProtectedFunc& func) { return vm.loadTexture(e, imageID, [func](VisualsComponent& vis) { func(vis); }); },
				[](VisualsHandler& vm, Entity e, const std::string& imageID, LoadPolicy policy) { vm.loadTexture(e, imageID, policy); },
				[](VisualsHandler& vm, Entity e, const std::string& imageID) { vm.loadTexture(e, imageID); });
			visualsmngrType["loadMetadata"] = & VisualsHandler::loadMetadata;
			visualsmngrType["setVisible"] = & VisualsHandler::setVisible;
			visualsmngrType["bindSpriteToAnimation"] = sol::overload([](VisualsHandler& vm, Entity e) { vm.bindSpriteToAnimation(e); },
				[](VisualsHandler& vm, Entity e, std::size_t multiSpriteIndex, std::size_t multiAnimationIndex) { vm.bindSpriteToAnimation(e, multiSpriteIndex, multiAnimationIndex); });
			visualsmngrType["bindArrayToAnimation"] = sol::overload([](VisualsHandler& vm, Entity e, std::size_t index) { vm.bindArrayToAnimation(e, index); },
				[](VisualsHandler& vm, Entity e, std::size_t index, std::size_t multiAnimationIndex) { vm.bindArrayToAnimation(e, index, multiAnimationIndex); });
			visualsmngrType["setAnimationState"] = sol::overload([](VisualsHandler& vm, Entity e, const std::string& key) { return vm.setAnimationState(e, key); },
				[](VisualsHandler& vm, Entity e, const std::string& key, std::size_t multiAnimationIndex) { return vm.setAnimationState(e, key, multiAnimationIndex); });
			visualsmngrType["newAnimationState"] = & VisualsHandler::newAnimationState;
			visualsmngrType["setRunning"] = sol::overload([](VisualsHandler& vm, Entity e, bool running) { return vm.setRunning(e, running); },
				[](VisualsHandler& vm, Entity e, bool running, std::size_t animationIndex) { return vm.setRunning(e, running, animationIndex); });
			visualsmngrType["setAnimationSpeed"] = sol::overload([](VisualsHandler& vm, Entity e, float speed) { return vm.setAnimationSpeed(e, speed); },
				[](VisualsHandler& vm, Entity e, float speed, std::size_t animationIndex) { return vm.setAnimationSpeed(e, speed, animationIndex); });
			visualsmngrType["getLowerBound"] = & VisualsHandler::getLowerBound;
			visualsmngrType["getUpperBound"] = & VisualsHandler::getUpperBound;
			visualsmngrType["getUntransformedLowerBound"] = & VisualsHandler::getUntransformedLowerBound;
			visualsmngrType["getUntransformedUpperBound"] = & VisualsHandler::getUntransformedUpperBound;
			visualsmngrType["setRotation"] = sol::overload([](VisualsHandler& vm, Entity e, float rotation) { return vm.setRotation(e, rotation); },
			[](VisualsHandler& vm, Entity e, float rotation, std::size_t multiindex) { return vm.setRotation(e, rotation, multiindex); });
			visualsmngrType["setScale"] = sol::overload([](VisualsHandler& vm, Entity e, float scalex, float scaley) { return vm.setScale(e, scalex, scaley); },
				[](VisualsHandler& vm, Entity e, float scalex, float scaley, std::size_t multiindex) { return vm.setScale(e, scalex, scaley, multiindex); });
			visualsmngrType["setOrigin"] = sol::overload([](VisualsHandler& vm, Entity e, const sf::Vector2f& origin) { return vm.setOrigin(e, origin); },
				[](VisualsHandler& vm, Entity e, const sf::Vector2f& origin, std::size_t multiindex) { return vm.setOrigin(e, origin, multiindex); });
			visualsmngrType["setSpriteColor"] = sol::overload([](VisualsHandler& vm, Entity e, const sf::Color& color) { return vm.setSpriteColor(e, color); },
				[](VisualsHandler& vm, Entity e, const sf::Color& color, std::size_t multiindex) { return vm.setSpriteColor(e, color, multiindex); });
			visualsmngrType["setArrayRectColor"] = [](VisualsHandler& vm, Entity e, const sf::Color& color, std::size_t index) { vm.setArrayRectColor(e, color, index); };
			visualsmngrType["setOpacity"] = & VisualsHandler::setOpacity;
			visualsmngrType["loadBigTexture"] = sol::overload([](VisualsHandler& vm, Entity e, const std::string& imageID, LoadPolicy policy) { vm.loadBigTexture(e, imageID, policy); },
				[](VisualsHandler& vm, Entity e, const std::string& imageID) { vm.loadBigTexture(e, imageID); });
			visualsmngrType["setBigSpriteVisibility"] = [](VisualsHandler& vm, Entity e, bool visible) { vm.setBigSpriteVisibility(e, visible); };
			visualsmngrType["setBigSpritePosition"] = [](VisualsHandler& vm, Entity e, const sf::Vector2f& position) { vm.setBigSpritePosition(e, position); };
			visualsmngrType["setBigSpriteColor"] = [](VisualsManager& vm, Entity e, const sf::Color& color) { vm.setBigSpriteColor(e, color); };
			visualsmngrType["setBigSpriteScale"] = [](VisualsHandler& vm, Entity e, const sf::Vector2f& scale) { vm.setBigSpriteScale(e, scale); };
			visualsmngrType["setBigSpriteOrigin"] = [](VisualsHandler& vm, Entity e, const sf::Vector2f& origin) { vm.setBigSpriteOrigin(e, origin); };
			visualsmngrType["setBigSpriteRotation"] = [](VisualsHandler& vm, Entity e, float rotation) { vm.setBigSpriteRotation(e, rotation); };

			script::Usertype<Camera> camType = state.registerUsertype<Camera>("Camera");
			camType["lockToEntity"] = & Camera::lockToEntity;
			camType["lookAt"] = & Camera::lookAt;
			camType["setZoom"] = & Camera::setZoom;
			camType["getZoom"] = & Camera::getZoom;
			camType["reset"] =  & Camera::reset;
			camType["setDeadZoneRadius"] =  & Camera::setDeadZoneRadius;
			camType["setScrollSpeed"] =  & Camera::setScrollSpeed;
			camType["getCenter"] = & Camera::getCenter;
			camType["attachAffector"] = & Camera::attachAffector;
			camType["setNoise"] = & Camera::setNoise;
			camType["getNoise"] = & Camera::getNoise;

            state.registerFunction("screenShake", sol::overload( &Camera::makeScreenShake, []() { return Camera::makeScreenShake(750, 60, 10); } ));

            state.registerFunction("smoothZoom", &Camera::makeSmoothZoom);
        }
    }
}
