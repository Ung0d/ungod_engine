#ifndef UEDIT_AUDIO_ACTIONS_H
#define UEDIT_AUDIO_ACTIONS_H

#include "ungod/base/Entity.h"
#include "ungod/audio/Audio.h"

namespace uedit
{
    class ActionManager;

    class AudioActions
    {
    public:
        AudioActions(ActionManager& am) : mActionManager(am) {}

        void loadAudioEmitterTrack(ungod::Entity e, const std::string& file);
        void setAudioEmitterDistanceCap(ungod::Entity e, float cap);
        void setAudioEmitterActive(ungod::Entity e, bool active);
        void setAudioEmitterVolume(ungod::Entity e, float vol);

    private:
        ActionManager& mActionManager;
    };
}

#endif