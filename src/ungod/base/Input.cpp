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

namespace ungod
{
    void InputHandler::handleEvent(const sf::Event& event)
    {
        for (auto& b : mBindings)
        {
            switch(b.input)
            {
                case MOUSE_INPUT:
                {
                    if (event.type == sf::Event::MouseButtonPressed && b.code == event.mouseButton.button)
                    {
                        if (!b.down)
                        {
                            b.down = true;
                            mPressedSignal(b.key);
                        }
                    }
                    else if (event.type == sf::Event::MouseButtonReleased && b.code == event.mouseButton.button)
                    {
                         b.down = false;
                         mReleasedSignal(b.key);
                    }
                    break;
                }
                case KEYBOARD_INPUT:
                {
                    if (event.type == sf::Event::KeyPressed && b.code == event.key.code)
                    {
                        if (!b.down)
                        {
                            b.down = true;
                            mPressedSignal(b.key);
                        }
                    }
                    else if (event.type == sf::Event::KeyReleased && b.code == event.key.code)
                    {
                         b.down = false;
                         mReleasedSignal(b.key);
                    }
                    break;
                }
                default: break;
            }
        }
    }


    void InputHandler::update()
    {
        for (auto& b : mBindings)
            if (b.down) mDownSignal(b.key);
    }


    void InputHandler::loadBindings(MetaNode node)
    {
        forEachAttribute(node, [this] (MetaAttribute attr)
         {
            auto key = attr.name();
            auto val = tieStringDesc( attr.value() );
            if (canBind((InputType)val.first, val.second))
                mBindings.emplace_back( key, val.first, val.second );
         });
    }


    void InputHandler::clearBindings()
    {
        mBindings.clear();
    }


    void InputHandler::addBinding(const std::string& key, InputType input, int code)
    {
        if (canBind(input, code))
            mBindings.emplace_back( key, input, code );
    }


    std::pair<int, int> InputHandler::tieStringDesc(const std::string& desc)
    {
        std::vector<std::string> tiedBind(ungod::convertToVector(desc));
        try
        {
            if(tiedBind.size() != 2)
                return {-1,-1};
            return {std::stoi(tiedBind[0]), std::stoi(tiedBind[1])};
        }
        catch(const std::invalid_argument& ia)
        {
            Logger::info("Invalid argument occured during key-binding-validation. Cant perform action.");
            Logger::endl();
                return {-1,-1};
        }
        catch(const std::out_of_range& oor)
        {
                return {-1,-1};
        }
    }

    std::string InputHandler::bindingAsString(std::pair<int, int> binding)
    {
        switch(binding.first)
        {
            case MOUSE_INPUT:
            {
                switch(binding.second)
                {
                case sf::Mouse::Left:
                    return "M1";
                case sf::Mouse::Right:
                    return "M2";
                default:
                    return "";
                }
                break;
            }
            case KEYBOARD_INPUT:
            {
                switch(binding.second)
                {
                case sf::Keyboard::A:
                    return "A";
                case sf::Keyboard::B:
                    return "B";
                case sf::Keyboard::C:
                    return "C";
                case sf::Keyboard::D:
                    return "D";
                case sf::Keyboard::E:
                    return "E";
                case sf::Keyboard::F:
                    return "F";
                case sf::Keyboard::G:
                    return "G";
                case sf::Keyboard::H:
                    return "H";
                case sf::Keyboard::I:
                    return "I";
                case sf::Keyboard::J:
                    return "J";
                case sf::Keyboard::K:
                    return "K";
                case sf::Keyboard::L:
                    return "L";
                case sf::Keyboard::N:
                    return "N";
                case sf::Keyboard::M:
                    return "M";
                case sf::Keyboard::O:
                    return "O";
                case sf::Keyboard::P:
                    return "P";
                case sf::Keyboard::Q:
                    return "Q";
                case sf::Keyboard::R:
                    return "R";
                case sf::Keyboard::S:
                    return "S";
                case sf::Keyboard::T:
                    return "T";
                case sf::Keyboard::U:
                    return "U";
                case sf::Keyboard::V:
                    return "V";
                case sf::Keyboard::W:
                    return "W";
                case sf::Keyboard::X:
                    return "X";
                case sf::Keyboard::Y:
                    return "Y";
                case sf::Keyboard::Z:
                    return "Z";
                case sf::Keyboard::Num0:
                    return "0";
                case sf::Keyboard::Num1:
                    return "1";
                case sf::Keyboard::Num2:
                    return "2";
                case sf::Keyboard::Num3:
                    return "3";
                case sf::Keyboard::Num4:
                    return "4";
                case sf::Keyboard::Num5:
                    return "5";
                case sf::Keyboard::Num6:
                    return "6";
                case sf::Keyboard::Num7:
                    return "7";
                case sf::Keyboard::Num8:
                    return "8";
                case sf::Keyboard::Num9:
                    return "9";
                case sf::Keyboard::LAlt:
                    return "LAlt";
                case sf::Keyboard::RAlt:
                    return "RAlt";
                case sf::Keyboard::Space:
                    return "Space";
                case sf::Keyboard::Tab:
                    return "Tab";
                case sf::Keyboard::LControl:
                    return "LControl";
                case sf::Keyboard::RControl:
                    return "RControl";
                case sf::Keyboard::Right:
                    return "->";
                case sf::Keyboard::Left:
                    return "<-";
                case sf::Keyboard::Up:
                    return "^";
                case sf::Keyboard::Down:
                    return "v";
                case sf::Keyboard::F1:
                    return "F1";
                case sf::Keyboard::F2:
                    return "F2";
                case sf::Keyboard::F3:
                    return "F3";
                case sf::Keyboard::F4:
                    return "F4";
                case sf::Keyboard::F5:
                    return "F5";
                case sf::Keyboard::F6:
                    return "F6";
                case sf::Keyboard::F7:
                    return "F7";
                case sf::Keyboard::F8:
                    return "F8";
                case sf::Keyboard::F9:
                    return "F9";
                case sf::Keyboard::F10:
                    return "F10";
                case sf::Keyboard::F11:
                    return "F11";
                case sf::Keyboard::F12:
                    return "F12";
                default:
                    return "";
                }
                break;
            }
            default: break;
        }
        return "";
    }

    bool InputHandler::canBind(const InputType inputType, int keyValue)
    {
        switch(inputType)
        {
            case MOUSE_INPUT:
            {
                switch(keyValue)
                {
                case sf::Mouse::Left:
                case sf::Mouse::Right:
                    return true;
                default:
                    return false;
                }
                break;
            }
            case KEYBOARD_INPUT:
            {
                switch(keyValue)
                {
                case sf::Keyboard::A:
                case sf::Keyboard::B:
                case sf::Keyboard::C:
                case sf::Keyboard::D:
                case sf::Keyboard::E:
                case sf::Keyboard::F:
                case sf::Keyboard::G:
                case sf::Keyboard::H:
                case sf::Keyboard::I:
                case sf::Keyboard::J:
                case sf::Keyboard::K:
                case sf::Keyboard::L:
                case sf::Keyboard::N:
                case sf::Keyboard::M:
                case sf::Keyboard::O:
                case sf::Keyboard::P:
                case sf::Keyboard::Q:
                case sf::Keyboard::R:
                case sf::Keyboard::S:
                case sf::Keyboard::T:
                case sf::Keyboard::U:
                case sf::Keyboard::V:
                case sf::Keyboard::W:
                case sf::Keyboard::X:
                case sf::Keyboard::Y:
                case sf::Keyboard::Z:
                case sf::Keyboard::Num0:
                case sf::Keyboard::Num1:
                case sf::Keyboard::Num2:
                case sf::Keyboard::Num3:
                case sf::Keyboard::Num4:
                case sf::Keyboard::Num5:
                case sf::Keyboard::Num6:
                case sf::Keyboard::Num7:
                case sf::Keyboard::Num8:
                case sf::Keyboard::Num9:
                case sf::Keyboard::LAlt:
                case sf::Keyboard::RAlt:
                case sf::Keyboard::Space:
                case sf::Keyboard::Tab:
                case sf::Keyboard::LControl:
                case sf::Keyboard::RControl:
                case sf::Keyboard::Right:
                case sf::Keyboard::Left:
                case sf::Keyboard::Up:
                case sf::Keyboard::Down:
                case sf::Keyboard::F1:
                case sf::Keyboard::F2:
                case sf::Keyboard::F3:
                case sf::Keyboard::F4:
                case sf::Keyboard::F5:
                case sf::Keyboard::F6:
                case sf::Keyboard::F7:
                case sf::Keyboard::F8:
                case sf::Keyboard::F9:
                case sf::Keyboard::F10:
                case sf::Keyboard::F11:
                case sf::Keyboard::F12:
                    return true;
                default:
                    return false;
                }
                break;
            }
            default: break;
        }
        return false;
    }


    void InputHandler::onPressed(const std::function<void(const std::string&)>& callback)
    {
        mPressedSignal.connect(callback);
    }


    void InputHandler::onDown(const std::function<void(const std::string&)>& callback)
    {
        mDownSignal.connect(callback);
    }


    void InputHandler::onReleased(const std::function<void(const std::string&)>& callback)
    {
        mReleasedSignal.connect(callback);
    }


    void Doublebuffer::processMousePos(int x, int y, const sf::RenderTarget& target, quad::QuadTree<Entity>& quadtree, owls::Signal<Entity>& enter, owls::Signal<Entity>& exit)
    {
        //compute the actual world position of the mouse and pull entities that likely collide with that position
        sf::Vector2f mouseWorldPos = target.mapPixelToCoords( {x, y} );
        quad::PullResult<Entity> pull;
        quadtree.retrieve(pull, { mouseWorldPos.x, mouseWorldPos.y, 0.0f, 0.0f });

        dom::Utility<Entity>::iterate<TransformComponent>(pull.getList(),
        [this, mouseWorldPos] (Entity e, TransformComponent& transf)
        {
            if (transf.getBounds().contains( mouseWorldPos ))
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


    void InputManager::handleEvent(const sf::Event& event, const sf::RenderTarget& target)
    {
        InputHandler::handleEvent(event);
        processMouse(event, target);
    }


    void InputManager::processMouse(const sf::Event& event, const sf::RenderTarget& target)
    {
        switch (event.type)
        {
            case sf::Event::MouseMoved:
            {
                mHoveredEntities.processMousePos(event.mouseMove.x, event.mouseMove.y, target, mQuadtree, mMouseEnterSignal, mMouseExitSignal);
                break;
            }
            case sf::Event::MouseButtonPressed:
            {
                mClickedEntities.processMousePos(event.mouseButton.x, event.mouseButton.y, target, mQuadtree, mMouseClickedSignal, mMouseReleasedSignal);
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


    void InputManager::onMouseEnter(const std::function<void(Entity)>& callback)
    {
        mMouseEnterSignal.connect(callback);
    }


    void InputManager::onMouseClick(const std::function<void(Entity)>& callback)
    {
        mMouseClickedSignal.connect(callback);
    }


    void InputManager::onMouseExit(const std::function<void(Entity)>& callback)
    {
        mMouseExitSignal.connect(callback);
    }


    void InputManager::onMouseReleased(const std::function<void(Entity)>& callback)
    {
        mMouseReleasedSignal.connect(callback);
    }
}
