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



    SoundProfileManager::SoundProfileManager() : mMuteSound(false)
    {
        mVolumeSettings.resize(1, 1.0f);
    }

    ProfileHandle SoundProfileManager::initSoundProfile(const std::string& key)
    {
        return &(*(mSoundProfiles.emplace(key, SoundProfile()).first));
    }

    ProfileHandle SoundProfileManager::getSoundProfile(const std::string& key) 
    {
        auto res = mSoundProfiles.find(key);
        if (res != mSoundProfiles.end())
            return &(*(res));
        else
        {
            Logger::warning("Can not find a sound profile named", key);
            return {};
        }
    }

    void SoundProfileManager::initSounds(const std::string& key, std::size_t num)
    {
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

    float SoundProfileManager::getVolume(std::size_t index)
    {
        return mVolumeSettings[index];
    }


    void SoundProfileManager::setMuteSound(bool mute)
    {
        mMuteSound = mute;
        if (mute)
            for (std::size_t i = 0; i < SOUND_PLAY_CAP; ++i)
                mSoundslots[i].mPlaying = false;
    }

    SoundSlot* SoundProfileManager::getFreeSlot() 
    {
        if (mMuteSound)
            return nullptr;
        for (std::size_t i = 0; i < SOUND_PLAY_CAP; i++)
            if (!mSoundslots[i].mProfile.mIter)
                return &mSoundslots[i];
        return nullptr;
    }

    SoundProfileManager::~SoundProfileManager()
    {
        for (std::size_t i = 0; i < SOUND_PLAY_CAP; ++i)
            mSoundslots[i].mSound.stop();
    }




    void SoundHandler::init(SoundProfileManager& soundprofilemngr, const AudioListener* listener)
    {
        mSoundProfileMngr = &soundprofilemngr;
        mListener = listener;
    }

    void SoundHandler::connectProfile(Entity e, const std::string& profileKey)
    {
        auto profile = mSoundProfileMngr->getSoundProfile(profileKey);
        if (profile.valid())
            connectProfile(e, profile);
    }

    void SoundHandler::connectProfile(SoundEmitterComponent& emitter, const std::string& profileKey)
    {
        auto profile = mSoundProfileMngr->getSoundProfile(profileKey);
        if (profile.valid())
            connectProfile(emitter, profile);
    }

    void SoundHandler::connectProfile(Entity e, ProfileHandle profile)
    {
        SoundEmitterComponent& sound = e.modify<SoundEmitterComponent>();
        connectProfile(sound, profile);
    }

    void SoundHandler::connectProfile(SoundEmitterComponent& emitter, ProfileHandle profile)
    {
        emitter.mProfile = profile;
    }

    void SoundHandler::playSound(Entity e, std::size_t index, std::size_t volumeSetting, float pitch)
    {
        SoundEmitterComponent& sound = e.modify<SoundEmitterComponent>();
        float scaling = 1.0f;
        if (e.has<TransformComponent>())
        {
            scaling = mListener->getScaling(e.get<TransformComponent>().getCenterPosition(), sound.mDistanceCap);
        }
        playSound(sound.mProfile, index, scaling, volumeSetting, pitch);
    }

    void SoundHandler::playSound(const std::string& key, std::size_t index, float scaling, std::size_t volumeSetting, float pitch)
    {
        auto profile = mSoundProfileMngr->getSoundProfile(key);
        if (profile.valid())
            playSound(profile, index, scaling, volumeSetting, pitch);
    }

    void SoundHandler::playSound(ProfileHandle profile, std::size_t index, float scaling, std::size_t volumeSetting, float pitch)
    {
        SoundSlot* free = mSoundProfileMngr->getFreeSlot();

        if (!free)
            return;

        profile.mIter->second.mSounds[index]->linkageCounter++;

        free->mProfile = profile;
        free->mIndex = index;
        free->mSound.setBuffer(free->mProfile.mIter->second.mSounds[index]->sound.get());

        free->mSound.setVolume(100.0f * mSoundProfileMngr->getVolume(volumeSetting) * scaling);
        free->mSound.setPitch(pitch);

        free->mPlaying = true;
        free->mPlayTimer = (float)free->mSound.getBuffer()->getDuration().asMilliseconds();
        free->mSound.play();

        mSoundBegin(profile.mIter->first, index);

        mOccupied.emplace_back(free);
    }

    void SoundHandler::update(float delta)
    {
        for (auto* slot : mOccupied)
        {
            if (!slot->mPlaying && slot->mProfile.mIter)
            {
                slot->mProfile.mIter->second.mSounds[slot->mIndex]->linkageCounter--;
                if (slot->mProfile.mIter->second.mExpired && slot->mProfile.mIter->second.mSounds[slot->mIndex]->linkageCounter == 0)
                    slot->mProfile.mIter->second.mSounds.erase(slot->mProfile.mIter->second.mSounds.begin() + slot->mIndex);
                mSoundEnd(slot->mProfile.mIter->first, slot->mIndex);
                slot->mProfile = { nullptr };
                slot->mIndex = 0;
            }
            else
            {
                if (slot->mPlayTimer > 0)
                    slot->mPlayTimer -= delta;
                else
                    slot->mPlaying = false;
            }
        }
    }

    void SoundHandler::onSoundBegin(const std::function<void(std::string, std::size_t)>& callback)
    {
        mSoundBegin.connect(callback);
    }

    void SoundHandler::onSoundEnd(const std::function<void(std::string, std::size_t)>& callback)
    {
        mSoundEnd.connect(callback);
    }
}
