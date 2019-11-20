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

#ifndef UNGOD_AUDIO_LISTENER_H
#define UNGOD_AUDIO_LISTENER_H

#include <SFML/System/Vector2.hpp>

namespace ungod
{
    class Camera;
	class World;

    /** \brief Defines a listener object that is used by sound effects to determine the world position of a listener.
    * The volume of these sounds is then adapted according to the distance to the listener. */
    class AudioListener
    {
    public:
        /** \brief Returns a volume scaling in range [0,1] according to the given audio emission point. */
        virtual float getScaling(const sf::Vector2f audioEmission, float cap) const = 0;

        /** \brief Returns the world position of the listener. */
        virtual sf::Vector2f getWorldPosition() const = 0;
    };


    /** \brief "Default" listener object that defines an "everywhere" listener that has minimum distance to an arbitrary audio emission point. */
    class GlobalListener : public AudioListener
    {
    public:
        /** \brief Returns a volume scaling in range [0,1] according to the given audio emission point. */
        virtual float getScaling(const sf::Vector2f audioEmission, float cap) const override;

        /** \brief Returns {0,0}. */
        virtual sf::Vector2f getWorldPosition() const override;
    };


    /** \brief An audio listener that scales audio volume according to the camera position. */
    class CameraListener : public AudioListener
    {
    public:
		CameraListener(const Camera& camera, const World& world) : mCamera(camera), mWorld(world) {}

        /** \brief Returns a volume scaling in range [0,1] according to the given audio emission point. */
        virtual float getScaling(const sf::Vector2f audioEmission, float cap) const override;

        /** \brief Returns the world position of the listener. */
        virtual sf::Vector2f getWorldPosition() const override;

    private:
        const Camera& mCamera;
		const World& mWorld;
    };
}

#endif // UNGOD_AUDIO_LISTENER_H
