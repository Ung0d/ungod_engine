#include "StateProperties.h"

namespace uedit
{
    wxBEGIN_EVENT_TABLE(StateProperties, wxDialog)
        EVT_BUTTON(wxID_OK, StateProperties::onOk)
    wxEND_EVENT_TABLE()

    StateProperties::StateProperties(ungod::ScriptedGameState& state,
                        wxWindow * parent, wxWindowID id,
                        const wxPoint & pos) :
                            wxDialog(parent, id, _("state properties"), pos, wxDefaultSize, wxDEFAULT_DIALOG_STYLE),
                            mGameState(state)
    {
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        {
            mDebugBounds = new wxCheckBox(this, -1, _("debug entity bounds"));
            mDebugBounds->SetValue(state.debugEntityBoundsActive());
            vbox->Add(mDebugBounds, 1, wxCENTER);
        }
        {
            mDebugTexrects = new wxCheckBox(this, -1, _("debug texture rects"));
            mDebugTexrects->SetValue(state.debugTextureRectsActive());
            vbox->Add(mDebugTexrects, 1, wxCENTER);
        }
        {
            mDebugColliders = new wxCheckBox(this, -1, _("debug colliders"));
            mDebugColliders->SetValue(state.debugCollidersActive());
            vbox->Add(mDebugColliders, 1, wxCENTER);
        }
        {
            mDebugAudioEmitters = new wxCheckBox(this, -1, _("debug audio emitters"));
            mDebugAudioEmitters->SetValue(state.debugAudioEmittersActive());
            vbox->Add(mDebugAudioEmitters, 1, wxCENTER);
        }
        {
            mDebugLightEmitters = new wxCheckBox(this, -1, _("debug light emitters"));
            mDebugLightEmitters->SetValue(state.debugLightEmittersActive());
            vbox->Add(mDebugLightEmitters, 1, wxCENTER);
        }

        wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
        hbox->Add(new wxButton( this, wxID_OK, _("Accept"), wxDefaultPosition, wxDefaultSize ), 1, wxCENTER);
        hbox->Add(new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize ), 1, wxCENTER);

        vbox->Add(hbox, 1, wxEXPAND);

        SetSizer(vbox);
        Fit();
        Center();
    }

    void StateProperties::onOk( wxCommandEvent & event )
    {
        mGameState.debugEntityBounds(mDebugBounds->GetValue());
        mGameState.debugTexrects(mDebugTexrects->GetValue());
        mGameState.debugColliders(mDebugColliders->GetValue());
        mGameState.debugAudioEmitters(mDebugAudioEmitters->GetValue());
        mGameState.debugLightEmitters(mDebugLightEmitters->GetValue());
        EndModal(wxID_OK);
    }
}
