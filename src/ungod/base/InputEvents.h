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

#ifndef INPUT_H
#define INPUT_H

#include "ungod/base/Entity.h"

namespace ungod
{
    class RenderLayer;

    /** \brief Implements entity with mouse interactions like entity hovered signals. */
    class InputEventManager 
    {
    public:
        InputEventManager(quad::QuadTree<Entity>& quadtree, RenderLayer const* renderlayer) : InputHandler(), mQuadtree(quadtree), mRenderLayer(renderlayer){}

        /** \brief Evaluates an input-event and sends out signals. */
        void handleEvent(const sf::Event& event, const sf::RenderTarget& target);

        /** \brief Evaluates input and emits mouseEnter, mouseclick and mouseExit signals. */
        void processMouse(const sf::Event& event, const sf::RenderTarget& target);

        /** \brief Registers new callback for the MouseEnter signal. */
        void onMouseEnter(const std::function<void(Entity)>& callback);

        /** \brief Registers new callback for the MouseClick signal. */
        void onMouseClick(const std::function<void(Entity)>& callback);

        /** \brief Registers new callback for the MouseExit signal. */
        void onMouseExit(const std::function<void(Entity)>& callback);

        /** \brief Registers new callback for the MouseReleased signal. */
        void onMouseReleased(const std::function<void(Entity)>& callback);

    private:
        quad::QuadTree<Entity>& mQuadtree;
        RenderLayer const* mRenderLayer;

        Doublebuffer mHoveredEntities;
        Doublebuffer mClickedEntities;

        owls::Signal<Entity> mMouseEnterSignal;
        owls::Signal<Entity> mMouseClickedSignal;
        owls::Signal<Entity> mMouseExitSignal;
        owls::Signal<Entity> mMouseReleasedSignal;
    };
}

#endif // INPUT_H
