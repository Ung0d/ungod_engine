#include "ActionManager.h"
#include "EditorFrame.h"
#include <wx/msgdlg.h>

namespace uedit
{
    ActionManager::ActionManager(EditorFrame* eframe) :
        mEFrame(eframe),
        mTransformActions(*this),
        mVisualsActions(*this),
        mSemanticsCollisionActions(*this),
         mMovementCollisionActions(*this),
        mTilemapActions(*this),
        mWaterActions(*this),
        mLightActions(*this),
        mAudioActions(*this),
        mParticleSystemActions(*this)
    {
    }



    void ActionManager::removeEntity(ungod::Entity e)
    {
        action(std::function([this](ungod::Entity e){ e.getWorld().getQuadTree().remove(e); }),
                        std::function([this](ungod::Entity e){ e.getWorld().getQuadTree().insert(e); }),
                        e);
        mRemovedEntities.emplace_back(e);
    }

    void ActionManager::deleteEntities()
    {
        for (const auto& e : mRemovedEntities)
        {
            e.getWorld().destroy(e);
        }
        mRemovedEntities.clear();
    }




    

    void ActionManager::attachScript(ungod::Entity e, const std::string& name)
    {
        std::string oldscript = "";
        if (e.get<ungod::EntityBehaviorComponent>().valid())
            oldscript = e.get<ungod::EntityBehaviorComponent>().getScriptName();
        action(std::function([this, name](ungod::Entity e){ e.getWorld().getBehaviorHandler().assignBehavior(e, name); }),
                        std::function([this, oldscript](ungod::Entity e)
                                     {
                                         if (oldscript != "")
                                            e.getWorld().getBehaviorHandler().assignBehavior(e, oldscript);
                                    }),
                            e);
    }
}
