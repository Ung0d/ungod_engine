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
		void SoundHandlerFrontEnd::connectProfile(const std::string& profileKey)
		{
			mHandler.connectProfile(mEntity, profileKey);
		}

		void SoundHandlerFrontEnd::connectProfile(ProfileHandle handle)
		{
			mHandler.connectProfile(mEntity, handle);
		}

		void SoundHandlerFrontEnd::playSound(std::size_t index, std::size_t volumeSetting, float pitch)
		{
			mHandler.playSound(mEntity, index, volumeSetting, pitch);
		}


		void MusicEmitterFrontEnd::loadMusic(const std::string& fileID)
		{
			mHandler.loadMusic(mEntity, fileID);
		}

		void MusicEmitterFrontEnd::setDistanceCap(float cap)
		{
			mHandler.setDistanceCap(mEntity, cap);
		}

		void MusicEmitterFrontEnd::setEmitterActive(bool active)
		{
			mHandler.setEmitterActive(mEntity, active);
		}

		void MusicEmitterFrontEnd::setEmitterVolume(float vol)
		{
			mHandler.setEmitterVolume(mEntity, vol);
		}

		float MusicEmitterFrontEnd::getDistanceCap() const
		{
			return mEntity.get<MusicEmitterComponent>().getDistanceCap();
		}

		float MusicEmitterFrontEnd::getVolume() const
		{
			return mEntity.get<MusicEmitterComponent>().getVolume();
		}

		bool MusicEmitterFrontEnd::isActive() const
		{
			return mEntity.get<MusicEmitterComponent>().isActive();
		}


        void registerAudio(ScriptStateBase& state)
        {
            state.registerUsertype<ProfileHandle>("SoundProfileHandle");

			script::Usertype<MusicManager> mmType = state.registerUsertype<MusicManager>("MusicManager");
			mmType["loadMusic"] = &MusicManager::loadMusic;
			mmType["loadPlaylist"] = [](MusicManager& mm, script::Environment files, bool randomPlay, float intervalMin, float intervalMax)
			{ mm.loadPlaylist(env2vec<std::string>(files), randomPlay, intervalMin, intervalMax); };
			mmType["playMusic"] = &MusicManager::playMusic;
			mmType["pauseMusic"] = &MusicManager::pauseMusic;
			mmType["stopMusic"] = &MusicManager::stopMusic;
			mmType["fadeoutMusic"] = &MusicManager::fadeoutMusic;
			mmType["fadeinMusic"] = &MusicManager::fadeinMusic;
			mmType["setMusicVolume"] = &MusicManager::setMusicVolume;
			mmType["unloadMusic"] = &MusicManager::unloadMusic;

			script::Usertype<SoundProfileManager> profmngrType = state.registerUsertype<SoundProfileManager>("SoundProfileManager");
			profmngrType["initSoundProfile"] = &SoundProfileManager::initSoundProfile;
			profmngrType["getSoundProfile"] = &SoundProfileManager::getSoundProfile;
			profmngrType["initSounds"] = sol::overload([](SoundProfileManager& spm, const std::string& key, std::size_t num) { spm.initSounds(key, num); },
				[](SoundProfileManager& spm, ProfileHandle handle, std::size_t num) { spm.initSounds(handle, num); });
			profmngrType["loadSound"] = sol::overload([](SoundProfileManager& spm, const std::string& key, const std::string& path, std::size_t i) { spm.loadSound(key, path, i); },
				[](SoundProfileManager& spm, ProfileHandle handle, const std::string& path, std::size_t i) { spm.loadSound(handle, path, i); });
			profmngrType["expireSounds"] = sol::overload([](SoundProfileManager& spm, const std::string& key) { spm.expireSounds(key); },
				[](SoundProfileManager& spm, ProfileHandle handle) { spm.expireSounds(handle);  });
			profmngrType["initVolumeSettings"] = &SoundProfileManager::initVolumeSettings;
			profmngrType["setVolume"] = &SoundProfileManager::setVolume;
			profmngrType["getVolume"] = &SoundProfileManager::getVolume;

			script::Usertype<SoundHandler> shType = state.registerUsertype<SoundHandler>("SoundHandler");
			shType["playSound"] = sol::overload([](SoundHandler& sh, const std::string& key, std::size_t index) { sh.playSound(key, index); },
				[](SoundHandler& sh, const std::string& key, std::size_t index, std::size_t volumeSetting, float pitch) { sh.playSound(key, index, 1.0f, volumeSetting, pitch); },

				[](SoundHandler& sh, ProfileHandle handle, std::size_t index) { sh.playSound(handle, index); },
				[](SoundHandler& sh, ProfileHandle handle, std::size_t index, std::size_t volumeSetting, float pitch) { sh.playSound(handle, index, 1.0f, volumeSetting, pitch); });
			shType["setMuteSound"] = &SoundHandler::setMuteSound;


			script::Usertype<SoundHandlerFrontEnd> shFrontType = state.registerUsertype<SoundHandlerFrontEnd>("SoundHandlerFrontEnd");
			shFrontType["play"] = sol::overload([](SoundHandlerFrontEnd& shfe, std::size_t index) { shfe.playSound(index);  },
				[](SoundHandlerFrontEnd& shfe, std::size_t index, std::size_t volumeSetting, float pitch) { shfe.playSound(index, volumeSetting, pitch);  });
			shFrontType["connectProfile"] = sol::overload([](SoundHandlerFrontEnd& shfe, const std::string& key) { shfe.connectProfile(key);  },
				[](SoundHandlerFrontEnd& shfe, ProfileHandle handle) { shfe.connectProfile(handle);  });


			script::Usertype<MusicEmitterMixer> mixerType = state.registerUsertype<MusicEmitterMixer>("MusicEmitterMixer");
			mixerType["setMuteSound"] = &MusicEmitterMixer::setMuteSound;


			script::Usertype<MusicEmitterFrontEnd> mixerFrontEndType = state.registerUsertype<MusicEmitterFrontEnd>("MusicEmitterFrontEnd");
			mixerFrontEndType["loadMusic"] = &MusicEmitterFrontEnd::loadMusic;
			mixerFrontEndType["setDistanceCap"] = &MusicEmitterFrontEnd::setDistanceCap;
			mixerFrontEndType["setEmitterActive"] = &MusicEmitterFrontEnd::setEmitterActive;
			mixerFrontEndType["setEmitterVolume"] = &MusicEmitterFrontEnd::setEmitterVolume;
			mixerFrontEndType["getDistanceCap"] = &MusicEmitterFrontEnd::getDistanceCap;
			mixerFrontEndType["getVolume"] = &MusicEmitterFrontEnd::getVolume;
			mixerFrontEndType["isActive"] = &MusicEmitterFrontEnd::isActive;
        }
    }
}
