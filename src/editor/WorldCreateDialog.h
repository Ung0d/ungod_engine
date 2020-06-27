#ifndef WORLD_CREATE_DIALOG_H
#define WORLD_CREATE_DIALOG_H

#include "wx/dialog.h"
#include "wx/textctrl.h"
#include "wx/string.h"
#include "wx/filepicker.h"

namespace uedit
{
    class WorldCreateDialog : public wxDialog
    {
    public:

        WorldCreateDialog ( wxWindow * parent, wxWindowID id, const wxString & title,
                      const wxPoint & pos = wxDefaultPosition );

		float getDepth() const { return mDepth; }
		const wxString& getName() const { return mName; }

    private:

        void onOk( wxCommandEvent & event );

        wxDECLARE_EVENT_TABLE();

    private:
        float mDepth;
        wxString mName;
        wxTextCtrl * mDepthCtrl;
        wxTextCtrl * mNameCtrl;
    };

	class WorldGraphNodeCreateDialog : public wxDialog
	{
	public:

		WorldGraphNodeCreateDialog(wxWindow* parent, wxWindowID id, const wxString& title,
			const wxPoint& pos = wxDefaultPosition);

		std::string getIdentifier() const { return mIdentifier; }
		std::string getDatafile() const { return mDatafile; }
		float getPosX() const { return mPosX; }
		float getPosY() const { return mPosY; }
		float getSizeX() const { return mSizeX; }
		float getSizeY() const { return mSizeY; }

	private:

		void onOk(wxCommandEvent& event);

	private:
		std::string mIdentifier;
		std::string mDatafile;
		float mPosX;
		float mPosY;
		float mSizeX;
		float mSizeY;
		wxTextCtrl* mPosXCtrl;
		wxTextCtrl* mPosYCtrl;
		wxTextCtrl* mSizeXCtrl;
		wxTextCtrl* mSizeYCtrl;
		wxTextCtrl* mIdentifierCtrl;
		wxTextCtrl* mDatafileCtrl;
	};
}

#endif // WORLD_CREATE_DIALOG_H


