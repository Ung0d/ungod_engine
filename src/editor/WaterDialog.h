#ifndef WATER_DIALOG_H
#define WATER_DIALOG_H

#include "wx/dialog.h"
#include "wx/textctrl.h"
#include "wx/string.h"
#include "wx/filepicker.h"

namespace uedit
{
    class WaterDialog : public wxDialog
    {
    public:

        WaterDialog ( wxWindow * parent, wxWindowID id );

        const wxString& getDistortionMapID() const;
        const wxString& getVertexShaderID() const;
        const wxString& getFragmentShaderID() const;

    private:

        void onOk( wxCommandEvent & event );

        wxDECLARE_EVENT_TABLE();

    private:
        wxString mDistortionID;
        wxString mVertexID;
        wxString mFragmentID;
        wxFilePickerCtrl * mDistortionPicker;
        wxFilePickerCtrl * mVertexPicker;
        wxFilePickerCtrl * mFragmentPicker;
    };
}

#endif // WATER_DIALOG_H

