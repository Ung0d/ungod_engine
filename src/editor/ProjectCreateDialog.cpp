#include "ProjectCreateDialog.h"
#include "wx/button.h"
#include "wx/log.h"
#include "Utility.h"
#include <string>

namespace uedit
{
    //connect events
    wxBEGIN_EVENT_TABLE(ProjectCreateDialog, wxDialog)
        EVT_BUTTON(wxID_OK, ProjectCreateDialog::onOk)
    wxEND_EVENT_TABLE()

    const wxSize ProjectCreateDialog::DEFAULT_SIZE = {210, 420};

    ProjectCreateDialog::ProjectCreateDialog ( wxWindow * parent, wxWindowID id, const wxString & title,
                           const wxPoint & position )
    : wxDialog( parent, id, title, position, DEFAULT_SIZE, wxDEFAULT_DIALOG_STYLE)
    {
        wxPoint p;
        p.x = 6; p.y = 2;
        mProjectPicker = new wxFilePickerCtrl( this, -1, fileDialogHeader("xml"), wxFileSelectorPromptStr, fileDialogWildcard("xml"), p, wxDefaultSize, wxFLP_SAVE  );
        p.y += 30;
        mGameMasterPicker = new wxFilePickerCtrl( this, -1, fileDialogHeader("xml"), wxFileSelectorPromptStr, fileDialogWildcard("xml"), p, wxDefaultSize, wxFLP_SAVE  );
        p.y += 60;
        new wxButton( this, wxID_OK, _("Create"), p, wxDefaultSize );
        p.x += 110;
        new wxButton( this, wxID_CANCEL, _("Cancel"), p, wxDefaultSize );
    }

    void ProjectCreateDialog::onOk( wxCommandEvent & event )
    {
        mProjectFilepath = mProjectPicker->GetPath();
        mGameMasterFilepath = mGameMasterPicker->GetPath();
        EndModal(wxID_OK);
    }

    const wxString& ProjectCreateDialog::getProjectFilepath() const
    {
        return mProjectFilepath;
    }

    const wxString& ProjectCreateDialog::getGameMasterFilepath() const
    {
        return mGameMasterFilepath;
    }
}



