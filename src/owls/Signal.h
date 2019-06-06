/*
* This is owls - a single file and header only framework for signals and event emitting.
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

#ifndef OWLS_H
#define OWLS_H

#include <functional>
#include <list>

namespace owls
{
    template <typename R, typename ... PARAM> class SignalLink;

    template<typename R, typename ... PARAM>  using Callback = std::function<R(PARAM...)>;
    template<typename R, typename ... PARAM>  using LinkCallback = std::function<R(SignalLink<R, PARAM...>, PARAM...)>;

    /** \brief A class that models a single callback which is attached to a signal.
    * The CallbackHandler implements ref counting to provide signal-reentrancy. */
    template<typename R, typename ... PARAM>
    class CallbackHandler
    {
    public:
        CallbackHandler(const Callback<R, PARAM...>& callback);

        void setCallback(const Callback<R, PARAM...>& callback);

        /** \brief Passes the arguments to the callback. */
        template<typename = std::enable_if<std::is_void<R>::value> >
        void operator()(PARAM... args);

        R operator()(PARAM... args);

        /** \brief Increments ref counter. */
        void refer();

        /** \brief Decrements ref counter. */
        void deref();

        /** \brief Returns ref counter. */
        unsigned refCount() const;

        /** \brief Disconnects this handler, that is, we will not use the Callback anymore. */
        void disconnect();

        /** \brief Returns connection status. */
        bool isConnected() const;

    private:
        Callback<R, PARAM...> mCallback;
        bool mDisconnected;
        unsigned mRefCount;
    };


    template <typename RETURN, typename ... PARAM> class SignalBase;

    /** \brief A linkage object between a signal and one of its callback. Is returned by
    * the connect method of Signal and can be used to disconnect later. */
    template<typename R, typename ... PARAM>
    class SignalLink
    {
    friend class SignalBase<R, PARAM...>;
    public:
        SignalLink() : mLink(nullptr) {}

        /** \brief Can be called to disconnect this linkage. The
        * callback attached at the connect-call, that returned this SignalLink
        * will be removed from the signals list. */
        void disconnect();

    private:
        SignalLink(CallbackHandler<R, PARAM...>* link);

    private:
        CallbackHandler<R, PARAM...>* mLink;
    };


    template<typename RETURN, typename ... PARAM>
    class SignalBase
    {
    public:
        /** \brief Connects a new callback. The returned SignalLink can be used to disconnect later. */
        SignalLink<RETURN, PARAM...> connect(const Callback<RETURN, PARAM...>& callback);

        /** \brief Connects a new (linked) callback. The returned SignalLink can be used to disconnect later. */
        SignalLink<RETURN, PARAM...> connect(const LinkCallback<RETURN, PARAM...>& callback);

        /** \brief Disconnects all callbacks. */
        void disconnectAll();

        std::size_t callbackCount() const;
    protected:
        std::list< CallbackHandler<RETURN, PARAM...> > mCallbacks;
    };


    /** \brief A signal that can be connected with callback functions.
    * The signal itself has the ability to invoke an emit-call, that
    * passes its arguments to all connected callbacks.
    */
    template<typename ... PARAM>
    class Signal : public SignalBase<void, PARAM...>
    {
    public:
        Signal() {}
        Signal(const Signal& other) = delete;

        /** \brief Emits the given parameters to all connected callbacks. */
        void emit(PARAM... param);

        /** \brief Emits the given parameters to all connected callbacks. */
        void operator()(PARAM... args);
    };

    template<typename T>
    class Request;

    /** \brief A request works like a signal, but can handle return values. This is
    * done by providing an Evaluator function, that generates a "order" on the return values.
    * The request always returns the "greatest" return value according to the Evaluator. */
    template<typename RETURN, typename ... PARAM>
    class Request<RETURN(PARAM...)> : public SignalBase<RETURN, PARAM...>
    {
    public:
        Request() {}
        Request(const Request& other) = delete;

        /** \brief Emits the given parameters to all connected callbacks. */
        template<typename EVALUATOR>
        void emit(EVALUATOR e, PARAM... param);

        /** \brief Emits the given parameters to all connected callbacks. */
        template<typename EVALUATOR>
        void operator()(EVALUATOR e, PARAM... args);
    };



    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////IMPLEMENTATION////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename R, typename ... PARAM>
    CallbackHandler<R, PARAM...>::CallbackHandler(const Callback<R, PARAM...>& callback) :
        mCallback(callback), mDisconnected(false), mRefCount(0) {}

    template<typename R, typename ... PARAM>
    void CallbackHandler<R, PARAM...>::setCallback(const Callback<R, PARAM...>& callback)
    {
        mCallback = callback;
    }

    template<typename R, typename ... PARAM>
    template<typename>
    void CallbackHandler<R, PARAM...>::operator()(PARAM... param)
    {
        mCallback( param... );
    }

    template<typename R, typename ... PARAM>
    R CallbackHandler<R, PARAM...>::operator()(PARAM... param)
    {
        return mCallback( param... );
    }

    template<typename R, typename ... PARAM>
    void CallbackHandler<R, PARAM...>::refer()
    {
        ++mRefCount;
    }

    template<typename R, typename ... PARAM>
    void CallbackHandler<R, PARAM...>::deref()
    {
        --mRefCount;
    }

    template<typename R, typename ... PARAM>
    unsigned CallbackHandler<R, PARAM...>::refCount() const
    {
        return mRefCount;
    }

    template<typename R, typename ... PARAM>
    void CallbackHandler<R, PARAM...>::disconnect()
    {
        mDisconnected = true;
    }

    template<typename R, typename ... PARAM>
    bool CallbackHandler<R, PARAM...>::isConnected() const
    {
        return !mDisconnected;
    }



    template<typename R, typename ... PARAM>
    SignalLink<R, PARAM...>::SignalLink(CallbackHandler<R, PARAM...>* link) : mLink(link) {}

    template<typename R, typename ... PARAM>
    void SignalLink<R, PARAM...>::disconnect()
    {
        if (mLink)
        {
            mLink->disconnect();
            mLink = nullptr;
        }

    }



    template<typename RETURN, typename ... PARAM>
    SignalLink<RETURN, PARAM...> SignalBase<RETURN, PARAM...>::connect(const Callback<RETURN, PARAM...>& callback)
    {
        mCallbacks.emplace_back(callback);
        return SignalLink<RETURN, PARAM...>( &(*std::prev(mCallbacks.end())) );
    }

    template<typename RETURN, typename ... PARAM>
    SignalLink<RETURN, PARAM...> SignalBase<RETURN, PARAM...>::connect(const LinkCallback<RETURN, PARAM...>& callback)
    {
        mCallbacks.emplace_back(nullptr);
        auto link = &(*std::prev(mCallbacks.end()));
        link->setCallback( [callback, link](PARAM ... param) { callback(link, param...); } );
        return SignalLink<RETURN, PARAM...>( link );
    }

    template<typename RETURN, typename ... PARAM>
    void SignalBase<RETURN, PARAM...>::disconnectAll()
    {
        for (auto& cb : mCallbacks)  cb.disconnect();
    }

    template<typename RETURN, typename ... PARAM>
    std::size_t SignalBase<RETURN, PARAM...>::callbackCount() const
    {
        return mCallbacks.size();
    }


    template<typename ... PARAM>
    void Signal<PARAM...>::emit(PARAM... param)
    {
        for (auto cb = this->mCallbacks.begin(); cb != this->mCallbacks.end();)
        {
            if (cb->isConnected())
            {
                cb->refer();
                (*cb)( param... );
                cb->deref();
            }
            if (!cb->isConnected() && cb->refCount() == 0)
                cb = this->mCallbacks.erase(cb);
            else
                ++cb;
        }
    }

    template<typename ... PARAM>
    void Signal<PARAM...>::operator()(PARAM... param)
    {
        emit(param... );
    }




    template<typename RETURN, typename ... PARAM>
    template<typename EVALUATOR>
    void Request<RETURN(PARAM...)>::emit(EVALUATOR e, PARAM... param)
    {
        for (auto cb = this->mCallbacks.begin(); cb != this->mCallbacks.end();)
        {
            if (cb->isConnected())
            {
                cb->refer();
                e( (*cb)( param... ) );
                cb->deref();
            }
            if (!cb->isConnected() && cb->refCount() == 0)
                cb = this->mCallbacks.erase(cb);
            else
                ++cb;
        }
    }

    template<typename RETURN, typename ... PARAM>
    template<typename EVALUATOR>
    void Request<RETURN(PARAM...)>::operator()(EVALUATOR e, PARAM... param)
    {
        emit(e, param... );
    }
}

#endif // OWLS_H
