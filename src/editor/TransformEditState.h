#ifndef UEDIT_TRANSFORM_EDIT_STATE_H
#define UEDIT_TRANSFORM_EDIT_STATE_H

#include "ungod/base/Transform.h"
#include "ungod/visual/Renderer.h"
#include "EntityEditState.h"

namespace uedit
{
    /** \brief A base class that for entity-preview behavior (state pattern impl) */
    class TransformEditState : public EditState
    {
    public:
        TransformEditState(EntityPreview&) {}

        virtual void handleInput(EntityPreview& preview, const sf::Event& event);
        virtual void update(EntityPreview& preview, float delta);
        virtual void render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states);
    };

}

#endif // UEDIT_TRANSFORM_EDIT_STATE_H
