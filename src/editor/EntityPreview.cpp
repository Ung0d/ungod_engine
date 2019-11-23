#include "EntityPreview.h"
#include "EditorFrame.h"
#include <algorithm>

namespace uedit
{
    EntityPreview::EntityPreview(ungod::Entity e,
                      WorldActionWrapper& waw,
                                 wxWindow* parent,
                      wxWindowID id,
                      const wxPoint& cPosition,
                      const wxSize& cSize,
                      long cStyle) :
                          RenderArea(parent, id, cPosition, cSize, cStyle),
                          mEntity(e),
                          mWorldAction(waw),
                          mCamContrl(mCamera)
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
    }

    void EntityPreview::render(sf::RenderWindow& window, sf::RenderStates states)
    {
        if (mEntity.has<ungod::TransformComponent>())
        {
            window.setSize(sf::Vector2u((unsigned)GetSize().x, (unsigned)GetSize().y));
            if (mEntity.has<ungod::VisualsComponent>())
                ungod::Renderer::renderEntity(mEntity, mEntity.modify<ungod::TransformComponent>(), mEntity.modify<ungod::VisualsComponent>(), window, states);
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
}
