#include "WorldActionWrapper.h"
#include "EditorFrame.h"
#include <wx/msgdlg.h>

namespace uedit
{
    WorldActionWrapper::WorldActionWrapper(EditorFrame* eframe) :
        mEFrame(eframe)
    {

    }

    void WorldActionWrapper::setEntityPosition(ungod::Entity e, const sf::Vector2f& pos)
    {
        sf::Vector2f ecurpos = e.get<ungod::TransformComponent>().getPosition();
        mEFrame->action(std::function([this, e, pos](){ e.getWorld().getTransformManager().setPosition(e, pos); }),
                        std::function([this, e, ecurpos](){ e.getWorld().getTransformManager().setPosition(e, ecurpos); }));
    }

    void WorldActionWrapper::setEntityScale(ungod::Entity e, const sf::Vector2f& scale)
    {
        sf::Vector2f ecurscale = e.get<ungod::TransformComponent>().getScale();
        mEFrame->action(std::function([this, e, scale](){ e.getWorld().getTransformManager().setScale(e, scale); }),
                        std::function([this, e, ecurscale](){ e.getWorld().getTransformManager().setScale(e, ecurscale); }));
    }

    void WorldActionWrapper::setEntityBaseLineOffset(ungod::Entity e, const sf::Vector2f& offset)
    {
        sf::Vector2f curoffset = e.get<ungod::TransformComponent>().getScale();
        mEFrame->action(std::function([this, e, offset](){ e.getWorld().getTransformManager().setBaseLineOffsets(e, offset); }),
                        std::function([this, e, curoffset](){ e.getWorld().getTransformManager().setBaseLineOffsets(e, curoffset); }));
    }


    void WorldActionWrapper::startEntityMoveSession(ungod::Entity e)
    {
        mEntityMovements.emplace(e, sf::Vector2f{0,0});
    }

    void WorldActionWrapper::moveEntity(ungod::Entity e, const sf::Vector2f& mv)
    {
        auto res = mEntityMovements.find(e);
        if (res != mEntityMovements.end())
        {
            e.getWorld().getTransformManager().move(e, mv);
            res->second += mv;
        }

    }

    void WorldActionWrapper::endEntityMoveSession(ungod::Entity e)
    {
        auto res = mEntityMovements.find(e);
        if (res != mEntityMovements.end())
        {
            e.getWorld().getTransformManager().move(e, -res->second);
            mEFrame->action<const sf::Vector2f&>(std::function([this, e](const sf::Vector2f& mv){ e.getWorld().getTransformManager().move(e, mv); }),
                            std::function([this, e](const sf::Vector2f& mv){ e.getWorld().getTransformManager().move(e, -1.0f*mv); }),
                            res->second);
            mEntityMovements.erase(res);
        }
    }


    void WorldActionWrapper::setEntityHideForCamera(ungod::Entity e, bool hideForCamera)
    {
        bool prev = e.get<ungod::VisualsComponent>().isHiddenForCamera();
        mEFrame->action(std::function([this, hideForCamera](ungod::Entity e)
                                { e.getWorld().getVisualsManager().setHideForCamera(e, hideForCamera); } ),
                            std::function([this, prev](ungod::Entity e)
                                { e.getWorld().getVisualsManager().setHideForCamera(e, prev); } ),
                            e);
    }


    void WorldActionWrapper::loadTexture(ungod::Entity e, const std::string& file)
    {
        std::string oldFile = e.get<ungod::VisualsComponent>().getFilePath();
        mEFrame->action(std::function([this, file](ungod::Entity e){ e.getWorld().getVisualsManager().loadTexture(e, file, ungod::LoadPolicy::SYNC); }),
                            std::function([this, oldFile](ungod::Entity e){ if (oldFile != "") e.getWorld().getVisualsManager().loadTexture(e, oldFile, ungod::LoadPolicy::SYNC); }),
                            e);
    }

    void WorldActionWrapper::loadMetadata(ungod::Entity e, const std::string& file)
    {
        std::string oldFile = e.get<ungod::SpriteMetadataComponent>().getFilePath();
        mEFrame->action(std::function([this, file](ungod::Entity e){ e.getWorld().getVisualsManager().loadMetadata(e, file); }),
                            std::function([this, oldFile](ungod::Entity e){ if (oldFile != "") e.getWorld().getVisualsManager().loadMetadata(e, oldFile); }),
                            e);
    }

    void WorldActionWrapper::setSpriteTextureRect(ungod::Entity e, const std::string& key)
    {
        auto oldrect = e.get<ungod::SpriteComponent>().getSprite().getTextureRect();
        mEFrame->action(std::function([this, key](ungod::Entity e){ e.getWorld().getVisualsManager().setSpriteTextureRect(e, key); }),
                            std::function([this, oldrect](ungod::Entity e){ e.getWorld().getVisualsManager().setSpriteTextureRect(e, oldrect); }),
                            e);
    }

    void WorldActionWrapper::setMultiSpriteTextureRect(ungod::Entity e, const std::string& key, std::size_t i)
    {
        auto oldrect = e.get<ungod::MultiSpriteComponent>().getComponent(i).getSprite().getTextureRect();
        mEFrame->action(std::function([this, key](ungod::Entity e, std::size_t i){ e.getWorld().getVisualsManager().setSpriteTextureRect(e, key, i); }),
                            std::function([this, oldrect](ungod::Entity e, std::size_t i){ e.getWorld().getVisualsManager().setSpriteTextureRect(e, oldrect, i); }),
                            e, i);
    }

    void WorldActionWrapper::setSpritePosition(ungod::Entity e, const sf::Vector2f& pos)
    {
        auto oldpos = e.get<ungod::SpriteComponent>().getSprite().getPosition();
        mEFrame->action(std::function([this, pos](ungod::Entity e){ e.getWorld().getVisualsManager().setSpritePosition(e, pos); }),
                            std::function([this, oldpos](ungod::Entity e){ e.getWorld().getVisualsManager().setSpritePosition(e, oldpos); }),
                            e);
    }

    void WorldActionWrapper::setMultiSpritePosition(ungod::Entity e, const sf::Vector2f& pos, std::size_t i)
    {
        auto oldpos = e.get<ungod::MultiSpriteComponent>().getComponent(i).getSprite().getPosition();
        mEFrame->action(std::function([this, pos](ungod::Entity e, std::size_t i){ e.getWorld().getVisualsManager().setSpritePosition(e, pos, i); }),
                            std::function([this, oldpos](ungod::Entity e, std::size_t i){ e.getWorld().getVisualsManager().setSpritePosition(e, oldpos, i); }),
                            e, i);
    }

    void WorldActionWrapper::setVertexarrayPosition(ungod::Entity e, const sf::Vector2f& pos, std::size_t i)
    {
        auto oldpos = e.get<ungod::VertexArrayComponent>().getVertices().getPosition(i);
        mEFrame->action(std::function([this, pos](ungod::Entity e, std::size_t i){ e.getWorld().getVisualsManager().setTextureRectPosition(e, pos, i); }),
                            std::function([this, oldpos](ungod::Entity e, std::size_t i){ e.getWorld().getVisualsManager().setTextureRectPosition(e, oldpos, i); }),
                            e, i);
    }

    void WorldActionWrapper::setSpriteScale(ungod::Entity e, const sf::Vector2f& scale)
    {
        auto oldscale = e.get<ungod::SpriteComponent>().getSprite().getScale();
        mEFrame->action(std::function([this, scale](ungod::Entity e){ e.getWorld().getVisualsManager().setScale(e, scale); }),
                            std::function([this, oldscale](ungod::Entity e){ e.getWorld().getVisualsManager().setScale(e, oldscale); }),
                            e);
    }

    void WorldActionWrapper::setSpriteRotation(ungod::Entity e, float rota)
    {
        auto oldrota = e.get<ungod::SpriteComponent>().getSprite().getRotation();
        mEFrame->action(std::function([this, rota](ungod::Entity e){ e.getWorld().getVisualsManager().setRotation(e, rota); }),
                            std::function([this, oldrota](ungod::Entity e){ e.getWorld().getVisualsManager().setRotation(e, oldrota); }),
                            e);
    }

    void WorldActionWrapper::setMultiSpriteScale(ungod::Entity e, const sf::Vector2f& scale, std::size_t i)
    {
        auto oldscale = e.get<ungod::MultiSpriteComponent>().getComponent(i).getSprite().getScale();
        mEFrame->action(std::function([this, scale](ungod::Entity e, std::size_t i){ e.getWorld().getVisualsManager().setScale(e, scale, i); }),
                            std::function([this, oldscale](ungod::Entity e, std::size_t i){ e.getWorld().getVisualsManager().setScale(e, oldscale, i); }),
                            e, i);
    }

    void WorldActionWrapper::setMultiSpriteRotation(ungod::Entity e, float rota, std::size_t i)
    {
        auto oldrota = e.get<ungod::MultiSpriteComponent>().getComponent(i).getSprite().getRotation();
        mEFrame->action(std::function([this, rota](ungod::Entity e, std::size_t i){ e.getWorld().getVisualsManager().setRotation(e, rota, i); }),
                            std::function([this, oldrota](ungod::Entity e, std::size_t i){ e.getWorld().getVisualsManager().setRotation(e, oldrota, i); }),
                            e, i);
    }

    void WorldActionWrapper::setVertexRectCount(ungod::Entity e, std::size_t numrect)
    {
        auto oldnum = e.get<ungod::VertexArrayComponent>().getVertices().textureRectCount();
        mEFrame->action(std::function([this, numrect](ungod::Entity e){ e.getWorld().getVisualsManager().initTextureRects(e, numrect); }),
                        std::function([this, oldnum](ungod::Entity e){ e.getWorld().getVisualsManager().initTextureRects(e, oldnum); }),
                            e);
    }

    void WorldActionWrapper::setVertexArrayRect(ungod::Entity e, std::size_t i, const std::string& key)
    {
        auto oldrect = e.get<ungod::VertexArrayComponent>().getVertices().getTextureRect(i);
        mEFrame->action(std::function([this, key](ungod::Entity e, std::size_t i){ e.getWorld().getVisualsManager().setArrayTextureRect(e, i, key); }),
                        std::function([this, oldrect](ungod::Entity e, std::size_t i){ e.getWorld().getVisualsManager().setArrayTextureRect(e, oldrect, i); }),
                        e, i);
    }

    void WorldActionWrapper::setVertexArrayRectColor(ungod::Entity e, std::size_t i, const sf::Color& color)
    {
        auto oldcol = e.get<ungod::VertexArrayComponent>().getVertices().getRectColor(i);
        mEFrame->action(std::function([this, color](ungod::Entity e, std::size_t i){ e.getWorld().getVisualsManager().setArrayRectColor(e, color, i); }),
                        std::function([this, oldcol](ungod::Entity e, std::size_t i){ e.getWorld().getVisualsManager().setArrayRectColor(e, oldcol, i); }),
                        e, i);
    }

    void WorldActionWrapper::removeEntity(ungod::Entity e)
    {
        mEFrame->action(std::function([this](ungod::Entity e){ e.getWorld().getQuadTree().remove(e); }),
                        std::function([this](ungod::Entity e){ e.getWorld().getQuadTree().insert(e); }),
                        e);
        mRemovedEntities.emplace_back(e);
    }

    void WorldActionWrapper::deleteEntities()
    {
        for (const auto& e : mRemovedEntities)
        {
            e.getWorld().destroy(e);
        }
        mRemovedEntities.clear();
    }

    void WorldActionWrapper::reserveTileCount(ungod::Entity e, std::size_t num, const unsigned mapSizeX, const unsigned mapSizeY)
    {
        const ungod::TileMap& tm = e.get<ungod::TileMapComponent>().getTileMap();
        unsigned oldmapx = tm.getMapSizeX();
        unsigned oldmapy = tm.getMapSizeY();
        mEFrame->action(std::function([this, num, mapSizeX, mapSizeY](ungod::Entity e)
                            { e.getWorld().getTileMapRenderer().reserveTileCount(e, num, mapSizeX, mapSizeY);
                              for (std::size_t i = 0; i < num; ++i)
                                {
                                    e.getWorld().getTileMapRenderer().addTile(e,0,false);
                                };
                                }),
                        std::function([this, oldmapx, oldmapy](ungod::Entity e)
                            { e.getWorld().getTileMapRenderer().reserveTileCount(e, oldmapx*oldmapy, oldmapx, oldmapy); }),
                        e);
    }

    void WorldActionWrapper::setTileData(ungod::Entity e, const sf::Vector2f& position, int id)
    {
        ungod::Tile* tile = e.getWorld().getTileMapRenderer().getTile(e, position);
        if (!tile)
            return;

        setTileData(e, position, id, tile->isActive());
    }

    void WorldActionWrapper::setTileActive(ungod::Entity e, const sf::Vector2f& position, bool active)
    {
        ungod::Tile* tile = e.getWorld().getTileMapRenderer().getTile(e, position);
        if (!tile)
            return;

        setTileData(e, position, tile->getTileID(), active);
    }

    void WorldActionWrapper::setTileData(ungod::Entity e, const sf::Vector2f& position, int id, bool active)
    {
        ungod::Tile* tile = e.getWorld().getTileMapRenderer().getTile(e, position);
        if (!tile)
            return;

        int oldid = tile->getTileID();
        bool oldactiv = tile->isActive();
        mEFrame->action<ungod::Entity, const sf::Vector2f&>(std::function([this, id, active](ungod::Entity e, const sf::Vector2f& position)
                            {
                                ungod::Tile* tile = e.getWorld().getTileMapRenderer().getTile(e, position);
                                if (!tile) return;
                                tile->setTileID(id);
                                tile->setActive(active);
                            }),
                        std::function([this, oldid, oldactiv](ungod::Entity e, const sf::Vector2f& position)
                            {
                                ungod::Tile* tile = e.getWorld().getTileMapRenderer().getTile(e, position);
                                if (!tile) return;
                                tile->setTileID(oldid);
                                tile->setActive(oldactiv);
                            }),
                        e, position);
    }

    void WorldActionWrapper::loadTiles(ungod::Entity e, const std::string& tileID, const std::string& metaID,
                   unsigned cTileWidth, unsigned cTileHeight,
                   const std::vector<std::string>& keymap)
    {
        const ungod::TileMap& tm = e.get<ungod::TileMapComponent>().getTileMap();
        std::string oldID = tm.getImage().getFilePath();
        std::string oldMeta = tm.getMetaMap().getFilePath();
        unsigned oldTileWidth = tm.getTileWidth();
        unsigned oldTileHeight = tm.getTileHeight();
        std::vector<std::string> oldKeymap = tm.getKeyMap();

        mEFrame->action(std::function([this, tileID, metaID, cTileWidth, cTileHeight, keymap](ungod::Entity e)
                            { e.getWorld().getTileMapRenderer().loadTiles(e, tileID, metaID, cTileWidth, cTileHeight, keymap); }),
                        std::function([this, oldID, oldMeta, oldTileWidth, oldTileHeight, oldKeymap](ungod::Entity e)
                            { e.getWorld().getTileMapRenderer().loadTiles(e, oldID, oldMeta, oldTileWidth, oldTileHeight, oldKeymap); }),
                        e);
    }

    void WorldActionWrapper::addKey(ungod::Entity e, const std::string& key)
    {
        e.getWorld().getTileMapRenderer().addKey(e, key);
    }


    void WorldActionWrapper::reserveWaterTileCount(ungod::Entity e, std::size_t num, const unsigned mapSizeX, const unsigned mapSizeY)
    {
        const ungod::TileMap& tm = e.get<ungod::WaterComponent>().getWater().getTileMap();
        unsigned oldmapx = tm.getMapSizeX();
        unsigned oldmapy = tm.getMapSizeY();
        mEFrame->action(std::function([this, num, mapSizeX, mapSizeY](ungod::Entity e)
                            { e.getWorld().getTileMapRenderer().reserveWaterTileCount(e, num, mapSizeX, mapSizeY);
                              for (std::size_t i = 0; i < num; ++i)
                                {
                                    e.getWorld().getTileMapRenderer().addWaterTile(e,0,false);
                                };
                            }),
                        std::function([this, oldmapx, oldmapy](ungod::Entity e)
                            { e.getWorld().getTileMapRenderer().reserveWaterTileCount(e, oldmapx*oldmapy, oldmapx, oldmapy); }),
                        e);
    }

    void WorldActionWrapper::setWaterTileData(ungod::Entity e, const sf::Vector2f& position, int id)
    {
        ungod::Tile* tile = e.getWorld().getTileMapRenderer().getWaterTile(e, position);
        if (!tile)
            return;

        setWaterTileData(e, position, id, tile->isActive());
    }

    void WorldActionWrapper::setWaterTileActive(ungod::Entity e, const sf::Vector2f& position, bool active)
    {
        ungod::Tile* tile = e.getWorld().getTileMapRenderer().getWaterTile(e, position);
        if (!tile)
            return;

        setWaterTileData(e, position, tile->getTileID(), active);
    }

    void WorldActionWrapper::setWaterTileData(ungod::Entity e, const sf::Vector2f& position, int id, bool active)
    {
        ungod::Tile* tile = e.getWorld().getTileMapRenderer().getWaterTile(e, position);
        if (!tile)
            return;

        int oldid = tile->getTileID();
        bool oldactiv = tile->isActive();
        mEFrame->action<ungod::Entity, const sf::Vector2f&>(std::function([this, id, active](ungod::Entity e, const sf::Vector2f& position)
                            {
                                ungod::Tile* tile = e.getWorld().getTileMapRenderer().getWaterTile(e, position);
                                if (!tile) return;
                                tile->setTileID(id);
                                tile->setActive(active);
                            }),
                        std::function([this, oldid, oldactiv](ungod::Entity e, const sf::Vector2f& position)
                            {
                                ungod::Tile* tile = e.getWorld().getTileMapRenderer().getWaterTile(e, position);
                                if (!tile) return;
                                tile->setTileID(oldid);
                                tile->setActive(oldactiv);
                            }),
                        e, position);
    }

    void WorldActionWrapper::loadWaterTiles(ungod::Entity e, const std::string& tileID, const std::string& metaID,
                   unsigned cTileWidth, unsigned cTileHeight,
                   const std::vector<std::string>& keymap)
    {
        const ungod::TileMap& tm = e.get<ungod::WaterComponent>().getWater().getTileMap();
        std::string oldID = tm.getImage().getFilePath();
        std::string oldMeta = tm.getMetaMap().getFilePath();
        unsigned oldTileWidth = tm.getTileWidth();
        unsigned oldTileHeight = tm.getTileHeight();
        std::vector<std::string> oldKeymap = tm.getKeyMap();

        mEFrame->action(std::function([this, tileID, metaID, cTileWidth, cTileHeight, keymap](ungod::Entity e)
                            { e.getWorld().getTileMapRenderer().loadWaterTiles(e, tileID, metaID, cTileWidth, cTileHeight, keymap); }),
                        std::function([this, oldID, oldMeta, oldTileWidth, oldTileHeight, oldKeymap](ungod::Entity e)
                            { e.getWorld().getTileMapRenderer().loadWaterTiles(e, oldID, oldMeta, oldTileWidth, oldTileHeight, oldKeymap); }),
                        e);
    }

    void WorldActionWrapper::loadWaterShaders(ungod::Entity e, const std::string& distortionMap,
                  const std::string& fragmentShader, const std::string& vertexShader)

    {
        const ungod::Water& w = e.get<ungod::WaterComponent>().getWater();
        std::string oldDist = w.getDistortionMapID();
        std::string oldFrag = w.getFragmentShaderID();
        std::string oldVert = w.getVertexShaderID();

        mEFrame->action(std::function([this, distortionMap, fragmentShader, vertexShader](ungod::Entity e)
                            { e.getWorld().getTileMapRenderer().loadWaterShaders(e, distortionMap, fragmentShader, vertexShader, mEFrame->getCanvas()->getWindow()); }),
                        std::function([this, oldDist, oldFrag, oldVert](ungod::Entity e)
                            { e.getWorld().getTileMapRenderer().loadWaterShaders(e, oldDist, oldFrag, oldVert, mEFrame->getCanvas()->getWindow()); }),
                        e);
    }

    void WorldActionWrapper::addWaterKey(ungod::Entity e, const std::string& key)
    {
        e.getWorld().getTileMapRenderer().addWaterKey(e, key);
    }



    void WorldActionWrapper::setLightPosition(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Vector2f& position)
    {
        auto oldpos = le.getLight().getPosition();
        mEFrame->action(std::function([this, &le, position](ungod::Entity e){ e.getWorld().getLightSystem().setLocalLightPosition(le, e, position); }),
                        std::function([this, &le, oldpos](ungod::Entity e){ e.getWorld().getLightSystem().setLocalLightPosition(le, e, oldpos); }),
                        e);
    }

    void WorldActionWrapper::setLightScale(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Vector2f& scale)
    {
        auto oldscale = le.getLight().getScale();
        mEFrame->action(std::function([this, &le, scale](ungod::Entity e){ e.getWorld().getLightSystem().setLightScale(le, e, scale); }),
                        std::function([this,  &le, oldscale](ungod::Entity e){ e.getWorld().getLightSystem().setLightScale(le, e, oldscale); }),
                        e);
    }

    void WorldActionWrapper::setLightOrigin(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Vector2f& origin)
    {
        auto oldorigin = le.getLight().getSourcePoint();
        mEFrame->action(std::function([this, &le, origin](ungod::Entity e){ e.getWorld().getLightSystem().setLightOrigin(le, e, origin); }),
                        std::function([this, &le, oldorigin](ungod::Entity e){ e.getWorld().getLightSystem().setLightOrigin(le, e, oldorigin); }),
                        e);
    }

    void WorldActionWrapper::setLightColor(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Color& color)
    {
        auto oldcol = le.getLight().getColor();
        mEFrame->action(std::function([this, &le, color](ungod::Entity e){ e.getWorld().getLightSystem().setLightColor(le, color); }),
                        std::function([this, &le, oldcol](ungod::Entity e){ e.getWorld().getLightSystem().setLightColor(le, oldcol); }),
                        e);
    }

    void WorldActionWrapper::setLightRadius(ungod::Entity e, ungod::LightEmitterComponent& le, float radius)
    {
        auto oldrad = le.getLight().getRadius();
        mEFrame->action(std::function([this, &le, radius](ungod::Entity e){ e.getWorld().getLightSystem().setLightRadius(le, radius); }),
                        std::function([this, &le, oldrad](ungod::Entity e){ e.getWorld().getLightSystem().setLightRadius(le, oldrad); }),
                        e);
    }

    void WorldActionWrapper::setShadowExtendMultiplier(ungod::Entity e, ungod::LightEmitterComponent& le, float multiplier)
    {
        auto oldsem = le.getLight().getShadowExtendMultiplier();
        mEFrame->action(std::function([this, &le, multiplier](ungod::Entity e){ e.getWorld().getLightSystem().setShadowExtendMultiplier(le, multiplier); }),
                        std::function([this, &le, oldsem](ungod::Entity e){ e.getWorld().getLightSystem().setShadowExtendMultiplier(le, oldsem); }),
                        e);
    }

    void WorldActionWrapper::setLightActive(ungod::Entity e, ungod::LightEmitterComponent& le, bool active)
    {
        auto oldact = le.getLight().isActive();
        mEFrame->action(std::function([this, &le, active](ungod::Entity e){ e.getWorld().getLightSystem().setLightActive(le, active); }),
                        std::function([this, &le, oldact](ungod::Entity e){ e.getWorld().getLightSystem().setLightActive(le, oldact); }),
                        e);
    }

    void WorldActionWrapper::loadLightTexture(ungod::Entity e, ungod::LightEmitterComponent& le, const std::string& textureID)
    {
        auto oldfile = le.getLight().getImage().getFilePath();
        mEFrame->action(std::function([this, &le, textureID](ungod::Entity e){ e.getWorld().getLightSystem().loadLightTexture(le, textureID); }),
                        std::function([this, &le, oldfile](ungod::Entity e){ e.getWorld().getLightSystem().loadLightTexture(le, oldfile); }),
                        e);
    }

    void WorldActionWrapper::setPoint(ungod::Entity e, ungod::ShadowEmitterComponent& shadow, const sf::Vector2f& point, std::size_t pointIndex)
    {
        auto oldpoint = shadow.getCollider().getPoint(pointIndex);
        mEFrame->action(std::function([this, &shadow, point](ungod::Entity e, std::size_t pointIndex)
                                      { e.getWorld().getLightSystem().setPoint(shadow, e, point, pointIndex); }),
                        std::function([this, &shadow, oldpoint](ungod::Entity e, std::size_t pointIndex)
                                      { e.getWorld().getLightSystem().setPoint(shadow, e, oldpoint, pointIndex); }),
                        e, pointIndex);
    }

    void WorldActionWrapper::setPointCount(ungod::Entity e, ungod::ShadowEmitterComponent& shadow, std::size_t num)
    {
        auto oldnum = shadow.getCollider().getPointCount();
        mEFrame->action(std::function([this, &shadow, num](ungod::Entity e){ e.getWorld().getLightSystem().setPointCount(e, shadow, num); }),
                        std::function([this, &shadow, oldnum](ungod::Entity e){ e.getWorld().getLightSystem().setPointCount(e, shadow, oldnum); }),
                        e);
    }

    void WorldActionWrapper::setPoints(ungod::Entity e, const std::vector<sf::Vector2f>& points)
    {
        std::vector<sf::Vector2f> oldpoints;
        const ungod::LightCollider& lc = e.get<ungod::ShadowEmitterComponent>().getCollider();
        oldpoints.reserve(lc.getPointCount());
        for (std::size_t i = 0 ; i < lc.getPointCount(); ++i)
        {
            oldpoints.emplace_back(lc.getPoint(i));
        }
        mEFrame->action(std::function([this, points](ungod::Entity e){ e.getWorld().getLightSystem().setPoints(e, points); }),
                        std::function([this, oldpoints](ungod::Entity e){ e.getWorld().getLightSystem().setPoints(e, oldpoints); }),
                        e);
    }

    void WorldActionWrapper::setMultiPoints(ungod::Entity e, const std::vector<sf::Vector2f>& points, std::size_t multiIndex)
    {
        std::vector<sf::Vector2f> oldpoints;
        const ungod::LightCollider& lc = e.get<ungod::MultiShadowEmitter>().getComponent(multiIndex).getCollider();
        oldpoints.reserve(lc.getPointCount());
        for (std::size_t i = 0 ; i < lc.getPointCount(); ++i)
        {
            oldpoints.emplace_back(lc.getPoint(i));
        }
        mEFrame->action(std::function([this, points](ungod::Entity e, std::size_t multiIndex){ e.getWorld().getLightSystem().setPoints(e, points, multiIndex); }),
                        std::function([this, oldpoints](ungod::Entity e, std::size_t multiIndex){ e.getWorld().getLightSystem().setPoints(e, oldpoints, multiIndex); }),
                        e, multiIndex);
    }

    void WorldActionWrapper::setLightOverShape(ungod::Entity e, ungod::ShadowEmitterComponent& shadow, bool lightOverShape)
    {
        bool old = shadow.getCollider().getLightOverShape();
        mEFrame->action(std::function([this, &shadow, lightOverShape](ungod::Entity e){ e.getWorld().getLightSystem().setLightOverShape(shadow, lightOverShape); }),
                        std::function([this, &shadow, old](ungod::Entity e){ e.getWorld().getLightSystem().setLightOverShape(shadow, old); }),
                        e);
    }


    void WorldActionWrapper::setAffectorCallback(ungod::Entity e, ungod::LightAffectorComponent& affector, ungod::LightEmitterComponent& emitter, const std::function<void(float, ungod::LightEmitterComponent&)>& callback)
    {
        auto oldcallback = affector.getCallback();
        mEFrame->action(std::function([this, &affector, &emitter, callback](ungod::Entity e)
                                      { e.getWorld().getLightSystem().setAffectorCallback(callback, affector, emitter); }),
                        std::function([this, &affector, &emitter, oldcallback](ungod::Entity e)
                                      { e.getWorld().getLightSystem().setAffectorCallback(oldcallback, affector, emitter); }),
                        e);
    }

    void WorldActionWrapper::attachScript(ungod::Entity e, const std::string& name)
    {
        std::string oldscript = "";
        if (e.get<ungod::EntityBehaviorComponent>().valid())
            oldscript = e.get<ungod::EntityBehaviorComponent>().getScriptName();
        mEFrame->action(std::function([this, name](ungod::Entity e){ e.getWorld().getBehaviorManager().assignBehavior(e, name); }),
                        std::function([this, oldscript](ungod::Entity e)
                                     {
                                         if (oldscript != "")
                                            e.getWorld().getBehaviorManager().assignBehavior(e, oldscript);
                                    }),
                            e);
    }

    void WorldActionWrapper::floodFill(ungod::Entity e, std::size_t ix, std::size_t iy, const std::vector<int>& replacementIDs, bool activate)
    {
        ungod::TileMap oldtm = e.get<ungod::TileMapComponent>().getTileMap();
        mEFrame->action(std::function([this, ix, iy, replacementIDs, activate] (ungod::Entity e)
                                      { e.getWorld().getTileMapRenderer().floodFillTileMap(e, ix, iy, replacementIDs, activate); }),
                        std::function([this, oldtm] (ungod::Entity e)
                                      { e.getWorld().getTileMapRenderer().setTileMap(e, oldtm); } ),
                        e);
    }

    void WorldActionWrapper::floodFillWater(ungod::Entity e, std::size_t ix, std::size_t iy, const std::vector<int>& replacementIDs, bool activate)
    {
        ungod::Water oldwater = e.get<ungod::WaterComponent>().getWater();
        mEFrame->action(std::function([this, ix, iy, replacementIDs, activate] (ungod::Entity e)
                                      { e.getWorld().getTileMapRenderer().floodFillWater(e, ix, iy, replacementIDs, activate); }),
                        std::function([this, oldwater] (ungod::Entity e)
                                      { e.getWorld().getTileMapRenderer().setWater(e, oldwater); } ),
                        e);
    }

    void WorldActionWrapper::clearAffectors(ungod::Entity e)
    {
        ungod::ParticleSystem old = e.get<ungod::ParticleSystemComponent>().getSystem();
        mEFrame->action(std::function([this] (ungod::Entity e)
                                      { e.getWorld().getParticleSystemManager().clearAffectors(e); }),
                        std::function([this, old] (ungod::Entity e)
                                      { e.getWorld().getParticleSystemManager().setSystem(e, old); } ),
                        e);
    }

    void WorldActionWrapper::setParticleMaxForce(ungod::Entity e, float maxforce)
    {
        float old = e.get<ungod::ParticleSystemComponent>().getSystem().getMaxForce();
        mEFrame->action(std::function([this, maxforce] (ungod::Entity e)
                                      { e.getWorld().getParticleSystemManager().setMaxForce(e, maxforce); }),
                        std::function([this, old] (ungod::Entity e)
                                      { e.getWorld().getParticleSystemManager().setMaxForce(e, old); } ),
                        e);
    }

    void WorldActionWrapper::setParticleMaxVelocity(ungod::Entity e, float maxvel)
    {
        float old = e.get<ungod::ParticleSystemComponent>().getSystem().getMaxVelocity();
        mEFrame->action(std::function([this, maxvel] (ungod::Entity e)
                                      { e.getWorld().getParticleSystemManager().setMaxVelocity(e, maxvel); }),
                        std::function([this, old] (ungod::Entity e)
                                      { e.getWorld().getParticleSystemManager().setMaxVelocity(e, old); } ),
                        e);
    }

    void WorldActionWrapper::setParticleSpeed(ungod::Entity e, float speed)
    {
        float old = e.get<ungod::ParticleSystemComponent>().getSystem().getParticleSpeed();
        mEFrame->action(std::function([this, speed] (ungod::Entity e)
                                      { e.getWorld().getParticleSystemManager().setParticleSpeed(e, speed); }),
                        std::function([this, old] (ungod::Entity e)
                                      { e.getWorld().getParticleSystemManager().setParticleSpeed(e, old); } ),
                        e);
    }



    void WorldActionWrapper::loadAudioEmitterTrack(ungod::Entity e, const std::string& file)
    {
        std::string old = e.get<ungod::MusicEmitterComponent>().getFilePath();
        mEFrame->action(std::function([this, file] (ungod::Entity e)
                                      { e.getWorld().getAudioManager().getMusicEmitterMixer().loadMusic(e, file); }),
                        std::function([this, old] (ungod::Entity e)
                                      { e.getWorld().getAudioManager().getMusicEmitterMixer().loadMusic(e, old); }),
                        e);
    }

    void WorldActionWrapper::setAudioEmitterDistanceCap(ungod::Entity e, float cap)
    {
        float old = e.get<ungod::MusicEmitterComponent>().getDistanceCap();
        mEFrame->action(std::function([this, cap] (ungod::Entity e)
                                      { e.getWorld().getAudioManager().getMusicEmitterMixer().setDistanceCap(e, cap); }),
                        std::function([this, old] (ungod::Entity e)
                                      { e.getWorld().getAudioManager().getMusicEmitterMixer().setDistanceCap(e, old); }),
                        e);
    }

    void WorldActionWrapper::setAudioEmitterActive(ungod::Entity e, bool active)
    {
        bool old = e.get<ungod::MusicEmitterComponent>().isActive();
        mEFrame->action(std::function([this, active] (ungod::Entity e)
                                      { e.getWorld().getAudioManager().getMusicEmitterMixer().setEmitterActive(e, active); }),
                        std::function([this, old] (ungod::Entity e)
                                      { e.getWorld().getAudioManager().getMusicEmitterMixer().setEmitterActive(e, old); }),
                        e);
    }

    void WorldActionWrapper::setAudioEmitterVolume(ungod::Entity e, float vol)
    {
        float old = e.get<ungod::MusicEmitterComponent>().isActive();
        mEFrame->action(std::function([this, vol] (ungod::Entity e)
                                      { e.getWorld().getAudioManager().getMusicEmitterMixer().setEmitterVolume(e, vol); }),
                        std::function([this, old] (ungod::Entity e)
                                      { e.getWorld().getAudioManager().getMusicEmitterMixer().setEmitterVolume(e, old); }),
                        e);
    }

    void WorldActionWrapper::paintTile(ungod::TilemapBrush& brush, const sf::Vector2f& worldpos, bool connect)
    {
        if (!brush.getTilemap())
            return;
        ungod::Tile const* tile = brush.getTilemap()->getTiledata(worldpos);
        if (!tile)
            return;
        int tileID = tile->getTileID();
        mEFrame->action(std::function([&brush, worldpos, connect] ()
                                      { brush.paintTile(worldpos, connect); }),
                        std::function([&brush, worldpos, tileID] ()
                                      { brush.eraseTile(worldpos, tileID); }));
    }

    void WorldActionWrapper::eraseTile(ungod::TilemapBrush& brush, const sf::Vector2f& worldpos, int erasingID)
    {
        mEFrame->action(std::function([&brush, worldpos, erasingID] ()
                                      { brush.eraseTile(worldpos, erasingID); }),
                        std::function([&brush, worldpos] ()
                                      { brush.paintTile(worldpos); }));
    }
}
