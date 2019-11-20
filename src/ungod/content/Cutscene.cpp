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
            //LoadPolicy::SYNC load the data
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


    Scene::Scene(unsigned duration) : mDuration(duration)
    {
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
        target.draw(mText, states);
    }


    void Scene::setText(const std::string& text)
    {
        mText.setString(text);
    }


    void Scene::setTextPosition(const sf::Vector2f& percentagePos)
    {
        mTextPosition = percentagePos;
    }


    void Scene::setTextFillColor(const sf::Color& color)
    {
        mText.setFillColor(color);
    }


    void Scene::setTextOutlineColor(const sf::Color& color)
    {
        mText.setOutlineColor(color);
    }


    void Scene::setTextSize(unsigned textsize)
    {
        mText.setCharacterSize(textsize);
    }


    Cutscene::Cutscene() : mPlaying(false), mCurrent(0), mFadingEnabled(true)
    {
        mBlackScreen.setFillColor(sf::Color::Black);
    }


    Scene& Cutscene::addScene(unsigned duration)
    {
        mScenes.emplace_back(duration);
        mScenes.back().mText.setFont(mFont);
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

        bool status = false;

        if ((unsigned)mTimer.getElapsedTime().asMilliseconds() >= mScenes[mCurrent].mDuration)
        {
            if (mCurrent == mScenes.size()-1) //end of the cutscene
            {
                mCurrent++;
                mPlaying = false;
                    status = true;
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
                    status = true;
                }
            }
        }

        //calculate the transparency value for the scene (for fadein and fadeout effects)
        float alpha = 1.0f;
        if ((unsigned)mTimer.getElapsedTime().asMilliseconds() < mFadingDuration)
            alpha = (float)mTimer.getElapsedTime().asMilliseconds()/mFadingDuration;
        else if ((unsigned)mTimer.getElapsedTime().asMilliseconds() > mScenes[mCurrent].mDuration - mFadingDuration)
            alpha = ((float)mScenes[mCurrent].mDuration - (float)mTimer.getElapsedTime().asMilliseconds())/mFadingDuration;
        sf::Color blackScreenCol = sf::Color::Black;
        blackScreenCol.a = (sf::Uint8)(255*(1-alpha));
        mBlackScreen.setFillColor(blackScreenCol);

        return status;
    }


    void Cutscene::render(sf::RenderTarget& target, sf::RenderStates states) const
    {
        sf::View stor = target.getView();
        target.setView(target.getDefaultView());
        if (!mPlaying)
            return;
        mScenes[mCurrent].render(target, states);
        if (mFadingEnabled)
            target.draw(mBlackScreen, states);
        target.setView(stor);
    }


    void Cutscene::setScreenSize(const sf::Vector2u& screensize)
    {
        mScreenSize = screensize;
        mBlackScreen.setSize({(float)mScreenSize.x, (float)mScreenSize.y});
    }


    void Cutscene::setTextures()
    {
        const detail::SceneData& data = mDataLoader.getCurrent();
        for (unsigned i = 0; i < data.textures.size(); i++)
        {
            mScenes[mCurrent].mLayers[i].first.setTexture(data.textures[i]);
        }
        for (auto& layer : mScenes[mCurrent].mLayers)
        {
            sf::Vector2f scaling;
            scaling.x = mScreenSize.x / layer.first.getLocalBounds().width;
            scaling.y = mScreenSize.y / layer.first.getLocalBounds().height;
            float scale = (scaling.x+scaling.y)/2;
            layer.first.setScale({scale, scale});
            layer.first.setPosition({-(layer.first.getLocalBounds().width*scale-mScreenSize.x)/2, -(layer.first.getLocalBounds().height*scale-mScreenSize.y)/2});
        }
        mScenes[mCurrent].mText.setPosition({mScreenSize.x*mScenes[mCurrent].mTextPosition.x, mScreenSize.y*mScenes[mCurrent].mTextPosition.y});
    }


    void Cutscene::loadFont(const std::string& font)
    {
        mFont.loadFromFile(font);
    }
}
