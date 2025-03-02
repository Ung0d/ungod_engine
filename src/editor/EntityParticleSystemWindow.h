#ifndef ENTITY_PARTICLE_SYSTEM_PREVIEW_H
#define ENTITY_PARTICLE_SYSTEM_PREVIEW_H

#include "ActionManager.h"
#include "StatDisplayer.h"
#include "Utility.h"
#include "LabeledChoice.h"
#include "EntityPreview.h"
#include "wx/window.h"
#include "wx/choice.h"
#include "wx/notebook.h"
#include "wx/listbox.h"
#include "wx/dialog.h"
#include "wx/radiobut.h"

namespace uedit
{
    class UniversalEmitterPanel;

    class EntityParticleSystemWindow : public wxWindow
    {
    public:
        EntityParticleSystemWindow(ungod::Entity e, ActionManager& actionManager, EntityPreview& preview, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz);

        void refreshStats(); //calling this will reset all values in the stat displays to their currrent correct values+

        //setup methods called once on init
        void setupStatDisplays();
        void setupEmitter();

        //update methods called everytime something changes
        void updateEmitter();
        void updateTexrectInit();
        void updateAffectors();

        //called to update the particle system
        void onParticleEmitterChanged(const std::string& key, const ungod::PSData& data);
        void onTexrectInitChanged(const std::string& key, const ungod::PSData& data);
        void onAffectorsChanged(const std::string& key, const ungod::PSData& data);


    private:
        ungod::Entity mEntity;
        ActionManager& mActionManager;
        EntityPreview& mPreview;
        wxNotebook* mTabs;
        wxBoxSizer* mBox;
        LabeledChoice* mEmitterChoice;
        StatDisplay<float>* mMaxForce;
        StatDisplay<float>* mMaxVel;
        StatDisplay<float>* mSpeed;
        wxPanel* mEmitterPanel;
        wxPanel* mTexrectInitPanel;
        wxPanel* mAffectorsPanel;
    };



    class UniversalEmitterPanel : public wxPanel
    {
    public:
        UniversalEmitterPanel(ungod::Entity e, ActionManager& actionManager, wxWindow* parent);

        void onPosDistChoice(wxCommandEvent& event, ungod::Entity e, ActionManager& actionManager);
        void onVelDistChoice(wxCommandEvent& event, ungod::Entity e, ActionManager& actionManager);
        void onSpawnIntervalChoice(wxCommandEvent& event, ungod::Entity e, ActionManager& actionManager);
        void onLifetimeIntervalChoice(wxCommandEvent& event, ungod::Entity e, ActionManager& actionManager);

    private:
        LabeledChoice* mPosDistChoice;
        LabeledChoice* mVelDistCoice;
        LabeledChoice* mSpawnIntervalChoice;
        LabeledChoice* mLifetimeIntervalChoice;
        wxPanel* mPositionDistPanel;
        wxPanel* mVelocityDistPanel;
        wxPanel* mSpawnIntervalPanel;
        wxPanel* mLifetimeIntervalPanel;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    class AffectorsPanel : public wxPanel
    {
    public:
        AffectorsPanel(ungod::Entity e, ActionManager& actionManager, wxWindow* parent, int sel);

        void updateSelection();

        int getSelection() const { return mSel; }

    private:
        ungod::Entity mEntity;
        ActionManager& mActionManager;
        wxBoxSizer* mVBox;
        wxListBox* mList;
        wxButton* mAddNew;
        wxButton* mRemoveCurrent;
        wxButton* mClearAll;
        wxPanel* mAffectorPanel;
        int mSel;
    };

    class AffectorCreateDialog : public wxDialog
    {
    public:
        enum AffType
        {
            DIRECTIONAL_FORCE,
            DISPLACE_FORCE,
            FADE_OUT,
            FADE_IN,
            ANIMATED_PARTICLES,
            COLOR_SHIFT,
            ROTATE_PARTICLES,
            SCALE_PARTICLES,
            VELOCITY_BASED_ROTATION
        };

    public:
        AffectorCreateDialog(wxWindow* parent);


        AffType getType() const { return mType; }

    private:
        void onOk( wxCommandEvent & event );

    private:
        wxRadioButton* mChoiceDirecForce;
        wxRadioButton* mChoiceDisplForce;
        wxRadioButton* mChoiceFadeOut;
        wxRadioButton* mChoiceFadeIn;
        wxRadioButton* mChoiceAnimatedParticles;
        wxRadioButton* mChoiceColorShift;
        wxRadioButton* mChoiceRotateParticles;
        wxRadioButton* mChoiceScaleParticles;
        wxRadioButton* mChoiceVelocityBasedRotation;
        AffType mType;
    };


    class DirectionalForcePanel : public wxPanel
    {
    public:
        DirectionalForcePanel(ungod::Entity e, ActionManager& actionManager, std::size_t i, wxWindow* parent);


    private:
    };


    class DisplaceForcePanel : public wxPanel
    {
    public:
        DisplaceForcePanel(ungod::Entity e, ActionManager& actionManager, std::size_t i, wxWindow* parent);


    private:
    };


    class FadeOutPanel : public wxPanel
    {
    public:
        FadeOutPanel(ungod::Entity e, ActionManager& actionManager, std::size_t i, wxWindow* parent);


    private:
    };


    class AnimatedParticlesPanel : public wxPanel
    {
    public:
        AnimatedParticlesPanel(ungod::Entity e, ActionManager& actionManager, std::size_t i, wxWindow* parent);


    private:
    };


    class ColorShiftPanel : public wxPanel
    {
    public:
        ColorShiftPanel(ungod::Entity e, ActionManager& actionManager, std::size_t i, wxWindow* parent);


    private:
    };


    class RotateParticlePanel : public wxPanel
    {
    public:
        RotateParticlePanel(ungod::Entity e, ActionManager& actionManager, std::size_t i, wxWindow* parent);


    private:
    };


    class ScaleParticlePanel : public wxPanel
    {
    public:
        ScaleParticlePanel(ungod::Entity e, ActionManager& actionManager, std::size_t i, wxWindow* parent);


    private:
    };


    class VelocityBasedRotationPanel : public wxPanel
    {
    public:
        VelocityBasedRotationPanel(ungod::Entity e, ActionManager& actionManager, std::size_t i, wxWindow* parent);


    private:
    };


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TexrectInitailizerPanel : public wxPanel
    {
    public:
        TexrectInitailizerPanel(ungod::Entity e, ActionManager& actionManager, wxWindow* parent);

        void onTexrectInitChoice(wxCommandEvent& event, ungod::Entity e, ActionManager& actionManager);

    private:
        LabeledChoice* mTexrectInitChoice;
        wxPanel* mTexrectInitPanel;
    };


    class ExplicitTexrectPanel : public wxPanel
    {
    public:
        ExplicitTexrectPanel(ungod::Entity e, ActionManager& actionManager, wxWindow* parent);

    private:
        StatDisplay<int>* mLeft;
        StatDisplay<int>* mTop;
        StatDisplay<int>* mWidth;
        StatDisplay<int>* mHeight;
    };


    class TexrectByKeyPanel : public wxPanel
    {
    public:
        TexrectByKeyPanel(ungod::Entity e, ActionManager& actionManager, wxWindow* parent);

    private:
        wxButton* mSetSelected;
    };


    class MultipleTexrectByKeyPanel : public wxPanel
    {
    public:
        MultipleTexrectByKeyPanel(ungod::Entity e, ActionManager& actionManager, wxWindow* parent);

    private:
        wxButton* mSetSelected;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class FixedPositionDistPanel : public wxPanel
    {
    public:
        FixedPositionDistPanel(ungod::Entity e, ActionManager& actionManager, wxWindow* parent);

    private:
        StatDisplay<float>* mPosX;
        StatDisplay<float>* mPosY;
    };


    class EllipseDistPanel : public wxPanel
    {
    public:
        EllipseDistPanel(ungod::Entity e, ActionManager& actionManager, wxWindow* parent);

    private:
        StatDisplay<float>* mCenterX;
        StatDisplay<float>* mCenterY;
        StatDisplay<float>* mRadiusX;
        StatDisplay<float>* mRadiusY;
    };


    class RectangleDistPanel : public wxPanel
    {
    public:
        RectangleDistPanel(ungod::Entity e, ActionManager& actionManager, wxWindow* parent);

    private:
        StatDisplay<float>* mTopleftX;
        StatDisplay<float>* mTopleftY;
        StatDisplay<float>* mSizeX;
        StatDisplay<float>* mSizeY;
    };


    class LineSegmentDistPanel : public wxPanel
    {
    public:
        LineSegmentDistPanel(ungod::Entity e, ActionManager& actionManager, wxWindow* parent);

    private:
        StatDisplay<float>* mPoint1X;
        StatDisplay<float>* mPoint1Y;
        StatDisplay<float>* mPoint2X;
        StatDisplay<float>* mPoint2Y;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class FixedVelocityDistPanel : public wxPanel
    {
    public:
        FixedVelocityDistPanel(ungod::Entity e, ActionManager& actionManager, wxWindow* parent);

    private:
        StatDisplay<float>* mVelX;
        StatDisplay<float>* mVelY;
    };


    class ConeDistPanel : public wxPanel
    {
    public:
        ConeDistPanel(ungod::Entity e, ActionManager& actionManager, wxWindow* parent);

    private:
        StatDisplay<float>* mDirection1X;
        StatDisplay<float>* mDirection1Y;
        StatDisplay<float>* mDirection2X;
        StatDisplay<float>* mDirection2Y;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    class IntervalTickPanel : public wxPanel
    {
    public:
        IntervalTickPanel(ungod::Entity e, ActionManager& actionManager, wxWindow* parent);

    private:
        StatDisplay<float>* mMin;
        StatDisplay<float>* mMax;
        StatDisplay<int>* mNum;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class IntervalLifetimePanel : public wxPanel
    {
    public:
        IntervalLifetimePanel(ungod::Entity e, ActionManager& actionManager, wxWindow* parent);

    private:
        StatDisplay<float>* mMin;
        StatDisplay<float>* mMax;
    };
}

#endif // ENTITY_PARTICLE_SYSTEM_PREVIEW_H
