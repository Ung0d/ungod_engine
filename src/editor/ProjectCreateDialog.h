#ifndef UEDIT_PROJECT_CREATE_DIALOG_H
#define UEDIT_PROJECT_CREATE_DIALOG_H

#include "wx/dialog.h"
#include "wx/textctrl.h"
#include "wx/string.h"
#include "wx/filepicker.h"

namespace uedit
{
    class ProjectCreateDialog : public wxDialog
    {
    public:

        ProjectCreateDialog ( wxWindow * parent, wxWindowID id, const wxString & title,
                      const wxPoint & pos = wxDefaultPosition );

        const wxString& getProjectFilepath() const;
        const wxString& getGameMasterFilepath() const;

    private:

        wxString mProjectFilepath;
        wxString mGameMasterFilepath;
        wxFilePickerCtrl * mProjectPicker;
        wxFilePickerCtrl * mGameMasterPicker;

        void onOk( wxCommandEvent & event );

        wxDECLARE_EVENT_TABLE();

        static const wxSize DEFAULT_SIZE;
    };
}

#endif // PROJECT_CREATE_DIALOG_H



