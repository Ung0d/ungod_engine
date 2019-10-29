#ifndef SHEET_DIALOG_H
#define SHEET_DIALOG_H

#include "wx/dialog.h"
#include "wx/textctrl.h"
#include "wx/string.h"
#include "wx/filepicker.h"

namespace uedit
{
    class SheetDialog : public wxDialog
    {
    public:

        SheetDialog ( wxWindow * parent, wxWindowID id, const wxString & title,
                      const wxPoint & pos = wxDefaultPosition );

        const wxString& getSheetID() const;

        const wxString& getMetaID() const;

    private:

        void onOk( wxCommandEvent & event );

        wxDECLARE_EVENT_TABLE();

    private:
        wxString mSheetID;
        wxString mMetaID;
        wxFilePickerCtrl * mSheetPicker;
        wxFilePickerCtrl * mMetaPicker;

        static const wxSize DEFAULT_SIZE;
    };
}

#endif // SHEET_DIALOG_H
