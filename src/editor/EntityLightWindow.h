#ifndef ENTITY_LIGHT_WINDOW_H
#define ENTITY_LIGHT_WINDOW_H

#include "WorldActionWrapper.h"
#include "StatDisplayer.h"
#include "MultiComponentPanel.h"
#include "Utility.h"
#include "wx/window.h"
#include "wx/clrpicker.h"
#include "wx/notebook.h"
#include "wx/filepicker.h"
#include "wx/checkbox.h"

namespace uedit
{
    class LightComponentEdit : public wxPanel
    {
    public:
        LightComponentEdit(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent, ungod::LightEmitterComponent& lightComponent);

        void refreshStats();

    private:
        ungod::Entity mEntity;
        WorldActionWrapper& mWorldAction;
        ungod::LightEmitterComponent& mLightComponent;
        wxFilePickerCtrl* mTexPicker;
        StatDisplay<float>* mPosX;
        StatDisplay<float>* mPosY;
        StatDisplay<float>* mScaleX;
        StatDisplay<float>* mScaleY;
        StatDisplay<float>* mOriginX;
        StatDisplay<float>* mOriginY;
        StatDisplay<float>* mRadius;
        wxColourPickerCtrl* mColour;
        StatDisplay<float>* mShadowExtendMultiplier;

    private:
        void onLightTexturePicked(wxFileDirPickerEvent& event);
    };

    class FlickeringDialog : public wxDialog
    {
    public:

        FlickeringDialog ( wxWindow * parent, wxWindowID id, const wxString & title,
                      const wxPoint & pos = wxDefaultPosition );

        float getPeriod() const { return mPeriod; }
        float getStrength() const { return mStrength; }

    private:

        void onOk( wxCommandEvent & event );

        wxDECLARE_EVENT_TABLE();

    private:
        float mPeriod;
        float mStrength;
        wxTextCtrl * mPeriodCtrl;
        wxTextCtrl * mStrengthCtrl;
        static constexpr float DEFAULT_PERIOD = 500.0f;
        static constexpr float DEFAULT_STRENGTH = 1.0f;
    };


    class LightAffectorEdit : public wxPanel
    {
    public:
        LightAffectorEdit(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent, ungod::LightAffectorComponent& affector, ungod::LightEmitterComponent& emitter);

        void refreshStats();

        void choiceChanged(wxCommandEvent& event);

    private:
        ungod::Entity mEntity;
        WorldActionWrapper& mWorldAction;
        ungod::LightAffectorComponent& mAffectorComponent;
        ungod::LightEmitterComponent& mEmitterComponent;
        wxChoice* mChoice;
    };



    class LightColliderEdit : public wxPanel
    {
    public:
        LightColliderEdit(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent, ungod::ShadowEmitterComponent& shadowEmitter);

        void refreshStats();

        ungod::ShadowEmitterComponent const* getComponent() { return &mShadowEmitter; }

    private:
        ungod::Entity mEntity;
        WorldActionWrapper& mWorldAction;
        ungod::ShadowEmitterComponent& mShadowEmitter;
        std::vector<StatDisplay<float>*> mPosX;
        std::vector<StatDisplay<float>*> mPosY;
        std::vector<wxPanel*> mPanels;
        wxBoxSizer* mSizer;
        wxCheckBox* mLightOverShapeCheck;

    private:
        void createPosPanel(unsigned i);
    };



    class EntityLightWindow : public wxWindow
    {
    public:
        EntityLightWindow(ungod::Entity e, WorldActionWrapper& waw, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz);

        void onEntityContentsChanged(ungod::Entity e);

        void refreshStats();

        bool singleColliderActive() const;
        int multiColliderActive() const;

        ungod::ShadowEmitterComponent const* getActiveComponent() const;

    private:
        ungod::Entity mEntity;
        WorldActionWrapper& mWorldAction;
        wxNotebook* mNotebook;

        LightComponentEdit* mLightEdit;
        MultiComponentPanel<ungod::LightEmitterComponent, LightComponentEdit>* mMultiLightPanel;

        LightAffectorEdit* mAffectorEdit;
        MultiComponentPanel<ungod::LightAffectorComponent, LightAffectorEdit, ungod::LightEmitterComponent>* mMultiAffectorPanel;

        LightColliderEdit* mLightColliderEdit;
        MultiComponentPanel<ungod::ShadowEmitterComponent, LightColliderEdit>* mMultiLightColliderPanel;
    };
}

#endif // ENTITY_LIGHT_WINDOW_H

