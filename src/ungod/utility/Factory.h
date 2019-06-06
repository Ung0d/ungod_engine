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

#ifndef UNGOD_FACTORY_H
#define UNGOD_FACTORY_H

#include <memory>
#include <unordered_map>

namespace ungod
{
    /** \brief Base creator object for all sorts of objects. */
    template<typename BASE, typename ... PARAM>
    class Creator
    {
    public:
       virtual std::unique_ptr<BASE> create(PARAM&& ... param) = 0;
    };

    /** \brief A creator object for a concrete type. */
    template <typename BASE, typename T, typename ... PARAM>
    class CreatorImpl : public Creator<BASE>
    {
    public:
       virtual std::unique_ptr<BASE> create(PARAM&& ... param) { return std::unique_ptr<BASE>( new T(std::forward<PARAM>(param)...) ); }
    };


    /** \brief Constructs objects by string ids. */
    template<typename BASE, typename ... PARAM>
    class Factory
    {
    public:
       std::unique_ptr<BASE> create(const std::string& classname, PARAM&& ... param)
       {
           auto f = mTable.find(classname);
           if (f != mTable.end())
                return f->second->create(std::forward<PARAM>(param)...);
            else
                return std::unique_ptr<BASE>{};
       }

       template <typename T, typename ... PARAM>
       inline void registerCreator(const std::string& classname)
       {
           mTable.emplace(classname, std::unique_ptr< Creator<BASE, PARAM...> >( new CreatorImpl<BASE, T, PARAM...>() ));
       }

    private:
       std::unordered_map<std::string, std::unique_ptr< Creator<BASE, PARAM...> >> mTable;
    };
}

#endif // UNGOD_FACTORY_H
