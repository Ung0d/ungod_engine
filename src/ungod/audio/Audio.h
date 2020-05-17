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

#ifndef AUDIO_H
#define AUDIO_H

#include <array>
#include <vector>
#include <SFML/Audio.hpp>
#include "owls/Signal.h"
#include "ungod/audio/Sound.h"
#include "ungod/audio/MusicPlayer.h"
#include "ungod/audio/Listener.h"
#include "ungod/audio/MusicEmitter.h"
#include "ungod/serialization/Serializable.h"

namespace ungod
{
    class Entity;
    class AudioManager;
    class World;
    class Application;
    class ScriptedGameState;

    /** \brief Bundles a sound object, a counter, that indicates how many slots currently play
    * this sound, and a expired flag. */
    struct SoundBundle
    {
        Sound sound;
        int linkageCounter;

        SoundBundle();
    };

    /** \brief A sound profile that maps integer IDs to certain sounds.
    * Also provides counters that indicate for each sound, how many slots does use it currently. */
    class SoundProfile
    {
    friend class SoundProfileManager;
    friend class SoundHandler;
    friend class SoundSlot;
    friend class ProfileHandle;
     friend struct SerialBehavior<AudioManager>;
    private:
        std::vector< std::unique_ptr<SoundBundle> > mSounds;
        bool mExpired;

    public:
        SoundProfile() : mExpired(false) {}
        void initSounds(std::size_t num);
    };

    using ProfileMap = std::unordered_map<std::string, SoundProfile>;

    /** \brief A handle for a sound profile. In contact with the user. */
    class ProfileHandle
    {
    friend class SoundProfileManager;
    friend class SoundHandler;
    public:
        ProfileHandle(std::pair<const std::string, SoundProfile>* profile);
        ProfileHandle();

        /** \brief Returns the number of sounds loaded for the corresponding profile. */
        std::size_t getSoundCount() const;

        std::string getKey() const;

        bool valid() const;

    private:
        std::pair<const std::string, SoundProfile>* mIter;
    };

    /**
    * \ingroup Components
    * \brief A component that can be attached to an entity. Grants the ability to
    * emit sound effects. */
    class SoundEmitterComponent : public Serializable<SoundEmitterComponent>
    {
    friend class SoundHandler;
     friend struct SerialBehavior<SoundEmitterComponent, Entity, const World&>;
    friend struct DeserialBehavior<SoundEmitterComponent, Entity, World&>;

    static constexpr float DEFAULT_DISTANCE_CAP = 500.0f;

    private:
        ProfileHandle mProfile;
        float mDistanceCap;

    public:
        SoundEmitterComponent();
        SoundEmitterComponent(ProfileHandle profile);
    };

    /**
    * \brief Helper struct. Acts as a sound output slot.
    */
    class SoundSlot
    {
    friend class SoundHandler;
    private:
        sf::Sound mSound;
        bool mPlaying;
        float mPlayTimer;
        ProfileHandle mProfile;
        std::size_t mIndex;
        std::string mCurrentKey;

    public:
        SoundSlot();
        ~SoundSlot() {}
    };


    /**
    * \brief Manages all sound files and connects them to entities who can then play the sounds
    * via the SoundHandler or their WorldNode. 
    */
    class SoundProfileManager 
    {
    friend class SoundEmitterComponent;
    private:
        ProfileMap mSoundProfiles;
        std::vector<float> mVolumeSettings; 

    public:
        SoundProfileManager();
        SoundProfileManager(SoundProfileManager const&) = delete;
        SoundProfileManager& operator=(SoundProfileManager const&) = delete;

        /** \brief Instantiates a new SoundProfile in the manager and returns a ptr to it. */
        ProfileHandle initSoundProfile(const std::string& key);

        /** \brief Returns a handle to the given sound profile if it was initialized before. Elsewise it returns an invalid handle. */
        ProfileHandle getSoundProfile(const std::string& key);

        /** \brief Prepares the given SoundProfile to hold num sounds. */
        void initSounds(const std::string& key, std::size_t num);
        void initSounds(ProfileHandle profile, std::size_t num);

        /** \brief Loads a new sound effect with given id for the given sound profile. The id must be
        * in range 0...n-1 of a previous initSounds(key, n) call. */
        void loadSound( const std::string& key, const std::string& path, std::size_t index);
        void loadSound( ProfileHandle profile, const std::string& path, std::size_t index);

        /** \brief Expires all sounds from the given profile. Destroy the
        * underlying objects as soon as all instances of the sound, that are currently playing,
        * are ready. */
        void expireSounds( const std::string& key);
        void expireSounds( ProfileHandle profile);

        /** \brief Returns the load path of the sound with the given index. */
        std::string getLoadPath( const std::string& key, std::size_t index);
        std::string getLoadPath( ProfileHandle profile, std::size_t index);

        /** \brief Inits a given number of volume settings. */
        void initVolumeSettings(std::size_t num);

        /** \brief Sets the volume of the settings with the given index in the interval [0,1]. */
        void setVolume(std::size_t index, float volume);

        /** \brief Returns the volume level in range [0,1] */
        float getVolume(std::size_t index);
    };

    /**
    * \brief Manages all sound files and connects them to entities who can then play the sounds
    * via the SoundManager or their WorldNode.
    */
    class SoundHandler
    {
    friend class SoundEmitterComponent;
    private:
        static constexpr std::size_t SOUND_PLAY_CAP = 32;  ///maximum number of sounds playable concurrently

        std::array<SoundSlot, SOUND_PLAY_CAP> mSoundslots;
        owls::Signal<std::string, std::size_t> mSoundBegin;
        owls::Signal<std::string, std::size_t> mSoundEnd;
        bool mMuteSound;
        SoundProfileManager* mSoundProfileMngr;
        const AudioListener* mListener;

    public:
        SoundHandler();
        SoundHandler(SoundHandler const&) = delete;
        SoundHandler& operator=(SoundHandler const&) = delete;

        void init(SoundProfileManager& soundprofilemngr, const AudioListener* listener);

        /** \brief Connects the given sound profile with the given entity. */
        void connectProfile(Entity e, const std::string& profileKey);
        void connectProfile(Entity e, ProfileHandle profile);
        void connectProfile(SoundEmitterComponent& emitter, const std::string& profileKey);
        void connectProfile(SoundEmitterComponent& emitter, ProfileHandle profile);

        /** \brief Plays the sound effect with given id of the entity e. Emits a sound-begin and a sound-end signal. */
        void playSound(Entity e, std::size_t index, std::size_t volumeSetting = 0, float pitch = 1.0f);

        /** \brief Plays the sound with given index from the given sound profile. */
        void playSound(const std::string& key, std::size_t index, float scaling = 1.0f, std::size_t volumeSetting = 0, float pitch = 1.0f);
        void playSound(ProfileHandle profile, std::size_t index, float scaling = 1.0f, std::size_t volumeSetting = 0, float pitch = 1.0f);

        /** \brief Mutes all sounds. Stops all sounds currently playing. */
        void setMuteSound(bool mute = true);

        /**
        * \brief Internal update that clears out expired sounds.
        */
        void update(float delta);

        /** \brief Connects a callback for the sound begin signal. Arguments are
        * the name of the profile as string and the index of the sound. */
        void onSoundBegin(const std::function<void(std::string, std::size_t)>& callback);

        /** \brief Connects a callback for the sound end signal. Arguments are
        * the name of the profile as string and the index of the sound. */
        void onSoundEnd(const std::function<void(std::string, std::size_t)>& callback);

        /** \brief Stops playing of all active sounds. */
        ~SoundHandler();
    };
}

#endif // AUDIO_H
