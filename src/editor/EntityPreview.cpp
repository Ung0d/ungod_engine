#include "EntityPreview.h"
#include "EditorFrame.h"
#include "Canvas.h"
#include <algorithm>

namespace uedit
{
    EntityPreview::EntityPreview(const EditorCanvas& canvas, 
                                  ungod::Entity e,
                                  ActionManager& actionManager,
                                   wxWindow* parent,
                                  wxWindowID id,
                                  const wxPoint& cPosition,
                                  const wxSize& cSize,
                                  long cStyle) :
                          RenderArea(parent, id, cPosition, cSize, cStyle),
                          mEntity(e),
                          mActionManager(actionManager),
                          mCamContrl(mCamera),
                          mCanvas(canvas),
                          mRenderEnvironment(false),
                          mRenderDebugEnvironment(false)
    {
        toggleDefault();
        resetView();
    }

    void EntityPreview::handleInput(const sf::Event& event)
    {
        mCamContrl.handleEvent(event);

        mState->handleInput(*this, event);
    }

    void EntityPreview::update(float delta)
    {
        mState->update(*this, delta);
        mEntity.getWorld().getTileMapHandler().update({ mEntity }, mEntity.getWorld(), mCamera);
        mEntity.getWorld().getWaterHandler().targetSizeChanged(mEntity.getWorld(), mWindow.getSize());
    }

    void EntityPreview::render(sf::RenderWindow& window, sf::RenderStates states)
    {
        if (mRenderEnvironment)
        {
            mEntity.getWorld().getGraph().render(window, states);
            if (mRenderDebugEnvironment)
                mEntity.getWorld().getGraph().renderDebug(window, states, true, false);
        }
        else
        {
            if (mEntity.has<ungod::TransformComponent>())
            {
                window.setSize(sf::Vector2u((unsigned)GetSize().x, (unsigned)GetSize().y));
                if (mEntity.has<ungod::VisualsComponent>())
                    mCanvas.getEditorState()->getRenderer().renderEntity(mEntity, mEntity.modify<ungod::TransformComponent>(), mEntity.modify<ungod::VisualsComponent>(), window, states);
            }
        }
        mState->render(*this, window, states);
    }

    void EntityPreview::toggleDefault()
    {
        mState = std::unique_ptr<EditState>(new EditState());
    }

    void EntityPreview::resetView()
    {
        if (mEntity.has<ungod::TransformComponent>())
        {
            mCamera.lookAt( mEntity.get<ungod::TransformComponent>().getPosition() + 0.5f*mEntity.get<ungod::TransformComponent>().getSize() );
        }
    }

    void EntityPreview::lookAt(const sf::Vector2f& position)
    {
        mCamera.lookAt(position);
    }

    EntityPreview::~EntityPreview()
    {
        mEntity.getWorld().getWaterHandler().targetSizeChanged(mEntity.getWorld(), mCanvas.getWindow().getSize());
    }
}
