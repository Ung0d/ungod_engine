#include "TransformEditState.h"
#include "EntityPreview.h"

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
        ungod::Renderer::renderBounds(preview.mEntity.get<ungod::TransformComponent>(), window, states);
    }
}
