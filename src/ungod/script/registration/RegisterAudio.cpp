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

#include "ungod/script/registration/RegisterAudio.h"
#include "ungod/audio/Audio.h"
#include "ungod/base/World.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerAudio(ScriptStateBase& state)
        {
            state.registerUsertype<ProfileHandle>("SoundProfileHandle");

            state.registerUsertype<AudioManager>("AudioManager",
                                            "initMusic", &AudioManager::initMusic,
                                            "loadMusic", &AudioManager::loadMusic,
                                            "loadPlaylist", [] (AudioManager& am, script::Environment files, std::size_t index, bool randomPlay, float intervalMin, float intervalMax)
                                                 { am.loadPlaylist(env2vec<std::string>(files), index, randomPlay, intervalMin, intervalMax); },
                                            "playMusic", &AudioManager::playMusic,
                                            "pauseMusic", &AudioManager::pauseMusic,
                                            "stopMusic", &AudioManager::stopMusic,
                                            "fadeoutMusic", &AudioManager::fadeoutMusic,
                                            "fadeinMusic", &AudioManager::fadeinMusic,
                                            "setMusicVolume", &AudioManager::setMusicVolume,
                                            "initSoundProfile", &AudioManager::initSoundProfile,
                                            "getSoundProfile", &AudioManager::getSoundProfile,
                                            "initSounds", sol::overload( []  (AudioManager& am, const std::string& key, std::size_t num) { am.initSounds(key, num); },
                                                                         []  (AudioManager& am, ProfileHandle handle, std::size_t num) { am.initSounds(handle, num); }),
                                            "loadSound", sol::overload( []  (AudioManager& am, const std::string& key, const std::string& path, std::size_t i) { am.loadSound(key, path, i); },
                                                                         []  (AudioManager& am, ProfileHandle handle, const std::string& path, std::size_t i) { am.loadSound(handle, path, i); }),
                                            "playSound", sol::overload( []  (AudioManager& am, const std::string& key, std::size_t index) { am.playSound(key, index); },
                                                                        []  (AudioManager& am, const std::string& key, std::size_t index, std::size_t volumeSetting) { am.playSound(key, index, volumeSetting); },
                                                                        []  (AudioManager& am, const std::string& key, std::size_t index, std::size_t volumeSetting, float pitch) { am.playSound(key, index, volumeSetting, pitch); },
                                                                        []  (AudioManager& am, ProfileHandle handle, std::size_t index) { am.playSound(handle, index); },
                                                                        []  (AudioManager& am, ProfileHandle handle, std::size_t index, std::size_t volumeSetting) { am.playSound(handle, index, volumeSetting); },
                                                                        []  (AudioManager& am, ProfileHandle handle, std::size_t index, std::size_t volumeSetting, float pitch) { am.playSound(handle, index, volumeSetting, pitch); }),
                                            "initVolumeSettings", &AudioManager::initVolumeSettings,
                                            "setVolume", &AudioManager::setVolume,
                                            "setMuteMusic", &AudioManager::setMuteMusic,
                                            "setMuteSound", &AudioManager::setMuteSound,
                                            "getMusicEmitterMixer", &AudioManager::getMusicEmitterMixer
                                            );

            state.registerUsertype<MusicEmitterMixer>("MusicEmitterMixer",
                                                        "loadMusic", &MusicEmitterMixer::loadMusic,
                                                        "setDistanceCap", &MusicEmitterMixer::setDistanceCap,
                                                        "setEmitterActive", &MusicEmitterMixer::setEmitterActive,
                                                        "setEmitterVolume", &MusicEmitterMixer::setEmitterVolume);
        }
    }
}
