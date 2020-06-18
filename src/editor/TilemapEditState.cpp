#include "TilemapEditState.h"
#include "EntityPreview.h"
#include "EditorFrame.h"

namespace uedit
{
    TileMapEditBase::TileMapEditBase(EntityPreview& preview) : mPreview(preview)
    {
        mVertices.setPrimitiveType(sf::Lines);
    }


    void TileMapEditBase::render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states)
    {
        ungod::TileMap const* tm = &mPreview.mEntity.modify<ungod::TileMapComponent>().getTileMap();

		if (!preview.mEntity.has<ungod::TransformComponent>())
			return;

		states.transform *= preview.mEntity.get<ungod::TransformComponent>().getTransform();

        //build a grid and render it
        float horizontalSize = ceil( mPreview.mCamera.getView().getSize().x / (tm->getScale().x*tm->getTileWidth()) ) + 1;
        float verticalSize = ceil( mPreview.mCamera.getView().getSize().y / (tm->getScale().y*tm->getTileHeight()) ) + 1;
        mVertices.resize(horizontalSize*verticalSize*4);

        sf::Vector2f windowPosition = window.mapPixelToCoords(sf::Vector2i(0,0), mPreview.mCamera.getView());
        windowPosition = states.transform.getInverse().transformPoint(windowPosition);

        unsigned metaX = std::max(0, (int)floor((windowPosition.x - tm->getPosition().x) / tm->getTileWidth()));
        unsigned metaY = std::max(0, (int)floor((windowPosition.y - tm->getPosition().y) / tm->getTileHeight()));

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

        states.transform *= tm->getTransform();
        window.draw(mVertices, states);
    }


    TileMapEditState::TileMapEditState(EntityPreview& preview) : TileMapEditBase(preview), mMouseDown(false), mLastTileIndices(-1,-1)
    {
    }


    void TileMapEditState::handleInput(EntityPreview& preview, const sf::Event& event)
    {
        ungod::TileMap const* tm = &mPreview.mEntity.modify<ungod::TileMapComponent>().getTileMap();

        if (!mPreview.mEntity.has<ungod::VisualsComponent>())
            return;
        auto& v = mPreview.mEntity.modify<ungod::VisualsComponent>();
        if (!v.isLoaded())
            return;

        switch (event.type)
        {
        case sf::Event::MouseButtonPressed:
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                mMouseDown = true;
            }
            else if (event.mouseButton.button == sf::Mouse::Right)
            {
                sf::Vector2f worldpos = mPreview.mWindow.mapPixelToCoords(sf::Mouse::getPosition(mPreview.getWindow()), mPreview.mCamera.getView() );
                worldpos = mPreview.mEntity.modify<ungod::TransformComponent>().getTransform().getInverse().transformPoint(worldpos);
                sf::Vector2i indices = tm->getTileIndices(worldpos);
                if (indices.x > -1)
                {
                    std::string key = tm->getKeyMap()[tm->getTileID(indices.x, indices.y)];
                    mPreview.mActionManager.getEditorFrame()->getSheetPreview()->selectSheetKey(v.getFilePath(), key + " (object)");
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
        ungod::TileMap const* tm = &mPreview.mEntity.modify<ungod::TileMapComponent>().getTileMap();

        if (mMouseDown)
        {
            sf::Vector2f worldpos = mPreview.mWindow.mapPixelToCoords( sf::Mouse::getPosition(mPreview.mWindow), mPreview.mCamera.getView() );
            worldpos = mPreview.mEntity.modify<ungod::TransformComponent>().getTransform().getInverse().transformPoint(worldpos);
            sf::Vector2i tileIndices = tm->getTileIndices(worldpos);
            std::string key = mPreview.mActionManager.getEditorFrame()->getSheetPreview()->getCurrentKey();
            std::string type = mPreview.mActionManager.getEditorFrame()->getSheetPreview()->getCurrentType();
            if (key != "")
            {
                if (!mLastTileIndices.x == -1 || mLastTileIndices != tileIndices)
                {
                    mLastTileIndices = tileIndices;
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
                            mPreview.mActionManager.tilemapActions().setTile(mPreview.mEntity, -1, worldpos);
                        }
                        else //add key
                        {
                            if (tileID == (int)tm->getKeyMap().size())
                                mPreview.mActionManager.tilemapActions().addKey(mPreview.mEntity, key);
                            mPreview.mActionManager.tilemapActions().setTile(mPreview.mEntity, tileID, worldpos);
                        }
                    }
                    else if (type == "brush")
                    {
                        //TODO
                        //erase or paint?
                        /*if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                        {
                            mPreview.mActionManager.eraseTile(*mTMBrush, worldpos, );
                        }
                        else
                        {*/
                            //if user clicks on a tile and there is also a tile key selected in the preview-area, we have to make sure
                            //that the key-set this key belongs to, is completely contained in the keymap of the tilemap
                            //afterwards, we have to create a new brush for the identifier that can be deduced from the key, if no brush is
                            //currently present or if the identifier of the present brush is something different
                            if (!mPreview.mActionManager.getEditorFrame()->getSelectedWorld())
                                return;
                            if (!mTMBrush || mTMBrush->getIdentifier() != key)
                            {
                                auto keymap = ungod::TilemapBrush::makeKeymap(key);
                                for (const auto& k : keymap)
                                {
                                    if (std::find(tm->getKeyMap().begin(), tm->getKeyMap().end(), k) == tm->getKeyMap().end())
                                    {
                                        mPreview.mActionManager.tilemapActions().addKey(mPreview.mEntity, k);
                                    }
                                }
                                mTMBrush = mPreview.mActionManager.getEditorFrame()->getSelectedWorld()->getTileMapHandler().makeTilemapBrush(mPreview.mEntity, key);
                            }
                            mPreview.mActionManager.tilemapActions().paintTile(*mTMBrush, worldpos, !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl));
                        //}
                    }
                }
            }
        }
    }


    TileMapFloodFillState::TileMapFloodFillState(EntityPreview& preview) : TileMapEditBase(preview), mMouseDown(false)
    {
    }


    void TileMapFloodFillState::handleInput(EntityPreview& preview, const sf::Event& event)
    {
        ungod::TileMap const* tm = &mPreview.mEntity.modify<ungod::TileMapComponent>().getTileMap();

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
                    worldpos = mPreview.mEntity.modify<ungod::TransformComponent>().getTransform().getInverse().transformPoint(worldpos);
                    sf::Vector2i ti = tm->getTileIndices(worldpos);
                    std::string key = mPreview.mActionManager.getEditorFrame()->getSheetPreview()->getCurrentKey();
                    if (ti.x > -1 && key != "")
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

                        if (tileID == (int)tm->getKeyMap().size())
                            mPreview.mActionManager.tilemapActions().addKey(mPreview.mEntity, key);
                        mPreview.mActionManager.tilemapActions().floodFillTileMap(mPreview.mEntity, ti.x, ti.y, {tileID});
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
