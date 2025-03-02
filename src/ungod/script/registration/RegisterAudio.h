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

#ifndef REGISTER_AUDIO_H
#define REGISTER_AUDIO_H

#include "ungod/script/Behavior.h"

namespace ungod
{
    class Entity;
    class SoundHandler;
    class MusicEmitterMixer;
    class ProfileHandle;

    namespace scriptRegistration
    {
        class SoundHandlerFrontEnd
        {
        public:
            SoundHandlerFrontEnd(Entity& e, SoundHandler& h) : mEntity(e), mHandler(h) {}
            void connectProfile(const std::string& profileKey);
            void connectProfile(ProfileHandle handle);
            void playSound(std::size_t index, std::size_t volumeSetting=0u, float pitch=1.0f);
        private:
            Entity& mEntity;
            SoundHandler& mHandler;
        };

        class MusicEmitterFrontEnd
        {
        public:
            MusicEmitterFrontEnd(Entity& e, MusicEmitterMixer& h) : mEntity(e), mHandler(h) {}
            void loadMusic(const std::string& fileID);
            void setDistanceCap(float cap);
            void setEmitterActive(bool active);
            void setEmitterVolume(float vol);
            float getDistanceCap() const;
            float getVolume() const;
            bool isActive() const;
        private:
            Entity& mEntity;
            MusicEmitterMixer& mHandler;
        };

        /** \brief Registers audio functionality for scripts. */
        void registerAudio(ScriptStateBase& state);
    }
}


#endif
