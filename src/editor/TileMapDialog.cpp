#include "TileMapDialog.h"
#include "wx/button.h"
#include "wx/log.h"
#include "Utility.h"
#include <string>
#include "wx/sizer.h"

namespace uedit
{
    //connect events
    wxBEGIN_EVENT_TABLE(TileMapDialog, wxDialog)
        EVT_BUTTON(wxID_OK, TileMapDialog::onOk)
    wxEND_EVENT_TABLE()

    TileMapDialog::TileMapDialog ( wxWindow * parent, wxWindowID id )
    : wxDialog( parent, id, _("Init tilemap")), mTileWidth(0), mTileHeight(0)
    {
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        mWidthCtrl = new wxTextCtrl(this, -1, _("enter tile width"));
        mHeightCtrl = new wxTextCtrl(this, -1, _("enter tile height"));
        vbox->Add(mWidthCtrl, 1, wxEXPAND);
        vbox->Add(mHeightCtrl, 1, wxEXPAND);

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

    void TileMapDialog::onOk( wxCommandEvent & event )
    {
        try
        {
            mTileWidth = std::stoi( std::string(mWidthCtrl->GetLineText(0).mb_str()) );
            mTileHeight = std::stoi( std::string(mHeightCtrl->GetLineText(0).mb_str()) );
            mMapWidth = std::stoi( std::string(mMapWidthCtrl->GetLineText(0).mb_str()) );
            mMapHeight = std::stoi( std::string(mMapHeightCtrl->GetLineText(0).mb_str()) );

            EndModal(wxID_OK);
        }
        catch(const std::exception& e)
        {
            EndModal(wxID_CANCEL);
        }
    }

    unsigned TileMapDialog::getTileWidth() const
    {
        return mTileWidth;
    }

    unsigned TileMapDialog::getTileHeight() const
    {
        return mTileHeight;
    }

    unsigned TileMapDialog::getMapWidth() const
    {
        return mMapWidth;
    }

    unsigned TileMapDialog::getMapHeight() const
    {
        return mMapHeight;
    }
}

