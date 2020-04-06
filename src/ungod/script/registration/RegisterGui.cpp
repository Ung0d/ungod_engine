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

#include "ungod/script/registration/RegisterGui.h"
#include "ungod/gui/Gui.h"
#include "ungod/script/registration/RegistrationDetail.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerGui(ScriptStateBase& state)
        {
            //register utility stuff
            state.registerUsertype<tgui::Layout2d>("Layout2d");

            state.registerFunction("layout", [] (const std::string& x, const std::string& y) -> tgui::Layout2d { return tgui::Layout2d(x,y); } );

            state.registerEnum<tgui::Grid::Alignment>("Alignment",
                                         {{"UpperLeft", tgui::Grid::Alignment::UpperLeft},
                                         {"Up", tgui::Grid::Alignment::Up},
                                         {"UpperRight", tgui::Grid::Alignment::UpperRight},
                                         {"Right", tgui::Grid::Alignment::Right},
                                         {"BottomRight", tgui::Grid::Alignment::BottomRight},
                                         {"Bottom", tgui::Grid::Alignment::Bottom},
                                         {"BottomLeft", tgui::Grid::Alignment::BottomLeft},
                                         {"Left", tgui::Grid::Alignment::Left},
                                         {"Center", tgui::Grid::Alignment::Center}});
            state.registerEnum<tgui::Label::VerticalAlignment>("VerticalAlignment",
                                         {{"Top", tgui::Label::VerticalAlignment::Top},
                                         {"Center", tgui::Label::VerticalAlignment::Center},
                                         {"Bottom", tgui::Label::VerticalAlignment::Bottom}});
            state.registerEnum<tgui::Label::HorizontalAlignment>("HorizontalAlignment",
                                         {{"Left", tgui::Label::HorizontalAlignment::Left},
                                         {"Center", tgui::Label::HorizontalAlignment::Center},
                                         {"Right", tgui::Label::HorizontalAlignment::Right}});


            //register main gui
            script::Usertype<Gui> guiType = state.registerUsertype<Gui>("Gui");

            guiType["addWidget"] = sol::overload( [] (Gui& gui, const tgui::Widget::Ptr& widget, const std::string& name) { gui.add(widget, name); },
                                                                                      [] (Gui& gui, const tgui::Group::Ptr& widget, const std::string& name) { gui.add(widget, name); },
                                                                                      [] (Gui& gui, const tgui::Grid::Ptr& widget, const std::string& name) { gui.add(widget, name); },
                                                                                      [] (Gui& gui, const tgui::Panel::Ptr& widget, const std::string& name) { gui.add(widget, name); },
                                                                                      [] (Gui& gui, const tgui::Button::Ptr& widget, const std::string& name) { gui.add(widget, name); },
                                                                                      [] (Gui& gui, const tgui::RadioButton::Ptr& widget, const std::string& name) { gui.add(widget, name); },
                                                                                      [] (Gui& gui, const tgui::CheckBox::Ptr& widget, const std::string& name) { gui.add(widget, name); },
                                                                                      [] (Gui& gui, const tgui::ProgressBar::Ptr& widget, const std::string& name) { gui.add(widget, name); },
                                                                                      [] (Gui& gui, const tgui::Picture::Ptr& widget, const std::string& name) { gui.add(widget, name); },
                                                                                      [] (Gui& gui, const tgui::Scrollbar::Ptr& widget, const std::string& name) { gui.add(widget, name); },
                                                                                      [] (Gui& gui, const tgui::Label::Ptr& widget, const std::string& name) { gui.add(widget, name); },
                                                                                      [] (Gui& gui, const tgui::Tabs::Ptr& widget, const std::string& name) { gui.add(widget, name); },
                                                                                      [] (Gui& gui, const tgui::ChildWindow::Ptr& widget, const std::string& name) { gui.add(widget, name); },
                                                                                      [] (Gui& gui, const tgui::TiledBackground::Ptr& widget, const std::string& name) { gui.add(widget, name); });
            guiType["getWidgetName"] = &Gui::getWidgetName;
            guiType["getWidget"] = [] (Gui& gui, const std::string& name) { return gui.get(name); };
            guiType["getGroup"] = [] (Gui& gui, const std::string& name) { return gui.get<tgui::Group>(name); };
            guiType["getPanel"] = [] (Gui& gui, const std::string& name) { return gui.get<tgui::Panel>(name); };
            guiType["getButton"] = [] (Gui& gui, const std::string& name) { return gui.get<tgui::Button>(name); };
            guiType["getRadioButton"] = [] (Gui& gui, const std::string& name) { return gui.get<tgui::RadioButton>(name); };
            guiType["getCheckBox"] = [] (Gui& gui, const std::string& name) { return gui.get<tgui::CheckBox>(name); };
            guiType["getProgressBar"] = [] (Gui& gui, const std::string& name) { return gui.get<tgui::ProgressBar>(name); };
            guiType["getPicture"] = [] (Gui& gui, const std::string& name) { return gui.get<tgui::Picture>(name); };
            guiType["getScrollbar"] = [] (Gui& gui, const std::string& name) { return gui.get<tgui::Scrollbar>(name); };
            guiType["getLabel"] = [] (Gui& gui, const std::string& name) { return gui.get<tgui::Label>(name); };
            guiType["getTabs"] = [] (Gui& gui, const std::string& name) { return gui.get<tgui::Tabs>(name); };
            guiType["getGrid"] = [] (Gui& gui, const std::string& name) { return gui.get<tgui::Grid>(name); };
            guiType["getChildWindow"] = [] (Gui& gui, const std::string& name) { return gui.get<tgui::ChildWindow>(name); };
            guiType["getTiledBackground"] = [] (Gui& gui, const std::string& name) { return gui.get<tgui::TiledBackground>(name); };
            guiType["loadTheme"] = &Gui::loadTheme;
            guiType["getRenderer"] = &Gui::getRenderer;
            // signal connection
            guiType["connect"] = sol::overload(
                [] (Gui& gui, const tgui::Button::Ptr& button, const std::string& signalID, const ungod::script::ProtectedFunc& func)
               {
                    button->connect(signalID, [&gui, button, func] () { scriptRegistration::detail::callback(func, gui, button); });
               },
                [] (Gui& gui, const tgui::RadioButton::Ptr& button, const std::string& signalID, const ungod::script::ProtectedFunc& func)
               {
                    button->connect(signalID, [&gui, button, func] () { scriptRegistration::detail::callback(func, gui, button); });
               },
                [] (Gui& gui, const tgui::CheckBox::Ptr& checkbox, const std::string& signalID, const ungod::script::ProtectedFunc& func)
               {
                    checkbox->connect(signalID, [&gui, checkbox, func] () { scriptRegistration::detail::callback(func, gui, checkbox); });
               },
                [] (Gui& gui, const tgui::ProgressBar::Ptr& progressbar, const std::string& signalID, const ungod::script::ProtectedFunc& func)
               {
                    progressbar->connect(signalID, [&gui, progressbar, func] () { scriptRegistration::detail::callback(func, gui, progressbar); });
               },
                [] (Gui& gui, const tgui::Picture::Ptr& pic, const std::string& signalID, const ungod::script::ProtectedFunc& func)
               {
                    pic->connect(signalID, [&gui, pic, func] () { scriptRegistration::detail::callback(func, gui, pic); });
               },
                [] (Gui& gui, const tgui::Scrollbar::Ptr& scrollbar, const std::string& signalID, const ungod::script::ProtectedFunc& func)
               {
                    scrollbar->connect(signalID, [&gui, scrollbar, func] () { scriptRegistration::detail::callback(func, gui, scrollbar); });
               },
                [] (Gui& gui, const tgui::Label::Ptr& label, const std::string& signalID, const ungod::script::ProtectedFunc& func)
               {
                    label->connect(signalID, [&gui, label, func] () { scriptRegistration::detail::callback(func, gui, label); });
               },
                [] (Gui& gui, const tgui::Panel::Ptr& panel, const std::string& signalID, const ungod::script::ProtectedFunc& func)
               {
                    panel->connect(signalID, [&gui, panel, func] () { scriptRegistration::detail::callback(func, gui, panel); });
               },
                [] (Gui& gui, const tgui::Tabs::Ptr& tabs, const std::string& signalID, const ungod::script::ProtectedFunc& func)
               {
                    tabs->connect(signalID, [&gui, tabs, func] () { scriptRegistration::detail::callback(func, gui, tabs); });
               },
                [] (Gui& gui, const tgui::TiledBackground::Ptr& background, const std::string& signalID, const ungod::script::ProtectedFunc& func)
               {
                    background->connect(signalID, [&gui, background, func] () { scriptRegistration::detail::callback(func, gui, background); });
               },
                [] (Gui& gui, const tgui::ChildWindow::Ptr& childwindow, const std::string& signalID, const ungod::script::ProtectedFunc& func)
               {
                    childwindow->connect(signalID, [&gui, childwindow, func] () { scriptRegistration::detail::callback(func, gui, childwindow); });
               });

            state.registerUsertype<tgui::RendererData>("RendererData");

            //register widget stuff
            state.registerEnum<tgui::ShowAnimationType>("ShowAnimation",
                                         {{"Fade", tgui::ShowAnimationType::Fade},
                                         {"Scale", tgui::ShowAnimationType::Scale},
                                         {"SlideToBottom", tgui::ShowAnimationType::SlideToBottom},
                                         {"SlideToTop", tgui::ShowAnimationType::SlideToTop},
                                         {"SlideToLeft", tgui::ShowAnimationType::SlideToLeft},
                                         {"SlideToRight", tgui::ShowAnimationType::SlideToRight},
                                         {"SlideFromBottom", tgui::ShowAnimationType::SlideFromBottom},
                                         {"SlideFromTop", tgui::ShowAnimationType::SlideToTop},
                                         {"SlideFromLeft", tgui::ShowAnimationType::SlideFromLeft},
                                         {"SlideFromRight", tgui::ShowAnimationType::SlideFromRight}});

            script::Usertype<tgui::Widget> widgetType = state.registerUsertype<tgui::Widget>("Widget");
            widgetType["setPosition"] = sol::overload( [] (tgui::Widget& widget, const sf::Vector2f& pos) { widget.setPosition(pos); },
                                          [] (tgui::Widget& widget, float x, float y) { widget.setPosition({x,y}); },
                                          [] (tgui::Widget& widget, const tgui::Layout2d& layout) { widget.setPosition(layout); });
            widgetType["setSize"] = sol::overload( [] (tgui::Widget& widget, const sf::Vector2f& s) { widget.setSize(s); },
                                      [] (tgui::Widget& widget, float x, float y) { widget.setSize({x,y}); },
                                      [] (tgui::Widget& widget, const tgui::Layout2d& layout) { widget.setSize(layout); });
            widgetType["setRenderer"] = &tgui::Widget::setRenderer;
            widgetType["setFocused"] = &tgui::Widget::setFocused;
            widgetType["isFocused"] = &tgui::Widget::isFocused;
            widgetType["isEnabled"] = &tgui::Widget::isEnabled;
            widgetType["setEnabled"] = &tgui::Widget::setEnabled;
            widgetType["isVisible"] = &tgui::Widget::isVisible;
            widgetType["setVisible"] = &tgui::Widget::setVisible;
            widgetType["hideWithEffect"] = [] (tgui::Widget& widget, tgui::ShowAnimationType type, int32_t durationInMs) { widget.hideWithEffect(type, sf::milliseconds(durationInMs)); };
            widgetType["showWithEffect"] = [] (tgui::Widget& widget, tgui::ShowAnimationType type, int32_t durationInMs) { widget.showWithEffect(type, sf::milliseconds(durationInMs)); };

            //container (base for certain widgets)
            /*state.registerUsertype<tgui::Container>("Container",
                                                                "add", sol::overload( [] (tgui::Container& cont, const tgui::Widget::Ptr& widget, const std::string& name) { cont.add(widget, name); },
                                                                                      [] (tgui::Container& cont, const tgui::Group::Ptr& widget, const std::string& name) { cont.add(widget, name); },
                                                                                      [] (tgui::Container& cont, const tgui::Panel::Ptr& widget, const std::string& name) { cont.add(widget, name); },
                                                                                      [] (tgui::Container& cont, const tgui::Button::Ptr& widget, const std::string& name) { cont.add(widget, name); },
                                                                                      [] (tgui::Container& cont, const tgui::RadioButton::Ptr& widget, const std::string& name) { cont.add(widget, name); },
                                                                                      [] (tgui::Container& cont, const tgui::CheckBox::Ptr& widget, const std::string& name) { cont.add(widget, name); },
                                                                                      [] (tgui::Container& cont, const tgui::ProgressBar::Ptr& widget, const std::string& name) { cont.add(widget, name); },
                                                                                      [] (tgui::Container& cont, const tgui::Picture::Ptr& widget, const std::string& name) { cont.add(widget, name); },
                                                                                      [] (tgui::Container& cont, const tgui::Scrollbar::Ptr& widget, const std::string& name) { cont.add(widget, name); },
                                                                                      [] (tgui::Container& cont, const tgui::Label::Ptr& widget, const std::string& name) { cont.add(widget, name); },
                                                                                      [] (tgui::Container& cont, const tgui::Tabs::Ptr& widget, const std::string& name) { cont.add(widget, name); },
                                                                                      [] (tgui::Container& cont, const tgui::ChildWindow::Ptr& widget, const std::string& name) { cont.add(widget, name); },
                                                                                      [] (tgui::Container& cont, const tgui::TiledBackground::Ptr& widget, const std::string& name) { cont.add(widget, name); }),
                                                                "remove", &tgui::Container::remove,
                                                                "getWidget", [] (tgui::Container& cont, const std::string& name) { return cont.get(name); },
                                                                "getGroup", [] (tgui::Container& cont, const std::string& name) { return cont.get<tgui::Group>(name); },
                                                                "getPanel", [] (tgui::Container& cont, const std::string& name) { return cont.get<tgui::Panel>(name); },
                                                                "getButton", [] (tgui::Container& cont, const std::string& name) { return cont.get<tgui::Button>(name); },
                                                                "getRadioButton", [] (tgui::Container& cont, const std::string& name) { return cont.get<tgui::RadioButton>(name); },
                                                                "getCheckBox", [] (tgui::Container& cont, const std::string& name) { return cont.get<tgui::CheckBox>(name); },
                                                                "getProgressBar", [] (tgui::Container& cont, const std::string& name) { return cont.get<tgui::ProgressBar>(name); },
                                                                "getPicture", [] (tgui::Container& cont, const std::string& name) { return cont.get<tgui::Picture>(name); },
                                                                "getScrollbar", [] (tgui::Container& cont, const std::string& name) { return cont.get<tgui::Scrollbar>(name); },
                                                                "getLabel", [] (tgui::Container& cont, const std::string& name) { return cont.get<tgui::Label>(name); },
                                                                "getTabs", [] (tgui::Container& cont, const std::string& name) { return cont.get<tgui::Tabs>(name); },
                                                                "getChildWindow", [] (tgui::Container& cont, const std::string& name) { return cont.get<tgui::ChildWindow>(name); },
                                                                "getTiledBackground", [] (tgui::Container& cont, const std::string& name) { return cont.get<tgui::TiledBackground>(name); },
                                                                sol::base_classes, sol::bases<tgui::Widget>());*/

            //group
            /*state.registerFunction("createGroup", []() { return tgui::Group::create(); });
            state.registerUsertype<tgui::Group>("Group",
                                                            "setSize", sol::overload( [] (tgui::Group& g, const sf::Vector2f& s) { g.setSize(s); },
                                                                                      [] (tgui::Group& g, float x, float y) { g.setSize({x,y}); },
                                                                                      [] (tgui::Group& g, const tgui::Layout2d& layout) { g.setSize(layout); }),
                                                          sol::base_classes, sol::bases<tgui::Container, tgui::Widget>());*/

            //button
            /*state.registerFunction("createButton", []() { return tgui::Button::create(); });
            script::Usertype<tgui::Button> buttonType = state.registerUsertype<tgui::Button>("Button", sol::base_classes, sol::bases<tgui::Widget>());
            buttonType["setText"] = [] (tgui::Button& button, const std::string& text) { button.setText(text); };
            buttonType["getText"] = [] (tgui::Button& button) -> std::string { return button.getText(); };
            buttonType["setTextSize"] = &tgui::Button::setTextSize;
            buttonType["getTextSize"] = &tgui::Button::getTextSize;
            buttonType["setSize"] = sol::overload( [] (tgui::Button& button, const sf::Vector2f& s) { button.setSize(s); },
                                      [] (tgui::Button& button, float x, float y) { button.setSize({x,y}); },
                                      [] (tgui::Button& button, const tgui::Layout2d& layout) { button.setSize(layout); });

            //radio-button
            state.registerFunction("createRadioButton", []() { return tgui::RadioButton::create(); });
            script::Usertype<tgui::RadioButton> radioButtonType = state.registerUsertype<tgui::RadioButton>("RadioButton", sol::base_classes, sol::bases<tgui::Widget>());
            radioButtonType["setChecked"] = &tgui::RadioButton::setChecked;
            radioButtonType["isChecked"] = &tgui::RadioButton::isChecked;
            radioButtonType["setText"] = [] (tgui::RadioButton& rb, const std::string& text) { rb.setText(text); };
            radioButtonType["setTextSize"] = &tgui::RadioButton::setTextSize;
            radioButtonType["setTextClickable"] = &tgui::RadioButton::setTextClickable;
            radioButtonType["setSize"] = sol::overload( [] (tgui::RadioButton& button, const sf::Vector2f& s) { button.setSize(s); },
                                      [] (tgui::RadioButton& button, float x, float y) { button.setSize({x,y}); },
                                      [] (tgui::RadioButton& button, const tgui::Layout2d& layout) { button.setSize(layout); });

            //checkbox
            state.registerFunction("createCheckbox", []() { return tgui::CheckBox::create(); });
            state.registerUsertype<tgui::CheckBox>("CheckBox", sol::base_classes, sol::bases<tgui::RadioButton, tgui::Widget>());
            */
            //progress_bar
            state.registerFunction("createProgressBar", []() { return tgui::ProgressBar::create(); });
            state.registerEnum<tgui::ProgressBar::FillDirection>("FillDirection",
                                         {{"LeftToRight", tgui::ProgressBar::FillDirection::LeftToRight},
                                         {"RightToLeft", tgui::ProgressBar::FillDirection::RightToLeft},
                                         {"TopToBottom", tgui::ProgressBar::FillDirection::TopToBottom},
                                         {"BottomToTop", tgui::ProgressBar::FillDirection::BottomToTop}});
            script::Usertype<tgui::ProgressBar> progressBarType = state.registerUsertype<tgui::ProgressBar>("ProgressBar", sol::base_classes, sol::bases<tgui::Widget>());
            progressBarType["setSize"] = sol::overload( [] (tgui::ProgressBar& pb, const sf::Vector2f& s) { pb.setSize(s); },
                                      [] (tgui::ProgressBar& pb, float x, float y) { pb.setSize({x,y}); },
                                      [] (tgui::ProgressBar& pb, const tgui::Layout2d& layout) { pb.setSize(layout); });
            progressBarType["setMinimum"] = &tgui::ProgressBar::setMinimum;
            progressBarType["setMaximum"] = &tgui::ProgressBar::setMaximum;
            progressBarType["setValue"] = [] (tgui::ProgressBar& pb, float value) { pb.setValue((unsigned)value); };
            progressBarType["getValue"] = &tgui::ProgressBar::getValue;
            progressBarType["incrementValue"] = &tgui::ProgressBar::incrementValue;
            progressBarType["setText"] = [] (tgui::ProgressBar& pb, const std::string& text) { pb.setText(text); };
            progressBarType["setTextSize"] = &tgui::ProgressBar::setTextSize;
            progressBarType["setFillDirection"] = &tgui::ProgressBar::setFillDirection;
            progressBarType["getBackgroundSize"] = [] (tgui::ProgressBar& progbar) -> sf::Vector2i
            { return { progbar.getRenderer()->getTextureBackground().getData()->rect.width, progbar.getRenderer()->getTextureBackground().getData()->rect.height};  };
            progressBarType["getFillSize"] = [] (tgui::ProgressBar& progbar) -> sf::Vector2i
            { return { progbar.getRenderer()->getTextureFill().getData()->rect.width, progbar.getRenderer()->getTextureFill().getData()->rect.height};  };
            /*
            //picture
            state.registerFunction("createPicture", []() { return tgui::Picture::create();} );
            script::Usertype<tgui::Picture> pictureType = state.registerUsertype<tgui::Picture>("Picture",sol::base_classes, sol::bases<tgui::Widget>());
            pictureType["setSize"] = sol::overload( [] (tgui::Picture& pic, const sf::Vector2f& s) { pic.setSize(s); },
                                      [] (tgui::Picture& pic, float x, float y) { pic.setSize({x,y}); },
                                      [] (tgui::Picture& pic, const tgui::Layout2d& layout) { pic.setSize(layout); });
            pictureType["getTextureSize"] = [] (tgui::Picture& pic) -> sf::Vector2i
            { return { pic.getRenderer()->getTexture().getData()->rect.width, pic.getRenderer()->getTexture().getData()->rect.height};  };

            //scrollbar
            state.registerFunction("createScrollbar", []() { return tgui::Scrollbar::create(); });
            script::Usertype<tgui::Scrollbar> scrollbarType = state.registerUsertype<tgui::Scrollbar>("Scrollbar",sol::base_classes, sol::bases<tgui::Widget>());
            scrollbarType["setSize"] = sol::overload( [] (tgui::Scrollbar& scrollbar, const sf::Vector2f& s) { scrollbar.setSize(s); },
                                      [] (tgui::Scrollbar& scrollbar, float x, float y) { scrollbar.setSize({x,y}); },
                                      [] (tgui::Scrollbar& scrollbar, const tgui::Layout2d& layout) { scrollbar.setSize(layout); });
           scrollbarType["setMaximum"] = &tgui::Scrollbar::setMaximum;
           scrollbarType["setValue"] = &tgui::Scrollbar::setValue;
           scrollbarType["getValue"] = &tgui::Scrollbar::getValue;
           scrollbarType["setScrollAmount"] = &tgui::Scrollbar::setScrollAmount;
           scrollbarType["setAutoHide"] = &tgui::Scrollbar::setAutoHide;

            //grid
            state.registerFunction("createGrid", []() { return tgui::Grid::create(); });
            script::Usertype<tgui::Grid> gridType = state.registerUsertype<tgui::Grid>("Grid",sol::base_classes, sol::bases<tgui::Container, tgui::Widget>());
            gridType["setSize"] = sol::overload( [] (tgui::Grid& grid, const sf::Vector2f& s) { grid.setSize(s); },
                                      [] (tgui::Grid& grid, float x, float y) { grid.setSize({x,y}); },
                                      [] (tgui::Grid& grid, const tgui::Layout2d& layout) { grid.setSize(layout); });
            gridType["addWidget"] = sol::overload([] (tgui::Grid& grid, const tgui::Widget::Ptr& widget, unsigned int row, unsigned int column, float left, float top, float right, float bottom, tgui::Grid::Alignment alignment)
                                                { grid.addWidget(widget, row, column); },
                                      [] (tgui::Grid& grid, const tgui::Group::Ptr& widget, unsigned int row, unsigned int column, float left, float top, float right, float bottom, tgui::Grid::Alignment alignment)
                                      { grid.addWidget(widget, row, column, tgui::Borders{left, top, right, bottom}, alignment); },
                                      [] (tgui::Grid& grid, const tgui::Panel::Ptr& widget, unsigned int row, unsigned int column, float left, float top, float right, float bottom, tgui::Grid::Alignment alignment)
                                      { grid.addWidget(widget, row, column, tgui::Borders{left, top, right, bottom}, alignment); },
                                      [] (tgui::Grid& grid, const tgui::Button::Ptr& widget, unsigned int row, unsigned int column, float left, float top, float right, float bottom, tgui::Grid::Alignment alignment)
                                      { grid.addWidget(widget, row, column, tgui::Borders{left, top, right, bottom}, alignment); },
                                      [] (tgui::Grid& grid, const tgui::RadioButton::Ptr& widget, unsigned int row, unsigned int column, float left, float top, float right, float bottom, tgui::Grid::Alignment alignment)
                                      { grid.addWidget(widget, row, column, tgui::Borders{left, top, right, bottom}, alignment); },
                                      [] (tgui::Grid& grid, const tgui::CheckBox::Ptr& widget, unsigned int row, unsigned int column, float left, float top, float right, float bottom, tgui::Grid::Alignment alignment)
                                      { grid.addWidget(widget, row, column, tgui::Borders{left, top, right, bottom}, alignment); },
                                      [] (tgui::Grid& grid, const tgui::ProgressBar::Ptr& widget, unsigned int row, unsigned int column, float left, float top, float right, float bottom, tgui::Grid::Alignment alignment)
                                      { grid.addWidget(widget, row, column, tgui::Borders{left, top, right, bottom}, alignment); },
                                      [] (tgui::Grid& grid, const tgui::Picture::Ptr& widget, unsigned int row, unsigned int column, float left, float top, float right, float bottom, tgui::Grid::Alignment alignment)
                                      { grid.addWidget(widget, row, column, tgui::Borders{left, top, right, bottom}, alignment); },
                                      [] (tgui::Grid& grid, const tgui::Scrollbar::Ptr& widget, unsigned int row, unsigned int column, float left, float top, float right, float bottom, tgui::Grid::Alignment alignment)
                                      { grid.addWidget(widget, row, column, tgui::Borders{left, top, right, bottom}, alignment); },
                                      [] (tgui::Grid& grid, const tgui::Label::Ptr& widget, unsigned int row, unsigned int column, float left, float top, float right, float bottom, tgui::Grid::Alignment alignment)
                                      { grid.addWidget(widget, row, column, tgui::Borders{left, top, right, bottom}, alignment); },
                                      [] (tgui::Grid& grid, const tgui::Tabs::Ptr& widget, unsigned int row, unsigned int column, float left, float top, float right, float bottom, tgui::Grid::Alignment alignment)
                                      { grid.addWidget(widget, row, column, tgui::Borders{left, top, right, bottom}, alignment); },
                                      [] (tgui::Grid& grid, const tgui::ChildWindow::Ptr& widget, unsigned int row, unsigned int column, float left, float top, float right, float bottom, tgui::Grid::Alignment alignment)
                                      { grid.addWidget(widget, row, column, tgui::Borders{left, top, right, bottom}, alignment); },
                                      [] (tgui::Grid& grid, const tgui::TiledBackground::Ptr& widget, unsigned int row, unsigned int column, float left, float top, float right, float bottom, tgui::Grid::Alignment alignment)
                                      { grid.addWidget(widget, row, column, tgui::Borders{left, top, right, bottom}, alignment); });

            //label
            state.registerFunction("createLabel", []() { return tgui::Label::create(); });
            script::Usertype<tgui::Label> labelType = state.registerUsertype<tgui::Label>("Label",sol::base_classes, sol::bases<tgui::Widget>());
            labelType["setSize"] = sol::overload( [] (tgui::Label& label, const sf::Vector2f& s) { label.setSize(s); },
                                      [] (tgui::Label& label, float x, float y) { label.setSize({x,y}); },
                                      [] (tgui::Label& label, const tgui::Layout2d& layout) { label.setSize(layout); });
           labelType["setText"] = [] (tgui::Label& label, const std::string& text) { label.setText(text); };
           labelType["setTextSize"] = &tgui::Label::setTextSize;
           labelType["setVerticalAlignment"] = &tgui::Label::setVerticalAlignment;
           labelType["setHorizontalAlignment"] = &tgui::Label::setHorizontalAlignment;
           labelType["setAutoSize"] = &tgui::Label::setAutoSize;
           labelType["setMaximumTextWidth"] = &tgui::Label::setMaximumTextWidth;

            //panel
            state.registerFunction("createPanel", []() { return tgui::Panel::create(); });
            script::Usertype<tgui::Panel> panelType = state.registerUsertype<tgui::Panel>("Panel",sol::base_classes, sol::bases<tgui::Container, tgui::Widget>());
            panelType["setSize"] = sol::overload( [] (tgui::Panel& panel, const sf::Vector2f& s) { panel.setSize(s); },
                                      [] (tgui::Panel& panel, float x, float y) { panel.setSize({x,y}); },
                                      [] (tgui::Panel& panel, const tgui::Layout2d& layout) { panel.setSize(layout); });

            //tabs
            state.registerFunction("createTabs", []() { return tgui::Tabs::create(); });
            script::Usertype<tgui::Tabs> tabsType = state.registerUsertype<tgui::Tabs>("Tabs",sol::base_classes, sol::bases<tgui::Widget>());
            tabsType["setSize"] = sol::overload( [] (tgui::Tabs& tabs, const sf::Vector2f& s) { tabs.setSize(s); },
                                      [] (tgui::Tabs& tabs, float x, float y) { tabs.setSize({x,y}); },
                                      [] (tgui::Tabs& tabs, const tgui::Layout2d& layout) { tabs.setSize(layout); });
            tabsType["add"] = [] (tgui::Tabs& tabs, const std::string& text, bool selected) { tabs.add(text, selected); };
            tabsType["setTabHeight"] = &tgui::Tabs::setTabHeight;
            tabsType["setTextSize"] = &tgui::Tabs::setTextSize;
            tabsType["setMaximumTabWidth"] = &tgui::Tabs::setMaximumTabWidth;
            tabsType["setMinimumTabWidth"] = &tgui::Tabs::setMinimumTabWidth;
            tabsType["setAutoSize"] = &tgui::Tabs::setAutoSize ;
            tabsType["getTabsCount"] = &tgui::Tabs::getTabsCount;
            tabsType["changeText"] = [] (tgui::Tabs& tabs, std::size_t index, const std::string& text) { tabs.changeText(index, text); };
            tabsType["select"] = [] (tgui::Tabs& tabs, std::size_t index) { tabs.select(index); };
            tabsType["getText"] = [] (tgui::Tabs& tabs, std::size_t index) -> std::string { return tabs.getText(index); };
            tabsType["getSelectedIndex"] = &tgui::Tabs::getSelectedIndex;

            //tiled background
            state.registerFunction("createTiledBackground", []() { return tgui::TiledBackground::create(); });
            script::Usertype<tgui::TiledBackground> tiledBackgrType = state.registerUsertype<tgui::TiledBackground>("TiledBackground",sol::base_classes, sol::bases<tgui::Widget>());
            tiledBackgrType["setSize"] = sol::overload( [] (tgui::TiledBackground& background, const sf::Vector2f& s) { background.setSize(s); },
                                      [] (tgui::TiledBackground& background, float x, float y) { background.setSize({x,y}); },
                                      [] (tgui::TiledBackground& background, const tgui::Layout2d& layout) { background.setSize(layout); });

            tiledBackgrType["setPosition"] = sol::overload( [] (tgui::TiledBackground& background, const sf::Vector2f& pos) { background.setPosition(pos); },
                                          [] (tgui::TiledBackground& background, float x, float y) { background.setPosition({x,y}); },
                                          [] (tgui::TiledBackground& background, const tgui::Layout2d& layout) { background.setPosition(layout); });

            //child_window
            state.registerFunction("createChildWindow", []() { return tgui::ChildWindow::create(); });
            state.registerEnum<tgui::ChildWindow::TitleAlignment>("TitleAlignment",
                                         {{"Left", tgui::ChildWindow::TitleAlignment::Left},
                                         {"Center", tgui::ChildWindow::TitleAlignment::Center},
                                         {"Right", tgui::ChildWindow::TitleAlignment::Right}});
            script::Usertype<tgui::ChildWindow> childWindowType = state.registerUsertype<tgui::ChildWindow>("ChildWindow",sol::base_classes, sol::bases<tgui::Container, tgui::Widget>());
            childWindowType["setPosition"] = sol::overload( [] (tgui::ChildWindow& childwindow, const sf::Vector2f& pos) { childwindow.setPosition(pos); },
                                          [] (tgui::ChildWindow& childwindow, float x, float y) { childwindow.setPosition({x,y}); },
                                          [] (tgui::ChildWindow& childwindow, const tgui::Layout2d& layout) { childwindow.setPosition(layout); });
            childWindowType["setSize"] = sol::overload( [] (tgui::ChildWindow& childwindow, const sf::Vector2f& s) { childwindow.setSize(s); },
                                      [] (tgui::ChildWindow& childwindow, float x, float y) { childwindow.setSize({x,y}); },
                                      [] (tgui::ChildWindow& childwindow, const tgui::Layout2d& layout) { childwindow.setSize(layout); });
            childWindowType["setMinimumSize"] = &tgui::ChildWindow::setMinimumSize;
            childWindowType["setMaximumSize"] = &tgui::ChildWindow::setMaximumSize;
            childWindowType["setTitle"] = [] (tgui::ChildWindow& childwindow, const std::string& title) { childwindow.setTitle(title); };
            childWindowType["setTitleAlignment"] = &tgui::ChildWindow::setTitleAlignment;
            childWindowType["setResizable"] = &tgui::ChildWindow::setResizable;
            childWindowType["getFullSize"] = &tgui::ChildWindow::getFullSize; */
        }
    }
}
