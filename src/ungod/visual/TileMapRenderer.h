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

#ifndef UNGOD_TILEMAP_RENDERER_H
#define UNGOD_TILEMAP_RENDERER_H

#include <SFML/Graphics/RenderTexture.hpp>
#include "ungod/content/TileMap.h"
#include "ungod/content/TileMapBrush.h"
#include "ungod/content/Water.h"
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
            virtual void notifyTileChanged(Tile* tile, int id) const override;
        };
    }

    /**
    * \ingroup Components
    * \brief A component that manages a tilemap.
    */
    class TileMapComponent : public Serializable<TileMapComponent>
    {
    friend class TileMapRenderer;
	friend class Renderer;
    friend struct DeserialBehavior<TileMapComponent, Entity, World&, const Application&>;
    public:
        TileMapComponent() = default;

        const TileMap& getTileMap() const { return mTileMap; }

    private:
        TileMap mTileMap;
    };


    /**
    * \ingroup Components
    * \brief A component that manages a water-field.
    */
    class WaterComponent : public Serializable<WaterComponent>
    {
    friend class TileMapRenderer;
	friend class Renderer;
    friend struct DeserialBehavior<WaterComponent, Entity, World&, const Application&>;

    public:
        WaterComponent() = default;

        const Water& getWater() const { return mWater; }
    private:
        Water mWater;
    };

    /** \brief A class that manages and renders all existing tilemaps and water-entities. */
    class TileMapRenderer
    {
    public:
        TileMapRenderer(Application& app, World& world);

        /** \brief Reserves the given number of tiles for an entity that has a tilemap component attached. */
        bool reserveTileCount(Entity e, std::size_t num, const unsigned mapSizeX, const unsigned mapSizeY);

        /** \brief Appends a tile to the tilemap. */
        void addTile(Entity e, int id, unsigned material, bool active = true, bool blocked = false);

        /** \brief Appends a key to the tilemap keytable. */
        void addKey(Entity e, const std::string& key);

        /** \brief Accesses the tile at the given position. */
        Tile* getTile(Entity e, const sf::Vector2f& position);

        /** \brief Accesses the tile at the given position. */
        Tile* getTile(Entity e, unsigned x, unsigned y);

        /** \brief Sets the tiles for a tilemap. */
        bool setTiles(Entity e, std::vector<int> tiles, std::vector<unsigned> materials,
                             std::vector<bool> active, std::vector<bool> blocked, const unsigned mapSizeX, const unsigned mapSizeY);

        /** \brief Sets the tiles for a tilemap. */
        bool setTiles(Entity e, std::vector<int> tiles, std::vector<unsigned> materials,
                             std::vector<bool> active, const unsigned mapSizeX, const unsigned mapSizeY);

        /** \brief Sets the tiles for a tilemap. */
        bool setTiles(Entity e, std::vector<int> tiles, std::vector<unsigned> materials,
                             const unsigned mapSizeX, const unsigned mapSizeY);

        /** \brief Load tiles and meta ID for the entities tilemap. */
        void loadTiles(Entity e, const std::string& tileID, const std::string& metaID,
                       unsigned cTileWidth, unsigned cTileHeight,
                       const std::vector<std::string>& keymap = {});


        /** \brief Reserves the given number of tiles for an entity that has a water component attached. */
        bool reserveWaterTileCount(Entity e, std::size_t num, const unsigned mapSizeX, const unsigned mapSizeY);

        /** \brief Appends a tile to the water. */
        void addWaterTile(Entity e, int id, unsigned material, bool active = true, bool blocked = false);

        /** \brief Appends a key to the water keytable. */
        void addWaterKey(Entity e, const std::string& key);

        /** \brief Accesses the tile at the given position. */
        Tile* getWaterTile(Entity e, const sf::Vector2f& position);

        /** \brief Accesses the tile at the given position. */
        Tile* getWaterTile(Entity e, unsigned x, unsigned y);

        /** \brief Sets the tiles for a water. */
        bool setWaterTiles(Entity e, std::vector<int> tiles, std::vector<unsigned> materials,
                             std::vector<bool> active, std::vector<bool> blocked, const unsigned mapSizeX, const unsigned mapSizeY);

        /** \brief Sets the tiles for a water. */
        bool setWaterTiles(Entity e, std::vector<int> tiles, std::vector<unsigned> materials,
                             std::vector<bool> active, const unsigned mapSizeX, const unsigned mapSizeY);

        /** \brief Sets the tiles for a water. */
        bool setWaterTiles(Entity e, std::vector<int> tiles, std::vector<unsigned> materials,
                             const unsigned mapSizeX, const unsigned mapSizeY);

        /** \brief Load tiles and meta ID for the entities water. */
        void loadWaterTiles(Entity e, const std::string& tileID, const std::string& metaID,
                       unsigned cTileWidth, unsigned cTileHeight,
                       const std::vector<std::string>& keymap = {});

        inline static void loadWaterShaders(Entity e, const std::string& distortionMap,
                      const std::string& fragmentShader, const std::string& vertexShader,
                      const sf::RenderTarget& target)
        { loadWaterShaders(e.modify<WaterComponent>(), distortionMap, fragmentShader, vertexShader, target); }
        static void loadWaterShaders(WaterComponent& water, const std::string& distortionMap,
                      const std::string& fragmentShader, const std::string& vertexShader,
                      const sf::RenderTarget& target);

        /** \brief Activates or deactivates the rendering of reflections of nearby world-objects. */
        inline static void setWaterReflections(Entity e, bool flag)
        { setWaterReflections(e.modify<WaterComponent>(), flag); }
        static void setWaterReflections(WaterComponent& water, bool flag);

        /** \brief Activates or deactivates the rendering of the water shaders. */
        inline static void setWaterShaders(Entity e, bool flag)
        { setWaterShaders(e.modify<WaterComponent>(), flag); }
        static void setWaterShaders(WaterComponent& water, bool flag);

        /** \brief Sets the distortion-factor for the water shader. Controls the severity of the effect */
        inline static void setWaterDistortionFactor(Entity e, float distortion)
        { setWaterDistortionFactor(e.modify<WaterComponent>(), distortion); }
        static void setWaterDistortionFactor(WaterComponent& water, float distortion);

        /** \brief Sets the rise-factor for the water shader. Controls the animation speed. */
        inline static void setWaterFlowFactor(Entity e, float flow)
        { setWaterFlowFactor(e.modify<WaterComponent>(), flow); }
        static void setWaterFlowFactor(WaterComponent& water, float flow);

        /** \brief Sets the opacity decrease-factor for the reflections of entitys. Default value is 0.5f. */
        inline static void setWaterReflectionOpacity(Entity e, float opacity)
        { setWaterReflectionOpacity(e.modify<WaterComponent>(), opacity); }
        static void setWaterReflectionOpacity(WaterComponent& water, float opacity);

        /** \brief Performs flood fill for the given entity with a tilemap component. */
        inline static void floodFillTileMap(Entity e, unsigned ix, unsigned iy, const std::vector<int>& replacementIDs, bool activate)
        { floodFillTileMap(e.modify<TileMapComponent>(), ix, iy, replacementIDs, activate); }
        static void floodFillTileMap(TileMapComponent& tilemap, unsigned ix, unsigned iy, const std::vector<int>& replacementIDs, bool activate);

        /** \brief Performs flood fill for the given entity with a water component. */
        inline static void floodFillWater(Entity e, unsigned ix, unsigned iy, const std::vector<int>& replacementIDs, bool activate)
        { floodFillWater(e.modify<WaterComponent>(), ix, iy, replacementIDs, activate); }
        static void floodFillWater(WaterComponent& water, unsigned ix, unsigned iy, const std::vector<int>& replacementIDs, bool activate);

        /** \brief Sets a new tilemap for the given entity with a tilemap component. */
		void setTileMap(Entity e, const TileMap& tilemap);

        /** \brief Sets a new water for the given entity with a water component. */
		void setWater(Entity e, const Water& water);

        /** \brief Returns a brush object that can be used to paint on the tilemap associated with entity e.
        * Changes to the tilemap through the brush, are send notifications the renderer. */
        TilemapBrush makeTilemapBrush(Entity e, const std::string& identifier);

        /** \brief Returns a brush object that can be used to paint on the water associated with entity e.
        * Changes to the water through the brush, are send notifications the renderer. */
        TilemapBrush makeWaterBrush(Entity e, const std::string& identifier);


		/** \brief Changes the local position of a tilemap. */
		void setTilemapPosition(Entity e, const sf::Vector2f& position);

		/** \brief Changes the local position of a water. */
		void setWaterPosition(Entity e, const sf::Vector2f& position);

		/** \brief A method that moves the tilemaps and waters attached to the given entity. This method is usually only
		* used internally by the transform-manager. */
		void moveMaps(Entity e, const sf::Vector2f& vec);


		/** \brief Registers new callback for the ContentsChanged signal. */
		inline decltype(auto) onContentsChanged(const std::function<void(Entity, const sf::FloatRect&)>& callback)
		{
			mContentsChangedSignal.connect(callback);
		}

        /** \brief Applies a user defined function to the tilemap that is allowed to change its bounds. Is used e.g. for deserial. */
        void tilemapCallback(Entity e, TileMapComponent& tmc, const std::function<void(TileMap&)>& callback);


        /** \brief Applies a user defined function to the water that is allowed to change its bounds. Is used e.g. for deserial. */
        void waterCallback(Entity e, WaterComponent& wc, const std::function<void(Water&)>& callback);


        ~TileMapRenderer();

    private:
		World& mWorld;
        owls::SignalLink<void, const sf::Vector2u&> mAppSignalLink;
		owls::Signal<Entity, const sf::FloatRect&> mContentsChangedSignal;
    };
}

#endif // UNGOD_TILEMAP_RENDERER_H

