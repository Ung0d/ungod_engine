#ifndef UEDIT_TILEMAP_WINDOW_H
#define UEDIT_TILEMAP_WINDOW_H

#include "ActionManager.h"
#include "Utility.h"
#include "Canvas.h"
#include "wx/window.h"

namespace uedit
{
    class EntityDesigner;

    class EntityTileMapWindow : public wxWindow
    {
    public:
        EntityTileMapWindow(ungod::Entity e, EntityDesigner* d, ActionManager& actionManager, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz);

    private:
        ungod::Entity mEntity;
        ActionManager& mActionManager;
        EntityDesigner* mDesigner;

    private:
        void onInitClicked(wxCommandEvent & event);
        void onFitClicked(wxCommandEvent& event);
        void onViewReset(wxCommandEvent & event);
    };

    class EntityWaterWindow : public wxWindow
    {
    public:
        EntityWaterWindow(EditorCanvas& canvas, ungod::Entity e, EntityDesigner* d, ActionManager& actionManager, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz);

    private:
        ungod::Entity mEntity;
        ActionManager& mActionManager;
        EntityDesigner* mDesigner;

    private:
        void onInitClicked(wxCommandEvent & event);
        void onViewReset(wxCommandEvent & event);
    };
}

#endif // UEDIT_TILEMAP_WINDOW_H
