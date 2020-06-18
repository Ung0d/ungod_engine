#ifndef VISUALS_ORIGANIZER_H
#define VISUALS_ORIGANIZER_H

#include "ungod/base/Entity.h"
#include "ungod/visual/Visual.h"
#include "ungod/visual/Renderer.h"
#include "EntityEditState.h"
#include "RenderArea.h"
#include "ActionManager.h"
#include "CameraController.h"

namespace uedit
{
    class EditorCanvas;

    /** \brief A preview area that is able to draw an entity in its appearence in the world and has several toggleable states
    * for the deifferent types of editing. */
    class EntityPreview : public RenderArea
    {
    friend class TransformEditState;
    friend class VisualsEditState;
    friend class TileMapEditBase;
    friend class TileMapEditState;
    friend class TileMapFloodFillState;
    friend class ParticleEditState;
    friend class LightEditState;
    template<std::size_t CONTEXT>
    friend class CollidersEditState;
    public:
        EntityPreview(const EditorCanvas& canvas, 
                        ungod::Entity e,
                      ActionManager& actionManager,
                      wxWindow* parent = nullptr,
                          wxWindowID id = -1,
                          const wxPoint& cPosition = wxDefaultPosition,
                          const wxSize& cSize = wxDefaultSize,
                            long cStyle = 0);

        virtual void handleInput(const sf::Event& event) override;
        virtual void update(float delta) override;
        virtual void render(sf::RenderWindow& window, sf::RenderStates states) override;

        virtual ~EntityPreview() {}

        void toggleDefault();
        template<typename T, typename ... PARAM>
        void toggle(PARAM&& ... param)
        {
            mState = std::unique_ptr<EditState>(new T(*this, std::forward<PARAM>(param)...));
        }
        template<typename T>
        bool isInState() { return static_cast<bool>( dynamic_cast<T*>(mState.get()) ); }
        template<typename T>
        T& getState() { return static_cast<T&>( *mState ); }


        void resetView();

        void lookAt(const sf::Vector2f& position);

        ungod::Entity getEntity() { return mEntity; }
        ActionManager& getActionManager() { return mActionManager; }
        const EditorCanvas& getCanvas() const { return mCanvas; }

    private:
        ungod::Entity mEntity;
        ActionManager& mActionManager;
        std::unique_ptr<EditState> mState;
        CameraController mCamContrl;
        const EditorCanvas& mCanvas;
    };
}

#endif // VISUALS_ORIGANIZER_H
