#include "SheetDialog.h"
#include "wx/button.h"
#include "wx/log.h"
#include "Utility.h"

namespace uedit
{
    //connect events
    wxBEGIN_EVENT_TABLE(SheetDialog, wxDialog)
        EVT_BUTTON(wxID_OK, SheetDialog::onOk)
    wxEND_EVENT_TABLE()

    const wxSize SheetDialog::DEFAULT_SIZE = {210, 150};

    SheetDialog::SheetDialog ( wxWindow * parent, wxWindowID id, const wxString & title,
                           const wxPoint & position )
    : wxDialog( parent, id, title, position, DEFAULT_SIZE, wxDEFAULT_DIALOG_STYLE)
    {
        wxPoint p;
        p.x = 6; p.y = 2;

        mSheetPicker = new wxFilePickerCtrl( this, -1, fileDialogHeader("png"), wxFileSelectorPromptStr, fileDialogWildcard("png"), p  );
        p.y += 30;
        mMetaPicker = new wxFilePickerCtrl( this, -1, fileDialogHeader("xml"), wxFileSelectorPromptStr, fileDialogWildcard("xml"), p  );

        p.y += 60;
        new wxButton( this, wxID_OK, _("Load"), p, wxDefaultSize );
        p.x += 110;
        new wxButton( this, wxID_CANCEL, _("Cancel"), p, wxDefaultSize );
    }

    void SheetDialog::onOk( wxCommandEvent & event )
    {
        auto sheetpath = mSheetPicker->GetFileName();
        auto metapath = mMetaPicker->GetFileName();
        sheetpath.MakeRelativeTo();
        metapath.MakeRelativeTo();
        mSheetID = sheetpath.GetFullPath();
        mMetaID = metapath.GetFullPath();
        EndModal(wxID_OK);
    }

    const wxString& SheetDialog::getSheetID() const
    {
        return mSheetID;
    }

    const wxString& SheetDialog::getMetaID() const
    {
        return mMetaID;
    }
}
