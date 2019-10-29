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

        const wxString& getSheetID() const;
        const wxString& getMetaID() const;
        const wxString& getDistortionMapID() const;
        const wxString& getVertexShaderID() const;
        const wxString& getFragmentShaderID() const;
        unsigned getTileWidth() const;
        unsigned getTileHeight() const;
        unsigned getMapWidth() const;
        unsigned getMapHeight() const;

    private:

        void onOk( wxCommandEvent & event );

        wxDECLARE_EVENT_TABLE();

    private:
        wxString mSheetID;
        wxString mMetaID;
        wxString mDistortionID;
        wxString mVertexID;
        wxString mFragmentID;
        unsigned mTileWidth;
        unsigned mTileHeight;
        unsigned mMapWidth;
        unsigned mMapHeight;
        wxFilePickerCtrl * mSheetPicker;
        wxFilePickerCtrl * mMetaPicker;
        wxFilePickerCtrl * mDistortionPicker;
        wxFilePickerCtrl * mVertexPicker;
        wxFilePickerCtrl * mFragmentPicker;
        wxTextCtrl * mTileWidthCtrl;
        wxTextCtrl * mTileHeightCtrl;
        wxTextCtrl * mMapWidthCtrl;
        wxTextCtrl * mMapHeightCtrl;
    };
}

#endif // WATER_DIALOG_H

