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

        mSheetPicker = new wxFilePickerCtrl( this, -1, fileDialogHeader("png"), wxFileSelectorPromptStr, fileDialogWildcard("png")  );
        vbox->Add(mSheetPicker, 1, wxEXPAND);
        mMetaPicker = new wxFilePickerCtrl( this, -1, fileDialogHeader("xml"), wxFileSelectorPromptStr, fileDialogWildcard("xml")  );
        vbox->Add(mMetaPicker, 1, wxEXPAND);

        mDistortionPicker = new wxFilePickerCtrl( this, -1, fileDialogHeader("png"), wxFileSelectorPromptStr, fileDialogWildcard("png")  );
        vbox->Add(mDistortionPicker, 1, wxEXPAND);
        mVertexPicker = new wxFilePickerCtrl( this, -1, fileDialogHeader("vert"), wxFileSelectorPromptStr, fileDialogWildcard("vert")  );
        vbox->Add(mVertexPicker, 1, wxEXPAND);
        mFragmentPicker = new wxFilePickerCtrl( this, -1, fileDialogHeader("frag"), wxFileSelectorPromptStr, fileDialogWildcard("frag")  );
        vbox->Add(mFragmentPicker, 1, wxEXPAND);

        mTileWidthCtrl = new wxTextCtrl(this, -1, _("enter tile width"));
        mTileHeightCtrl = new wxTextCtrl(this, -1, _("enter tile height"));
        vbox->Add(mTileWidthCtrl, 1, wxEXPAND);
        vbox->Add(mTileHeightCtrl, 1, wxEXPAND);

        mMapWidthCtrl = new wxTextCtrl(this, -1, _("enter map width"));
        mMapHeightCtrl = new wxTextCtrl(this, -1, _("enter map height"));
        vbox->Add(mMapWidthCtrl, 1, wxEXPAND);
        vbox->Add(mMapHeightCtrl, 1, wxEXPAND);

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
        mSheetID = mSheetPicker->GetPath();
        mMetaID = mMetaPicker->GetPath();
        mDistortionID = mDistortionPicker->GetPath();
        mVertexID = mVertexPicker->GetPath();
        mFragmentID = mFragmentPicker->GetPath();

        try
        {
            mTileWidth = std::stoi( std::string(mTileWidthCtrl->GetLineText(0).mb_str()) );
            mTileHeight = std::stoi( std::string(mTileHeightCtrl->GetLineText(0).mb_str()) );
            mMapWidth = std::stoi( std::string(mMapWidthCtrl->GetLineText(0).mb_str()) );
            mMapHeight = std::stoi( std::string(mMapHeightCtrl->GetLineText(0).mb_str()) );

            EndModal(wxID_OK);
        }
        catch(const std::exception& e)
        {
            EndModal(wxID_CANCEL);
        }

        EndModal(wxID_OK);
    }

    const wxString& WaterDialog::getSheetID() const
    {
        return mSheetID;
    }

    const wxString& WaterDialog::getMetaID() const
    {
        return mMetaID;
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

    unsigned WaterDialog::getTileWidth() const
    {
        return mTileWidth;
    }

    unsigned WaterDialog::getTileHeight() const
    {
        return mTileHeight;
    }

    unsigned WaterDialog::getMapWidth() const
    {
        return mMapWidth;
    }

    unsigned WaterDialog::getMapHeight() const
    {
        return mMapHeight;
    }
}

