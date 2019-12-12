#ifndef UEDIT_ENTITY_COLLIDERS_WINDOW_H
#define UEDIT_ENTITY_COLLIDERS_WINDOW_H

#include <wx/window.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include "WorldActionWrapper.h"
#include "StatDisplayer.h"

namespace uedit
{
    template<std::size_t CONTEXT>
    class EntityCollidersWindow : public wxWindow
    {
    public:
        EntityCollidersWindow(ungod::Entity e, WorldActionWrapper& waw, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz);


        void onColliderChanged(ungod::Entity e);

        void displayRigidbody(int index);

        void updateRigidbodyList();

    private:
        ungod::Entity mEntity;
        WorldActionWrapper& mWaw;
        int mSelection;
        wxChoice* mRigidbodyChoice;
        wxListCtrl* mRigidbodies;
        wxPanel* mColliderDetail;
        wxTextCtrl* mCompCountCtrl;

    private:
        void onRigidbodyListSelect(wxListEvent& event);
        void onSetColliderCountClicked(wxCommandEvent& event);
    };


    template<std::size_t CONTEXT>
    class RectangleWindow : public wxPanel
    {
    public:
        RectangleWindow(ungod::Entity e, const ungod::Collider& c, wxWindow* parent, WorldActionWrapper& waw, int multiIndex = -1);

    private:
        ungod::RotatedRectConstAggregator mRect;
        StatDisplay<float>* mUpLeftX;
        StatDisplay<float>* mUpLeftY;
        StatDisplay<float>* mDownRightX;
        StatDisplay<float>* mDownRightY;
        StatDisplay<float>* mRotation;
        int mMulti;
    };


    template<std::size_t CONTEXT>
    class PointSetWindow : public wxPanel
    {
    public:
        PointSetWindow(ungod::Entity e, const ungod::Collider& c, wxWindow* parent, WorldActionWrapper& waw, int multiIndex = -1);

    private:
        ungod::PointSetConstAggregator mPointSet;
        std::vector<StatDisplay<float>*> mPointsX;
        std::vector<StatDisplay<float>*> mPointsY;
        int mMulti;
    };
}

#include "EntityCollidersWindow.inl"

#endif // UEDIT_ENTITY_COLLIDERS_WINDOW_H
