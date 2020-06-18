#ifndef WORLD_ACTION_WRAPPER_H
#define WORLD_ACTION_WRAPPER_H

#include "ungod/base/World.h"
#include "ungod/content/tilemap/FloodFill.h"
#include "ungod/utility/ScopedAccessor.h"
#include "ungod/content/tilemap/TilemapBrush.h"
#include "Command/Command.h"
#include "editor/TransformActions.h"
#include "editor/VisualsActions.h"
#include "editor/CollisionActions.h"
#include "editor/TilemapActions.h"
#include "editor/WaterActions.h"
#include "editor/LightActions.h"
#include "editor/AudioActions.h"
#include "editor/ParticleSystemActions.h" 

namespace uedit
{
    class EditorFrame;

    /**\brief  keeps track of all modifying operations and provides undo/redo functionality. */
    class ActionManager
    {
    public:
        ActionManager(EditorFrame* eframe);

        EditorFrame* getEditorFrame() { return mEFrame; }

        template<typename ... T>
        void action(const std::function<void(T...)>& execute, const std::function<void(T...)>& undo, T... t);

        void undo()
        {
            mActionManager.undo();
        }

        void redo()
        {
            mActionManager.redo();
        }


        void attachScript(ungod::Entity e, const std::string& name);

        //removes the given entity from the world and marks it for deletion
        //it IS NOT DELETED DIRECTLY, to ensure that the operation is redoable
        void removeEntity(ungod::Entity e);

        //call this to permanently delete removed entities
        //after calling this, redoing a removeEntity operation is undefined bahavior
        void deleteEntities();

        TransformActions& transformActions() { return mTransformActions; }

        VisualsActions& visualsActions() { return mVisualsActions; }

        template<std::size_t CONTEXT>
        CollisionActions<CONTEXT>& collisionActions() 
        { 
            if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
                return mSemanticsCollisionActions;
            else
                return mMovementCollisionActions;
        }

        TilemapActions& tilemapActions() { return mTilemapActions; }

        WaterActions& waterActions() { return mWaterActions; }

        LightActions& lightActions() { return mLightActions; }

        AudioActions& audioActions() { return mAudioActions; }

        ParticleSystemActions& particleSystemActions() { return mParticleSystemActions; } 

    private:
        command::ActionManager mActionManager;
        TransformActions mTransformActions;
        VisualsActions mVisualsActions;
        CollisionActions<ungod::SEMANTICS_COLLISION_CONTEXT> mSemanticsCollisionActions;
        CollisionActions<ungod::MOVEMENT_COLLISION_CONTEXT> mMovementCollisionActions;
        TilemapActions mTilemapActions;
        WaterActions mWaterActions;
        LightActions mLightActions;
        AudioActions mAudioActions;
        ParticleSystemActions mParticleSystemActions;

    private:
        EditorFrame* mEFrame;
        std::vector<ungod::Entity> mRemovedEntities;
    };
}

#include "CollisionActions.inl"
#include "ParticleSystemActions.inl"

#endif // WORLD_ACTION_WRAPPER_H
