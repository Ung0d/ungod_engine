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

#ifndef UNGOD_MUSIC_PLAYER_H
#define UNGOD_MUSIC_PLAYER_H

#include <vector>
#include <SFML/Audio.hpp>
#include "ungod/serialization/Serializable.h"
#include "ungod/audio/Music.h"

namespace ungod
{
    class AudioManager;

/** \brief A base class for music players. */
    class MusicPlayerBase
    {
    friend class AudioManager;
     friend struct SerialBehavior<AudioManager>;
    public:
        MusicPlayerBase();

        virtual bool isLoaded() const = 0;
        virtual void play() = 0;
        virtual void pause() = 0;
        virtual void stop() = 0;
        virtual void setVolume(float volume) = 0;
        virtual float getVolume() const = 0;
        virtual void update(float delta);

    protected:
        bool mFadingDirection;
        float mMilliseconds;
        bool mFadingActive;;
    };


    /** \brief A music player that plays and repeats a single music file. */
    class MusicPlayerSingle : public MusicPlayerBase
    {
    public:
        /** \brief Constructor loads the music file. Does not play automatically. */
        MusicPlayerSingle(const std::string& fileID);

        virtual bool isLoaded() const override;
        virtual void play() override;
        virtual void pause() override;
        virtual void stop() override;
        virtual void setVolume(float volume) override;
        virtual float getVolume() const override;

    private:
        MusicData mData;
    };


    /** \brief A music player that sequentially iterates over a playlist. */
    class Playlist : public MusicPlayerBase
    {
    public:
        /** \brief Constructor loads the music files. Does not play automatically. */
        Playlist(const std::vector< std::string >& fileIDs, bool random, float intervalMin, float intervalMax);

        virtual bool isLoaded() const override;
        virtual void play() override;
        virtual void pause() override;
        virtual void stop() override;
        virtual void setVolume(float volume) override;
        virtual float getVolume() const override;
        virtual void update(float delta) override;

    private:
        float mIntervalMin;
        float mIntervalMax;
        std::vector<MusicData> mData;
        std::size_t mCurrentlyPlaying;
        bool mRandom;
        bool mSilence;
        bool mStopped;
        float mTimer;
    };
}

#endif // UNGOD_MUSIC_PLAYER_H
