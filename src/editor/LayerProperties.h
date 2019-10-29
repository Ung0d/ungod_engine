#ifndef UEDIT_LAYER_PROPERTIES_H
#define UEDIT_LAYER_PROPERTIES_H

#include "ungod/base/World.h"
#include "Utility.h"
#include "wx/frame.h"
#include "wx/stattext.h"
#include "wx/clrpicker.h"
#include "wx/checkbox.h"

namespace uedit
{
    class LayerPropertiesWindow : public wxFrame
    {
    public:
        LayerPropertiesWindow(ungod::World* world,
                              wxWindow * parent, wxWindowID id,
                              const wxPoint & pos = wxDefaultPosition);

    private:
        ungod::World* mWorld;

    private:
        void onAmbientColourPick(wxColourPickerEvent& event);
        void onLightCheck(wxCommandEvent& event);
    };
}

#endif // UEDIT_LAYER_PROPERTIES_H
