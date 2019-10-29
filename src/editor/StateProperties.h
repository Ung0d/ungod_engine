#ifndef UEDIT_STATE_PROPERTIES_H
#define UEDIT_STATE_PROPERTIES_H

#include "ungod/application/ScriptedGameState.h"
#include "wx/dialog.h"
#include "wx/checkbox.h"
#include "wx/button.h"
#include "wx/sizer.h"

namespace uedit
{
    class StateProperties : public wxDialog
    {
    public:
        StateProperties(ungod::ScriptedGameState& state,
                        wxWindow * parent, wxWindowID id,
                        const wxPoint & pos = wxDefaultPosition);

    private:
        ungod::ScriptedGameState& mGameState;
        wxCheckBox* mDebugBounds;
        wxCheckBox* mDebugTexrects;
        wxCheckBox* mDebugColliders;
        wxCheckBox* mDebugAudioEmitters;
        wxCheckBox* mDebugLightEmitters;

    private:
        void onOk( wxCommandEvent & event );

        wxDECLARE_EVENT_TABLE();
    };
}

#endif // UEDIT_STATE_PROPERTIES_H
