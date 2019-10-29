#ifndef UEDIT_EAUDIOEMIT_WINDOW_H
#define UEDIT_EAUDIOEMIT_WINDOW_H

#include "WorldActionWrapper.h"
#include "StatDisplayer.h"
#include "Utility.h"
#include "wx/window.h"
#include "wx/checkbox.h"
#include "wx/filepicker.h"

namespace uedit
{
    class EntityAudioEmitterWindow : public wxWindow
    {
    public:
        EntityAudioEmitterWindow(ungod::Entity e, WorldActionWrapper& waw, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz);


    private:
        ungod::Entity mEntity;
        WorldActionWrapper& mWorldAction;
        wxFilePickerCtrl* mFilepicker;
        StatDisplay<float>* mVolume;
        StatDisplay<float>* mDistanceCap;
        wxCheckBox* mActive;

    private:
        void onActiveChecked(wxCommandEvent& event);
        void refreshStats();
        void onFilepick(wxFileDirPickerEvent& event);
    };
}

#endif // UEDIT_EAUDIOEMIT_WINDOW_H
