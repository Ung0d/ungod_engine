#ifndef UEDIT_TILEMAP_WINDOW_H
#define UEDIT_TILEMAP_WINDOW_H

#include "WorldActionWrapper.h"
#include "Utility.h"
#include "Canvas.h"
#include "wx/window.h"

namespace uedit
{
    class EntityDesigner;

    class EntityTileMapWindow : public wxWindow
    {
    public:
        EntityTileMapWindow(EditorCanvas& canvas, ungod::Entity e, EntityDesigner* d, WorldActionWrapper& waw, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz);

    private:
        ungod::Entity mEntity;
        WorldActionWrapper& mWorldAction;
        EntityDesigner* mDesigner;

    private:
        void onInitClicked(wxCommandEvent & event);
        void onFitClicked(wxCommandEvent& event);
        void onViewReset(wxCommandEvent & event);
    };

    class EntityWaterWindow : public wxWindow
    {
    public:
        EntityWaterWindow(EditorCanvas& canvas, ungod::Entity e, EntityDesigner* d, WorldActionWrapper& waw, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz);

    private:
        ungod::Entity mEntity;
        WorldActionWrapper& mWorldAction;
        EntityDesigner* mDesigner;

    private:
        void onInitClicked(wxCommandEvent & event);
        void onViewReset(wxCommandEvent & event);
    };
}

#endif // UEDIT_TILEMAP_WINDOW_H
