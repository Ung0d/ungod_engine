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

#ifndef UNGOD_SCOPED_ACCESSOR_H
#define UNGOD_SCOPED_ACCESSOR_H

#include <functional>

namespace ungod
{
    template<typename DATA>
    class ScopedAccessor;

    namespace detail
    {
        template<typename DATA>
        class ScopedInitializer
        {
        friend class ScopedAccessor<DATA>;
        public:
            ScopedInitializer(DATA& data, const std::function<void()>& callback) : mData(data), mCallback(callback) {}

        private:
            DATA& mData;
            std::function<void()> mCallback;
        };
    }

    /** \brief An object that wraps a reference to some arbitrary data shall be accessed and that may be
    * altered in some way, together with the reference to a callback, that is invoked automatically, after the
    * scope of the accessor is left (that is, when this accessor is destroyed).
    * WARNING: This accessor is no meant to be stored permanently in any case. Its a pure local object, that is
    * returned, grants access to the data to modify it and emit a data changed signal upon its destruction (or something like that)). */
    template<typename DATA>
    class ScopedAccessor final
    {
    public:
        ScopedAccessor(detail::ScopedInitializer<DATA> init) : mData(init.mData), mCallback(init.mCallback) {}

        ScopedAccessor(const ScopedAccessor<DATA>&) = delete;
        ScopedAccessor<DATA>& operator=(const ScopedAccessor<DATA>&) = delete;

        DATA* operator->() const { return &mData; }

        void setData(const DATA& data) { mData = data; }

        ~ScopedAccessor() { mCallback(); }

    private:
        DATA& mData;
        std::function<void()> mCallback;
    };
}

#endif // UNGOD_SCOPED_ACCESSOR_H
