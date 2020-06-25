#include "ProjectCreateDialog.h"
#include "wx/button.h"
#include "wx/log.h"
#include "wx/stattext.h"
#include "Utility.h"
#include <string>

namespace uedit
{
    //connect events
    wxBEGIN_EVENT_TABLE(ProjectCreateDialog, wxDialog)
        EVT_BUTTON(wxID_OK, ProjectCreateDialog::onOk)
    wxEND_EVENT_TABLE()


    ProjectCreateDialog::ProjectCreateDialog ( wxWindow * parent, wxWindowID id, const wxString & title,
                           const wxPoint & position )
    : wxDialog( parent, id, title, position)
    {
        wxSizer* boxsizer = new wxBoxSizer(wxVERTICAL);

        wxGridSizer* gridSizer = new wxGridSizer(2,2,10,10);

        mProjectPicker = new wxFilePickerCtrl( this, -1, fileDialogHeader("xml"), wxFileSelectorPromptStr, fileDialogWildcard("xml"), wxDefaultPosition, wxDefaultSize, wxFLP_SAVE  );
        gridSizer->Add(new wxStaticText( this, -1, "Project file" ), 0, wxALIGN_CENTER_VERTICAL);
        gridSizer->Add(mProjectPicker, 0, wxALIGN_CENTER_VERTICAL);

        mGameMasterPicker = new wxFilePickerCtrl( this, -1, fileDialogHeader("xml"), wxFileSelectorPromptStr, fileDialogWildcard("xml"), wxDefaultPosition, wxDefaultSize, wxFLP_SAVE  );
        gridSizer->Add(new wxStaticText(this, -1, "Game master file"), 0, wxALIGN_CENTER_VERTICAL);
        gridSizer->Add(mGameMasterPicker, 0, wxALIGN_CENTER_VERTICAL);

        wxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
        buttonSizer->Add(new wxButton(this, wxID_OK, _("Create")), 0, wxALIGN_CENTER_VERTICAL, 8);
        buttonSizer->Add(new wxButton(this, wxID_CANCEL, _("Cancel")), 0, wxALIGN_CENTER_VERTICAL, 8);

        boxsizer->Add(gridSizer, 0, wxALIGN_CENTER_HORIZONTAL, 30);
        boxsizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 30);

        SetSizer(boxsizer);
        Layout();
        Fit();
    }

    void ProjectCreateDialog::onOk( wxCommandEvent & event )
    {
        auto projectFilename = mProjectPicker->GetFileName();
        auto masterFilename = mGameMasterPicker->GetFileName();
        projectFilename.MakeRelativeTo();
        masterFilename.MakeRelativeTo();
        mProjectFilepath = projectFilename.GetFullPath();
        mGameMasterFilepath = masterFilename.GetFullPath();
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



