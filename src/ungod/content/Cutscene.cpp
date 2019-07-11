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

#include "ungod/content/Cutscene.h"
#include "ungod/base/Logger.h"

namespace ungod
{
    namespace detail
    {
        SceneDataLoader::SceneDataLoader() : mBufferingSwitch(false) {}

        const SceneData& SceneDataLoader::getCurrent() const
        {
            return mDataBuffers[mBufferingSwitch];
        }

        bool SceneDataLoader::isNextReady() const
        {
            return mDataBuffers[!mBufferingSwitch].status.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
        }

        void SceneDataLoader::loadFirst(const Scene* scene)
        {
            //sync load the data
            mBufferingSwitch = false;
            load(scene, mBufferingSwitch);
        }

        void SceneDataLoader::loadNext(const Scene* scene)
        {
            mDataBuffers[!mBufferingSwitch].status = std::async(std::launch::async, &SceneDataLoader::load, this, scene, !mBufferingSwitch);
        }

        void SceneDataLoader::load(const Scene* scene, bool bufferSwitch)
        {
            mDataBuffers[bufferSwitch].textures.clear();
            for (unsigned i = 0; i < scene->mLayers.size(); i++)
            {
                mDataBuffers[bufferSwitch].textures.emplace_back();
                mDataBuffers[bufferSwitch].textures.back().loadFromFile(scene->mLayers[i].second);
            }
        }
    }

    namespace CutsceneEffects
    {
         void LayerTransition::init(sf::BigSprite& sprite)
         {
            sprite.setPosition(mStartingPosition);
         }

         void LayerTransition::apply(sf::BigSprite& sprite, float delta)
         {
             sprite.move(mDirection.x*delta*mSpeed/20, mDirection.y*delta*mSpeed/20);
         }
    }

    void Scene::addLayer(const std::string& texture)
    {
        mLayers.emplace_back(sf::BigSprite{}, texture);
    }


    void Scene::applyEffects(float delta)
    {
        for (const auto& effect : mEffects)
        {
            if (effect.second < mLayers.size())
            {
                effect.first->apply(mLayers[effect.second].first, delta);
            }
            else
            {
                Logger::warning("Tried to apply an effect to a scene, that does not exist.");
                Logger::endl();
            }
        }
    }


    void Scene::render(sf::RenderTarget& target, sf::RenderStates states) const
    {
        for (const auto& layer : mLayers)
            target.draw(layer.first, states);
    }


    Scene& Cutscene::addScene(unsigned duration)
    {
        mScenes.emplace_back(duration);
        return mScenes.back();
    }


    void Cutscene::play()
    {
        if (mScenes.size() == 0)
        {
            Logger::warning("Tried to play a cutscene that has no scenes applied to it.");
            Logger::endl();
            return;
        }
        mDataLoader.loadFirst(&mScenes[0]);
        if (mScenes.size() > 1)
            mDataLoader.loadNext(&mScenes[1]);
        mTimer.restart();
        mCurrent = 0u;
        mPlaying = true;
        setTextures();
    }


    bool Cutscene::update(float delta)
    {
        if (!mPlaying)
            return false;

        mScenes[mCurrent].applyEffects(delta);

        if ((unsigned)mTimer.getElapsedTime().asMilliseconds() >= mScenes[mCurrent].mDuration)
        {
            if (mCurrent == mScenes.size()-1) //end of the cutscene
            {
                mCurrent++;
                mPlaying = false;
                    return true;
            }
            else
            {
                if (mDataLoader.isNextReady())
                {
                    mCurrent++;
                    mDataLoader.swapBuffers();
                    if (mCurrent < mScenes.size()-1)
                        mDataLoader.loadNext(&mScenes[mCurrent+1]);
                    setTextures();
                    mTimer.restart();
                    return true;
                }
            }
        }
        return false;
    }


    void Cutscene::render(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (!mPlaying)
            return;
        mScenes[mCurrent].render(target, states);
    }


    void Cutscene::setScreenSize(const sf::Vector2u& screensize)
    {
        for (auto& scene : mScenes)
        {
            for (auto& layer : scene.mLayers)
            {
                sf::Vector2f scaling;
                scaling.x = layer.first.getLocalBounds().width / screensize.x;
                scaling.y = layer.first.getLocalBounds().height / screensize.y;
                layer.first.setScale(scaling);
            }
        }
    }


    void Cutscene::setTextures()
    {
        const detail::SceneData& data = mDataLoader.getCurrent();
        for (unsigned i = 0; i < data.textures.size(); i++)
        {
            mScenes[mCurrent].mLayers[i].first.setTexture(data.textures[i]);
        }
    }
}
