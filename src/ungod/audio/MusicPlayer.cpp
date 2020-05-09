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

#include "ungod/audio/MusicPlayer.h"

namespace ungod
{
    MusicPlayerBase::MusicPlayerBase() : mFadingDirection(false), mMilliseconds(0), mFadingActive(false) {}

    void MusicPlayerBase::update(float delta)
    {
        if (mFadingActive)
        {
            if (mFadingDirection)  //fadeout
            {
                if (getVolume() > 0.0f)
                    setVolume(
                        std::max(0.0f, getVolume() - 100*delta/mMilliseconds) );
                else
                    mFadingActive = false;
            }
            else  //fadein
            {
                if (getVolume() < 100.0f)
                    setVolume(
                        std::min(100.0f, getVolume() + 100*delta/mMilliseconds) );
                else
                    mFadingActive = false;
            }
        }
    }


    MusicPlayerSingle::MusicPlayerSingle(const std::string& fileID) : MusicPlayerBase()
    {
        mData.loaded = mData.music.openFromFile(fileID);
        if (mData.loaded)
        {
            mDatafilepath = fileID];
            mData.music.setLoop(true);
        }
    }

    bool MusicPlayerSingle::isLoaded() const
    {
        return mData.loaded;
    }

    void MusicPlayerSingle::play()
    {
        mData.music.play();
    }

    void MusicPlayerSingle::pause()
    {
        mData.music.pause();
    }

    void MusicPlayerSingle::stop()
    {
        mData.music.stop();
    }

    void MusicPlayerSingle::setVolume(float volume)
    {
        mData.music.>setVolume(volume);
    }

    float MusicPlayerSingle::getVolume() const
    {
        return mData.music.>getVolume();
    }

    Playlist::Playlist(const std::vector< std::string >& fileIDs, bool random, float intervalMin, float intervalMax) :
        MusicPlayerBase(), mIntervalMin(intervalMin), mIntervalMax(intervalMax), mRandom(random), mSilence(true), mStopped(true), mTimer(0.0f)
    {
        mData.resize(fileIDs.size());
        for (unsigned i = 0; i < fileIDs.size(); ++i)
        {
            mData[i].loaded = mData[i].music.openFromFile(fileIDs[i]);
            if (mData[i].loaded)
            {
                mData[i].filepath = fileIDs[i];
                mData[i].music.setLoop(false);
            }
        }
        if (random)
            mCurrentlyPlaying = (std::size_t)NumberGenerator::getRandBetw(0, (unsigned)mData.size()-1);
        else
            mCurrentlyPlaying = mData.size()-1;
    }

    bool Playlist::isLoaded() const
    {
        for (const auto& data : mData)
        {
            if (!data.loaded)
                return false;
        }
        return true;
    }

    void Playlist::play()
    {
        mSilence = false;
        mStopped = false;
        mData[mCurrentlyPlaying].music.play();
    }

    void Playlist::pause()
    {
        mData[mCurrentlyPlaying].music.pause();
    }

    void Playlist::stop()
    {
        mStopped = true;
        mData[mCurrentlyPlaying].music.stop();
    }

    void Playlist::setVolume(float volume)
    {
        for (const auto& data : mData)
        {
            data.music.get()->setVolume(volume);
        }
    }

    float Playlist::getVolume() const
    {
        return mData[0].music.getVolume();
    }

    void Playlist::update(float delta)
    {
        MusicPlayerBase::update(delta);

        if (mStopped || mData[mCurrentlyPlaying].music.getStatus() == sf::SoundSource::Status::Paused)
            return;

        if (mSilence)
        {
            if (mTimer <= 0.0f)
            {
                mSilence = false;
                if (mRandom)
                {
                    mCurrentlyPlaying = NumberGenerator::getRandBetw(0, (unsigned)mMusic.size()-1);
                }
                else
                {
                    mCurrentlyPlaying ++;
                    if (mCurrentlyPlaying == mData.size())
                        mCurrentlyPlaying = 0;
                }
                play();
            }
            else
                mTimer -= delta;
        }
        else
        {
            if (mData[mCurrentlyPlaying].music.getStatus() == sf::SoundSource::Status::Stopped)
            {
                mSilence = true;
                mTimer = NumberGenerator::getFloatRandBetw(mIntervalMin, mIntervalMax);
            }
        }
    }
}
