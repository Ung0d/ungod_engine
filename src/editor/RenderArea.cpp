#include "RenderArea.h"

namespace uedit
{
    wxBEGIN_EVENT_TABLE( RenderArea, wxControl )
        EVT_IDLE( RenderArea::onIdle )
        EVT_PAINT( RenderArea::onPaint )
        EVT_SIZE( RenderArea::onSizeChanged )
    wxEND_EVENT_TABLE()

    RenderArea::RenderArea(wxWindow* parent, wxWindowID id, const wxPoint& cPosition, const wxSize& cSize, long cStyle)
        : wxControl(parent, id, cPosition, cSize, cStyle), mWindow(GetHandle()), mCamera(mWindow)
    {
    }

    const sf::RenderWindow& RenderArea::getWindow() const
    {
        return mWindow;
    }

    void RenderArea::onIdle(wxIdleEvent& idleEvent)
    {
        Refresh(false);
    }

    void RenderArea::onPaint(wxPaintEvent& paintEvent)
    {
        mWindow.clear();

        if (HasFocus())
        {
            sf::Event event;
            while (mWindow.pollEvent(event))
            {
                handleInput(event);
                mCamera.handleEvent(event);
            }
        }

        update(mTimer.restart().asMilliseconds());

        mCamera.renderBegin();
        render(mWindow, sf::RenderStates{});
        mCamera.renderEnd();

        mWindow.display();
    }

    void RenderArea::onSizeChanged(wxSizeEvent& sizeEvent)
    {
        mWindow.setSize( sf::Vector2u{ (unsigned)sizeEvent.GetSize().x, (unsigned)sizeEvent.GetSize().y } );
        mCamera.setViewSize(sf::Vector2f{(float)sizeEvent.GetSize().x, (float)sizeEvent.GetSize().y});
    }
}
