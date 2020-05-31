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
		bool VisualHandlerFrontEnd::isLoaded() const
		{
			return mEntity.get<VisualsComponent>().isLoaded();
		}

		bool VisualHandlerFrontEnd::isVisible() const
		{
			return mEntity.get<VisualsComponent>().isVisible();
		}

		void VisualHandlerFrontEnd::setVisible(bool visible)
		{
			mHandler.setVisible(mEntity, visible);
		}

		float VisualHandlerFrontEnd::getOpacity() const
		{
			return mEntity.get<VisualsComponent>().getOpacity();
		}

		void VisualHandlerFrontEnd::loadTexture(const std::string& imageID)
		{
			mHandler.loadTexture(mEntity, imageID, LoadPolicy::ASYNC);
		}

		void VisualHandlerFrontEnd::loadMetadata(const std::string& file)
		{
			mHandler.loadMetadata(mEntity, file);
		}

		unsigned VisualHandlerFrontEnd::getVertexRectCount() const
		{
			return mEntity.get<VertexArrayComponent>().getVertices().textureRectCount();
		}

		sf::Vector2f VisualHandlerFrontEnd::getVertexRectPosition(unsigned index) const
		{
			return mEntity.get<VertexArrayComponent>().getVertices().getPosition(index);
		}

		sf::FloatRect VisualHandlerFrontEnd::getVertexTexureRect(unsigned index) const
		{
			return mEntity.get<VertexArrayComponent>().getVertices().getTextureRect(index);
		}

		sf::FloatRect VisualHandlerFrontEnd::getVertexBounds() const
		{
			return mEntity.get<VertexArrayComponent>().getVertices().getBounds();
		}

		bool VisualHandlerFrontEnd::newVertexTextureRect(const sf::FloatRect& rect)
		{
			return mHandler.newVertexTextureRect(mEntity, rect);
		}

		bool VisualHandlerFrontEnd::newVertexTextureRect(const std::string& key)
		{
			return mHandler.newVertexTextureRect(mEntity, key);
		}

		void VisualHandlerFrontEnd::setVertexRectPosition(const sf::Vector2f& pos, unsigned index)
		{
			mHandler.setTextureRectPosition(mEntity, pos, index);
		}

		void VisualHandlerFrontEnd::setVertexPoints(unsigned index, const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3, const sf::Vector2f& p4)
		{
			mHandler.setPoints(mEntity, index, p1, p2, p3, p4);
		}

		bool VisualHandlerFrontEnd::newVertexTextureRect()
		{
			return mHandler.newVertexTextureRect(mEntity);
		}

		void VisualHandlerFrontEnd::setVertexRectColor(const sf::Color& color, unsigned index)
		{
			mHandler.setArrayRectColor(mEntity, color, index);
		}

		const sf::Vector2f& VisualHandlerFrontEnd::getPoint(unsigned rectIndex, unsigned pointIndex)
		{
			return mHandler.getPoint(mEntity, rectIndex, pointIndex);
		}

		const sf::Vector2f& VisualHandlerFrontEnd::getSpritePosition() const
		{
			return mEntity.get<SpriteComponent>().getSprite().getPosition();
		}

		const sf::Vector2f& VisualHandlerFrontEnd::getSpritePosition(unsigned index) const
		{
			return mEntity.get<MultiSpriteComponent>().getComponent(index).getSprite().getPosition();
		}

		const sf::Vector2f& VisualHandlerFrontEnd::getSpriteScale() const
		{
			return mEntity.get<SpriteComponent>().getSprite().getScale();
		}

		const sf::Vector2f& VisualHandlerFrontEnd::getSpriteScale(unsigned index) const
		{
			return mEntity.get<MultiSpriteComponent>().getComponent(index).getSprite().getScale();
		}

		float VisualHandlerFrontEnd::getSpriteRotation() const
		{
			return mEntity.get<SpriteComponent>().getSprite().getRotation();
		}

		float VisualHandlerFrontEnd::getSpriteRotation(unsigned index) const
		{
			return mEntity.get<MultiSpriteComponent>().getComponent(index).getSprite().getRotation();
		}

		const sf::Vector2f& VisualHandlerFrontEnd::getSpriteOrigin() const
		{
			return mEntity.get<SpriteComponent>().getSprite().getOrigin();
		}

		const sf::Vector2f& VisualHandlerFrontEnd::getSpriteOrigin(unsigned index) const
		{
			return mEntity.get<MultiSpriteComponent>().getComponent(index).getSprite().getOrigin();
		}

		sf::FloatRect VisualHandlerFrontEnd::getSpriteTextureRect() const
		{
			return mEntity.get<SpriteComponent>().getSprite().getTextureRect();
		}

		sf::FloatRect VisualHandlerFrontEnd::getSpriteTextureRect(unsigned index) const
		{
			return mEntity.get<MultiSpriteComponent>().getComponent(index).getSprite().getTextureRect();
		}

		sf::FloatRect VisualHandlerFrontEnd::getSpriteBounds() const
		{
			return mEntity.get<SpriteComponent>().getSprite().getBounds();
		}

		sf::FloatRect VisualHandlerFrontEnd::getSpriteBounds(unsigned index) const
		{
			return mEntity.get<MultiSpriteComponent>().getComponent(index).getSprite().getBounds();
		}

		sf::FloatRect VisualHandlerFrontEnd::getSpriteUntransformedBounds() const
		{
			return mEntity.get<SpriteComponent>().getSprite().getUntransformedBounds();
		}

		sf::FloatRect VisualHandlerFrontEnd::getSpriteUntransformedBounds(unsigned index) const
		{
			return mEntity.get<MultiSpriteComponent>().getComponent(index).getSprite().getUntransformedBounds();
		}

		void VisualHandlerFrontEnd::setSpriteTextureRect(const sf::FloatRect& rect) 
		{
			mHandler.setSpriteTextureRect(mEntity, rect);
		}

		void VisualHandlerFrontEnd::setSpriteTextureRect(const sf::FloatRect& rect, unsigned index) 
		{
			mHandler.setSpriteTextureRect(mEntity, rect, index);
		}

		void VisualHandlerFrontEnd::setSpriteTextureRect(const std::string& key) 
		{
			mHandler.setSpriteTextureRect(mEntity, key);
		}
		
		void VisualHandlerFrontEnd::setSpriteTextureRect(const std::string& key, unsigned index) 
		{
			mHandler.setSpriteTextureRect(mEntity, key, index);
		}

		void VisualHandlerFrontEnd::setSpritePosition(const sf::Vector2f& pos) 
		{
			mHandler.setSpritePosition(mEntity, pos);
		}

		void VisualHandlerFrontEnd::setSpritePosition(const sf::Vector2f& pos, unsigned index) 
		{
			mHandler.setSpritePosition(mEntity, pos, index);
		}


		void VisualHandlerFrontEnd::setSpriteRotation(float rotation)
		{
			mHandler.setRotation(mEntity, rotation);
		}

		void VisualHandlerFrontEnd::setSpriteRotation(float rotation, unsigned index)
		{
			mHandler.setRotation(mEntity, rotation, index);
		}

		void VisualHandlerFrontEnd::setSpriteScale(const sf::Vector2f& scale)
		{
			mHandler.setScale(mEntity, scale);
		}

		void VisualHandlerFrontEnd::setSpriteScale(const sf::Vector2f& scale, unsigned index)
		{
			mHandler.setScale(mEntity, scale, index);
		}

		void VisualHandlerFrontEnd::setSpriteOrigin(const sf::Vector2f& origin)
		{
			mHandler.setOrigin(mEntity, origin);
		}

		void VisualHandlerFrontEnd::setSpriteOrigin(const sf::Vector2f& origin, unsigned index)
		{
			mHandler.setOrigin(mEntity, origin, index);
		}

		void VisualHandlerFrontEnd::setSpriteColor(const sf::Color& color)
		{
			mHandler.setSpriteColor(mEntity, color);
		}

		void VisualHandlerFrontEnd::setSpriteColor(const sf::Color& color, unsigned index)
		{
			mHandler.setSpriteColor(mEntity, color, index);
		}


		void VisualHandlerFrontEnd::bindSpriteToAnimation()
		{
			mHandler.bindSpriteToAnimation(mEntity);
		}

		void VisualHandlerFrontEnd::bindSpriteToAnimation(unsigned spriteIndex, unsigned animationIndex)
		{
			mHandler.bindSpriteToAnimation(mEntity, spriteIndex, animationIndex);
		}

		void VisualHandlerFrontEnd::bindVertexToAnimation(unsigned vertexIndex)
		{
			mHandler.bindArrayToAnimation(mEntity, vertexIndex);
		}

		void VisualHandlerFrontEnd::bindVertexToAnimation(unsigned vertexIndex, unsigned multiAnimationIndex)
		{
			mHandler.bindArrayToAnimation(mEntity, vertexIndex, multiAnimationIndex);
		}

		void VisualHandlerFrontEnd::setAffectorActive(bool active)
		{
			mEntity.modify<VisualAffectorComponent>().setActive(active);
		}

		bool VisualHandlerFrontEnd::isAffectorActive() const
		{
			return mEntity.get<VisualAffectorComponent>().isActive();
		}

		void VisualHandlerFrontEnd::setAffectorActive(bool active, unsigned index)
		{
			mEntity.modify<MultiVisualAffectorComponent>().getComponent(index).setActive(active);
		}

		bool VisualHandlerFrontEnd::isAffectorActive(unsigned index) const
		{
			return mEntity.modify<MultiVisualAffectorComponent>().getComponent(index).isActive();
		}

		bool VisualHandlerFrontEnd::isAnimationLooping() const
		{
			return mEntity.get<AnimationComponent>().getAnimation().isLooping();
		}

		bool VisualHandlerFrontEnd::isAnimationLooping(unsigned index) const
		{
			return mEntity.get<MultiAnimationComponent>().getComponent(index).getAnimation().isLooping();
		}

		bool VisualHandlerFrontEnd::isAnimationRunning() const
		{
			return mEntity.get<AnimationComponent>().getAnimation().isRunning();
		}

		bool VisualHandlerFrontEnd::isAnimationRunning(unsigned index) const
		{
			return mEntity.get<MultiAnimationComponent>().getComponent(index).getAnimation().isRunning();
		}

		std::string VisualHandlerFrontEnd::getAnimationKey() const
		{
			return mEntity.get<AnimationComponent>().getAnimation().getKey();
		}

		std::string VisualHandlerFrontEnd::getAnimationKey(unsigned index) const
		{
			return mEntity.get<MultiAnimationComponent>().getComponent(index).getAnimation().getKey();
		}

		unsigned VisualHandlerFrontEnd::getFrameIndex() const
		{
			return mEntity.get<AnimationComponent>().getAnimation().getCurrentIndex();
		}

		unsigned VisualHandlerFrontEnd::getFrameIndex(unsigned index) const
		{
			return mEntity.get<MultiAnimationComponent>().getComponent(index).getAnimation().getCurrentIndex();
		}

		float VisualHandlerFrontEnd::getAnimationSpeed() const
		{
			return mEntity.get<AnimationComponent>().getAnimation().getSpeed();
		}

		float VisualHandlerFrontEnd::getAnimationSpeed(unsigned index) const
		{
			return mEntity.get<MultiAnimationComponent>().getComponent(index).getAnimation().getSpeed();
		}

		void VisualHandlerFrontEnd::setAnimationState(const std::string& key)
		{
			mHandler.setAnimationState(mEntity, key);
		}

		void VisualHandlerFrontEnd::setAnimationState(const std::string& key, unsigned multiAnimationIndex)
		{
			mHandler.setAnimationState(mEntity, key, multiAnimationIndex);
		}

		void VisualHandlerFrontEnd::newAnimationState(const std::string& key)
		{
			mHandler.newAnimationState(mEntity, key);
		}

		void VisualHandlerFrontEnd::setAnimationRunning(bool running)
		{
			mHandler.setRunning(mEntity, running);
		}

		void VisualHandlerFrontEnd::setAnimationRunning(bool running, unsigned multiAnimationIndex)
		{
			mHandler.setRunning(mEntity, running, multiAnimationIndex);
		}

		void VisualHandlerFrontEnd::setAnimationSpeed(float speed)
		{
			mHandler.setAnimationSpeed(mEntity, speed);
		}

		void VisualHandlerFrontEnd::setAnimationSpeed(float speed, unsigned multiAnimationInde)
		{
			mHandler.setAnimationSpeed(mEntity, speed, multiAnimationInde);
		}

		bool VisualHandlerFrontEnd::isBigSpriteLoaded() const
		{
			return mEntity.get<BigSpriteComponent>().isLoaded();
		}

		bool VisualHandlerFrontEnd::isBigSpriteVisible() const
		{
			return mEntity.get<BigSpriteComponent>().isVisible();
		}

		std::string VisualHandlerFrontEnd::getBigSpriteFilePath() const
		{
			return mEntity.get<BigSpriteComponent>().getFilePath();
		}

		const sf::Vector2f& VisualHandlerFrontEnd::getBigSpritePosition() const
		{
			return mEntity.get<BigSpriteComponent>().getBigSprite().getPosition();
		}

		const sf::Vector2f& VisualHandlerFrontEnd::getBigSpriteScale() const
		{
			return mEntity.get<BigSpriteComponent>().getBigSprite().getScale();
		}

		const sf::Vector2f& VisualHandlerFrontEnd::getBigSpriteOrigin() const
		{
			return mEntity.get<BigSpriteComponent>().getBigSprite().getOrigin();
		}

		float VisualHandlerFrontEnd::getBigSpriteRotation() const
		{
			return mEntity.get<BigSpriteComponent>().getBigSprite().getRotation();
		}

		sf::FloatRect VisualHandlerFrontEnd::getBigSpriteBounds() const
		{
			return mEntity.get<BigSpriteComponent>().getBigSprite().getGlobalBounds();
		}

		sf::Color VisualHandlerFrontEnd::getBigSpriteColor() const
		{
			return mEntity.get<BigSpriteComponent>().getBigSprite().getColor();
		}

		void VisualHandlerFrontEnd::loadBigTexture(const std::string& imageID, LoadPolicy policy)
		{
			mHandler.loadBigTexture(mEntity, imageID, policy);
		}

		void VisualHandlerFrontEnd::setBigSpriteVisibility(bool visible)
		{
			mHandler.setBigSpriteVisibility(mEntity, visible);
		}

		void VisualHandlerFrontEnd::setBigSpritePosition(const sf::Vector2f& position)
		{
			mHandler.setBigSpritePosition(mEntity, position);
		}

		void VisualHandlerFrontEnd::setBigSpriteScale(const sf::Vector2f& scale)
		{
			mHandler.setBigSpriteScale(mEntity, scale);
		}

		void VisualHandlerFrontEnd::setBigSpriteOrigin(const sf::Vector2f& origin)
		{
			mHandler.setBigSpriteOrigin(mEntity, origin);
		}

		void VisualHandlerFrontEnd::setBigSpriteColor(const sf::Color& color)
		{
			mHandler.setBigSpriteColor(mEntity, color);
		}

		void VisualHandlerFrontEnd::setBigSpriteRotation(float rotation)
		{
			mHandler.setBigSpriteRotation(mEntity, rotation);
		}


		sf::Vector2f VisualHandlerFrontEnd::getLowerBound() const
		{
			return mHandler.getLowerBound(mEntity);
		}

		sf::Vector2f VisualHandlerFrontEnd::getUpperBound() const
		{
			return mHandler.getUpperBound(mEntity);
		}

		void VisualHandlerFrontEnd::setOpacity(float opac)
		{
			mHandler.setOpacity(mEntity, opac);
		}




        void registerVisuals(ScriptStateBase& state, Application& app)
        {
			script::Usertype<VisualHandlerFrontEnd> visualsHandlerType = state.registerUsertype<VisualHandlerFrontEnd>("VisualHandlerFrontEnd");
			visualsHandlerType["isLoaded"] = &VisualHandlerFrontEnd::isLoaded;
			visualsHandlerType["isVisible"] = &VisualHandlerFrontEnd::isVisible;
			visualsHandlerType["setVisible"] = &VisualHandlerFrontEnd::setVisible;
			visualsHandlerType["getOpacity"] = &VisualHandlerFrontEnd::getOpacity;
			visualsHandlerType["loadTexture"] = &VisualHandlerFrontEnd::loadTexture;
			visualsHandlerType["loadMetadata"] = &VisualHandlerFrontEnd::loadMetadata;
			visualsHandlerType["getVertexRectCount"] = &VisualHandlerFrontEnd::getVertexRectCount;
			visualsHandlerType["getVertexRectPosition"] = &VisualHandlerFrontEnd::getVertexRectPosition;
			visualsHandlerType["getVertexTexureRect"] = &VisualHandlerFrontEnd::getVertexTexureRect;
			visualsHandlerType["getVertexBounds"] = &VisualHandlerFrontEnd::getVertexBounds;
			visualsHandlerType["setVertexRectPosition"] = &VisualHandlerFrontEnd::setVertexRectPosition;
			visualsHandlerType["setVertexPoints"] = &VisualHandlerFrontEnd::setVertexPoints;
			visualsHandlerType["newVertexTextureRect"] = sol::overload([](VisualHandlerFrontEnd& vh) { return vh.newVertexTextureRect(); },
																[](VisualHandlerFrontEnd& vh, const sf::FloatRect& rect) { return vh.newVertexTextureRect(rect); },
																[](VisualHandlerFrontEnd& vh, const std::string& key) { return vh.newVertexTextureRect(key); });
			visualsHandlerType["setVertexRectColor"] = &VisualHandlerFrontEnd::setVertexRectColor;
			visualsHandlerType["getPoint"] = &VisualHandlerFrontEnd::getPoint;
			visualsHandlerType["getSpritePosition"] = sol::overload([](VisualHandlerFrontEnd& vh) { return vh.getSpritePosition(); },
																	[](VisualHandlerFrontEnd& vh, unsigned index) { return vh.getSpritePosition(index); });
			visualsHandlerType["getSpriteScale"] = sol::overload([](VisualHandlerFrontEnd& vh) { return vh.getSpriteScale(); },
																[](VisualHandlerFrontEnd& vh, unsigned index) { return vh.getSpriteScale(index); });
			visualsHandlerType["getSpriteRotation"] = sol::overload([](VisualHandlerFrontEnd& vh) { return vh.getSpriteRotation(); },
																	[](VisualHandlerFrontEnd& vh, unsigned index) { return vh.getSpriteRotation(index); });
			visualsHandlerType["getSpriteOrigin"] = sol::overload([](VisualHandlerFrontEnd& vh) { return vh.getSpriteOrigin(); },
																	[](VisualHandlerFrontEnd& vh, unsigned index) { return vh.getSpriteOrigin(index); });
			visualsHandlerType["getSpriteTextureRect"] = sol::overload([](VisualHandlerFrontEnd& vh) { return vh.getSpriteTextureRect(); },
																		[](VisualHandlerFrontEnd& vh, unsigned index) { return vh.getSpriteTextureRect(index); });
			visualsHandlerType["getSpriteBounds"] = sol::overload([](VisualHandlerFrontEnd& vh) { return vh.getSpriteBounds(); },
																[](VisualHandlerFrontEnd& vh, unsigned index) { return vh.getSpriteBounds(index); });
			visualsHandlerType["getSpriteUntransformedBounds"] = sol::overload([](VisualHandlerFrontEnd& vh) { return vh.getSpriteUntransformedBounds(); },
																				[](VisualHandlerFrontEnd& vh, unsigned index) { return vh.getSpriteUntransformedBounds(index); });
			visualsHandlerType["setSpriteTextureRect"] = sol::overload([](VisualHandlerFrontEnd& vh, const sf::FloatRect& rect) { vh.setSpriteTextureRect(rect); },
																		[](VisualHandlerFrontEnd& vh, const sf::FloatRect& rect, unsigned index) { vh.setSpriteTextureRect(rect, index); },
																		[](VisualHandlerFrontEnd& vh, const std::string& key) { vh.setSpriteTextureRect(key); },
																			[](VisualHandlerFrontEnd& vh, const std::string& key, unsigned index) { vh.setSpriteTextureRect(key, index); });
			visualsHandlerType["setSpritePosition"] = sol::overload([](VisualHandlerFrontEnd& vh, const sf::Vector2f& pos) { vh.setSpritePosition(pos); },
																		[](VisualHandlerFrontEnd& vh, const sf::Vector2f& pos, unsigned index) { vh.setSpritePosition(pos, index); });
			visualsHandlerType["setSpriteRotation"] = sol::overload([](VisualHandlerFrontEnd& vh, float rotation) { vh.setSpriteRotation(rotation); },
																	[](VisualHandlerFrontEnd& vh, float rotation, unsigned index) { vh.setSpriteRotation(rotation, index); });
			visualsHandlerType["setSpriteScale"] = sol::overload([](VisualHandlerFrontEnd& vh, const sf::Vector2f& scale) { vh.setSpriteScale(scale); },
																[](VisualHandlerFrontEnd& vh, const sf::Vector2f& scale, unsigned index) { vh.setSpriteScale(scale, index); });
			visualsHandlerType["setSpriteOrigin"] = sol::overload([](VisualHandlerFrontEnd& vh, const sf::Vector2f& origin) { vh.setSpriteOrigin(origin); },
																[](VisualHandlerFrontEnd& vh, const sf::Vector2f& origin, unsigned index) { vh.setSpriteOrigin(origin, index); });
			visualsHandlerType["setSpriteColor"] = sol::overload([](VisualHandlerFrontEnd& vh, const sf::Color& color) { vh.setSpriteColor(color); },
																[](VisualHandlerFrontEnd& vh, const sf::Color& color, unsigned index) { vh.setSpriteColor(color, index); });
			visualsHandlerType["bindSpriteToAnimation"] = sol::overload([](VisualHandlerFrontEnd& vh) { vh.bindSpriteToAnimation(); },
																		[](VisualHandlerFrontEnd& vh, unsigned spriteIndex, unsigned animationIndex) { vh.bindSpriteToAnimation(spriteIndex, animationIndex); });
			visualsHandlerType["bindVertexToAnimation"] = sol::overload([](VisualHandlerFrontEnd& vh, unsigned vertexIndex) { vh.bindVertexToAnimation(vertexIndex); },
																	[](VisualHandlerFrontEnd& vh, unsigned vertexIndex, unsigned animationIndex) { vh.bindVertexToAnimation(vertexIndex, animationIndex); });
			visualsHandlerType["setAffectorActive"] = sol::overload([](VisualHandlerFrontEnd& vh, bool active) { vh.setAffectorActive(active); },
																		[](VisualHandlerFrontEnd& vh, bool active, unsigned index) { vh.setAffectorActive(active, index); });
			visualsHandlerType["isAffectorActive"] = sol::overload([](VisualHandlerFrontEnd& vh) { return vh.isAffectorActive(); },
																	[](VisualHandlerFrontEnd& vh, unsigned index) { return vh.isAffectorActive(index); });
			visualsHandlerType["isAnimationLooping"] = sol::overload([](VisualHandlerFrontEnd& vh) { return vh.isAnimationLooping(); },
																	[](VisualHandlerFrontEnd& vh, unsigned index) { return vh.isAnimationLooping(index); });
			visualsHandlerType["isAnimationRunning"] = sol::overload([](VisualHandlerFrontEnd& vh) { return vh.isAnimationRunning(); },
																	[](VisualHandlerFrontEnd& vh, unsigned index) { return vh.isAnimationRunning(index); });
			visualsHandlerType["getAnimationKey"] = sol::overload([](VisualHandlerFrontEnd& vh) { return vh.getAnimationKey(); },
																	[](VisualHandlerFrontEnd& vh, unsigned index) { return vh.getAnimationKey(index); });
			visualsHandlerType["getFrameIndex"] = sol::overload([](VisualHandlerFrontEnd& vh) { return vh.getFrameIndex(); },
																[](VisualHandlerFrontEnd& vh, unsigned index) { return vh.getFrameIndex(index); });
			visualsHandlerType["getAnimationSpeed"] = sol::overload([](VisualHandlerFrontEnd& vh) { return vh.getAnimationSpeed(); },
															[](VisualHandlerFrontEnd& vh, unsigned index) { return vh.getAnimationSpeed(index); });
			visualsHandlerType["setAnimationState"] = sol::overload([](VisualHandlerFrontEnd& vh, const std::string& key) { return vh.setAnimationState(key); },
																	[](VisualHandlerFrontEnd& vh, const std::string& key, unsigned index) { return vh.setAnimationState(key, index); });
			visualsHandlerType["newAnimationState"] = &VisualHandlerFrontEnd::newAnimationState;
			visualsHandlerType["setAnimationRunning"] = sol::overload([](VisualHandlerFrontEnd& vh, bool running) { return vh.setAnimationRunning(running); },
																		[](VisualHandlerFrontEnd& vh, bool running, unsigned index) { return vh.setAnimationRunning(running, index); });
			visualsHandlerType["setAnimationSpeed"] = sol::overload([](VisualHandlerFrontEnd& vh, float speed) { return vh.setAnimationSpeed(speed); },
																	[](VisualHandlerFrontEnd& vh, float speed, unsigned index) { return vh.setAnimationSpeed(speed, index); });
			visualsHandlerType["isBigSpriteLoaded"] = &VisualHandlerFrontEnd::isBigSpriteLoaded;
			visualsHandlerType["isBigSpriteVisible"] = &VisualHandlerFrontEnd::isBigSpriteVisible;
			visualsHandlerType["getBigSpriteFilePath"] = &VisualHandlerFrontEnd::getBigSpriteFilePath;
			visualsHandlerType["getBigSpritePosition"] = &VisualHandlerFrontEnd::getBigSpritePosition;
			visualsHandlerType["getBigSpriteScale"] = &VisualHandlerFrontEnd::getBigSpriteScale;
			visualsHandlerType["getBigSpriteOrigin"] = &VisualHandlerFrontEnd::getBigSpriteOrigin;
			visualsHandlerType["getBigSpriteRotation"] = &VisualHandlerFrontEnd::getBigSpriteRotation;
			visualsHandlerType["getBigSpriteBounds"] = &VisualHandlerFrontEnd::getBigSpriteBounds;
			visualsHandlerType["getBigSpriteColor"] = &VisualHandlerFrontEnd::getBigSpriteColor;
			visualsHandlerType["loadBigTexture"] = &VisualHandlerFrontEnd::loadBigTexture;
			visualsHandlerType["setBigSpriteVisibility"] = &VisualHandlerFrontEnd::setBigSpriteVisibility;
			visualsHandlerType["setBigSpritePosition"] = &VisualHandlerFrontEnd::setBigSpritePosition;
			visualsHandlerType["setBigSpriteScale"] = &VisualHandlerFrontEnd::setBigSpriteScale;
			visualsHandlerType["setBigSpriteOrigin"] = &VisualHandlerFrontEnd::setBigSpriteOrigin;
			visualsHandlerType["setBigSpriteColor"] = &VisualHandlerFrontEnd::setBigSpriteColor;
			visualsHandlerType["setBigSpriteRotation"] = &VisualHandlerFrontEnd::setBigSpriteRotation;
			visualsHandlerType["getLowerBound"] = &VisualHandlerFrontEnd::getLowerBound;
			visualsHandlerType["getUpperBound"] = &VisualHandlerFrontEnd::getUpperBound;
			visualsHandlerType["setOpacity"] = &VisualHandlerFrontEnd::setOpacity;


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
