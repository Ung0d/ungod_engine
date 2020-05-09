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

#ifndef ASSET_DATA_H
#define ASSET_DATA_H

#include <future>
#include <atomic>
#include <memory>
#include <string>
#include <list>
#include <atomic>

namespace ungod
{
    template<typename T, typename ... PARAM>
    class Asset;
    /**
    * \brief Databundle that refers to a specific asset on harddrive.
    */
    template<typename T, typename ... PARAM>
    struct AssetData
    {
		AssetData() : referenceCount(0u), isLoaded(false) {}

        //members
        std::unique_ptr<T> asset; ///< a unique-ownership ptr that stores the asset
        std::string filepath; ///< the path of the asset that functions as identifier
        std::atomic<unsigned> referenceCount; ///< counter for the assets that refer to the data
        bool isLoaded; ///< is true when the asset was successfully loaded
        std::future<void> future; ///< holds the result of a std::aLoadPolicy::SYNC call
        std::list< std::pair< const Asset<T, PARAM...>*, std::function <void (T&)> > > callbackStack; ///<stores callbacks while asset is loading
        std::mutex mutex; ///< used for syncronization
    };
}

#endif // ASSET_DATA_H
