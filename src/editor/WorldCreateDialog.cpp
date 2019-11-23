#include "WorldCreateDialog.h"
#include "wx/button.h"
#include "wx/log.h"
#include "Utility.h"
#include <string>
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "ungod/base/Logger.h"

namespace uedit
{
    //connect events
    wxBEGIN_EVENT_TABLE(WorldCreateDialog, wxDialog)
        EVT_BUTTON(wxID_OK, WorldCreateDialog::onOk)
    wxEND_EVENT_TABLE()

    WorldCreateDialog::WorldCreateDialog ( wxWindow * parent, wxWindowID id, const wxString & title,
                           const wxPoint & position )
    : wxDialog( parent, id, title, position), mDepth(1.0f)
    {
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
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
            mDepth = std::stof( std::string(mDepthCtrl->GetLineText(0).mb_str()) );
            mName = mNameCtrl->GetLineText(0);

            EndModal(wxID_OK);
        }
        catch(const std::exception& e)
        {
            EndModal(wxID_CANCEL);
        }
    }

	WorldGraphNodeCreateDialog::WorldGraphNodeCreateDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos)
		: wxDialog(parent, id, title, pos), mPosX(0), mPosY(0), mSizeX(0), mSizeY(0)
	{
		wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
		{
			wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
			hbox->Add(new wxStaticText(this, -1, "position x:"), 1, wxCENTER);
			mPosXCtrl = new wxTextCtrl(this, -1, _("0"));
			hbox->Add(mPosXCtrl, 1, wxCENTER);
			vbox->Add(hbox, 1, wxCENTER);
		}
		{
			wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
			hbox->Add(new wxStaticText(this, -1, "position y:"), 1, wxCENTER);
			mPosYCtrl = new wxTextCtrl(this, -1, _("0"));
			hbox->Add(mPosYCtrl, 1, wxCENTER);
			vbox->Add(hbox, 1, wxCENTER);
		}
		{
			wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
			hbox->Add(new wxStaticText(this, -1, "width:"), 1, wxCENTER);
			mSizeXCtrl = new wxTextCtrl(this, -1, _("1000"));
			hbox->Add(mSizeXCtrl, 1, wxCENTER);
			vbox->Add(hbox, 1, wxCENTER);
		}
		{
			wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
			hbox->Add(new wxStaticText(this, -1, "height:"), 1, wxCENTER);
			mSizeYCtrl = new wxTextCtrl(this, -1, _("1000"));
			hbox->Add(mSizeYCtrl, 1, wxCENTER);
			vbox->Add(hbox, 1, wxCENTER);
		}
		{
			wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
			hbox->Add(new wxStaticText(this, -1, "identifier:"), 1, wxCENTER);
			mIdentifierCtrl = new wxTextCtrl(this, -1, _("identifier"));
			hbox->Add(mIdentifierCtrl, 1, wxCENTER);
			vbox->Add(hbox, 1, wxCENTER);
		}
		{
			wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
			hbox->Add(new wxStaticText(this, -1, "datafile:"), 1, wxCENTER);
			mDatafileCtrl = new wxTextCtrl(this, -1, _("datafile"));
			hbox->Add(mDatafileCtrl, 1, wxCENTER);
			vbox->Add(hbox, 1, wxCENTER);
		}
		{
			wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
			wxButton* okBut = new wxButton(this, wxID_OK, _("Create"));
			okBut->Bind(wxEVT_BUTTON, &WorldGraphNodeCreateDialog::onOk, this);
			hbox->Add(okBut, 1, wxCENTER);
			hbox->Add(new wxButton(this, wxID_CANCEL, _("Cancel")), 1, wxCENTER);
			vbox->Add(hbox, 1, wxCENTER);
		}
		SetSizer(vbox);
		Fit();
	}

	void WorldGraphNodeCreateDialog::onOk(wxCommandEvent& event)
	{
		try
		{
			mPosX = std::stof(std::string(mPosXCtrl->GetLineText(0).mb_str()));
			mPosY = std::stof(std::string(mPosYCtrl->GetLineText(0).mb_str()));
			mSizeX = std::stof(std::string(mSizeXCtrl->GetLineText(0).mb_str()));
			mSizeY = std::stof(std::string(mSizeYCtrl->GetLineText(0).mb_str()));
			mIdentifier = mIdentifierCtrl->GetLineText(0).mb_str();
			mDatafile = mDatafileCtrl->GetLineText(0).mb_str();

			EndModal(wxID_OK);
		}
		catch (const std::exception & e)
		{
			EndModal(wxID_CANCEL);
		}
	}
}


