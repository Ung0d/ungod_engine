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

#include "ungod/base/Input.h"
#include "ungod/base/Transform.h"
#include "ungod/visual/RenderLayer.h"
#include "ungod/visual/Camera.h"

namespace ungod
{
    void InputEventManager::handleEvent(const sf::Event& event, const sf::RenderTarget& target)
    {
        processMouse(event, target);
    }


    void InputEventManager::processMouse(const sf::Event& event, const sf::RenderTarget& target)
    {
        switch (event.type)
        {
            case sf::Event::MouseMoved:
            {
                mHoveredEntities.processMousePos(event.mouseMove.x, event.mouseMove.y, target, mQuadtree, mRenderLayer, mMouseEnterSignal, mMouseExitSignal);
                break;
            }
            case sf::Event::MouseButtonPressed:
            {
                mClickedEntities.processMousePos(event.mouseButton.x, event.mouseButton.y, target, mQuadtree, mRenderLayer, mMouseClickedSignal, mMouseReleasedSignal);
                break;
            }
            case sf::Event::MouseButtonReleased:
            {
                mClickedEntities.clearBuffers();
                break;
            }
            default:
                break;
        }
    }


    void InputEventManager::onMouseEnter(const std::function<void(Entity)>& callback)
    {
        mMouseEnterSignal.connect(callback);
    }


    void InputEventManager::onMouseClick(const std::function<void(Entity)>& callback)
    {
        mMouseClickedSignal.connect(callback);
    }


    void InputEventManager::onMouseExit(const std::function<void(Entity)>& callback)
    {
        mMouseExitSignal.connect(callback);
    }


    void InputEventManager::onMouseReleased(const std::function<void(Entity)>& callback)
    {
        mMouseReleasedSignal.connect(callback);
    }
}
