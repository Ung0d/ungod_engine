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

#ifndef ASSET_HANDLER_H
#define ASSET_HANDLER_H

#include <mutex>  
#include <shared_mutex>
#include <unordered_map>
#include "ungod/base/Logger.h"
#include "ungod/ressource_management/AssetData.h"
#include "ungod/ressource_management/LoadBehavior.h"

namespace ungod
{
    /** \brief Defines how an asset should be loaded. */
    enum class LoadPolicy : bool {  SYNC = true, ///<asset is loaded syncronously and can be used directly afterwarts
                              ASYNC = false ///<asset is loaded in another thread. If another thread tries to use that assets while its still loading, it will wait.
                            };

    /** \brief Base class for all handlers. */
    class Handler
    {
    private:
        const std::string typeStr;
        //_________________________________________________________________
        Handler(const Handler&) = delete;                  //dont allow copy-constructor
        Handler& operator=(const Handler&) = delete;       //dont allow assignment operator

    public:
        Handler(const std::string& cTypeStr) : typeStr(cTypeStr) {}
        const std::string getType() { return typeStr; }

        virtual void update() = 0;

        virtual ~Handler() {}
    };

    /**
    * \brief Interface for a concrete handler for a given asset type T. Maps filepaths
    * to AssetData-bundles. Users can use Asset<T> objects to retrieve and load
    * assets from the handler.
    */
    template<typename T, typename ... PARAM>
    class AssetHandler : public Handler
    {
    friend class Asset<T, PARAM...>;
    public:
        //constructor
        AssetHandler() : Handler(LoadBehavior<T, PARAM...>::getIdentifier()) {}

        virtual void update() override;

    private:
        std::unordered_map< std::string, std::shared_ptr<AssetData<T, PARAM...>> > mAssetData;  ///<stores loaded assets under fileID as key
        std::list< std::weak_ptr<AssetData<T, PARAM...>> > mPending; ///<stores asset data that is loading asyc and invokes callbacks on main thread if finished
        mutable std::shared_mutex mLoadDropMutex; ///< protects loading and dropping

    private:
        /**
        * \brief Tells the handler that an asset requires AssetData refering to the given path.
        * If its no loaded yet the data is loaded with the given policy.
        * Note that if policy is async, a pointer to "unfinished" data is returned
        * (a loading process starts in another thread).
        */
        AssetData<T, PARAM...>* getData(const std::string& path, const LoadPolicy policy, PARAM&& ... param);

        /**
        * \brief Tells the handler that an asset refering to the given AssetData was dropped.
        * If there is no asset left that refers to the data, the data is deleted.
        */
        void drop(AssetData<T, PARAM...>* data, Asset<T, PARAM...>* requester);

        /** \brief Factory method for an empty asset. */
        static std::unique_ptr<T> acquireAsset();

        /** \brief Async load utility method. */
        void asyncLoad(std::weak_ptr<AssetData<T, PARAM...>> weakData, const std::string filepath, PARAM&& ... param);

        /** \brief Sync load utility method. */
        void syncLoad(std::shared_ptr<AssetData<T, PARAM...>> data, const std::string filepath, PARAM&& ... param);

		~AssetHandler();
    };

    /** \brief Manages und bundles together the assetshandlers of all types.
    * There should be only one asset manager at a time. */
    class AssetManager
    {
    public:
        AssetManager();

        template<typename T, typename ... PARAM>
        AssetHandler<T, PARAM...>* getHandler();

        void update();

    private:
        std::vector< std::unique_ptr<Handler> > mHandlers;
        std::mutex mMutex;

        static std::atomic<std::size_t> sID;

        template<typename T, typename ... PARAM>
        struct AssetTypeTraits
        {
            static std::size_t getID()
            {
                static std::size_t id = sID++;
                return id;
            }
        };
    };

    template<typename T, typename ... PARAM>
    AssetHandler<T, PARAM...>* AssetManager::getHandler()
    {
        std::size_t id = AssetTypeTraits<T, PARAM...>::getID();
        if (id == mHandlers.size())
        {
            std::unique_lock<std::mutex> lock(mMutex);
            mHandlers.emplace_back(new AssetHandler<T, PARAM...>());
        }
        return static_cast<AssetHandler<T, PARAM...>*>( mHandlers[id].get() );
    }



    template<typename T, typename ... PARAM>
    void AssetHandler<T, PARAM...>::update()
    {
		for (auto data = mPending.begin(); data != mPending.end();)
		{
			if (data->expired())
				data = mPending.erase(data);
			else
			{
				auto shared = data->lock();
				if (shared->future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
				{
					while (!shared->callbackStack.empty()) //invoke callbacks
					{
						//pair contains a pointer to the requesting asset(first) and the callback(second)
						auto callback = std::move(shared->callbackStack.front());
						shared->callbackStack.pop_front();
						callback.second(*(shared->asset));
					}
					data = mPending.erase(data);
				}
				else
					data++;
			}
        }
    }


    template<typename T, typename ... PARAM>
    AssetData<T, PARAM...>* AssetHandler<T, PARAM...>::getData(const std::string& path, const LoadPolicy policy, PARAM&& ... param)
    { 
        {
            std::shared_lock lock(mLoadDropMutex);
            auto res = mAssetData.find(path);
            if (res != mAssetData.end())
            {
                res->second->referenceCount++; //atomic
                return res->second.get();
            }
        }
        //not loaded yet
        std::unique_lock lock(mLoadDropMutex);
        auto res = mAssetData.emplace(path, std::make_shared<AssetData<T, PARAM...>>());
        auto data = res.first->second;
        if (!res.second) //second check to prevent race conditions
        {
            data->referenceCount++; //atomic
            return data.get(); // when this return happens, the asset data is already in a prepared state (either fully loaded or loading with valid future)
        }
        data->referenceCount = 1;
        data->isLoaded = false;
        data->filepath = path;

        if (policy == LoadPolicy::SYNC)
        {
            Logger::info("Now sync loading asset ");
            Logger::info(path);
            Logger::endl();

            data->future = std::async(std::launch::deferred, &AssetHandler<T, PARAM...>::syncLoad,
                this, data, path, std::forward<PARAM>(param)...);
            data->future.wait();
        }
        else //load async
        {
            Logger::info("Now async loading asset ");
            Logger::info(path);
            Logger::endl();

            data->future = std::async(std::launch::async, &AssetHandler<T, PARAM...>::asyncLoad,
                this, std::weak_ptr<AssetData<T, PARAM...>>{data}, path, std::forward<PARAM>(param)...);

            mPending.emplace_back(data);
        }
        return data.get();
    }

    template<typename T, typename ... PARAM>
    void AssetHandler<T, PARAM...>::drop(AssetData<T, PARAM...>* data, Asset<T, PARAM...>* requester)
    {
        std::unique_lock lock(mLoadDropMutex);
        data->referenceCount--;
        for(auto callbackPair = data->callbackStack.begin();
            callbackPair != data->callbackStack.end();)
        {
            if(callbackPair->first == requester)
                callbackPair = data->callbackStack.erase(callbackPair);
            else
                ++callbackPair;
        }
        if(data->referenceCount == 0)
        {
            Logger::info("Now dropping asset ");
            Logger::info(data->filepath);
            Logger::endl();
			data->future.wait();
            mAssetData.erase(data->filepath);
        }
    }

    template<typename T, typename ... PARAM>
    std::unique_ptr<T> AssetHandler<T, PARAM...>::acquireAsset()
    {
        return std::make_unique<T>();
    }

    template<typename T, typename ... PARAM>
    void AssetHandler<T, PARAM...>::asyncLoad(std::weak_ptr<AssetData<T, PARAM...>> weakData, const std::string filepath, PARAM&& ... param)
    {
		auto emptyPtr = acquireAsset();
        bool success = LoadBehavior<T, PARAM...>::loadFromFile(filepath, *emptyPtr, std::forward<PARAM>(param)...);
        //todo: can this be dropped?
		if (weakData.expired()) return; //all refering assets dropped before loading was complete
		auto sharedData = weakData.lock();
        std::lock_guard<std::mutex> lg(sharedData->mutex);
        sharedData->asset = std::move(emptyPtr);
        sharedData->isLoaded = success;
        if (!success)
        {
            Logger::error("Could not load asset ");
            Logger::error(filepath);
            Logger::endl();
        }
    }

    template<typename T, typename ... PARAM>
    void AssetHandler<T, PARAM...>::syncLoad(std::shared_ptr<AssetData<T, PARAM...>> data, const std::string filepath, PARAM&& ... param)
    {
        data->asset = acquireAsset();
        data->isLoaded = LoadBehavior<T, PARAM...>::loadFromFile(filepath, *data->asset, std::forward<PARAM>(param)...);

        if (!data->isLoaded)
        {
            Logger::error("Could not load asset ");
            Logger::error(filepath);
            Logger::endl();
        }
    }

	template<typename T, typename ... PARAM>
	AssetHandler<T, PARAM...>::~AssetHandler()
	{
		//wait for every pending futures to avoid memory leaks
		for (const auto& data : mPending)
			if (!data.expired())
				data.lock()->future.wait();
	}
}

#endif // ASSET_HANDLER_H
