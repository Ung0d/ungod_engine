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

#ifndef UNGOD_MUSIC_EMITTER_H
#define UNGOD_MUSIC_EMITTER_H

#include <utility>
#include <array>
#include "ungod/audio/Music.h"
#include "ungod/audio/Listener.h"
#include "ungod/serialization/Serializable.h"
#include "quadtree/QuadTree.h"

namespace ungod
{
    class Entity;
    class MusicEmitterMixer;
    class TransformComponent;
    class World;
    class Application;

    //TODO give music emitters a priority value that can be used to determine which are played and which not if the cap is exceeded

    /** \brief A component that is able to emit music whose volume is scaled according to the audio managers listener. */
    class MusicEmitterComponent : public Serializable<MusicEmitterComponent>
    {
    friend class MusicEmitterMixer;
    friend struct SerialBehavior<MusicEmitterComponent, Entity, const World&, const Application&>;
    friend struct DeserialBehavior<MusicEmitterComponent, Entity, World&, const Application&>;

    public:

        static constexpr float DEFAULT_DISTANCE_CAP = 500.0f;

        MusicEmitterComponent();

        MusicEmitterComponent(const MusicEmitterComponent&);

        /** \brief Returns the filepath of the music file or "" if unloaded. **/
        std::string getFilePath() const;

        /** \brief Returns the distance cap value. **/
        float getDistanceCap() const;

        /** \brief Returns the volume in range [0,1]. **/
        float getVolume() const;

        /** \brief Returns true if and only if the emitter is active. **/
        bool isActive() const;

    private:
        MusicData mMusicData;
        float mDistanceCap;
        float mVolume;
        bool mActive;
        bool mBound;
    };


    /** \brief A mixer class for music emitters. Holds a cap for the maximum number of emitters playing concurrently. */
    class MusicEmitterMixer
    {
    public:
        static constexpr std::size_t MUSIC_PLAY_CAP = 5;  ///maximum number of sounds playable concurrently
        static constexpr float DEFAULT_MAX_DISTANCE_CAP = 3000.0f;

        MusicEmitterMixer();

        /** \brief Loads the music for the music emitter. */
        void loadMusic(Entity e, const std::string& fileID);

        /** \brief Sets the distance cap for the music emitter. */
        void setDistanceCap(Entity e, float cap);

        /** \brief Activates or deactivates the music emitter. */
        void setEmitterActive(Entity e, bool active);

        /** \brief Sets the music emitter volume in range [0,1]. */
        void setEmitterVolume(Entity e, float vol);

        /** \brief Mutes all currently active tracks. */
        void muteAll();

        /** \brief updates the music volumes and may starts new emitters */
        void update(float delta, AudioListener* listener, quad::QuadTree<Entity>* quadtree);

    private:
        std::array<std::pair<MusicEmitterComponent*, TransformComponent*>, MUSIC_PLAY_CAP> mCurrentlyPlaying;
        float mMaxDistanceCap;
    };
}

#endif // UNGOD_MUSIC_EMITTER_H
