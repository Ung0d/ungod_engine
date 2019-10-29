#ifndef UEDIT_ENTITY_SCRIPT_WINDOW_H
#define UEDIT_ENTITY_SCRIPT_WINDOW_H

#include "WorldActionWrapper.h"
#include "ScriptManager.h"
#include "Utility.h"
#include "wx/window.h"
#include "wx/combobox.h"
#include "wx/stattext.h"
#include "wx/panel.h"

namespace uedit
{
    enum ScriptWindowIDs
    {
        SCRIPT_WINDOW_COMBOBOX
    };

    class EntityScriptWindow : public wxWindow
    {
    public:
        EntityScriptWindow(ungod::Entity e, WorldActionWrapper& waw, ScriptManager& sm, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz);

    private:
        ungod::Entity mEntity;
        WorldActionWrapper& mWorldAction;
        ScriptManager& mScriptManager;
        wxComboBox* mScriptsComboBox;

    private:
        void onScriptLoad(wxCommandEvent& event);
        void onScriptSelected(wxCommandEvent& event);
    };
}

#endif // UEDIT_ENTITY_SCRIPT_WINDOW
