#ifndef UEDIT_TILEMAP_EDIT_STATE_H
#define UEDIT_TILEMAP_EDIT_STATE_H

#include <unordered_set>
#include "ungod/content/tilemap/TileMap.h"
#include "ungod/content/water/Water.h"
#include "ungod/content/tilemap/TilemapBrush.h"
#include "EntityEditState.h"
#include <optional>

namespace uedit
{
    class TileMapEditBase : public EditState
    {
    public:
        TileMapEditBase(EntityPreview& preview);

        virtual void render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states) override;

    protected:
        EntityPreview& mPreview;
        sf::VertexArray mVertices;
    };

    /** \brief A state for editing tilemaps. */
    class TileMapEditState : public TileMapEditBase
    {
    public:
        TileMapEditState(EntityPreview& preview);


        virtual void handleInput(EntityPreview& preview, const sf::Event& event) override;
        virtual void update(EntityPreview& preview, float delta) override;

    private:
        bool mMouseDown;
        sf::Vector2i mLastTileIndices;
        std::optional<ungod::TilemapBrush> mTMBrush;
    };

    /** \brief A state for editing tilemaps. */
    class TileMapFloodFillState : public TileMapEditBase
    {
    public:
        TileMapFloodFillState(EntityPreview& preview);


        virtual void handleInput(EntityPreview& preview, const sf::Event& event) override;
        virtual void update(EntityPreview& preview, float delta) override;

    private:
        bool mMouseDown;
    };
}

#endif // UEDIT_TILEMAP_EDIT_STATE_H
