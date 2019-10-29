#include "WorldCreateDialog.h"
#include "wx/button.h"
#include "wx/log.h"
#include "Utility.h"
#include <string>
#include "wx/sizer.h"
#include "wx/stattext.h"

namespace uedit
{
    //connect events
    wxBEGIN_EVENT_TABLE(WorldCreateDialog, wxDialog)
        EVT_BUTTON(wxID_OK, WorldCreateDialog::onOk)
    wxEND_EVENT_TABLE()

    WorldCreateDialog::WorldCreateDialog ( wxWindow * parent, wxWindowID id, const wxString & title,
                           const wxPoint & position )
    : wxDialog( parent, id, title, position), mPosX(0), mPosY(0), mSizeX(0), mSizeY(0), mDepth(1.0f)
    {
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        {
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
            hbox->Add(new wxStaticText(this, -1, "map pos x:"), 1, wxCENTER);
            mXCtrl = new wxTextCtrl(this, -1, _("0"));
            hbox->Add(mXCtrl, 1, wxCENTER);
            vbox->Add(hbox, 1, wxCENTER);
        }
        {
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
            hbox->Add(new wxStaticText(this, -1, "map pos y:"), 1, wxCENTER);
            mYCtrl = new wxTextCtrl(this, -1, _("0"));
            hbox->Add(mYCtrl, 1, wxCENTER);
            vbox->Add(hbox, 1, wxCENTER);
        }
        {
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
            hbox->Add(new wxStaticText(this, -1, "map width:"), 1, wxCENTER);
            mSizeXCtrl = new wxTextCtrl(this, -1, _("1000"));
            hbox->Add(mSizeXCtrl, 1, wxCENTER);
            vbox->Add(hbox, 1, wxCENTER);
        }
        {
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
            hbox->Add(new wxStaticText(this, -1, "map height:"), 1, wxCENTER);
            mSizeYCtrl = new wxTextCtrl(this, -1, _("1000"));
            hbox->Add(mSizeYCtrl, 1, wxCENTER);
            vbox->Add(hbox, 1, wxCENTER);
        }
        {
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
            hbox->Add(new wxStaticText(this, -1, "render depth:"), 1, wxCENTER);
            mDepthCtrl = new wxTextCtrl(this, -1, _("1"));
            hbox->Add(mDepthCtrl, 1, wxCENTER);
            vbox->Add(hbox, 1, wxCENTER);
        }
        {
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
            hbox->Add(new wxStaticText(this, -1, "name:"), 1, wxCENTER);
            mNameCtrl = new wxTextCtrl(this, -1, _("world"));
            hbox->Add(mNameCtrl, 1, wxCENTER);
            vbox->Add(hbox, 1, wxCENTER);
        }
        {
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
            hbox->Add(new wxButton( this, wxID_OK, _("Create")), 1, wxCENTER);
            hbox->Add(new wxButton( this, wxID_CANCEL, _("Cancel")), 1, wxCENTER);
            vbox->Add(hbox, 1, wxCENTER);
        }
        SetSizer(vbox);
        Fit();
    }

    void WorldCreateDialog::onOk( wxCommandEvent & event )
    {
        try
        {
            mPosX = std::stof( std::string(mXCtrl->GetLineText(0).mb_str()) );
            mPosY = std::stof( std::string(mYCtrl->GetLineText(0).mb_str()) );
            mSizeX = std::stof( std::string(mSizeXCtrl->GetLineText(0).mb_str()) );
            mSizeY = std::stof( std::string(mSizeYCtrl->GetLineText(0).mb_str()) );
            mDepth = std::stof( std::string(mDepthCtrl->GetLineText(0).mb_str()) );
            mName = mNameCtrl->GetLineText(0);

            EndModal(wxID_OK);
        }
        catch(const std::exception& e)
        {
            EndModal(wxID_CANCEL);
        }
    }

    float WorldCreateDialog::getPosX() const
    {
        return mPosX;
    }

    float WorldCreateDialog::getPosY() const
    {
        return mPosY;
    }

    float WorldCreateDialog::getSizeX() const
    {
        return mSizeX;
    }

    float WorldCreateDialog::getSizeY() const
    {
        return mSizeY;
    }

    float WorldCreateDialog::getDepth() const
    {
        return mDepth;
    }

    const wxString& WorldCreateDialog::getName() const
    {
        return mName;
    }
}


