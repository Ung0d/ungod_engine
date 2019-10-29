#ifndef UEDIT_ENTITY_EDIT_STATE_H
#define UEDIT_ENTITY_EDIT_STATE_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

namespace uedit
{
    class EntityPreview;

    /** \brief A base class that for entity-preview behavior (state pattern impl) */
    class EditState
    {
    public:
        virtual void handleInput(EntityPreview& preview, const sf::Event& event) {}
        virtual void update(EntityPreview& preview, float delta) {}
        virtual void render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states) {}
    };
}

#endif // UEDIT_ENTITY_EDIT_STATE_H
