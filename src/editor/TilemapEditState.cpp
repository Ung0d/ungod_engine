#include "TilemapEditState.h"
#include "EntityPreview.h"
#include "EditorFrame.h"

namespace uedit
{
    TileMapEditBase::TileMapEditBase(EntityPreview& preview, bool water) : mWater(water), mPreview(preview)
    {
        mVertices.setPrimitiveType(sf::Lines);
    }


    void TileMapEditBase::render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states)
    {
        ungod::TileMap const* tm;
        if (mWater)
            tm = &mPreview.mEntity.modify<ungod::WaterComponent>().getWater().getTileMap();
        else
            tm = &mPreview.mEntity.modify<ungod::TileMapComponent>().getTileMap();

        //build a grid and render it
        if (tm->getImage().isLoaded() && tm->getImage().get()->getSize().x > 0)
        {
            float horizontalSize = ceil( mPreview.mCamera.getView().getSize().x / (tm->getScale().x*tm->getTileWidth()) ) + 1;
            float verticalSize = ceil( mPreview.mCamera.getView().getSize().y / (tm->getScale().y*tm->getTileHeight()) ) + 1;
            mVertices.resize(horizontalSize*verticalSize*4);

            sf::Vector2f windowPosition = window.mapPixelToCoords(sf::Vector2i(0,0));

            unsigned metaX = std::max(0, (int)floor((windowPosition.x - tm->getPosition().x) / (tm->getScale().x*tm->getTileWidth())));
            unsigned metaY = std::max(0, (int)floor((windowPosition.y - tm->getPosition().y) / (tm->getScale().y*tm->getTileHeight())));

            for (unsigned x = metaX; x < metaX + horizontalSize && x < tm->getMapSizeX(); x++)
                for (unsigned y = metaY; y < metaY + verticalSize && y < tm->getMapSizeY(); y++)
            {
                sf::Vertex* vert = &mVertices[(y - metaY + (x - metaX) * verticalSize) * 4];

                // define the 4 corners
                vert[0].position = sf::Vector2f(x * tm->getTileWidth(), y * tm->getTileHeight());
                vert[1].position = sf::Vector2f((x + 1) * tm->getTileWidth(), y * tm->getTileHeight());
                vert[2].position = sf::Vector2f(x * tm->getTileWidth(), y * tm->getTileHeight());
                vert[3].position = sf::Vector2f(x * tm->getTileWidth(), (y + 1) * tm->getTileHeight());

                vert[0].color = sf::Color::Green;
                vert[1].color = sf::Color::Green;
                vert[2].color = sf::Color::Green;
                vert[3].color = sf::Color::Green;
            }
        }

        states.transform *= tm->getTransform();
        window.draw(mVertices, states);
    }


    TileMapEditState::TileMapEditState(EntityPreview& preview, bool water) : TileMapEditBase(preview, water), mMouseDown(false), mLastTile(nullptr)
    {
    }


    void TileMapEditState::handleInput(EntityPreview& preview, const sf::Event& event)
    {
        ungod::TileMap const* tm;
        if (mWater)
            tm = &mPreview.mEntity.modify<ungod::WaterComponent>().getWater().getTileMap();
        else
            tm = &mPreview.mEntity.modify<ungod::TileMapComponent>().getTileMap();

        switch (event.type)
        {
        case sf::Event::MouseButtonPressed:
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                mMouseDown = true;
                mLastTile = nullptr;
            }
            else if (event.mouseButton.button == sf::Mouse::Right)
            {
                sf::Vector2f worldpos = mPreview.mWindow.mapPixelToCoords( {event.mouseButton.x, event.mouseButton.y}, mPreview.mCamera.getView() );
                ungod::Tile const* tile = tm->getTiledata(worldpos);
                if (tile && tile->isActive())
                {
                    std::string key = tm->getKeyMap()[tile->getTileID()];
                    mPreview.mWorldAction.getEditorFrame()->getSheetPreview()->selectSheetKey(tm->getImage().getFilePath(), key + " (object)");
                }
            }
            break;
        }
        case sf::Event::MouseButtonReleased:
        {
            mMouseDown = false;
            if (event.mouseButton.button == sf::Mouse::Left && mTMBrush)
                mTMBrush->resetMemory();
            break;
        }
        default:
            break;
        }
    }

    void TileMapEditState::update(EntityPreview& preview, float delta)
    {
        ungod::TileMap const* tm;
        if (mWater)
            tm = &mPreview.mEntity.modify<ungod::WaterComponent>().getWater().getTileMap();
        else
            tm = &mPreview.mEntity.modify<ungod::TileMapComponent>().getTileMap();

        if (mMouseDown)
        {
            sf::Vector2f worldpos = mPreview.mWindow.mapPixelToCoords( {sf::Mouse::getPosition(mPreview.mWindow).x, sf::Mouse::getPosition(mPreview.mWindow).y}, mPreview.mCamera.getView() );
            ungod::Tile const* tile = tm->getTiledata(worldpos);
            std::string key = mPreview.mWorldAction.getEditorFrame()->getSheetPreview()->getCurrentKey();
            std::string type = mPreview.mWorldAction.getEditorFrame()->getSheetPreview()->getCurrentType();
            if (tile && key != "")
            {
                if (!mLastTile || mLastTile != tile)
                {
                    mLastTile = tile;
                    if (type == "object")
                    {
                        //retrieve the id for the key or add it to the keylist and create a new id
                        int tileID = tm->getKeyMap().size();
                        for (unsigned i = 0; i < tm->getKeyMap().size(); ++i)
                        {
                            if (tm->getKeyMap()[i] == key)
                            {
                                tileID = (int)i;
                                break;
                            }
                        }

                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) //remove key
                        {
                            if (mWater)
                                mPreview.mWorldAction.setWaterTileData(mPreview.mEntity, worldpos, !tile->isActive());
                            else
                                mPreview.mWorldAction.setTileActive(mPreview.mEntity, worldpos, !tile->isActive());
                        }
                        else //add key
                        {
                            if (mWater)
                            {
                                if (tileID == (int)tm->getKeyMap().size())
                                    mPreview.mWorldAction.addWaterKey(mPreview.mEntity, key);
                                mPreview.mWorldAction.setWaterTileData(mPreview.mEntity, worldpos, tileID, tile->getMaterialID(), true, tile->isBlocked());
                            }
                            else
                            {
                                if (tileID == (int)tm->getKeyMap().size())
                                    mPreview.mWorldAction.addKey(mPreview.mEntity, key);
                                mPreview.mWorldAction.setTileData(mPreview.mEntity, worldpos, tileID, tile->getMaterialID(), true, tile->isBlocked());
                            }
                        }
                    }
                    else if (type == "brush")
                    {
                        //TODO
                        //erase or paint?
                        /*if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                        {
                            mPreview.mWorldAction.eraseTile(*mTMBrush, worldpos, );
                        }
                        else
                        {*/
                            //if user clicks on a tile and there is also a tile key selected in the preview-area, we have to make sure
                            //that the key-set this key belongs to, is completely contained in the keymap of the tilemap
                            //afterwards, we have to create a new brush for the identifier that can be deduced from the key, if no brush is
                            //currently present or if the identifier of the present brush is something different
                            if (!mPreview.mWorldAction.getEditorFrame()->getSelectedWorld())
                                return;
                            if (!mTMBrush || mTMBrush->getIdentifier() != key)
                            {
                                auto keymap = ungod::TilemapBrush::makeKeymap(key);
                                for (const auto& k : keymap)
                                {
                                    if (std::find(tm->getKeyMap().begin(), tm->getKeyMap().end(), k) == tm->getKeyMap().end())
                                    {
                                        if (mWater)
                                            mPreview.mWorldAction.addWaterKey(mPreview.mEntity, k);
                                        else
                                            mPreview.mWorldAction.addKey(mPreview.mEntity, k);
                                    }
                                }
                                if (mWater)
                                    mTMBrush = mPreview.mWorldAction.getEditorFrame()->getSelectedWorld()->getTileMapRenderer().makeWaterBrush(mPreview.mEntity, key);
                                else
                                    mTMBrush = mPreview.mWorldAction.getEditorFrame()->getSelectedWorld()->getTileMapRenderer().makeTilemapBrush(mPreview.mEntity, key);
                            }
                            mPreview.mWorldAction.paintTile(*mTMBrush, worldpos, !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl));
                        //}
                    }
                }
            }
        }
    }


    TileMapFloodFillState::TileMapFloodFillState(EntityPreview& preview, bool water) : TileMapEditBase(preview, water), mMouseDown(false)
    {
    }


    void TileMapFloodFillState::handleInput(EntityPreview& preview, const sf::Event& event)
    {
        ungod::TileMap const* tm;
        if (mWater)
            tm = &mPreview.mEntity.modify<ungod::WaterComponent>().getWater().getTileMap();
        else
            tm = &mPreview.mEntity.modify<ungod::TileMapComponent>().getTileMap();

        switch (event.type)
        {
        case sf::Event::MouseButtonPressed:
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                if (!mMouseDown)
                {
                    mMouseDown = true;

                    sf::Vector2f worldpos = mPreview.mWindow.mapPixelToCoords( {sf::Mouse::getPosition(mPreview.mWindow).x, sf::Mouse::getPosition(mPreview.mWindow).y}, mPreview.mCamera.getView() );
                    ungod::Tile const* tile = tm->getTiledata(worldpos);
                    std::string key = mPreview.mWorldAction.getEditorFrame()->getSheetPreview()->getCurrentKey();
                    if (tile && key != "")
                    {
                        //retrieve the id for the key or add it to the keylist and create a new id
                        int tileID = tm->getKeyMap().size();
                        for (unsigned i = 0; i < tm->getKeyMap().size(); ++i)
                        {
                            if (tm->getKeyMap()[i] == key)
                            {
                                tileID = (int)i;
                                break;
                            }
                        }

                        auto ti = tm->getTileIndices(worldpos);

                        if (mWater)
                        {
                            if (tileID == (int)tm->getKeyMap().size())
                                mPreview.mWorldAction.addWaterKey(mPreview.mEntity, key);
                            mPreview.mWorldAction.floodFillWater(mPreview.mEntity, ti.x, ti.y, {tileID}, true);
                        }
                        else
                        {
                            if (tileID == (int)tm->getKeyMap().size())
                                mPreview.mWorldAction.addKey(mPreview.mEntity, key);
                            mPreview.mWorldAction.floodFill(mPreview.mEntity, ti.x, ti.y, {tileID}, true);
                        }
                    }
                }
            }
            break;
        }
        case sf::Event::MouseButtonReleased:
        {
            mMouseDown = false;
            break;
        }
        default:
            break;
        }
    }

    void TileMapFloodFillState::update(EntityPreview& preview, float delta)
    {
    }
}
