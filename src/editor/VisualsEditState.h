#ifndef UEDIT_VISUALS_EDIT_STATE_H
#define UEDIT_VISUALS_EDIT_STATE_H

#include "EntityEditState.h"

namespace uedit
{
    /** \brief A state for visual editing. */
    class VisualsEditState : public EditState
    {
    public:
        VisualsEditState(EntityPreview& preview);

        void selectSprite();
        void deselectSprite();

        void selectMultiSprite(std::size_t i);
        void deselectMultiSprite(std::size_t i);

        void selectVertices(std::size_t i);
        void deselectVertices(std::size_t i);


        virtual void handleInput(EntityPreview& preview, const sf::Event& event) override;
        virtual void update(EntityPreview& preview, float delta) override;
        virtual void render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states) override;

    private:
        EntityPreview& mPreview;
        bool mMouseDown;
        bool mCtrlDown;
        sf::Vector2i mLastMouse;
        bool mSpriteSelected;
        std::vector< std::size_t > mSelectedMultiSprites;
        std::vector< std::size_t > mSelectedVertices;
    };

}

#endif // UEDIT_VISUALS_EDIT_STATE_H
