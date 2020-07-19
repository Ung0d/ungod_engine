#include "CameraController.h"

namespace uedit
{
    CameraController::CameraController(ungod::Camera& cam) :
        mMouseLastPos(sf::Mouse::getPosition()), mCam(cam) {}

    void CameraController::handleEvent(const sf::Event& curEvent)
    {
        switch (curEvent.type)
        {
            case sf::Event::MouseWheelScrolled:
            {
                if (curEvent.mouseWheelScroll.delta < 0)
                {
                    if (mCam.getZoom() < 10.0f)
                        mCam.setZoom( mCam.getZoom() - curEvent.mouseWheelScroll.delta * 0.1f );
                }
                else
                {
                    if (mCam.getZoom() > 0.1f)
                        mCam.setZoom( mCam.getZoom() - curEvent.mouseWheelScroll.delta * 0.1f );
                }
                break;
            }
            case sf::Event::MouseMoved:
            {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
                {
                   mCam.lookAt( mCam.getCenter() - sf::Vector2f{ (float)(curEvent.mouseMove.x - mMouseLastPos.x) * mCam.getZoom(), (float)(curEvent.mouseMove.y - mMouseLastPos.y) * mCam.getZoom() } );
                }
                mMouseLastPos.x = curEvent.mouseMove.x;
                mMouseLastPos.y = curEvent.mouseMove.y;
                break;
            }
            default:
                break;
        }
    }
}
