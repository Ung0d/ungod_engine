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
            state.registerUsertype<VisualsComponent>("Visuals",
                                                  "isLoaded", &VisualsComponent::isLoaded,
                                                  "isVisible", &VisualsComponent::isVisible,
                                                  "getOpacity", &VisualsComponent::getOpacity);

            state.registerUsertype<VertexArrayComponent>("VertexArrayComponent",
                                                  "textureRectCount", [] (const VertexArrayComponent& vert) {return vert.getVertices().textureRectCount();},
                                                  "getPosition", [] (const VertexArrayComponent& vert, std::size_t index) {return vert.getVertices().getPosition(index);},
                                                  "getTextureRect",[] (const VertexArrayComponent& vert, std::size_t index) {return vert.getVertices().getTextureRect(index);},
                                                  "getBounds", [] (const VertexArrayComponent& vert) {return vert.getVertices().getBounds();});

            state.registerUsertype<SpriteComponent>("SpriteComponent",
                                                  "getPosition", [] (const SpriteComponent& sprite) {return sprite.getSprite().getPosition();},
                                                  "getScale", [] (const SpriteComponent& sprite) {return sprite.getSprite().getScale();},
                                                  "getRotation", [] (const SpriteComponent& sprite) {return sprite.getSprite().getRotation();},
                                                  "getOrigin", [] (const SpriteComponent& sprite) {return sprite.getSprite().getOrigin();},
                                                  "getTextureRect", [] (const SpriteComponent& sprite) {return sprite.getSprite().getTextureRect();},
                                                  "getBounds", [] (const SpriteComponent& sprite) {return sprite.getSprite().getBounds();},
                                                  "getUntransformedBounds", [] (const SpriteComponent& sprite) {return sprite.getSprite().getUntransformedBounds();});

            detail::registerMultiComponent<Sprite>(state, "MultiSpriteComponent");

            state.registerUsertype<VisualAffectorComponent>("VisualAffectorComponent",
                                                  "setActive", &VisualAffectorComponent::setActive,
                                                  "isActive", &VisualAffectorComponent::isActive);

            detail::registerMultiComponent<VisualAffectorComponent>(state, "MultiVisualAffectorComponent");

            state.registerUsertype<SpriteMetadataComponent>("SpriteMetadataComponent");

            state.registerUsertype<AnimationComponent>("AnimationComponent",
                                                  "isLooping", [] (const AnimationComponent& anim) {return anim.getAnimation().isLooping();},
                                                  "isRunning", [] (const AnimationComponent& anim) {return anim.getAnimation().isRunning();},
                                                  "getKey", [] (const AnimationComponent& anim) {return anim.getAnimation().getKey();},
                                                  "getCurrentIndex", [] (const AnimationComponent& anim) {return anim.getAnimation().getCurrentIndex();},
                                                  "getSpeed", [] (const AnimationComponent& anim) {return anim.getAnimation().getSpeed();});

            detail::registerMultiComponent<AnimationComponent>(state, "MultiAnimationComponent");

            state.registerUsertype<VisualsManager>("VisualsManager",
                                                         "initTextureRects", &VisualsManager::initTextureRects,
                                                         "setSpriteTextureRect", sol::overload([] (VisualsManager& vm, Entity e, const sf::FloatRect& rect) { vm.setSpriteTextureRect(e, rect); },
                                                                                               [] (VisualsManager& vm, Entity e, const sf::FloatRect& rect, std::size_t index) { vm.setSpriteTextureRect(e, rect, index); },
                                                                                               [] (VisualsManager& vm, Entity e, const std::string& key) { vm.setSpriteTextureRect(e, key); },
                                                                                               [] (VisualsManager& vm, Entity e, const std::string& key, std::size_t index) { vm.setSpriteTextureRect(e, key, index); }),
                                                         "setSpritePosition", sol::overload([] (VisualsManager& vm, Entity e, const sf::Vector2f& pos) { vm.setSpritePosition(e, pos); },
                                                                                            [] (VisualsManager& vm, Entity e, const sf::Vector2f& pos, std::size_t index) { vm.setSpritePosition(e, pos, index); }),
                                                         "setArrayTextureRect", sol::overload( [] (VisualsManager& vm, Entity e, const sf::FloatRect& rect, std::size_t index) { vm.setArrayTextureRect(e, rect, index); },
                                                                                               [] (VisualsManager& vm, Entity e, std::size_t index, const std::string& key) { vm.setArrayTextureRect(e, index, key); }),
                                                         "setTextureRectPosition", &VisualsManager::setTextureRectPosition,
                                                         "setPoints", [] (VisualsManager& vm, Entity e, std::size_t index, const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3, const sf::Vector2f& p4)
                                                            { vm.setPoints(e, index, p1, p2, p3, p4); },
                                                         "getPoint", &VisualsManager::getPoint,
                                                         "newTextureRect", sol::overload([] (VisualsManager& vm, Entity e, const sf::FloatRect& rect) { return vm.newTextureRect(e, rect); },
                                                                                            [] (VisualsManager& vm, Entity e) { return vm.newTextureRect(e); },
                                                                                            [] (VisualsManager& vm, Entity e, const std::string& key) { return vm.newTextureRect(e, key); }),
                                                         "loadTexture", sol::overload([] (VisualsManager& vm, Entity e, const std::string& imageID, const script::ProtectedFunc& func) { return vm.loadTexture(e, imageID, [func] (VisualsComponent& vis) { func(vis); }); },
                                                                                      [] (VisualsManager& vm, Entity e, const std::string& imageID, LoadPolicy policy) { vm.loadTexture(e, imageID, policy); },
                                                                                      [] (VisualsManager& vm, Entity e, const std::string& imageID) { vm.loadTexture(e, imageID); }),
                                                         "loadMetadata", &VisualsManager::loadMetadata,
                                                         "setVisible", &VisualsManager::setVisible,
                                                         "bindSpriteToAnimation", sol::overload( [] (VisualsManager& vm, Entity e) { vm.bindSpriteToAnimation(e); },
                                                                                               [] (VisualsManager& vm, Entity e, std::size_t multiSpriteIndex, std::size_t multiAnimationIndex) { vm.bindSpriteToAnimation(e, multiSpriteIndex, multiAnimationIndex); }),
                                                         "bindArrayToAnimation", sol::overload( [] (VisualsManager& vm, Entity e, std::size_t index) { vm.bindArrayToAnimation(e, index); },
                                                                                               [] (VisualsManager& vm, Entity e, std::size_t index, std::size_t multiAnimationIndex) { vm.bindArrayToAnimation(e, index, multiAnimationIndex); }),
                                                         "setAnimationState", sol::overload( [] (VisualsManager& vm, Entity e, const std::string& key) { return vm.setAnimationState(e, key); },
                                                                                               [] (VisualsManager& vm, Entity e, const std::string& key, std::size_t multiAnimationIndex) { return vm.setAnimationState(e, key, multiAnimationIndex); }),
                                                         "newAnimationState", &VisualsManager::newAnimationState,
                                                         "setRunning", sol::overload([] (VisualsManager& vm, Entity e, bool running) { return vm.setRunning(e, running); },
                                                                                     [] (VisualsManager& vm, Entity e, bool running, std::size_t animationIndex) { return vm.setRunning(e, running, animationIndex); }),
                                                         "setAnimationSpeed", sol::overload([] (VisualsManager& vm, Entity e, float speed) { return vm.setAnimationSpeed(e, speed); },
                                                                                     [] (VisualsManager& vm, Entity e, float speed, std::size_t animationIndex) { return vm.setAnimationSpeed(e, speed, animationIndex); }),
                                                         "getLowerBound", &VisualsManager::getLowerBound,
                                                         "getUpperBound", &VisualsManager::getUpperBound,
                                                         "getUntransformedLowerBound", &VisualsManager::getUntransformedLowerBound,
                                                         "getUntransformedUpperBound", &VisualsManager::getUntransformedUpperBound,
                                                         "setRotation", sol::overload([] (VisualsManager& vm, Entity e, float rotation) { return vm.setRotation(e, rotation); },
                                                                                     [] (VisualsManager& vm, Entity e, float rotation, std::size_t multiindex) { return vm.setRotation(e, rotation, multiindex); }),
                                                         "setScale", sol::overload([] (VisualsManager& vm, Entity e, float scalex, float scaley) { return vm.setScale(e, scalex, scaley); },
                                                                                     [] (VisualsManager& vm, Entity e, float scalex, float scaley, std::size_t multiindex) { return vm.setScale(e, scalex, scaley, multiindex); }),
                                                         "setOrigin", sol::overload([] (VisualsManager& vm, Entity e, const sf::Vector2f& origin) { return vm.setOrigin(e, origin); },
                                                                                     [] (VisualsManager& vm, Entity e, const sf::Vector2f& origin, std::size_t multiindex) { return vm.setOrigin(e, origin, multiindex); }),
                                                         "setSpriteColor", sol::overload([] (VisualsManager& vm, Entity e, const sf::Color& color) { return vm.setSpriteColor(e, color); },
                                                                                     [] (VisualsManager& vm, Entity e, const sf::Color& color, std::size_t multiindex) { return vm.setSpriteColor(e, color, multiindex); }),
                                                         "setArrayRectColor", [] (VisualsManager& vm, Entity e, const sf::Color& color, std::size_t index) { vm.setArrayRectColor(e, color, index); },
                                                         "setOpacity", &VisualsManager::setOpacity);

            state.registerUsertype<Camera>("Camera",
                                           "lockToEntity", &Camera::lockToEntity,
                                           "lookAt", &Camera::lookAt,
                                           "setZoom", &Camera::setZoom,
                                           "getZoom", &Camera::getZoom,
                                           "reset", &Camera::reset,
                                           "setDeadZoneRadius", &Camera::setDeadZoneRadius,
                                           "setScrollSpeed", &Camera::setScrollSpeed,
                                           "getCenter", &Camera::getCenter,
                                           "attachAffector", &Camera::attachAffector,
                                           "setNoise", &Camera::setNoise,
                                           "getNoise", &Camera::getNoise);

            state.registerFunction("screenShake", sol::overload( &Camera::makeScreenShake, []() { return Camera::makeScreenShake(750, 60, 10); } ));

            state.registerFunction("smoothZoom", &Camera::makeSmoothZoom);
        }
    }
}
