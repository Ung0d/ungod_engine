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

        float getPosX() const;
        float getPosY() const;
        float getSizeX() const;
        float getSizeY() const;
        float getDepth() const;
        const wxString& getName() const;

    private:

        void onOk( wxCommandEvent & event );

        wxDECLARE_EVENT_TABLE();

    private:
        float mPosX;
        float mPosY;
        float mSizeX;
        float mSizeY;
        float mDepth;
        wxString mName;
        wxTextCtrl * mXCtrl;
        wxTextCtrl * mYCtrl;
        wxTextCtrl * mSizeXCtrl;
        wxTextCtrl * mSizeYCtrl;
        wxTextCtrl * mDepthCtrl;
        wxTextCtrl * mNameCtrl;
    };
}

#endif // WORLD_CREATE_DIALOG_H


