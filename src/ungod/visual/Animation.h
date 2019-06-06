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


#ifndef UNGOD_ANIMATION_H
#define UNGOD_ANIMATION_H

#include <SFML/Graphics.hpp>
#include "ungod/serialization/MetaNode.h"

namespace ungod
{
    class MetaMap;

    class Animation
    {
    public:
        /** \brief Default constructs an Animation. */
        Animation();

        /** \brief Updates the animation and may set the mext image. May also set the running flag to false,
        * if OnTimeOnly is active. */
        void update(float delta, sf::Vertex* vertices);

        /** \brief Returns true if the animation is looping. */
        bool isLooping() const;

        /** \brief Returns true if the animation is currently running. */
        bool isRunning() const;

        /** \brief Sets the running flag. */
        void setRunning(bool running);

        /** \brief Returns the string identifier for the animation state current setup. */
        std::string getKey() const;

        /** \brief Returns the index of the current frame. */
        unsigned getCurrentIndex() const;

        /** \brief Returns the number of frames for the current active animation. */
        unsigned getFrameCount() const;

        /** \brief Sets the frame (by index) of the current active animation. */
        void setFrame(unsigned i, sf::Vertex* vertices);

        /** \brief Returns play speed of the animation. */
        float getSpeed() const;

        /** \brief Sets play speed of the animation. */
        void setSpeed(float speed);

        /** \brief Starts the animation with given key. */
        bool startAnimation(const MetaMap& meta, const std::string& key, sf::Vertex* vertices);

        /** \brief Starts the animation from the appropriate metanode. */
        void startAnimation(MetaNode node, sf::Vertex* vertices);

        /** \brief Returns the bounds of the animation, that is the minimal rect, that totally contains
        * all of the frames. */
        sf::IntRect getBounds() const;

    private:
        void setupFrame(MetaNode previous, sf::Vertex* vertices);

        void nextImage(sf::Vertex* vertices);

    private:
        float mTimer;
        bool mLoop;
        bool mReverse;
        bool mRunning;
        bool mOneTimeOnly;
        unsigned mCurrentIndex;
        MetaNode mCurrentNode;
        float mSpeed;
    };
}

#endif // UNGOD_ANIMATION_H
