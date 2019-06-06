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
        mMusic.load(fileID);
        if (mMusic.isLoaded())
            mMusic.get()->setLoop(true);
    }

    bool MusicPlayerSingle::isLoaded() const
    {
        return mMusic.isLoaded();
    }

    void MusicPlayerSingle::play()
    {
        mMusic.get()->play();
    }

    void MusicPlayerSingle::pause()
    {
        mMusic.get()->pause();
    }

    void MusicPlayerSingle::stop()
    {
        mMusic.get()->stop();
    }

    void MusicPlayerSingle::setVolume(float volume)
    {
        mMusic.get()->setVolume(volume);
    }

    float MusicPlayerSingle::getVolume() const
    {
        return mMusic.get()->getVolume();
    }

    Playlist::Playlist(const std::vector< std::string >& fileIDs, bool random, float intervalMin, float intervalMax) :
        MusicPlayerBase(), mIntervalMin(intervalMin), mIntervalMax(intervalMax), mRandom(random), mSilence(true), mStopped(true), mTimer(0.0f)
    {
        mMusic.resize(fileIDs.size());
        for (unsigned i = 0; i < fileIDs.size(); ++i)
        {
            mMusic[i].load(fileIDs[i]);
            if (mMusic[i].isLoaded())
                mMusic[i].get()->setLoop(false);
        }
        mCurrentlyPlaying = mMusic.size()-1;
    }

    bool Playlist::isLoaded() const
    {
        for (const auto& music : mMusic)
        {
            if (!music.isLoaded())
                return false;
        }
        return true;
    }

    void Playlist::play()
    {
        mSilence = false;
        mStopped = false;
        mMusic[mCurrentlyPlaying].get()->play();
    }

    void Playlist::pause()
    {
        mMusic[mCurrentlyPlaying].get()->pause();
    }

    void Playlist::stop()
    {
        mStopped = true;
        mMusic[mCurrentlyPlaying].get()->stop();
    }

    void Playlist::setVolume(float volume)
    {
        for (const auto& music : mMusic)
        {
            music.get()->setVolume(volume);
        }
    }

    float Playlist::getVolume() const
    {
        return mMusic[0].get()->getVolume();
    }

    void Playlist::update(float delta)
    {
        MusicPlayerBase::update(delta);

        if (mStopped || mMusic[mCurrentlyPlaying].get()->getStatus() == sf::SoundSource::Status::Paused)
            return;

        if (mSilence)
        {
            if (mTimer <= 0.0f)
            {
                mSilence = false;
                if (mRandom)
                {
                    mCurrentlyPlaying = NumberGenerator::getRandBetw(0, mMusic.size()-1);
                }
                else
                {
                    mCurrentlyPlaying ++;
                    if (mCurrentlyPlaying == mMusic.size())
                        mCurrentlyPlaying = 0;
                }
                play();
            }
            else
                mTimer -= delta;
        }
        else
        {
            if (mMusic[mCurrentlyPlaying].get()->getStatus() == sf::SoundSource::Status::Stopped)
            {
                mSilence = true;
                mTimer = NumberGenerator::getFloatRandBetw(mIntervalMin, mIntervalMax);
            }
        }
    }
}
