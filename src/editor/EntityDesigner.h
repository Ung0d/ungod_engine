#ifndef UEDIT_ENTITY_DESIGNER_H
#define UEDIT_ENTITY_DESIGNER_H

#include "ungod/base/Entity.h"
#include "EntityPreview.h"
#include "EntityTransformWindow.h"
#include "EntityVisualsWindow.h"
#include "EntityCollidersWindow.h"
#include "EntityScriptWindow.h"
#include "EntityTileMapWindow.h"
#include "EntityLightWindow.h"
#include "EntityParticleSystemWindow.h"
#include "EntityAudioEmitterWindow.h"
#include "EntityCollidersWindow.h"
#include "ActionManager.h"
#include "Canvas.h"
#include "wx/dialog.h"
#include "wx/frame.h"
#include "wx/checklst.h"
#include "wx/notebook.h"
#include "wx/string.h"
#include "wx/msgdlg.h"

namespace uedit
{
    class EntityDesigner : public wxFrame
    {
    public:

        EntityDesigner ( EditorCanvas& canvas, ActionManager& actionManager, ungod::Entity e,
                      wxWindow * parent, wxWindowID id,
                      const wxPoint & pos = wxDefaultPosition );

        void onEntityContentsChanged(ungod::Entity e, ungod::World& world);
        void onEntityPositionChanged(ungod::Entity e, ungod::World& world, const sf::Vector2f& position);
        void onEntitySizeChanged(ungod::Entity e, ungod::World& world, const sf::Vector2f& siz);
        void onParticleEmitterChanged(ungod::Entity e, ungod::World& world, const std::string& key, const ungod::PSData& data);
        void onTexrectInitChanged(ungod::Entity e, ungod::World& world, const std::string& key, const ungod::PSData& data);
        void onAffectorsChanged(ungod::Entity e, ungod::World& world, const std::string& key, const ungod::PSData& data);
        void onColliderChanged(ungod::Entity e, ungod::World& world);

        ungod::Entity getEntity() const { return mEntity; }

        EntityPreview* getEntityPreview() { return mEntityPreview; }

    private:

        void onComponentCheck( wxCommandEvent& event );

        wxDECLARE_EVENT_TABLE();

        //helper method for optional components to be registered in the checklistbox
        template<typename C>
        void checkOptional(wxString name, std::vector<std::size_t>& checkLater)
        {
            if (mEntity.getInstantiation()->checkOptional<C>())
            {
                if (mEntity.has<C>())
                    checkLater.emplace_back(mComponentNames.size());
                mComponentNames.emplace_back(name);
                mAddCallbacks.emplace_back([this] () { mEntity.add<C>(); });
                mRemoveCallbacks.emplace_back([this] () { mEntity.rem<C>(); });
            }
        }

        void buildComponentWindow(wxWindow* window);
        void buildSpecWindows();

        void reset();

        void onWindowClose(wxCloseEvent& event);

        void onTabChanged(wxBookCtrlEvent& event);

        void onEditUndo(wxCommandEvent& event);
        void onEditRedo(wxCommandEvent& event);

    private:

        static const wxSize DEFAULT_SIZE;

        EditorCanvas& mCanvas;
        ActionManager& mActionManager;
        ungod::Entity mEntity;
        std::vector<wxString> mComponentNames;
        std::vector<std::function<void()>> mAddCallbacks;
        std::vector<std::function<void()>> mRemoveCallbacks;
        wxNotebook* mTabs;
        wxCheckListBox* mChecker;
        EntityTransformWindow* mTransformOrganizer;
        EntityVisualsWindow* mVisualsOrganizer;
        EntityScriptWindow* mScriptOrganizer;
        EntityTileMapWindow* mTileMapOrganizer;
        EntityWaterWindow* mWaterOrganizer;
        EntityLightWindow* mLightOrganizer;
        EntityParticleSystemWindow* mParticlesOrganizer;
        EntityAudioEmitterWindow* mAudioEmitterOrganizer;
        EntityPreview* mEntityPreview;
        EntityCollidersWindow<ungod::MOVEMENT_COLLISION_CONTEXT>* mMovementCollidersOrganizer;
        EntityCollidersWindow<ungod::SEMANTICS_COLLISION_CONTEXT>* mSemanticsCollidersOrganizer;
    };
}

#endif // UEDIT_ENTITY_DESGINER_H
