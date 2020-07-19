/*
* This file is part of the ungod - framework.
* Copyright (C) 2016 Felix Becker - fb132550@uni-greifswald.de
*
* This software is provided 'as-is', without any express or
* implied warranty. In no event will the authors be held
* liable for any damages arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute
* it freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented;
*    you must not claim that you wrote the original software.
*    If you use this software in a product, an acknowledgment
*    in the product documentation would be appreciated but
*    is not required.
*
* 2. Altered source versions must be plainly marked as such,
*    and must not be misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any
*    source distribution.
*/

#ifndef UNGOD_TILEMAP_HANDLER_H
#define UNGOD_TILEMAP_HANDLER_H

#include <SFML/Graphics/RenderTexture.hpp>
#include "owls/Signal.h"
#include "ungod/content/tilemap/TileMap.h"
#include "ungod/content/tilemap/TileMapBrush.h"
#include "ungod/base/Entity.h"

namespace ungod
{
    class Camera;
	class Renderer;


    namespace detail
    {
        //wraps a tilemap reference, can be overloaded if secondary code must be called on setTile operations
        class TilemapRendererChangeNotificator : public TilemapChangeNotificator
        {
        public:
            virtual void notifyTileChanged(int id, unsigned x, unsigned y) const override;
        };
    }

    /**
    * \ingroup Components
    * \brief A component that manages a tilemap.
    */
    class TileMapComponent : public Serializable<TileMapComponent>
    {
    friend class TileMapHandler;
	friend class Renderer;
    friend struct DeserialBehavior<TileMapComponent, Entity, DeserialMemory&>;
    public:
        TileMapComponent() = default;

        const TileMap& getTileMap() const { return mTileMap; }

    private:
        TileMap mTileMap;
    };


    /** \brief A class that manages tilemaps. */
    class TileMapHandler
    {
    public:
        TileMapHandler() = default;

        void init(World& world);

        void update(const std::list<Entity>& entities, const World& world);
        void update(const std::list<Entity>& entities, const World& world, const ungod::Camera& camera);

        /** \brief Sets the tiles for a tilemap takes ownership of the tiledata. */
        bool setTiles(Entity e, const TileData& tiles, unsigned mapSizeX, unsigned mapSizeY);

        /** \brief Sets a single tile of the tilemap. */
        void setTile(Entity e, int id, unsigned x, unsigned y);

        /** \brief Sets the key data that connects tile ids to texture infos. Requires a MetaDataComponent. */
        void setTileDims(Entity e, 
            unsigned tileWidth, unsigned tileHeight,
            const std::vector<std::string>& keymap = {});

        /** \brief Appends a key to the tilemap keytable. Requires a MetaDataComponent. */
        void addKey(Entity e, const std::string& key);

        /** \brief Performs flood fill for the given entity with a tilemap component. */
        inline void floodFillTileMap(Entity e, unsigned ix, unsigned iy, const std::vector<int>& replacementIDs)
        { floodFillTileMap(e.modify<TileMapComponent>(), ix, iy, replacementIDs); }
        void floodFillTileMap(TileMapComponent& tilemap, unsigned ix, unsigned iy, const std::vector<int>& replacementIDs);

        /** \brief Returns a brush object that can be used to paint on the tilemap associated with entity e.
        * Changes to the tilemap through the brush, are send notifications the renderer. */
        TilemapBrush makeTilemapBrush(Entity e, const std::string& identifier);

		/** \brief Changes the local position of a tilemap. */
		void setTilemapPosition(Entity e, const sf::Vector2f& position);

		/** \brief A method that moves the tilemaps and waters attached to the given entity. This method is usually only
		* used internally by the transform-manager. */
		void moveTilemap(Entity e, const sf::Vector2f& vec);

		/** \brief Registers new callback for the ContentsChanged signal. */
		inline decltype(auto) onContentsChanged(const std::function<void(Entity, const sf::FloatRect&)>& callback)
		{
			mContentsChangedSignal.connect(callback);
		}

        /** \brief Applies a user defined function to the tilemap that is allowed to change its bounds. Is used e.g. for deserial. */
        void tilemapCallback(Entity e, TileMapComponent& tmc, const std::function<void(TileMap&)>& callback);

    private:
		owls::Signal<Entity, const sf::FloatRect&> mContentsChangedSignal;

    private:
        void viewSizeChanged(const World& world, const sf::Vector2f& viewsize);
    };
}

#endif // UNGOD_TILEMAP_HANDLER_H

