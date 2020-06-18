#include "AudioActions.h"
#include "ActionManager.h"

namespace uedit
{
    void AudioActions::loadAudioEmitterTrack(ungod::Entity e, const std::string& file)
    {
        std::string old = e.get<ungod::MusicEmitterComponent>().getFilePath();
        mActionManager.action(std::function([this, file](ungod::Entity e)
            { e.getWorld().getMusicEmitterMixer().loadMusic(e, file); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getMusicEmitterMixer().loadMusic(e, old); }),
            e);
    }

    void AudioActions::setAudioEmitterDistanceCap(ungod::Entity e, float cap)
    {
        float old = e.get<ungod::MusicEmitterComponent>().getDistanceCap();
        mActionManager.action(std::function([this, cap](ungod::Entity e)
            { e.getWorld().getMusicEmitterMixer().setDistanceCap(e, cap); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getMusicEmitterMixer().setDistanceCap(e, old); }),
            e);
    }

    void AudioActions::setAudioEmitterActive(ungod::Entity e, bool active)
    {
        bool old = e.get<ungod::MusicEmitterComponent>().isActive();
        mActionManager.action(std::function([this, active](ungod::Entity e)
            { e.getWorld().getMusicEmitterMixer().setEmitterActive(e, active); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getMusicEmitterMixer().setEmitterActive(e, old); }),
            e);
    }

    void AudioActions::setAudioEmitterVolume(ungod::Entity e, float vol)
    {
        float old = e.get<ungod::MusicEmitterComponent>().isActive();
        mActionManager.action(std::function([this, vol](ungod::Entity e)
            { e.getWorld().getMusicEmitterMixer().setEmitterVolume(e, vol); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getMusicEmitterMixer().setEmitterVolume(e, old); }),
            e);
    }
}