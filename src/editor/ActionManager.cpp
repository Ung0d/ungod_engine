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
        mBehaviorActions(*this),
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
}
