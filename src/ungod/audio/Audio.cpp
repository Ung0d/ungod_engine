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

#include "ungod/audio/Audio.h"
#include "ungod/base/World.h"

namespace ungod
{
    SoundBundle::SoundBundle() : sound(), linkageCounter(0) {}


    void SoundProfile::initSounds(std::size_t num)
    {
        mSounds.reserve(num);
        for (std::size_t i = 0; i < num; ++i)
            mSounds.push_back(std::make_unique<SoundBundle>());
    }


    ProfileHandle::ProfileHandle(ProfileMap::iterator iter) : mIter(&(*iter))  {}

    ProfileHandle::ProfileHandle() : mIter(nullptr) {}

    std::size_t ProfileHandle::getSoundCount() const
    {
        return mIter->second.mSounds.size();
    }

    std::string ProfileHandle::getKey() const
    {
        return mIter->first;
    }


    SoundEmitterComponent::SoundEmitterComponent() : mProfile(), mDistanceCap(DEFAULT_DISTANCE_CAP) {}

    SoundEmitterComponent::SoundEmitterComponent(ProfileHandle profile) : mProfile(profile), mDistanceCap(DEFAULT_DISTANCE_CAP) {}


    SoundSlot::SoundSlot() : mSound(),
                             mPlaying(false),
                             mPlayTimer(0),
                             mProfile(),
                             mIndex(0) {}



    AudioManager::AudioManager() : mMuteMusic(false), mMuteSound(false), mMusicEmitterMixer()
    {
        mVolumeSettings.resize(1, 1.0f);
        mListener = std::unique_ptr<AudioListener>( new GlobalListener() );
    }


    AudioManager::AudioManager(ScriptedGameState* master) : mMuteMusic(false), mMuteSound(false), mMusicEmitterMixer()
    {
        mVolumeSettings.resize(1, 1.0f);
        mListener = std::unique_ptr<AudioListener>( new CameraListener(master->getCamera()) );
    }


    void AudioManager::initMusic(std::size_t num)
    {
        mMusic.reserve(num);
        for (std::size_t i = 0; i < num; ++i)
            mMusic.emplace_back();
        mMusicVolumes.resize(num, 1.0f);
    }

    void AudioManager::loadMusic(const std::string& fileID, std::size_t index)
    {
        mMusic[index] = std::unique_ptr<MusicPlayerBase>( new MusicPlayerSingle(fileID) );
    }

    void AudioManager::loadPlaylist(const std::vector<std::string>& fileIDs, std::size_t index, bool randomPlay, float intervalMin, float intervalMax)
    {
        if (fileIDs.size() == 0)
        {
            ungod::Logger::warning("Tried to load a music playlist with empty file list.");
            ungod::Logger::endl();
            return;
        }
        mMusic[index] = std::unique_ptr<MusicPlayerBase>( new Playlist(fileIDs, randomPlay, intervalMin, intervalMax) );
    }

    void AudioManager::playMusic(std::size_t index)
    {
        if (mMuteMusic)
            return;

        if(!mMusic[index]->isLoaded())
        {
            ungod::Logger::warning("Tried to play music that was not successfully loaded.");
            ungod::Logger::endl();
            return;
        }
        mMusic[index]->setVolume(100.0f * mMusicVolumes[index]);
        mMusic[index]->play();
    }

    void AudioManager::pauseMusic(std::size_t index)
    {
        if(!mMusic[index]->isLoaded())
        {
            ungod::Logger::warning("Tried to pause music that was not successfully loaded.");
            ungod::Logger::endl();
            return;
        }
        mMusic[index]->pause();
    }

    void AudioManager::stopMusic(std::size_t index)
    {
        if(!mMusic[index]->isLoaded())
        {
            ungod::Logger::warning("Tried to stop music that was not successfully loaded.");
            ungod::Logger::endl();
            return;
        }
        mMusic[index]->stop();
    }

    void AudioManager::fadeoutMusic(float milliseconds, std::size_t index)
    {
        mMusic[index]->mFadingActive = true;
        mMusic[index]->mFadingDirection = true;
        mMusic[index]->mMilliseconds = milliseconds;
    }

    void AudioManager::fadeinMusic(float milliseconds, std::size_t index)
    {
        mMusic[index]->mFadingActive = true;
        mMusic[index]->mFadingDirection = false;
        mMusic[index]->mMilliseconds = milliseconds;
    }

    void AudioManager::setMusicVolume(float volume, std::size_t index)
    {
        mMusicVolumes[index] = volume;
        mMusic[index]->setVolume(100.0f * mMusicVolumes[index]);
    }

    ProfileHandle AudioManager::initSoundProfile(const std::string& key)
    {
        return mSoundProfiles.emplace(key, SoundProfile()).first;
    }

    ProfileHandle AudioManager::getSoundProfile(const std::string& key)
    {
        return mSoundProfiles.find(key);
    }

    void AudioManager::connectProfile(Entity e, const std::string& profileKey)
    {
        auto profile = mSoundProfiles.find(profileKey);
        if (profile != mSoundProfiles.end())
            connectProfile(e, profile);
    }

    void AudioManager::connectProfile(SoundEmitterComponent& emitter, const std::string& profileKey)
    {
        auto profile = mSoundProfiles.find(profileKey);
        if (profile != mSoundProfiles.end())
            connectProfile(emitter, profile);
    }

    void AudioManager::connectProfile(Entity e, ProfileHandle profile)
    {
        SoundEmitterComponent& sound = e.modify<SoundEmitterComponent>();
        connectProfile(sound, profile);
    }

    void AudioManager::connectProfile(SoundEmitterComponent& emitter, ProfileHandle profile)
    {
        emitter.mProfile = profile;
    }

    void AudioManager::initSounds(const std::string& key, std::size_t num)
    {
        auto profile = mSoundProfiles.find(key);
        if (profile != mSoundProfiles.end())
            initSounds(profile, num);
    }

    void AudioManager::initSounds(ProfileHandle profile, std::size_t num)
    {
        profile.mIter->second.initSounds(num);
    }

    void AudioManager::loadSound( const std::string& key, const std::string& path, std::size_t index)
    {
        auto profile = mSoundProfiles.find(key);
        if (profile != mSoundProfiles.end())
            loadSound(profile, path, index);
    }

    void AudioManager::loadSound( ProfileHandle profile, const std::string& path, std::size_t index)
    {
        profile.mIter->second.mSounds[index]->sound.load(path);
    }

    void AudioManager::expireSounds( const std::string& key)
    {
        auto profile = mSoundProfiles.find(key);
        if (profile != mSoundProfiles.end())
            expireSounds(profile);
    }

    void AudioManager::expireSounds( ProfileHandle profile)
    {
        profile.mIter->second.mExpired = true;
        for (auto iter = profile.mIter->second.mSounds.begin(); iter != profile.mIter->second.mSounds.end();)
        {
            if (iter->get()->linkageCounter == 0)
                iter = profile.mIter->second.mSounds.erase( iter );
            else
                ++iter;
        }
    }

    void AudioManager::playSound(Entity e, std::size_t index, std::size_t volumeSetting, float pitch)
    {
        SoundEmitterComponent& sound = e.modify<SoundEmitterComponent>();
        float scaling = 1.0f;
        if (e.has<TransformComponent>())
        {
            scaling = mListener->getScaling(e.get<TransformComponent>().getCenterPosition(), sound.mDistanceCap);
        }
        playSound(sound.mProfile, index, scaling, volumeSetting, pitch);
    }

    void AudioManager::playSound(const std::string& key, std::size_t index, float scaling, std::size_t volumeSetting, float pitch)
    {
        auto profile = mSoundProfiles.find(key);
        if (profile != mSoundProfiles.end())
            playSound(profile, index, scaling, volumeSetting, pitch);
    }

    void AudioManager::playSound(ProfileHandle profile, std::size_t index, float scaling, std::size_t volumeSetting, float pitch)
    {
        if (mMuteSound)
            return;

        //find free slot
        std::size_t free = SOUND_PLAY_CAP;
        for (std::size_t i = 0; i < SOUND_PLAY_CAP; i++)
        {
            if (!mSoundslots[i].mProfile.mIter)
            {
                free = i;
                break;
            }
        }

        if(free == SOUND_PLAY_CAP)
            return;

        profile.mIter->second.mSounds[index]->linkageCounter ++;

        mSoundslots[free].mProfile = profile;
        mSoundslots[free].mIndex = index;
        mSoundslots[free].mSound.setBuffer(*(mSoundslots[free].mProfile.mIter->second.mSounds[index]->sound.get()));

        mSoundslots[free].mSound.setVolume( 100.0f * mVolumeSettings[volumeSetting] * scaling );
        mSoundslots[free].mSound.setPitch(pitch);

        mSoundslots[free].mPlaying = true;
        mSoundslots[free].mPlayTimer = mSoundslots[free].mSound.getBuffer()->getDuration().asMilliseconds();
        mSoundslots[free].mSound.play();

        mSoundBegin(profile.mIter->first, index);
    }

    std::string AudioManager::getLoadPath( const std::string& key, std::size_t index)
    {
        auto profile = mSoundProfiles.find(key);
        if (profile != mSoundProfiles.end())
            return getLoadPath(profile, index);
        else
            return "";
    }

    std::string AudioManager::getLoadPath( ProfileHandle profile, std::size_t index)
    {
        return profile.mIter->second.mSounds[index]->sound.getFilePath();
    }

    void AudioManager::initVolumeSettings(std::size_t num)
    {
        mVolumeSettings.resize(num, 1.0f);
    }

    void AudioManager::setVolume(std::size_t index, float volume)
    {
        mVolumeSettings[index] = volume;
    }

    void AudioManager::setMuteMusic(bool mute)
    {
        mMuteMusic = mute;
        if (mute)
        {
            for (const auto& music : mMusic)
            {
                music->stop();
            }
        }
    }

    void AudioManager::setMuteSound(bool mute)
    {
        mMuteSound = mute;
        if (mute)
        {
            for (std::size_t i = 0; i < SOUND_PLAY_CAP; ++i)
            {
                mSoundslots[i].mPlaying = false;
            }

            mMusicEmitterMixer.muteAll();
        }
    }

    void AudioManager::update(float delta, quad::QuadTree<Entity>* quadtree)
    {
        for (std::size_t i = 0; i < SOUND_PLAY_CAP; ++i)
        {
            if (!mSoundslots[i].mPlaying && mSoundslots[i].mProfile.mIter)
            {
                mSoundslots[i].mProfile.mIter->second.mSounds[mSoundslots[i].mIndex]->linkageCounter--;
                if (mSoundslots[i].mProfile.mIter->second.mExpired && mSoundslots[i].mProfile.mIter->second.mSounds[mSoundslots[i].mIndex]->linkageCounter == 0)
                    mSoundslots[i].mProfile.mIter->second.mSounds.erase( mSoundslots[i].mProfile.mIter->second.mSounds.begin() + mSoundslots[i].mIndex );
                mSoundEnd(mSoundslots[i].mProfile.mIter->first, mSoundslots[i].mIndex);
                mSoundslots[i].mProfile = mSoundProfiles.end();
                mSoundslots[i].mIndex = 0;
            }
            else
            {
                if(mSoundslots[i].mPlayTimer > 0)
                    mSoundslots[i].mPlayTimer -= delta;
                else
                    mSoundslots[i].mPlaying = false;
            }
        }

        for (auto& music : mMusic)
        {
            if (music->isLoaded())
                music->update(delta);
        }

        if (quadtree && !mMuteSound)
            mMusicEmitterMixer.update(delta, static_cast<AudioListener*>(mListener.get()), quadtree);
    }


    void AudioManager::onSoundBegin( const std::function<void(std::string, std::size_t)>& callback )
    {
        mSoundBegin.connect(callback);
    }

    void AudioManager::onSoundEnd( const std::function<void(std::string, std::size_t)>& callback )
    {
        mSoundEnd.connect(callback);
    }

    AudioManager::~AudioManager()
    {
        for (auto& music : mMusic)
        {
            if (music->isLoaded())
                music->stop();
        }
        for (std::size_t i = 0; i < SOUND_PLAY_CAP; ++i)
        {
            mSoundslots[i].mSound.stop();
        }
    }
}
