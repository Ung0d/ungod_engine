#ifndef UEDIT_RENDER_AREA_H
#define UEDIT_RENDER_AREA_H

#include <SFML/Graphics.hpp>
#include "wx/control.h"
#include "ungod/visual/Camera.h"

namespace uedit
{
    /** \brief An area in the gui that renders ungod::stuff. */
    class RenderArea : public wxControl
    {
    public :
        RenderArea(wxWindow* parent = nullptr,
                      wxWindowID id = -1,
                      const wxPoint& cPosition = wxDefaultPosition,
                      const wxSize& cSize = wxDefaultSize,
                      long cStyle = 0);

        virtual ~RenderArea() { }

        virtual void handleInput(const sf::Event& event) {}
        virtual void update(float delta) {}
        virtual void render(sf::RenderWindow& window, sf::RenderStates states) {}

        const sf::RenderWindow& getWindow() const;

        /** \brief Returns the position of the mouse in the render areas coordinate system. */
        sf::Vector2f getMousePosition() const;

    protected:
        sf::RenderWindow mWindow;
        sf::Clock mTimer;
        ungod::Camera mCamera;

    private :
        void onIdle(wxIdleEvent& idleEvent);
        void onPaint(wxPaintEvent& paintEvent);
        void onSizeChanged(wxSizeEvent& sizeEvent);

    public:
        wxDECLARE_EVENT_TABLE();
    };
}

#endif // UEDIT_RENDER_AREA_H
