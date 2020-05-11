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


    ProfileHandle::ProfileHandle(std::pair<const std::string, SoundProfile>* profile) : mIter(profile)  {}

    ProfileHandle::ProfileHandle() : mIter(nullptr) {}

    std::size_t ProfileHandle::getSoundCount() const
    {
        return mIter->second.mSounds.size();
    }

    std::string ProfileHandle::getKey() const
    {
        return mIter->first;
    }

    bool ProfileHandle::valid() const
    {
        return mIter != nullptr;
    }


    SoundEmitterComponent::SoundEmitterComponent() : mProfile(), mDistanceCap(DEFAULT_DISTANCE_CAP) {}

    SoundEmitterComponent::SoundEmitterComponent(ProfileHandle profile) : mProfile(profile), mDistanceCap(DEFAULT_DISTANCE_CAP) {}


    SoundSlot::SoundSlot() : mSound(),
                             mPlaying(false),
                             mPlayTimer(0),
                             mProfile(),
                             mIndex(0) {}



    SoundProfileManager::SoundProfileManager() 
    {
        mVolumeSettings.resize(1, 1.0f);
    }

    ProfileHandle SoundProfileManager::initSoundProfile(const std::string& key)
    {
        std::unique_lock lock(mManagerMutex);
        return &(*(mSoundProfiles.emplace(key, SoundProfile()).first));
    }

    ProfileHandle SoundProfileManager::getSoundProfile(const std::string& key) const
    {
        std::shared_lock lock(mManagerMutex);
        auto res = mSoundProfiles.find(key);
        if (res != mSoundProfiles.end())
            return &(*(res));
        else
        {
            Logger::warning("Can not find a sound profile named ");
            Logger::warning("key");
            Logger::endl();
            return {};
        }
    }

    void SoundProfileManager::initSounds(const std::string& key, std::size_t num)
    {
        std::shared_lock lock(mManagerMutex);
        auto profile = mSoundProfiles.find(key);
        if (profile != mSoundProfiles.end())
            initSounds(&(*(profile)), num);
    }

    void SoundProfileManager::initSounds(ProfileHandle profile, std::size_t num)
    {
        profile.mIter->second.initSounds(num);
    }

    void SoundProfileManager::loadSound( const std::string& key, const std::string& path, std::size_t index)
    {
        std::shared_lock lock(mManagerMutex);
        auto profile = mSoundProfiles.find(key);
        if (profile != mSoundProfiles.end())
            loadSound(&(*(profile)), path, index);
    }

    void SoundProfileManager::loadSound( ProfileHandle profile, const std::string& path, std::size_t index)
    {
        profile.mIter->second.mSounds[index]->sound.load(path);
    }

    void SoundProfileManager::expireSounds( const std::string& key)
    {
        std::shared_lock lock(mManagerMutex);
        auto profile = mSoundProfiles.find(key);
        if (profile != mSoundProfiles.end())
            expireSounds(&(*(profile)));
    }

    void SoundProfileManager::expireSounds( ProfileHandle profile)
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

    std::string SoundProfileManager::getLoadPath( const std::string& key, std::size_t index)
    {
        std::shared_lock lock(mManagerMutex);
        auto profile = mSoundProfiles.find(key);
        if (profile != mSoundProfiles.end())
            return getLoadPath(&(*(profile)), index);
        else
            return "";
    }

    std::string SoundProfileManager::getLoadPath( ProfileHandle profile, std::size_t index)
    {
        return profile.mIter->second.mSounds[index]->sound.getFilePath();
    }

    void SoundProfileManager::initVolumeSettings(std::size_t num)
    {
        mVolumeSettings.resize(num, 1.0f);
    }

    void SoundProfileManager::setVolume(std::size_t index, float volume)
    {
        mVolumeSettings[index] = volume;
    }

    void SoundProfileManager::getVolume(std::size_t index)
    {
        return mVolumeSettings[index];
    }



    SoundManager::SoundManager(SoundProfileManager& soundprofilemngr, const AudioListener& listener) : 
        mMuteSound(false), mSoundProfileMngr(soundprofilemngr), mListener(listener)
    {
    }

    void SoundManager::connectProfile(Entity e, const std::string& profileKey)
    {
        auto profile = mSoundProfileMngr.getSoundProfile(profileKey);
        if (profile.valid())
            connectProfile(e, profile);
    }

    void SoundManager::connectProfile(SoundEmitterComponent& emitter, const std::string& profileKey)
    {
        auto profile = mSoundProfileMngr.getSoundProfile(profileKey);
        if (profile.valid())
            connectProfile(emitter, profile);
    }

    void SoundManager::connectProfile(Entity e, ProfileHandle profile)
    {
        SoundEmitterComponent& sound = e.modify<SoundEmitterComponent>();
        connectProfile(sound, profile);
    }

    void SoundManager::connectProfile(SoundEmitterComponent& emitter, ProfileHandle profile)
    {
        emitter.mProfile = profile;
    }

    void SoundManager::playSound(Entity e, std::size_t index, std::size_t volumeSetting, float pitch)
    {
        SoundEmitterComponent& sound = e.modify<SoundEmitterComponent>();
        float scaling = 1.0f;
        if (e.has<TransformComponent>())
        {
            scaling = mListener->getScaling(e.get<TransformComponent>().getCenterPosition(), sound.mDistanceCap);
        }
        playSound(sound.mProfile, index, scaling, volumeSetting, pitch);
    }

    void SoundManager::playSound(const std::string& key, std::size_t index, float scaling, std::size_t volumeSetting, float pitch)
    {
        auto profile = mSoundProfileMngr.getSoundProfile(profileKey);
        if (profile.valid())
            playSound(profile, index, scaling, volumeSetting, pitch);
    }

    void SoundManager::playSound(ProfileHandle profile, std::size_t index, float scaling, std::size_t volumeSetting, float pitch)
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

        if (free == SOUND_PLAY_CAP)
            return;

        profile.mIter->second.mSounds[index]->linkageCounter++;

        mSoundslots[free].mProfile = profile;
        mSoundslots[free].mIndex = index;
        mSoundslots[free].mSound.setBuffer(mSoundslots[free].mProfile.mIter->second.mSounds[index]->sound.get());

        mSoundslots[free].mSound.setVolume(100.0f * mSoundProfileMngr.getVolume(volumeSetting) * scaling);
        mSoundslots[free].mSound.setPitch(pitch);

        mSoundslots[free].mPlaying = true;
        mSoundslots[free].mPlayTimer = (float)mSoundslots[free].mSound.getBuffer()->getDuration().asMilliseconds();
        mSoundslots[free].mSound.play();

        mSoundBegin(profile.mIter->first, index);
    }

    void SoundManager::setMuteSound(bool mute)
    {
        mMuteSound = mute;
        if (mute)
            for (std::size_t i = 0; i < SOUND_PLAY_CAP; ++i)
                mSoundslots[i].mPlaying = false;
    }

    void SoundManager::update(float delta)
    {
        for (std::size_t i = 0; i < SOUND_PLAY_CAP; ++i)
        {
            if (!mSoundslots[i].mPlaying && mSoundslots[i].mProfile.mIter)
            {
                mSoundslots[i].mProfile.mIter->second.mSounds[mSoundslots[i].mIndex]->linkageCounter--;
                if (mSoundslots[i].mProfile.mIter->second.mExpired && mSoundslots[i].mProfile.mIter->second.mSounds[mSoundslots[i].mIndex]->linkageCounter == 0)
                    mSoundslots[i].mProfile.mIter->second.mSounds.erase(mSoundslots[i].mProfile.mIter->second.mSounds.begin() + mSoundslots[i].mIndex);
                mSoundEnd(mSoundslots[i].mProfile.mIter->first, mSoundslots[i].mIndex);
                mSoundslots[i].mProfile = { nullptr };
                mSoundslots[i].mIndex = 0;
            }
            else
            {
                if (mSoundslots[i].mPlayTimer > 0)
                    mSoundslots[i].mPlayTimer -= delta;
                else
                    mSoundslots[i].mPlaying = false;
            }
        }
    }

    void SoundManager::onSoundBegin(const std::function<void(std::string, std::size_t)>& callback)
    {
        mSoundBegin.connect(callback);
    }

    void SoundManager::onSoundEnd(const std::function<void(std::string, std::size_t)>& callback)
    {
        mSoundEnd.connect(callback);
    }

    SoundManager::~SoundManager()
    {
        for (std::size_t i = 0; i < SOUND_PLAY_CAP; ++i)
        {
            mSoundslots[i].mSound.stop();
        }
    }
}
