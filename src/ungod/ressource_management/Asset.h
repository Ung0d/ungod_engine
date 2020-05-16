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

#ifndef ASSET_H
#define ASSET_H

#include "ungod/ressource_management/AssetHandler.h"

namespace ungod
{
    template<typename T, typename ... PARAM>
    struct AssetData;

    /** \brief A base class for all assets. */
    class BaseAsset
    {
    protected:
        static AssetManager* sManager; ///< provides access to the handler for assets of type T.

    public:
        /** \brief Sets the internal pointer to the AssetManager. */
        static void setManager(AssetManager& manager);
    };

    /** \brief Provides a interface for concrete assets that refer to a AssetData. */
    template<typename T, typename ... PARAM>
    class Asset : public BaseAsset
    {
    private:
        AssetData<T, PARAM...>* mData;

    protected:
		static T& getDefault();

    public:
        /** \brief Load constructor. */
        Asset(const std::string& filePath, const LoadPolicy policy, PARAM&& ... param);
        Asset(const std::string& filePath);

        /** \brief Deferred load constructor. Creates empty asset. */
        Asset();

        /** \brief Assignment operator for assets. Increments reference count. */
        Asset<T, PARAM...>& operator=(const Asset<T, PARAM...>& rightSide);

        /** \brief Copy constructor for assets. Increments reference count. */
        Asset(const Asset<T, PARAM...>& toCopy);

        /** \brief Loads a new asset data and drops the old one (if the asset was non-empty). */
        void load(const std::string filePath, PARAM&& ... param);

        /** \brief Loads a new asset data and drops the old one (if the asset was non-empty). */
        void load(const std::string filePath, const LoadPolicy policy, PARAM&& ... param);

        /**
        * \brief Drops the loaded asset. Decrements reference count and may unload the asset.
        */
        void drop();

        /** \brief Returns the filepath of the loaded object or "" if asset is empty. */
        std::string getFilePath() const;

        /**
        * \brief Returns a ptr to the loaded asset or the default asset if the asset is empty.
        * If the LoadPolicy was async, than this method will return the default asset as long as
        * the loading thread is working. Returns a const reference and follows read/write lock semantics.
        * Therefore: Call does never block by itself, but may be blocked by a modifing thread.
        */
        const T& get() const;

        /**
        * \brief Returns a ptr to the loaded asset or the default asset if the asset is empty.
        * If the LoadPolicy was async, than this method will wait if the thread loading the
        * asset is not ready yet. The return value is NEVER NULL. Returns a const reference and follows read/write lock semantics.
        * Therefore: Call does never block by itself, but may be blocked by a modifing thread.
        */
        const T& getWait() const;

        /** \brief Invokes a callback on the loaded asset. Invokes only if loading was successfully.
        * Will invoke the callback with the default asset if the asset is empty or broken.
        * If the LoadPolicy was async the callback is stored and invoked once the loading-thread has done its job.
        * Passes a const reference and follows read/write lock semantics.
        * Therefore: Call does never block by itself, but may be blocked by a modifing thread.
        */
        void get(std::function<void(const T&)> callback) const;

        /**
        * \brief Returns true if and only if the asset is loaded and ready to use. Will return false if
        * it was default constructed and load-method was never called, if the asset was dropped somehow or
        * if the asset is currently loading aLoadPolicy::SYNC.
        */
        bool isLoaded() const;

        /**
        *\brief Statically sets the handler for assets of type T. Must not be
        * called in normal circumstances. AssetManager class will call automatically
        * if a AssetManager is constructed.
        */
        static void instantiateHandler();
        /**
        * \brief Loads a default asset using the explicit LoadBehavior specialization for type T.
        */
        static bool loadDefault(const std::string& filepath);

        /** \brief Returns the number of linked instances. */
        unsigned getRefCount() const;

         /** \brief Destructor decrements the referencecount. */
        virtual ~Asset();
    };


	template <typename T, typename ... PARAM>
	T& Asset<T, PARAM...>::getDefault()
	{
		static T defT;
		return defT;
	}

    template <typename T, typename ... PARAM>
    Asset<T, PARAM...>& Asset<T, PARAM...>::operator=(const Asset<T, PARAM...>& rightSide)
    {
        mData = rightSide.mData;
        if (mData)
            mData->referenceCount++;
        return *this;
    }

    template <typename T, typename ... PARAM>
    Asset<T, PARAM...>::Asset(const Asset<T, PARAM...>& toCopy)
    {
        mData = toCopy.mData;
        if (mData)
            mData->referenceCount++;
    }

    template<typename T, typename ... PARAM>
    Asset<T, PARAM...>::Asset(const std::string& filePath, const LoadPolicy policy, PARAM&& ... param) :
        mData(nullptr)
    {
        load(filePath, policy, std::forward<PARAM>(param)...);
    }

    template<typename T, typename ... PARAM>
    Asset<T, PARAM...>::Asset(const std::string& filePath) :
        mData(nullptr)
    {
        load(filePath);
    }

    template<typename T, typename ... PARAM>
    Asset<T, PARAM...>::Asset() :
        mData(nullptr)
    {}

    template <typename T, typename ... PARAM>
    void Asset<T, PARAM...>::load(const std::string filePath, PARAM&& ... param)
    {
        load(filePath, LoadPolicy::SYNC, std::forward<PARAM>(param)...);
    }

    template <typename T, typename ... PARAM>
    void Asset<T, PARAM...>::load(const std::string filePath, const LoadPolicy policy, PARAM&& ... param)
    {
        AssetHandler<T, PARAM...>* handler = static_cast<AssetHandler<T, PARAM...>*>( sManager->getHandler<T, PARAM...>() );
        if (mData)
            handler->drop(mData, this);
        mData = handler->getData(filePath, policy, std::forward<PARAM>(param)... );
    }

    template <typename T, typename ... PARAM>
    void Asset<T, PARAM...>::drop()
    {
        AssetHandler<T, PARAM...>* handler = static_cast<AssetHandler<T, PARAM...>*>( sManager->getHandler<T, PARAM...>() );
        if (!mData) return;
        handler->drop(mData, this);
        mData = nullptr;
    }

    template<typename T, typename ... PARAM>
    std::string Asset<T, PARAM...>::getFilePath() const
    {
        if(mData) return mData->filepath;
        else return "";
    }

    template<typename T, typename ... PARAM>
    const T& Asset<T, PARAM...>::get() const
    {
        if (isLoaded())
        {
            return *mData->asset;
        }
        else return getDefault();
    }

    template<typename T, typename ... PARAM>
    const T& Asset<T, PARAM...>::getWait() const
    {
        if (mData)
        {
            mData->future.wait();
            return *mData->asset;
        }
        else return getDefault();
    }

    template<typename T, typename ... PARAM>
    void Asset<T, PARAM...>::get(std::function<void(const T&)> callback) const
    {
        if (mData)
        {
            if (mData->future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                if (mData->isLoaded)
                    callback(*mData->asset);
                else
                    callback(getDefault());
            }
            else
            {
                std::lock_guard<std::mutex> lg(mData->mutex);
                if (!mData->isLoaded)
                    mData->callbackStack.emplace_back( this, callback );
                else
                    callback(*mData->asset);
            }
        }
    }

    template <typename T, typename ... PARAM>
    bool Asset<T, PARAM...>::isLoaded() const
    {
        return mData && mData->future.wait_for(std::chrono::seconds(0)) == std::future_status::ready && mData->isLoaded;
    }

    template <typename T, typename ... PARAM>
    bool Asset<T, PARAM...>::loadDefault(const std::string& filepath)
    {
        return LoadBehavior<T, PARAM...>::loadFromFile(filepath, getDefault());
    }

    template <typename T, typename ... PARAM>
    unsigned Asset<T, PARAM...>::getRefCount() const
    {
        return mData->referenceCount;
    }

    template <typename T, typename ... PARAM>
    Asset<T, PARAM...>::~Asset()
    {
        drop();
    }
}

#endif // ASSET_H
