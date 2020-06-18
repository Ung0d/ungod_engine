#ifndef UEDIT_ENTITY_COLLIDERS_WINDOW_H
#define UEDIT_ENTITY_COLLIDERS_WINDOW_H

#include <wx/window.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include "ActionManager.h"
#include "StatDisplayer.h"

namespace uedit
{
    class BaseColliderDisplay : public wxPanel
    {
    public:
        BaseColliderDisplay(wxWindow* parent) : wxPanel(parent) {}

        virtual void refresh() {}

        virtual ungod::ColliderType getType() const { return ungod::ColliderType::UNDEFINED; }
    };


    template<std::size_t CONTEXT>
    class EntityCollidersWindow : public wxWindow
    {
    public:
        EntityCollidersWindow(ungod::Entity e, ActionManager& actionManager, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz);


        void onColliderChanged(ungod::Entity e);

        void displayRigidbody(int index, bool single);

        void updateRigidbodyList();

        ungod::RigidbodyComponent<CONTEXT>* getActiveComponent() const;

    private:
        ungod::Entity mEntity;
        ActionManager& mActionManager;
        int mSelection;
        bool mSingleSelected;
        wxChoice* mRigidbodyChoice;
        wxListCtrl* mRigidbodies;
        BaseColliderDisplay* mColliderDetail;
        wxTextCtrl* mCompCountCtrl;

    private:
        void onRigidbodyListSelect(wxListEvent& event);
        void onSetColliderCountClicked(wxCommandEvent& event);
    };


    template<std::size_t CONTEXT>
    class RectangleWindow : public BaseColliderDisplay
    {
    public:
        RectangleWindow(ungod::Entity e, const ungod::Collider& c, wxWindow* parent, ActionManager& actionManager, int multiIndex = -1);

        virtual void refresh() override;

        virtual ungod::ColliderType getType() const { return ungod::ColliderType::ROTATED_RECT; }

    private:
        ungod::Entity mEntity;
        ungod::RotatedRectConstAggregator mRect;
        StatDisplay<float>* mUpLeftX;
        StatDisplay<float>* mUpLeftY;
        StatDisplay<float>* mDownRightX;
        StatDisplay<float>* mDownRightY;
        StatDisplay<float>* mRotation;
        int mMulti;
        ActionManager& mActionManager;
    };


    template<std::size_t CONTEXT>
    class PointSetWindow : public BaseColliderDisplay
    {
    public:
        PointSetWindow(ungod::Entity e, const ungod::Collider& c, wxWindow* parent, ActionManager& actionManager, int multiIndex = -1);

        virtual void refresh() override;

        virtual ungod::ColliderType getType() const { return mType; }

    private:
        ungod::Entity mEntity;
        ungod::PointSetConstAggregator mPointSet;
        std::vector<StatDisplay<float>*> mPointsX;
        std::vector<StatDisplay<float>*> mPointsY;
        int mMulti;
        ActionManager& mActionManager;
        wxBoxSizer* mVbox;
        ungod::ColliderType mType;


        void addPoint(unsigned i);
    };
}

#include "EntityCollidersWindow.inl"

#endif // UEDIT_ENTITY_COLLIDERS_WINDOW_H
