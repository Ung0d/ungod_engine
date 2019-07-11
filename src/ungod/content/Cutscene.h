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

#ifndef UNGOD_CUTSCENE_H
#define UNGOD_CUTSCENE_H

#include <future>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <utility>
#include <memory>
#include "sfml_ext/BigSprite.hpp"
#include "sfml_ext/BigTexture.hpp"
#include "ungod/audio/Audio.h"

namespace ungod
{
    class Scene;

    namespace detail
    {
        //all data used by a scene that needs to be loaded by the loader
        struct SceneData
        {
            std::vector<sf::BigTexture> textures;
            std::future<void> status;
        };

        //uses double buffering to provide current data and load next simultaneously
        class SceneDataLoader
        {
        public:
            SceneDataLoader();

            const SceneData& getCurrent() const;

            bool isNextReady() const;

            void loadFirst(const Scene* scene);

            void loadNext(const Scene* scene);

            void swapBuffers() { mBufferingSwitch = !mBufferingSwitch; }

        private:
            std::array<SceneData, 2> mDataBuffers; //<one data is used to display the current scene, the other to load the data for the next scene in
            bool mBufferingSwitch;

        private:
                void load(const Scene* scene, bool bufferSwitch);
        };
    }

    namespace CutsceneEffects
    {
        class LayerEffectBase
        {
        public:
            virtual void init(sf::BigSprite& sprite) {}
            virtual void apply(sf::BigSprite& sprite, float delta) = 0;
        };

        class LayerTransition : public LayerEffectBase
        {
        public:
            LayerTransition(const sf::Vector2f& direction, const sf::Vector2f& startingPos, float speed) :
                mStartingPosition(startingPos), mDirection(direction), mSpeed(speed) {}

            virtual void init(sf::BigSprite& sprite) override;
            virtual void apply(sf::BigSprite& sprite, float delta) override;

        private:
            sf::Vector2f mStartingPosition;
            sf::Vector2f mDirection;
            float mSpeed;
        };
    }

    /** \brief A scene is the building block of a cutscene. Each scene can consist of several layers.
    * Effects like movement can be applied to each layer individually.
    */
    class Scene
    {
    friend class ungod::detail::SceneDataLoader;
    friend class Cutscene;
    public:
        Scene(unsigned duration) : mDuration(duration) {}

        /** \brief Adds a new layer to the scene given a texture. This texture may be larger than the
        * devices internal tex size limit. */
        void addLayer(const std::string& texture);

        /** \brief Adds a new effect to the layer with given index. The index of the layer is defined by the order
        * of "addLayer" calls starting at 0. Multiple effects may be applied to the same layer. */
        template<typename EFFECT, typename ... PARAM>
        void addEffectToLayer(unsigned index, PARAM&& ... param);

        /** \brief Applies effects to all layers. */
        void applyEffects(float delta);

        /** \brief Renders the scene. */
        void render(sf::RenderTarget& target, sf::RenderStates states) const;

    private:
        unsigned mDuration;
        std::vector<std::pair<sf::BigSprite, std::string>> mLayers;
        std::vector<std::pair<std::unique_ptr<CutsceneEffects::LayerEffectBase>, unsigned>> mEffects;
    };

    /** \brief A cutscene is a stack of scenes that are invoked subsequently.
    * Each scene has a user defined duration until the next scene occurs.
    * Scene transitions can be customized with special effects (fading).
    * Scenes are streamed. On play, only the first scene is loaded into memory and subsequent scenes
    * are loaded async during the runtime of the current scene.
    */
    class Cutscene
    {
    public:
        Cutscene() = default;

        /** \brief Creates a new scene given its duration in ms. */
        Scene& addScene(unsigned duration);

        /** \brief Plays the cutscene.*/
        void play();

        /** \brief Queries whether the cutscene is currently playing. */
        bool isPlaying() const { return mPlaying; }

        /** \brief Updates the current scenes effects. Returns true if and only if a new scene was started as the result of the update or the cutscene has ended. */
        bool update(float delta);

        /** \brief Renders the current scene. */
        void render(sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief If the screen size changes, this changes the size of the internal layers. */
        void setScreenSize(const sf::Vector2u& screensize);

        /** \brief Returns a reference to the underlying audio manager. */
        AudioManager& getAudioManager() {return mAudioManager;}
        const AudioManager& getAudioManager() const {return mAudioManager;}

        /** \brief Returns the index of the current active scene. */
        unsigned getSceneIndex() const { return mCurrent; }

    private:
        std::vector<Scene> mScenes;
        bool mPlaying;
        unsigned mCurrent;
        sf::Clock mTimer;
        detail::SceneDataLoader mDataLoader;
        AudioManager mAudioManager;

    private:
        void setTextures();
    };





    template<typename EFFECT, typename ... PARAM>
    void Scene::addEffectToLayer(unsigned index, PARAM&& ... param)
    {
        mEffects.emplace_back(std::unique_ptr<CutsceneEffects::LayerEffectBase>(new EFFECT(std::forward<PARAM>(param)...)), index);
    }
}

#endif // UNGOD_CUTSCENE_H
