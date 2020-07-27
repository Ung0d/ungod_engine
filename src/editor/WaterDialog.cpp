#include "WaterDialog.h"
#include "wx/button.h"
#include "wx/log.h"
#include "Utility.h"

namespace uedit
{
    //connect events
    wxBEGIN_EVENT_TABLE(WaterDialog, wxDialog)
        EVT_BUTTON(wxID_OK, WaterDialog::onOk)
    wxEND_EVENT_TABLE()

    WaterDialog::WaterDialog ( wxWindow * parent, wxWindowID id )
    : wxDialog( parent, id, "init water", wxDefaultPosition)
    {
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        mDistortionPicker = new wxFilePickerCtrl( this, -1, fileDialogHeader("png"), wxFileSelectorPromptStr, fileDialogWildcard("png")  );
        vbox->Add(mDistortionPicker, 1, wxEXPAND);
        mVertexPicker = new wxFilePickerCtrl( this, -1, fileDialogHeader("vert"), wxFileSelectorPromptStr, fileDialogWildcard("vert")  );
        vbox->Add(mVertexPicker, 1, wxEXPAND);
        mFragmentPicker = new wxFilePickerCtrl( this, -1, fileDialogHeader("frag"), wxFileSelectorPromptStr, fileDialogWildcard("frag")  );
        vbox->Add(mFragmentPicker, 1, wxEXPAND);

        wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
        hbox->Add(new wxButton( this, wxID_OK, _("Load")),1,wxCENTER);
        hbox->Add(new wxButton( this, wxID_CANCEL, _("Cancel")),1,wxCENTER);
        vbox->Add(hbox, 1, wxEXPAND);

        SetSizer(vbox);
        Fit();
        Layout();
        Center();
    }

    void WaterDialog::onOk( wxCommandEvent & event )
    {
        mDistortionID = mDistortionPicker->GetPath();
        mVertexID = mVertexPicker->GetPath();
        mFragmentID = mFragmentPicker->GetPath();

        EndModal(wxID_OK);
    }

    const wxString& WaterDialog::getDistortionMapID() const
    {
        return mDistortionID;
    }

    const wxString& WaterDialog::getVertexShaderID() const
    {
        return mVertexID;
    }

    const wxString& WaterDialog::getFragmentShaderID() const
    {
        return mFragmentID;
    }
}

