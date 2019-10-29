#ifndef UEDIT_TILEMAP_EDIT_STATE_H
#define UEDIT_TILEMAP_EDIT_STATE_H

#include <unordered_set>
#include "ungod/content/TileMap.h"
#include "ungod/content/Water.h"
#include "ungod/content/TilemapBrush.h"
#include "EntityEditState.h"

namespace uedit
{
    class TileMapEditBase : public EditState
    {
    public:
        TileMapEditBase(EntityPreview& preview, bool water = false);

        virtual void render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states) override;

    protected:
        bool mWater;
        EntityPreview& mPreview;
        sf::VertexArray mVertices;
    };

    /** \brief A state for editing tilemaps. */
    class TileMapEditState : public TileMapEditBase
    {
    public:
        TileMapEditState(EntityPreview& preview, bool water = false);


        virtual void handleInput(EntityPreview& preview, const sf::Event& event) override;
        virtual void update(EntityPreview& preview, float delta) override;

    private:
        bool mMouseDown;
        ungod::Tile const* mLastTile;
        std::optional<ungod::TilemapBrush> mTMBrush;
    };

    /** \brief A state for editing tilemaps. */
    class TileMapFloodFillState : public TileMapEditBase
    {
    public:
        TileMapFloodFillState(EntityPreview& preview, bool water = false);


        virtual void handleInput(EntityPreview& preview, const sf::Event& event) override;
        virtual void update(EntityPreview& preview, float delta) override;

    private:
        bool mMouseDown;
    };
}

#endif // UEDIT_TILEMAP_EDIT_STATE_H
