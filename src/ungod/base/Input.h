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

#include <SFML/Window/Event.hpp>
#include <list>
#include <unordered_set>
#include "owls/Signal.h"
#include "ungod/base/Utility.h"
#include "ungod/serialization/MetaNode.h"
#include "ungod/base/Entity.h"

namespace ungod
{
    /** \brief A struct holding information about a key binding. */
    struct KeyBinding
    {
        std::string key;
        int input;
        int code;
        bool down;

        KeyBinding(const std::string& k, int i, int c) : key(k), input(i), code(c), down(false) {}
    };

    /** \brief A class that manages player input. */
    class InputHandler
    {
    public:
        enum InputType {MOUSE_INPUT = 0, KEYBOARD_INPUT = 1};

        /** \brief Default constructor. */
        InputHandler() {}

        /** \brief Evaluates an input-event and sends out signals. */
        void handleEvent(const sf::Event& event);

        /** \brief Emits signals for keys that are currently pressed every frame. */
        void update();

        /** \brief Loads information from the given XML-node and stores it in the
        * bindings map. Attributes of the node are assumed to be keys, values of the attributes
        * are pairs of integers refering to the input device and a specific key.
        * This does not unload existing bindings. */
        void loadBindings(MetaNode node);

        /** \brief Clears all currently loaded bindings. */
        void clearBindings();

        /** \brief Manually adds a binding. */
        void addBinding(const std::string& key, InputType input, int code);

        /**
        * \brief Helper method, that returns for a given input type and key value, whether it
        * can be used as a key binding.
        */
        static bool canBind(const InputType inputType, int keyValue);

        /** \brief Registers a callback. */
        void onPressed(const std::function<void(const std::string&)>& callback);

        /** \brief Registers a callback. */
        void onDown(const std::function<void(const std::string&)>& callback);

        /** \brief Registers a callback. */
        void onReleased(const std::function<void(const std::string&)>& callback);

    protected:
        std::list< KeyBinding > mBindings;

        owls::Signal< std::string > mPressedSignal;
        owls::Signal< std::string > mDownSignal;
        owls::Signal< std::string > mReleasedSignal;

    private:
        /**
        * \brief Helper function for internal usage. Ties a string, describing a
        * pair of inputType and keyID into a pair of integer values.
        */
        static std::pair<int, int> tieStringDesc(const std::string& desc);

        /**
        * \brief Helper method that takes a inputType and a enum value and
        * maps that to a string that describes the underlying key/button.
        */
        static std::string bindingAsString(std::pair<int, int> binding);
    };

    /** \brief Helper class to maintain a swapable double-buffer of entities. */
    struct Doublebuffer
    {
        std::array< std::unordered_set< Entity >, 2 > entities;
        bool swapper;

        Doublebuffer() : swapper(true) {}

        void processMousePos(int x, int y, const sf::RenderTarget& target, quad::QuadTree<Entity>& quadtree, owls::Signal<Entity>& enter, owls::Signal<Entity>& exit);

        void clearBuffers();
    };

    /** \brief Sets world and entity functionality on top of the InputHandler like entity hovered signals. */
    class InputManager : public InputHandler
    {
    public:
        InputManager(quad::QuadTree<Entity>& quadtree) : InputHandler(), mQuadtree(quadtree) {}

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

        Doublebuffer mHoveredEntities;
        Doublebuffer mClickedEntities;

        owls::Signal<Entity> mMouseEnterSignal;
        owls::Signal<Entity> mMouseClickedSignal;
        owls::Signal<Entity> mMouseExitSignal;
        owls::Signal<Entity> mMouseReleasedSignal;
    };
}

#endif // INPUT_H
