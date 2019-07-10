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

            state.registerEnum("Alignment",
                                         "UpperLeft", tgui::Grid::Alignment::UpperLeft,
                                         "Up", tgui::Grid::Alignment::Up,
                                         "UpperRight", tgui::Grid::Alignment::UpperRight,
                                         "Right", tgui::Grid::Alignment::Right,
                                         "BottomRight", tgui::Grid::Alignment::BottomRight,
                                         "Bottom", tgui::Grid::Alignment::Bottom,
                                         "BottomLeft", tgui::Grid::Alignment::BottomLeft,
                                         "Left", tgui::Grid::Alignment::Left,
                                         "Center", tgui::Grid::Alignment::Center);
            state.registerEnum("VerticalAlignment",
                                         "Top", tgui::Label::VerticalAlignment::Top,
                                         "Center", tgui::Label::VerticalAlignment::Center,
                                         "Bottom", tgui::Label::VerticalAlignment::Bottom);
            state.registerEnum("HorizontalAlignment",
                                         "Left", tgui::Label::HorizontalAlignment::Left,
                                         "Center", tgui::Label::HorizontalAlignment::Center,
                                         "Right", tgui::Label::HorizontalAlignment::Right);


            //register main gui
            state.registerUsertype<Gui>("Gui",
                                                        "addWidget", sol::overload( [] (Gui& gui, const tgui::Widget::Ptr& widget, const std::string& name) { gui.add(widget, name); },
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
                                                                                      [] (Gui& gui, const tgui::TiledBackground::Ptr& widget, const std::string& name) { gui.add(widget, name); }),
                                                        "getWidgetName", &Gui::getWidgetName,
                                                        "getWidget", [] (Gui& gui, const std::string& name) { return gui.get(name); },
                                                        "getGroup", [] (Gui& gui, const std::string& name) { return gui.get<tgui::Group>(name); },
                                                        "getPanel", [] (Gui& gui, const std::string& name) { return gui.get<tgui::Panel>(name); },
                                                        "getButton", [] (Gui& gui, const std::string& name) { return gui.get<tgui::Button>(name); },
                                                        "getRadioButton", [] (Gui& gui, const std::string& name) { return gui.get<tgui::RadioButton>(name); },
                                                        "getCheckBox", [] (Gui& gui, const std::string& name) { return gui.get<tgui::CheckBox>(name); },
                                                        "getProgressBar", [] (Gui& gui, const std::string& name) { return gui.get<tgui::ProgressBar>(name); },
                                                        "getPicture", [] (Gui& gui, const std::string& name) { return gui.get<tgui::Picture>(name); },
                                                        "getScrollbar", [] (Gui& gui, const std::string& name) { return gui.get<tgui::Scrollbar>(name); },
                                                        "getLabel", [] (Gui& gui, const std::string& name) { return gui.get<tgui::Label>(name); },
                                                        "getTabs", [] (Gui& gui, const std::string& name) { return gui.get<tgui::Tabs>(name); },
                                                        "getGrid", [] (Gui& gui, const std::string& name) { return gui.get<tgui::Grid>(name); },
                                                        "getChildWindow", [] (Gui& gui, const std::string& name) { return gui.get<tgui::ChildWindow>(name); },
                                                        "getTiledBackground", [] (Gui& gui, const std::string& name) { return gui.get<tgui::TiledBackground>(name); },
                                                        "loadTheme", &Gui::loadTheme,
                                                        "getRenderer", &Gui::getRenderer,
                                                        // signal connection
                                                        "connect", sol::overload(
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
                                                           }) );
            state.registerUsertype<tgui::RendererData>("RendererData");

            //register widget stuff
            state.registerEnum("ShowAnimation",
                                         "Fade", tgui::ShowAnimationType::Fade,
                                         "Scale", tgui::ShowAnimationType::Scale,
                                         "SlideToBottom", tgui::ShowAnimationType::SlideToBottom,
                                         "SlideToTop", tgui::ShowAnimationType::SlideToTop,
                                         "SlideToLeft", tgui::ShowAnimationType::SlideToLeft,
                                         "SlideToRight", tgui::ShowAnimationType::SlideToRight,
                                         "SlideFromBottom", tgui::ShowAnimationType::SlideFromBottom,
                                         "SlideFromTop", tgui::ShowAnimationType::SlideToTop,
                                         "SlideFromLeft", tgui::ShowAnimationType::SlideFromLeft,
                                         "SlideFromRight", tgui::ShowAnimationType::SlideFromRight);
            state.registerUsertype<tgui::Widget>("Widget",
                                                            "setPosition", sol::overload( [] (tgui::Widget& widget, const sf::Vector2f& pos) { widget.setPosition(pos); },
                                                                                          [] (tgui::Widget& widget, float x, float y) { widget.setPosition({x,y}); },
                                                                                          [] (tgui::Widget& widget, const tgui::Layout2d& layout) { widget.setPosition(layout); }),
                                                            "setSize", sol::overload( [] (tgui::Widget& widget, const sf::Vector2f& s) { widget.setSize(s); },
                                                                                      [] (tgui::Widget& widget, float x, float y) { widget.setSize({x,y}); },
                                                                                      [] (tgui::Widget& widget, const tgui::Layout2d& layout) { widget.setSize(layout); }),
                                                            "setRenderer", &tgui::Widget::setRenderer,
                                                            "setFocused", &tgui::Widget::setFocused,
                                                            "isFocused", &tgui::Widget::isFocused,
                                                            "isEnabled", &tgui::Widget::isEnabled,
                                                            "setEnabled", &tgui::Widget::setEnabled,
                                                            "isVisible", &tgui::Widget::isVisible,
                                                            "setVisible", &tgui::Widget::setVisible,
                                                            "hideWithEffect", [] (tgui::Widget& widget, tgui::ShowAnimationType type, int32_t durationInMs) { widget.hideWithEffect(type, sf::milliseconds(durationInMs)); },
                                                            "showWithEffect", [] (tgui::Widget& widget, tgui::ShowAnimationType type, int32_t durationInMs) { widget.showWithEffect(type, sf::milliseconds(durationInMs)); });

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
            state.registerFunction("createButton", []() { return tgui::Button::create(); });
            state.registerUsertype<tgui::Button>("Button",
                                                           "setText", [] (tgui::Button& button, const std::string& text) { button.setText(text); },
                                                           "getText", [] (tgui::Button& button) -> std::string { return button.getText(); },
                                                           "setTextSize", &tgui::Button::setTextSize,
                                                           "getTextSize", &tgui::Button::getTextSize,
                                                            "setSize", sol::overload( [] (tgui::Button& button, const sf::Vector2f& s) { button.setSize(s); },
                                                                                      [] (tgui::Button& button, float x, float y) { button.setSize({x,y}); },
                                                                                      [] (tgui::Button& button, const tgui::Layout2d& layout) { button.setSize(layout); }),
                                                            sol::base_classes, sol::bases<tgui::Widget>());

            //radio-button
            state.registerFunction("createRadioButton", []() { return tgui::RadioButton::create(); });
            state.registerUsertype<tgui::RadioButton>("RadioButton",
                                                           "setChecked", &tgui::RadioButton::setChecked,
                                                           "isChecked", &tgui::RadioButton::isChecked,
                                                           "setText", [] (tgui::RadioButton& rb, const std::string& text) { rb.setText(text); },
                                                           "setTextSize", &tgui::RadioButton::setTextSize,
                                                           "setTextClickable", &tgui::RadioButton::setTextClickable,
                                                            "setSize", sol::overload( [] (tgui::RadioButton& button, const sf::Vector2f& s) { button.setSize(s); },
                                                                                      [] (tgui::RadioButton& button, float x, float y) { button.setSize({x,y}); },
                                                                                      [] (tgui::RadioButton& button, const tgui::Layout2d& layout) { button.setSize(layout); }),
                                                            sol::base_classes, sol::bases<tgui::Widget>());

            //checkbox
            state.registerFunction("createCheckbox", []() { return tgui::CheckBox::create(); });
            state.registerUsertype<tgui::CheckBox>("CheckBox",
                                                            sol::base_classes, sol::bases<tgui::RadioButton, tgui::Widget>());

            //progress_bar
            state.registerFunction("createProgressBar", []() { return tgui::ProgressBar::create(); });
            state.registerEnum("FillDirection",
                                         "LeftToRight", tgui::ProgressBar::FillDirection::LeftToRight,
                                         "RightToLeft", tgui::ProgressBar::FillDirection::RightToLeft,
                                         "TopToBottom", tgui::ProgressBar::FillDirection::TopToBottom,
                                         "BottomToTop", tgui::ProgressBar::FillDirection::BottomToTop);
            state.registerUsertype<tgui::ProgressBar>("ProgressBar",
                                                            "setSize", sol::overload( [] (tgui::ProgressBar& pb, const sf::Vector2f& s) { pb.setSize(s); },
                                                                                      [] (tgui::ProgressBar& pb, float x, float y) { pb.setSize({x,y}); },
                                                                                      [] (tgui::ProgressBar& pb, const tgui::Layout2d& layout) { pb.setSize(layout); }),
                                                            "setMinimum", &tgui::ProgressBar::setMinimum,
                                                            "setMaximum", &tgui::ProgressBar::setMaximum,
                                                            "setValue", [] (tgui::ProgressBar& pb, float value) { pb.setValue((unsigned)value); },
                                                            "getValue", &tgui::ProgressBar::getValue,
                                                            "incrementValue", &tgui::ProgressBar::incrementValue,
                                                            "setText", [] (tgui::ProgressBar& pb, const std::string& text) { pb.setText(text); } ,
                                                            "setTextSize", &tgui::ProgressBar::setTextSize,
                                                            "setFillDirection", &tgui::ProgressBar::setFillDirection,
                                                           "getBackgroundSize", [] (tgui::ProgressBar& progbar) -> sf::Vector2i
                                                            { return { progbar.getRenderer()->getTextureBackground().getData()->rect.width, progbar.getRenderer()->getTextureBackground().getData()->rect.height};  } ,
                                                           "getFillSize", [] (tgui::ProgressBar& progbar) -> sf::Vector2i
                                                            { return { progbar.getRenderer()->getTextureFill().getData()->rect.width, progbar.getRenderer()->getTextureFill().getData()->rect.height};  } ,
                                                            sol::base_classes, sol::bases<tgui::Widget>());

            //picture
            state.registerFunction("createPicture", []() { return tgui::Picture::create();} );
            state.registerUsertype<tgui::Picture>("Picture",
                                                            "setSize", sol::overload( [] (tgui::Picture& pic, const sf::Vector2f& s) { pic.setSize(s); },
                                                                                      [] (tgui::Picture& pic, float x, float y) { pic.setSize({x,y}); },
                                                                                      [] (tgui::Picture& pic, const tgui::Layout2d& layout) { pic.setSize(layout); }),
                                                            "getTextureSize", [] (tgui::Picture& pic) -> sf::Vector2i
                                                            { return { pic.getRenderer()->getTexture().getData()->rect.width, pic.getRenderer()->getTexture().getData()->rect.height};  },
                                                            sol::base_classes, sol::bases<tgui::Widget>());

            //scrollbar
            state.registerFunction("createScrollbar", []() { return tgui::Scrollbar::create(); });
            state.registerUsertype<tgui::Scrollbar>("Scrollbar",
                                                            "setSize", sol::overload( [] (tgui::Scrollbar& scrollbar, const sf::Vector2f& s) { scrollbar.setSize(s); },
                                                                                      [] (tgui::Scrollbar& scrollbar, float x, float y) { scrollbar.setSize({x,y}); },
                                                                                      [] (tgui::Scrollbar& scrollbar, const tgui::Layout2d& layout) { scrollbar.setSize(layout); }),
                                                           "setMaximum", &tgui::Scrollbar::setMaximum,
                                                           "setValue", &tgui::Scrollbar::setValue,
                                                           "getValue", &tgui::Scrollbar::getValue,
                                                           "setScrollAmount", &tgui::Scrollbar::setScrollAmount,
                                                           "setAutoHide", &tgui::Scrollbar::setAutoHide,
                                                            sol::base_classes, sol::bases<tgui::Widget>());

            //grid
            state.registerFunction("createGrid", []() { return tgui::Grid::create(); });
            state.registerUsertype<tgui::Grid>("Grid",
                                                "setSize", sol::overload( [] (tgui::Grid& grid, const sf::Vector2f& s) { grid.setSize(s); },
                                                                          [] (tgui::Grid& grid, float x, float y) { grid.setSize({x,y}); },
                                                                          [] (tgui::Grid& grid, const tgui::Layout2d& layout) { grid.setSize(layout); }),
                                               "addWidget", sol::overload([] (tgui::Grid& grid, const tgui::Widget::Ptr& widget, unsigned int row, unsigned int column, float left, float top, float right, float bottom, tgui::Grid::Alignment alignment)
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
                                                                          { grid.addWidget(widget, row, column, tgui::Borders{left, top, right, bottom}, alignment); }),
                                                sol::base_classes, sol::bases<tgui::Widget>());

            //label
            state.registerFunction("createLabel", []() { return tgui::Label::create(); });
            state.registerUsertype<tgui::Label>("Label",
                                                            "setSize", sol::overload( [] (tgui::Label& label, const sf::Vector2f& s) { label.setSize(s); },
                                                                                      [] (tgui::Label& label, float x, float y) { label.setSize({x,y}); },
                                                                                      [] (tgui::Label& label, const tgui::Layout2d& layout) { label.setSize(layout); }),
                                                           "setText", [] (tgui::Label& label, const std::string& text) { label.setText(text); },
                                                           "setTextSize", &tgui::Label::setTextSize,
                                                           "setVerticalAlignment", &tgui::Label::setVerticalAlignment,
                                                           "setHorizontalAlignment", &tgui::Label::setHorizontalAlignment,
                                                           "setAutoSize", &tgui::Label::setAutoSize,
                                                           "setMaximumTextWidth", &tgui::Label::setMaximumTextWidth,
                                                            sol::base_classes, sol::bases<tgui::Widget>());

            //panel
            state.registerFunction("createPanel", []() { return tgui::Panel::create(); });
            state.registerUsertype<tgui::Panel>("Panel",
                                                            "setSize", sol::overload( [] (tgui::Panel& panel, const sf::Vector2f& s) { panel.setSize(s); },
                                                                                      [] (tgui::Panel& panel, float x, float y) { panel.setSize({x,y}); },
                                                                                      [] (tgui::Panel& panel, const tgui::Layout2d& layout) { panel.setSize(layout); }),
                                                            sol::base_classes, sol::bases<tgui::Container, tgui::Widget>());

            //tabs
            state.registerFunction("createTabs", []() { return tgui::Tabs::create(); });
            state.registerUsertype<tgui::Tabs>("Tabs",
                                                            "setSize", sol::overload( [] (tgui::Tabs& tabs, const sf::Vector2f& s) { tabs.setSize(s); },
                                                                                      [] (tgui::Tabs& tabs, float x, float y) { tabs.setSize({x,y}); },
                                                                                      [] (tgui::Tabs& tabs, const tgui::Layout2d& layout) { tabs.setSize(layout); }),
                                                            "add", [] (tgui::Tabs& tabs, const std::string& text, bool selected) { tabs.add(text, selected); } ,
                                                            "setTabHeight", &tgui::Tabs::setTabHeight ,
                                                            "setTextSize", &tgui::Tabs::setTextSize ,
                                                            "setMaximumTabWidth", &tgui::Tabs::setMaximumTabWidth ,
                                                            "setMinimumTabWidth", &tgui::Tabs::setMinimumTabWidth ,
                                                            "setAutoSize", &tgui::Tabs::setAutoSize ,
                                                            "getTabsCount", &tgui::Tabs::getTabsCount,
                                                            "changeText", [] (tgui::Tabs& tabs, std::size_t index, const std::string& text) { tabs.changeText(index, text); } ,
                                                            "select", [] (tgui::Tabs& tabs, std::size_t index) { tabs.select(index); },
                                                            "getText", [] (tgui::Tabs& tabs, std::size_t index) -> std::string { return tabs.getText(index); },
                                                            "getSelectedIndex", &tgui::Tabs::getSelectedIndex,
                                                            sol::base_classes, sol::bases<tgui::Widget>());

            //tiled background
            state.registerFunction("createTiledBackground", []() { return tgui::TiledBackground::create(); });
            state.registerUsertype<tgui::TiledBackground>("TiledBackground",
                                                            "setSize", sol::overload( [] (tgui::TiledBackground& background, const sf::Vector2f& s) { background.setSize(s); },
                                                                                      [] (tgui::TiledBackground& background, float x, float y) { background.setSize({x,y}); },
                                                                                      [] (tgui::TiledBackground& background, const tgui::Layout2d& layout) { background.setSize(layout); }),

                                                            "setPosition", sol::overload( [] (tgui::TiledBackground& background, const sf::Vector2f& pos) { background.setPosition(pos); },
                                                                                          [] (tgui::TiledBackground& background, float x, float y) { background.setPosition({x,y}); },
                                                                                          [] (tgui::TiledBackground& background, const tgui::Layout2d& layout) { background.setPosition(layout); }),
                                                            sol::base_classes, sol::bases<tgui::Widget>());

            //child_window
            state.registerFunction("createChildWindow", []() { return tgui::ChildWindow::create(); });
            state.registerEnum("TitleAlignment",
                                         "Left", tgui::ChildWindow::TitleAlignment::Left,
                                         "Center", tgui::ChildWindow::TitleAlignment::Center,
                                         "Right", tgui::ChildWindow::TitleAlignment::Right);
            state.registerUsertype<tgui::ChildWindow>("ChildWindow",
                                                            "setPosition", sol::overload( [] (tgui::ChildWindow& childwindow, const sf::Vector2f& pos) { childwindow.setPosition(pos); },
                                                                                          [] (tgui::ChildWindow& childwindow, float x, float y) { childwindow.setPosition({x,y}); },
                                                                                          [] (tgui::ChildWindow& childwindow, const tgui::Layout2d& layout) { childwindow.setPosition(layout); }),
                                                            "setSize", sol::overload( [] (tgui::ChildWindow& childwindow, const sf::Vector2f& s) { childwindow.setSize(s); },
                                                                                      [] (tgui::ChildWindow& childwindow, float x, float y) { childwindow.setSize({x,y}); },
                                                                                      [] (tgui::ChildWindow& childwindow, const tgui::Layout2d& layout) { childwindow.setSize(layout); }),
                                                            "setMinimumSize", &tgui::ChildWindow::setMinimumSize ,
                                                            "setMaximumSize", &tgui::ChildWindow::setMaximumSize ,
                                                            "setTitle", [] (tgui::ChildWindow& childwindow, const std::string& title) { childwindow.setTitle(title); } ,
                                                            "setTitleAlignment", &tgui::ChildWindow::setTitleAlignment ,
                                                            "setResizable", &tgui::ChildWindow::setResizable ,
                                                            "getFullSize", &tgui::ChildWindow::getFullSize,
                                                            sol::base_classes, sol::bases<tgui::Container, tgui::Widget>());
        }
    }
}
