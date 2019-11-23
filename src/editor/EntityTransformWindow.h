#ifndef ENTITY_TRANSFORM_PREVIEW_H
#define ENTITY_TRANSFORM_PREVIEW_H

#include "WorldActionWrapper.h"
#include "StatDisplayer.h"
#include "Utility.h"
#include "wx/window.h"
#include "wx/checkbox.h"

namespace uedit
{
    enum VisualsTransformWindowIDs
    {

    };

    class EntityTransformWindow : public wxWindow
    {
    public:
        EntityTransformWindow(ungod::Entity e, WorldActionWrapper& waw, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz);

        void onEntityContentsChanged(ungod::Entity e);
        void onEntityPositionChanged(ungod::Entity e, const sf::Vector2f& pos);
        void onEntitySizeChanged(ungod::Entity e, const sf::Vector2f& s);

    private:
        ungod::Entity mEntity;
        WorldActionWrapper& mWorldAction;
        StatDisplay<float>* mPosX;
        StatDisplay<float>* mPosY;
        StatDisplay<float>* mScaleX;
        StatDisplay<float>* mScaleY;
        StatDisplay<float>* mBaseLineOffsetX;
        StatDisplay<float>* mBaseLineOffsetY;

    private:
        void refreshStats();

        wxDECLARE_EVENT_TABLE();
    };
}

#endif // ENTITY_TRANSFORM_PREVIEW_H
