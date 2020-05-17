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

#include "ungod/audio/MusicEmitter.h"
#include "ungod/base/Transform.h"
#include "ungod/physics/Physics.h"
#include "quadtree/QuadTree.h"
#include "ungod/base/Entity.h"

namespace ungod
{
    MusicEmitterComponent::MusicEmitterComponent() : mDistanceCap(DEFAULT_DISTANCE_CAP), mVolume(1.0f), mActive(true), mBound(false) {}

    MusicEmitterComponent::MusicEmitterComponent(const MusicEmitterComponent& other) : 
        mDistanceCap(other.mDistanceCap), mVolume(other.mVolume), mActive(other.mActive), mBound(other.mBound)
    {
        if (other.mMusicData.loaded)
        {
            mMusicData.filepath = other.mMusicData.filepath;
            mMusicData.loaded = other.mMusicData.loaded;
            mMusicData.music.openFromFile(mMusicData.filepath);
            mMusicData.music.setLoop(true);
        }
    }

    std::string MusicEmitterComponent::getFilePath() const
    {
        return mMusicData.filepath;
    }


    float MusicEmitterComponent::getDistanceCap() const
    {
        return mDistanceCap;
    }


    float MusicEmitterComponent::getVolume() const
    {
        return mVolume;
    }


    bool MusicEmitterComponent::isActive() const
    {
        return mActive;
    }


    MusicEmitterMixer::MusicEmitterMixer() : 
        mMaxDistanceCap(DEFAULT_MAX_DISTANCE_CAP), mMuteSound(false)
    {
        for (unsigned i = 0; i < MUSIC_PLAY_CAP; i++)
        {
            mCurrentlyPlaying[i] = std::make_pair<MusicEmitterComponent*, TransformComponent*>(nullptr, nullptr);
        }
    }


    void MusicEmitterMixer::init(const AudioListener* listener)
    {
        mListener = listener;
    }


    void MusicEmitterMixer::loadMusic(Entity e, const std::string& fileID)
    {
        auto& data = e.modify<MusicEmitterComponent>().mMusicData;
        data.loaded = data.music.openFromFile(fileID);
        if (data.loaded)
        {
            data.filepath = fileID;
            data.music.setLoop(true);
        }
    }

    void MusicEmitterMixer::setDistanceCap(Entity e, float cap)
    {
        e.modify<MusicEmitterComponent>().mDistanceCap = cap;
    }

    void MusicEmitterMixer::setEmitterActive(Entity e, bool active)
    {
        e.modify<MusicEmitterComponent>().mActive = active;
        if (!active)
        {
            for (unsigned i = 0; i < MUSIC_PLAY_CAP; i++)
            {
                if (mCurrentlyPlaying[i].first && mCurrentlyPlaying[i].first->mMusicData.loaded)
                {
                    mCurrentlyPlaying[i].first->mMusicData.music.stop();
                    mCurrentlyPlaying[i].first = nullptr;
                    mCurrentlyPlaying[i].second = nullptr;
                }
            }
        }
    }

    void MusicEmitterMixer::setEmitterVolume(Entity e, float vol)
    {
        e.modify<MusicEmitterComponent>().mVolume = vol;
    }

    void MusicEmitterMixer::muteAll()
    {
        //update volumes of active emitters, may remove them
        for (unsigned i = 0; i < MUSIC_PLAY_CAP; i++)
        {
            if (!mCurrentlyPlaying[i].first)
                continue;

            if (mCurrentlyPlaying[i].first->mMusicData.loaded)
            {
                mCurrentlyPlaying[i].first->mMusicData.music.stop();
            }
            mCurrentlyPlaying[i].first->mBound = false;
            mCurrentlyPlaying[i].first = nullptr;
            mCurrentlyPlaying[i].second = nullptr;
        }
    }

    void MusicEmitterMixer::setMuteSound(bool mute)
    {
        mMuteSound = mute;
        muteAll();
    }

    void MusicEmitterMixer::update(float delta, quad::QuadTree<Entity>* quadtree)
    {
        if (mMuteSound)
            return;

        //check if new emitters are in range
        quad::PullResult<Entity> result;

        sf::Vector2f listenerWorldPos = mListener->getWorldPosition();
        quadtree->retrieve(result, { listenerWorldPos.x-mMaxDistanceCap*0.5f, listenerWorldPos.y-mMaxDistanceCap*0.5f, mMaxDistanceCap, mMaxDistanceCap });

        dom::Utility<Entity>::iterate<TransformComponent, MusicEmitterComponent>(result.getList(),
              [this, listenerWorldPos] (Entity e, TransformComponent& transf, MusicEmitterComponent& emitter)
              {
                  if (emitter.mActive && !emitter.mBound && emitter.mMusicData.loaded)
                  {
                      float dist = distance(transf.getCenterPosition(), listenerWorldPos);
                      if (dist < emitter.mDistanceCap)
                      {
                            //check if there is a free slot or a slot with greater distance
                            float maxdist = dist;
                            int indexToUse = -1;
                            for (unsigned i = 0; i < MUSIC_PLAY_CAP; i++)
                            {
                                if (!mCurrentlyPlaying[i].first) //free slot found
                                {
                                    indexToUse = (int)i;
                                    break;
                                }
                                else
                                {
                                    float distother = distance(mCurrentlyPlaying[i].second->getCenterPosition(), listenerWorldPos);
                                    if (distother > maxdist)
                                    {
                                        maxdist = distother;
                                        indexToUse = (int)i;
                                    }
                                }
                            }
                            if (indexToUse >= 0)
                            {
                                if (mCurrentlyPlaying[indexToUse].first)
                                {
                                    if (mCurrentlyPlaying[indexToUse].first->mMusicData.loaded)
                                        mCurrentlyPlaying[indexToUse].first->mMusicData.music.stop();
                                    mCurrentlyPlaying[indexToUse].first->mBound = false;
                                    mCurrentlyPlaying[indexToUse].first = nullptr;
                                    mCurrentlyPlaying[indexToUse].second = nullptr;
                                }
                                mCurrentlyPlaying[indexToUse].first = &emitter;
                                mCurrentlyPlaying[indexToUse].second = &transf;
                                mCurrentlyPlaying[indexToUse].first->mBound = true;
                                float scaling = mListener->getScaling(mCurrentlyPlaying[indexToUse].second->getCenterPosition() , mCurrentlyPlaying[indexToUse].first->mDistanceCap);
                                mCurrentlyPlaying[indexToUse].first->mMusicData.music.setVolume( 100.0f * mCurrentlyPlaying[indexToUse].first->mVolume * scaling );
                                mCurrentlyPlaying[indexToUse].first->mMusicData.music.play();
                            }
                      }
                  }
              });


        //update volumes of active emitters, may remove them
        for (unsigned i = 0; i < MUSIC_PLAY_CAP; i++)
        {
            if (!mCurrentlyPlaying[i].first)
                continue;

            if ( distance(mCurrentlyPlaying[i].second->getCenterPosition(), mListener->getWorldPosition()) > mCurrentlyPlaying[i].first->mDistanceCap )
            {
                if (mCurrentlyPlaying[i].first->mMusicData.loaded)
                    mCurrentlyPlaying[i].first->mMusicData.music.stop();
                mCurrentlyPlaying[i].first->mBound = false;
                mCurrentlyPlaying[i].first = nullptr;
                mCurrentlyPlaying[i].second = nullptr;
            }
            else if (mCurrentlyPlaying[i].first->mMusicData.loaded)
            {
                float scaling = mListener->getScaling(mCurrentlyPlaying[i].second->getCenterPosition() , mCurrentlyPlaying[i].first->mDistanceCap);
                mCurrentlyPlaying[i].first->mMusicData.music.setVolume( 100.0f * mCurrentlyPlaying[i].first->mVolume * scaling );
            }
        }
    }
}
