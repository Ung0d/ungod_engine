/**
* Action - A convenient single-file and header only implementation of the Action-pattern with modern c++.
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

#ifndef Action_H
#define Action_H

#include <deque>
#include <memory>
#include <tuple>

namespace command
{
    /** \brief A manager-object for Actions that works as a stack holding a predefined number of
    * Actions. The manager offers the possibility to undo the upmost operation on the stack. */
    class ActionManager
    {
    public:
        ActionManager() : mStackSize(100), mUndoMaker(mActionStack.end()) {}

        /** \brief Executes a new action function with arbitrary parameters and stores it on top of
        * the internal stack along with a corresponding redo function, taking exactly the same parameters. */
        template<typename FUNC, typename ... PARAM>
        inline void execute( FUNC action, FUNC undo, PARAM&& ... param );

        /** \brief Undo the last performed Action. (the Action that is on top of the stack) */
        inline void undo();

        /** \brief Redo an action that was undo()ne before. */
        inline void redo();

    private:
        /** \brief A base for all editor Actions than can be executed and made undone. */
        class BaseAction
        {
        friend class ActionManager;
            private:
                /** \brief Invokes the action. */
                virtual void execute() = 0;

                /** \brief Undo the action. */
                virtual void undo() = 0;
        };

        template<typename ... PARAM>
        class Action : public BaseAction
        {
        private:
            std::tuple<PARAM...> mParam;
            std::function<void(PARAM ...)> mActionCallback;
            std::function<void(PARAM ...)> mUndoCallback;

            template<std::size_t ...I>
            void callAction(std::index_sequence<I...>);

            template<std::size_t ...I>
            void callUndo(std::index_sequence<I...>);

            virtual void execute() override;
            virtual void undo() override;

        public:
            Action(std::function<void(PARAM ...)> cAction, std::function<void(PARAM ...)> cUndo, PARAM&& ... param);
        };


        unsigned mStackSize; ///<indicates how deep the "memory" of the Action-manager is
        std::deque< std::unique_ptr<BaseAction> > mActionStack;
        std::deque< std::unique_ptr<BaseAction> >::iterator mUndoMaker;
    };


    template<typename FUNC, typename ... PARAM>
    void ActionManager::execute( FUNC action, FUNC undo, PARAM&& ... param )
    {
        mActionStack.erase(mUndoMaker, mActionStack.end());
        mActionStack.emplace_back(std::unique_ptr<BaseAction>( new Action<PARAM...>( std::function<void(PARAM ...)>(action),
                                                                                     std::function<void(PARAM ...)>(undo),
                                                                                     std::forward<PARAM>(param)...) ));
        mUndoMaker = mActionStack.end();
        mActionStack.back()->execute();
        if (mActionStack.size() > mStackSize)
            mActionStack.pop_front();
    }

    void ActionManager::undo()
    {
        if (mUndoMaker != mActionStack.begin()) //remember that begin == end when container is empty
        {
            mUndoMaker = std::prev(mUndoMaker);
            mUndoMaker->get()->undo();
        }
    }

    void ActionManager::redo()
    {
        if (mUndoMaker != mActionStack.end())
        {
            mUndoMaker->get()->execute();
            mUndoMaker = std::next(mUndoMaker);
        }
    }


    template<typename ... PARAM>
    ActionManager::Action<PARAM...>::Action(std::function<void(PARAM ...)> cAction,
                                            std::function<void(PARAM ...)> cUndo,
                                            PARAM&& ... param) :
                mParam( std::forward_as_tuple(std::forward<PARAM>(param)...) )
    {
        mActionCallback = cAction;
        mUndoCallback = cUndo;
    }

    template<typename ... PARAM>
    template<std::size_t ...I>
    void ActionManager::Action<PARAM...>::callAction(std::index_sequence<I...>)
    {
        mActionCallback(std::get<I>(mParam)...);
    }

    template<typename ... PARAM>
    template<std::size_t ...I>
    void ActionManager::Action<PARAM...>::callUndo(std::index_sequence<I...>)
    {
        mUndoCallback(std::get<I>(mParam)...);
    }

    template<typename ... PARAM>
    void ActionManager::Action<PARAM...>::execute()
    {
        callAction( std::index_sequence_for<PARAM...>{} );
    }

    template<typename ... PARAM>
    void ActionManager::Action<PARAM...>::undo()
    {
        callUndo( std::index_sequence_for<PARAM...>{} );
    }
}

#endif // Action_H
