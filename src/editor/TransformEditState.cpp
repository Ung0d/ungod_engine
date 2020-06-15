#include "TransformEditState.h"
#include "EntityPreview.h"
#include "Canvas.h"

namespace uedit
{
    void TransformEditState::handleInput(EntityPreview& preview, const sf::Event& event)
    {
    }

    void TransformEditState::update(EntityPreview& preview, float delta)
    {

    }

    void TransformEditState::render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states)
    {
        preview.getCanvas().getEditorState()->getRenderer().renderBounds(preview.mEntity.get<ungod::TransformComponent>(), window, states);
    }
}
