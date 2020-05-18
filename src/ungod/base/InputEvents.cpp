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

#include "ungod/base/InputEvents.h"
#include "ungod/base/Transform.h"
#include "ungod/base/WorldGraphNode.h"
#include "ungod/visual/Camera.h"

namespace ungod
{
    void Doublebuffer::processMousePos(int x, int y, const sf::RenderTarget& target, const Camera& cam, quad::QuadTree<Entity>& quadtree, const WorldGraphNode& node, owls::Signal<Entity>& enter, owls::Signal<Entity>& exit)
    {
        //compute the global position of the mouse
        sf::Vector2f mouseWorldPos = target.mapPixelToCoords({ x, y }, cam.getView());
        //translate to the world local position
        mouseWorldPos = node.mapToLocalPosition(mouseWorldPos);
        //pull entities that likely collide with that position
        quad::PullResult<Entity> pull;
        quadtree.retrieve(pull, { mouseWorldPos.x, mouseWorldPos.y, 0.0f, 0.0f });

        dom::Utility<Entity>::iterate<TransformComponent>(pull.getList(),
            [this, mouseWorldPos](Entity e, TransformComponent& transf)
            {
                if (transf.getBounds().contains(mouseWorldPos))
                {
                    entities[swapper].insert(e);
                }
            });

        //for each entity that is hovered this frame
        for (const auto& e : entities[swapper])
        {
            //check whether it was not hovered last frame
            auto result = entities[!swapper].find(e);
            if (result == entities[!swapper].end())
            {
                enter(e);
            }
        }

        //for each entity that is hovered last frame
        for (const auto& e : entities[!swapper])
        {
            if (!e)
                continue;
            //check whether it was not hovered this frame
            auto result = entities[swapper].find(e);
            if (result == entities[swapper].end())
            {
                exit(e);
            }
        }

        //swap buffers
        swapper = !swapper;
        entities[swapper].clear();
    }

    void Doublebuffer::clearBuffers()
    {
        entities[swapper].clear();
        entities[!swapper].clear();
    }


    void InputEventHandler::handleEvent(const sf::Event& event, const sf::RenderTarget& target, const Camera& cam)
    {
        switch (event.type)
        {
            case sf::Event::MouseMoved:
            {
                mHoveredEntities.processMousePos(event.mouseMove.x, event.mouseMove.y, target, cam, mQuadtree, mNode, mMouseEnterSignal, mMouseExitSignal);
                break;
            }
            case sf::Event::MouseButtonPressed:
            {
                mClickedEntities.processMousePos(event.mouseButton.x, event.mouseButton.y, target, cam, mQuadtree, mNode, mMouseClickedSignal, mMouseReleasedSignal);
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


    void InputEventHandler::onMouseEnter(const std::function<void(Entity)>& callback)
    {
        mMouseEnterSignal.connect(callback);
    }


    void InputEventHandler::onMouseClick(const std::function<void(Entity)>& callback)
    {
        mMouseClickedSignal.connect(callback);
    }


    void InputEventHandler::onMouseExit(const std::function<void(Entity)>& callback)
    {
        mMouseExitSignal.connect(callback);
    }


    void InputEventHandler::onMouseReleased(const std::function<void(Entity)>& callback)
    {
        mMouseReleasedSignal.connect(callback);
    }
}
