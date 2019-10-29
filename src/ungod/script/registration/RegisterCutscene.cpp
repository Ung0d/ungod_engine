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

#include "ungod/script/registration/RegisterCutscene.h"
#include "ungod/content/Cutscene.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerCutscene(ScriptStateBase& state)
        {
               /* state.registerUsertype<Cutscene>("Cutscene",
                                                    "addScene", &Cutscene::addScene,
                                                    "play", &Cutscene::play,
                                                    "isPlaying", &Cutscene::isPlaying,
                                                    "getSceneIndex", &Cutscene::getSceneIndex,
                                                    "getAudioManager", [] (Cutscene& cs) -> AudioManager& { return cs.getAudioManager(); },
                                                    "loadFont", &Cutscene::loadFont,
                                                    "setSceneFading", &Cutscene::setSceneFading
                                                    );

                state.registerUsertype<Scene>("Scene",
                                                    "addLayer", &Scene::addLayer,
                                                    "addLayerTransitionEffect", [] (Scene& scene, unsigned index, const sf::Vector2f& direction, const sf::Vector2f& startingPos, float speed)
                                                                                    { scene.addEffectToLayer<CutsceneEffects::LayerTransition>(index, direction, startingPos, speed); },
                                                    "setText", &Scene::setText,
                                                    "setTextPosition", &Scene::setTextPosition,
                                                    "setTextFillColor", &Scene::setText,
                                                    "setTextOutlineColor", &Scene::setTextPosition,
                                                    "setTextSize", &Scene::setTextSize
                                                    );*/
        }
    }
}
