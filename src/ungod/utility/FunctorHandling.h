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

#ifndef UNGOD_FUNCTOR_HANDLING_H
#define UNGOD_FUNCTOR_HANDLING_H

#include <memory>
#include <vector>
#include <unordered_map>
#include "ungod/serialization/Serializable.h"
#include "ungod/serialization/SerialFunctorHandle.h"

namespace ungod
{
    namespace detail
    {
        struct FunctorDataDeepBase
        {
            virtual std::unique_ptr<FunctorDataDeepBase> deepcpy() const = 0;

            template<typename T>
            T* as() { return static_cast<T*>(this); }

            template<typename T>
            T const* as() const { return static_cast<T const*>(this); }

			virtual ~FunctorDataDeepBase() {}
        };
    }

    template<typename ... PARAM>
    struct BaseFunctorData : public detail::FunctorDataDeepBase, public PolymorphicSerializable< BaseFunctorData<PARAM...>, PARAM... >
    {
    };

    #define FUNCTOR_DATA(class) std::unique_ptr<detail::FunctorDataDeepBase> deepcpy() const override { return std::unique_ptr<detail::FunctorDataDeepBase>(new class(*this)); }

    /** \brief A handle to a functor together with its custom data that is consumed by the functor. */
    struct FunctorHandle : public Serializable<FunctorHandle>
    {
        std::size_t id;
        std::unique_ptr<detail::FunctorDataDeepBase> data;

        FunctorHandle() = default;
        FunctorHandle(const FunctorHandle& other);
        FunctorHandle& operator=(const FunctorHandle& other);

        explicit operator bool() const { return static_cast<bool>(data.get()); }

        void reset() { data.reset(); }
    };

    template<typename RETURN, typename ... PARAM> using Functor = std::function<RETURN(detail::FunctorDataDeepBase&, PARAM&&...)>;

    /** \brief A set of functors, that can be invoked by handles. */
    template<typename RETURN, typename ... PARAM>
    class FunctorSet
    {
    private:
        using DataFactory = std::function< std::unique_ptr<detail::FunctorDataDeepBase>() >;

        std::vector< Functor<RETURN, PARAM...> > mFunctors;
        std::vector< std::string > mKeys;
        std::unordered_map< std::string, std::size_t> mIDMap;


    public:
        /** \brief Invokes a functor given a functor handle. */
        RETURN invoke(const FunctorHandle& handle, PARAM ... param) const;

        /** \brief Adds a new functor to the set which is identified by the given key.
        * F is a function with signature void(DATA&, PARAM...). */
        template<typename DATA, typename F>
        void addFunctor(const std::string& key, const F& f);

        template<typename DATA, typename ... DATA_PARAM>
        FunctorHandle makeHandle(const std::string& key, DATA_PARAM&& ... param) const;


        /** \brief Returns the string key for the given handle. */
        std::string getKey(const FunctorHandle& handle) const;
    };




    template<typename RETURN, typename ... PARAM>
    RETURN FunctorSet<RETURN, PARAM...>::invoke(const FunctorHandle& handle, PARAM ... param) const
    {
        return mFunctors[ handle.id ]( *handle.data, std::forward<PARAM>(param)... );
    }


    template<typename RETURN, typename ... PARAM>
    template<typename DATA, typename F>
    void FunctorSet<RETURN, PARAM...>::addFunctor(const std::string& key, const F& f)
    {
        mFunctors.emplace_back([f] (detail::FunctorDataDeepBase& data, PARAM&&... param) { return f(*data.as<DATA>(), std::forward<PARAM>(param)...); });
        mKeys.emplace_back(key);
        mIDMap.emplace(key, mFunctors.size()-1);
    }


    template<typename RETURN, typename ... PARAM>
    template<typename DATA, typename ... DATA_PARAM>
    FunctorHandle FunctorSet<RETURN, PARAM...>::makeHandle(const std::string& key, DATA_PARAM&& ... param) const
    {
        auto f = mIDMap.find(key);
        if (f != mIDMap.end())
        {
            FunctorHandle handle;
            handle.id = f->second;
            DATA* data = new DATA();
            data->init(std::forward<DATA_PARAM>(param)...);
            handle.data = std::unique_ptr<detail::FunctorDataDeepBase>( data );
            return handle;
        }
        else
        {
            return FunctorHandle{};
        }
    }

    template<typename RETURN, typename ... PARAM>
    std::string FunctorSet<RETURN, PARAM...>::getKey(const FunctorHandle& handle) const
    {
        if (handle)
            return mKeys[handle.id];
        else
            return {};
    }


    #include "ungod/serialization/SerialFunctorHandle.inl"
}

#endif // UNGOD_FUNCTOR_HANDLING_H
