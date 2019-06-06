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

#ifndef UNGOD_GUI_H
#define UNGOD_GUI_H

#include "ungod/script/Behavior.h"
#include "ungod/audio/Audio.h"
#include "ungod/serialization/SerialComponents.h"
#include <TGUI/TGUI.hpp>
#include "ungod/gui/TiledBackground.h"

namespace ungod
{
    using WidgetPtr = tgui::Widget::Ptr;
    using WidgetConstPtr = tgui::Widget::ConstPtr;

    /** \brief Main gui class, that acts as a collection of widgets. Events and updates are forwarded to
    * the attched widgets. */
    class Gui : public tgui::Gui
    {
    public:
        /** \brief Default constructs a gui object for later initialization. */
        Gui();

        /** \brief Constructs a gui object and sets the render-target it shall be drawn to. */
        Gui(sf::RenderTarget& target);

        /** \brief Delete copy. */
        Gui(const Gui& copy) = delete;

        /** \brief Delete assigment. */
        Gui& operator=(const Gui& right) = delete;

        /** \brief Loads a theme file. Returns success. */
        void loadTheme(const std::string& filepath);

        /** \brief Returns renderer data for the given key. */
        std::shared_ptr<tgui::RendererData> getRenderer(const std::string& key);

        /** \brief Handles the given input-event and attempts to forward it to all widgets.
        * If it returns false, something went wrong and the event was ignored by all widgets. */
        bool handleEvent(sf::Event event);

        /** \brief Draws the gui. */
        void draw();

    private:
        tgui::Theme mTheme;
    };
}

#endif // UNGOD_GUI_H



