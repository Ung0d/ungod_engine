#ifndef UEDIT_TILEMAP_DIALOG_H
#define UEDIT_TILEMAP_DIALOG_H

#include "wx/dialog.h"
#include "wx/textctrl.h"
#include "wx/string.h"
#include "wx/filepicker.h"

namespace uedit
{
    class TileMapDialog : public wxDialog
    {
    public:

        TileMapDialog ( wxWindow * parent, wxWindowID id );

        unsigned getTileWidth() const;
        unsigned getTileHeight() const;
        unsigned getMapWidth() const;
        unsigned getMapHeight() const;

    private:

        void onOk( wxCommandEvent & event );

        wxDECLARE_EVENT_TABLE();

    private:
        unsigned mTileWidth;
        unsigned mTileHeight;
        unsigned mMapWidth;
        unsigned mMapHeight;
        wxTextCtrl * mWidthCtrl;
        wxTextCtrl * mHeightCtrl;
        wxTextCtrl * mMapWidthCtrl;
        wxTextCtrl * mMapHeightCtrl;
    };
}

#endif // UEDIT_TILEMAP_DIALOG_H

