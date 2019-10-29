#ifndef UEDIT_CAM_CONTROLLER_H
#define UEDIT_CAM_CONTROLLER_H

#include <SFML/Window/Event.hpp>
#include "ungod/visual/Camera.h"

namespace uedit
{
    class CameraController
    {
    public:
        CameraController(ungod::Camera& cam);

        void handleEvent(const sf::Event& curEvent);

        ungod::Camera& getCamera() { return mCam; }

    private:
        sf::Vector2i mMouseLastPos;
        ungod::Camera& mCam;
    };
}
 #endif
