#include "Canvas.h"
#include "EditorFrame.h"

namespace uedit
{
    EditorState::EditorState(ungod::Application& app, ungod::StateID id) :
        ungod::ScriptedGameState(app,id),
        mUpdateFlag(true),
        mRenderFlag(true),
        mDebugRendering(false),
        mCamContrl(getCamera())
    {
    }

    void EditorState::setUpdateFlag(bool update)
    {
         mUpdateFlag = update;
    }

    void EditorState::setRenderFlag(bool render)
    {
         mRenderFlag = render;
    }


    void EditorState::handleEvent(const sf::Event& curEvent)
    {
        if (mUpdateFlag)
        {
            ungod::ScriptedGameState::handleEvent(curEvent);
        }

        mCamContrl.handleEvent(curEvent);
    }

    void EditorState::update(const float delta)
    {
        if (mUpdateFlag)
        {
            ungod::ScriptedGameState::update(delta);
        }
    }

    void EditorState::render(sf::RenderTarget& target, sf::RenderStates states)
    {
        if (mRenderFlag)
        {
            ungod::ScriptedGameState::render(target, states);
        }
    }


    void EditorState::init()
    {
    }

    void EditorState::close()
    {
    }


    EntityEditState::EntityEditState(ungod::Application& app, ungod::StateID id, EditorState* editorstate, EditorFrame* editorframe) :
        ungod::State(app, id), mEditorState(editorstate), mEFrame(editorframe), mMouseLeftDown(false), mEntityMoveGoingOn(false), mDesignerOpenPressed(false),
        mCopyPressed(false), mPastePressed(false)
    {
        mMouseLastPos = sf::Mouse::getPosition();
    }

    void EntityEditState::handleEvent(const sf::Event& curEvent)
    {
        mEditorState->handleEvent(curEvent);

        switch (curEvent.type)
        {
        case sf::Event::MouseButtonPressed:
        {
            if (curEvent.mouseButton.button == sf::Mouse::Left)
            {
                if (!mMouseLeftDown)
                {
                    onClick(curEvent);
                    mMouseLeftDown = true;
                }
            }
            break;
        }
        case sf::Event::MouseButtonReleased:
        {
            mMouseLeftDown = false;
            if (mEntityMoveGoingOn)
            {
                mEntityMoveGoingOn = false;
                for (const auto& e : mSelectedEntities)
                {
                    if (e.has<ungod::TransformComponent>())
                        mEFrame->getWorldActionWrapper().endEntityMoveSession(e);
                }
            }
            break;
        }
        case sf::Event::MouseMoved:
        {
            if (mMouseLeftDown)
            {
                if (!mEntityMoveGoingOn)
                {
                    mEntityMoveGoingOn = true;
                    for (const auto& e : mSelectedEntities)
                    {
                        if (e.has<ungod::TransformComponent>())
                            mEFrame->getWorldActionWrapper().startEntityMoveSession(e);
                    }
                }

                for (const auto& e : mSelectedEntities)
                {
                    if (e.has<ungod::TransformComponent>())
                    {
                        sf::Vector2f mouseMove = mEditorState->getCamera().getZoom() * sf::Vector2f{(float)(curEvent.mouseMove.x - mMouseLastPos.x), (float)(curEvent.mouseMove.y - mMouseLastPos.y)};
                        mEFrame->getWorldActionWrapper().moveEntity(e, mouseMove);
                    }
                }
            }
            mMouseLastPos.x = curEvent.mouseMove.x;
            mMouseLastPos.y = curEvent.mouseMove.y;
            break;
        }
        case sf::Event::KeyPressed:
        {
            switch(curEvent.key.code)
            {
                case sf::Keyboard::Delete:
                {
                    auto err = wxMessageDialog(nullptr, _("Do you really want to delete the selected entities?"), wxMessageBoxCaptionStr, wxOK | wxCANCEL | wxCENTER);
                    if (err.ShowModal() == wxID_OK)
                    {
                        for (const auto& e : mSelectedEntities)
                        {
                            mEFrame->getWorldActionWrapper().removeEntity(e);
                        }
                        clearEntitySelection();
                    }
                    break;
                }
                case sf::Keyboard::E:
                {
                    if (!mDesignerOpenPressed)
                    {
                        mDesignerOpenPressed = true;
                        openDesigners();
                    }
                    break;
                }
                case sf::Keyboard::C:
                {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && !mCopyPressed)
                    {
                        mCopyPressed = true;
                        mCopyTimeCenter = mEFrame->getCanvas()->getWindow().mapPixelToCoords(
                            {(int)mEFrame->getCanvas()->getWindow().getSize().x/2,
                             (int)mEFrame->getCanvas()->getWindow().getSize().y/2},
                              mEFrame->getCanvas()->getEditorState()->getCamera().getView() );
                        mCopiedEntities = mSelectedEntities;
                    }
                    break;
                }
                case sf::Keyboard::V:
                {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && !mPastePressed)
                    {
                        mPastePressed = true;
                        std::vector<ungod::Entity> cpy;
                        cpy.reserve(mCopiedEntities.size());
                        for (auto& e : mCopiedEntities)
                        {
                            cpy.emplace_back(e.getWorld().makeCopy(e));  //replace entities by their copies
                        }
                        mEFrame->addEntities(cpy, mCopyTimeCenter);
                    }
                    break;
                }
                default: break;
            }
            break;
        }
        case sf::Event::KeyReleased:
        {
            switch(curEvent.key.code)
                {
                case sf::Keyboard::Left:
                    {
                        for (const auto& e : mSelectedEntities)
                        {
                            mEFrame->getWorldActionWrapper().startEntityMoveSession(e);
                            mEFrame->getWorldActionWrapper().moveEntity(e, sf::Vector2f{-1,0});
                            mEFrame->getWorldActionWrapper().endEntityMoveSession(e);
                        }
                        break;
                    }
                case sf::Keyboard::Right:
                    {
                        for (const auto& e : mSelectedEntities)
                        {
                            mEFrame->getWorldActionWrapper().startEntityMoveSession(e);
                            mEFrame->getWorldActionWrapper().moveEntity(e, sf::Vector2f{1,0});
                            mEFrame->getWorldActionWrapper().endEntityMoveSession(e);
                        }
                        break;
                    }
                case sf::Keyboard::Up:
                    {
                        for (const auto& e : mSelectedEntities)
                        {
                            mEFrame->getWorldActionWrapper().startEntityMoveSession(e);
                            mEFrame->getWorldActionWrapper().moveEntity(e, sf::Vector2f{-1,0});
                            mEFrame->getWorldActionWrapper().endEntityMoveSession(e);
                        }
                        break;
                    }
                case sf::Keyboard::Down:
                    {
                        for (const auto& e : mSelectedEntities)
                        {
                            mEFrame->getWorldActionWrapper().startEntityMoveSession(e);
                            mEFrame->getWorldActionWrapper().moveEntity(e, sf::Vector2f{1,0});
                            mEFrame->getWorldActionWrapper().endEntityMoveSession(e);
                        }
                        break;
                    }
                case sf::Keyboard::E:
                    {
                        mDesignerOpenPressed = false;
                        break;
                    }
                case sf::Keyboard::C:
                    {
                        mCopyPressed = false;
                        break;
                    }
                case sf::Keyboard::V:
                    {
                        mPastePressed = false;
                        break;
                    }
                default: break;
                }
            break;
        }
        default:
            break;
        }
    }

    void EntityEditState::update(const float delta)
    {
        mEditorState->update(delta);
    }

    void EntityEditState::render(sf::RenderTarget& target, sf::RenderStates states)
    {
        mEditorState->toggleDebugmode(true);
        mEditorState->render(target, states);
        mEditorState->toggleDebugmode(false);

        for (const auto& e : mSelectedEntities)
        {
            mEditorState->getCamera().renderBegin(e.getWorld().getRenderDepth());
            sf::RenderStates local = states;
            sf::RectangleShape rect;
            if (e.has<ungod::TransformComponent>())
            {
                  ungod::TransformComponent& transf = e.modify<ungod::TransformComponent>();
                  local.transform *= transf.getTransform();  //apply the transform of the entity
                  rect.setSize(transf.getLowerBounds() - transf.getUpperBounds());
                  rect.setPosition(transf.getUpperBounds());
            }
            if (e.has<ungod::TileMapComponent>())
            {
                  ungod::TileMapComponent& tm = e.modify<ungod::TileMapComponent>();
                  rect.setSize(sf::Vector2f{tm.getTileMap().getBounds().width, tm.getTileMap().getBounds().height});
                  rect.setPosition(tm.getTileMap().getPosition());
            }
            if (e.has<ungod::WaterComponent>())
            {
                  ungod::WaterComponent& water = e.modify<ungod::WaterComponent>();
                  rect.setSize(sf::Vector2f{water.getWater().getTileMap().getBounds().width, water.getWater().getTileMap().getBounds().height});
                  rect.setPosition(water.getWater().getTileMap().getPosition());
            }
            if (rect.getSize().x > 0 && rect.getSize().y > 0)
            {
                rect.setFillColor(sf::Color(255, 0, 0, 70));
                rect.setOutlineThickness(2);
                rect.setOutlineColor(sf::Color::Red);
                target.draw(rect, local);
            }
            mEditorState->getCamera().renderEnd();
        }
    }

    void EntityEditState::selectEntity(ungod::Entity e)
    {
        for (auto es = mSelectedEntities.begin(); es != mSelectedEntities.end(); ++es)
        {
            if (*es == e)
            {
                mSelectedEntities.erase(es);
                return;
            }
        }
        mSelectedEntities.emplace_back(e);
    }

    void EntityEditState::clearEntitySelection()
    {
        mSelectedEntities.clear();
    }

    void EntityEditState::init()
    {
        mEditorState->init();
    }

    void EntityEditState::close()
    {
        mEditorState->close();
    }

    void EntityEditState::onClick(const sf::Event& event)
    {
        if (!mEFrame->getSelectedWorld())
            return;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
        {
            bool anyClicked = false;
            //retrieve entities near mouse position
            sf::Vector2f mouseWorldPos = mApp->getWindow().mapPixelToCoords( {event.mouseButton.x, event.mouseButton.y}, mEditorState->getCamera().getView(mEFrame->getSelectedWorld()->getRenderDepth()));
            quad::PullResult<ungod::Entity> pull;
            mEFrame->getSelectedWorld()->getQuadTree().retrieve(pull, { mouseWorldPos.x, mouseWorldPos.y, 0.0f, 0.0f });
            for (const auto& e : pull.getList())
            {
                if (e.get<ungod::TransformComponent>().getBounds().contains( mouseWorldPos ))
                {
                    selectEntity(e);
                    anyClicked = true;
                }
            }
            if (!anyClicked)  //look for tilemaps and waters
            {
                for (const auto& e : mEFrame->getSelectedWorld()->getTileMapRenderer().getTileMapEntities())
                {
                    if (e.get<ungod::TileMapComponent>().getTileMap().getTiledata( mouseWorldPos ))
                    {
                        selectEntity(e);
                        anyClicked = true;
                    }
                }
                for (const auto& e : mEFrame->getSelectedWorld()->getTileMapRenderer().getWaterEntities())
                {
                    if (e.get<ungod::WaterComponent>().getWater().getTileMap().getTiledata( mouseWorldPos ))
                    {
                        selectEntity(e);
                        anyClicked = true;
                    }
                }
            }
            if (!anyClicked) //still non selected? clear!
            {
                clearEntitySelection();
            }
        }
    }

    void EntityEditState::openDesigners()
    {
        for (const auto& e : mSelectedEntities)
        {
            mEFrame->openEntityDesigner(e);
        }
    }


    wxBEGIN_EVENT_TABLE( EditorCanvas, wxControl )
        EVT_IDLE( EditorCanvas::OnIdle )
        EVT_PAINT( EditorCanvas::OnPaint )
        EVT_ERASE_BACKGROUND( EditorCanvas::OnEraseBackground )
        EVT_SIZE( EditorCanvas::onSizeChanged )
    wxEND_EVENT_TABLE()


    EditorCanvas::EditorCanvas(EditorFrame* editorframe,
                               wxWindow* parent,
                              wxWindowID id,
                              const wxPoint& cPosition,
                              const wxSize& cSize,
                              long cStyle) :
            wxControl(parent, id, cPosition, cSize, cStyle),
            ungod::Application("editor_app")
    {
        //set up the ungod application
        loadConfig();
        createWindow(GetHandle());
        init();

        getStateManager().addState<EditorState>(EDITOR_STATE);
        mEditorState = getStateManager().getState<EditorState>(EDITOR_STATE);
        mEditorState->setTranscendency(false);
        mEditorState->setTransparency(false);

        getStateManager().addState<EntityEditState>(ENTITY_EDIT_STATE, mEditorState, editorframe);
        mEntityEditState = getStateManager().getState<EntityEditState>(ENTITY_EDIT_STATE);
        mEntityEditState->setTranscendency(false);
        mEntityEditState->setTransparency(false);

        getStateManager().moveToForeground(EDITOR_STATE);
    }

    void EditorCanvas::load(const std::string& filepath)
    {
        mCurrentFile = filepath;
        if (boost::filesystem::exists(boost::filesystem::path(filepath)))
        {
            mEditorState->load(filepath);
        }
    }

    void EditorCanvas::save()
    {
        mEditorState->save(mCurrentFile);
    }


    void EditorCanvas::lookAt(const sf::Vector2f& position) const
    {
        mEditorState->mCamContrl.getCamera().lookAt(position);
    }


    const sf::Vector2f& EditorCanvas::getCameraCenter() const
    {
        return mEditorState->mCamContrl.getCamera().getCenter();
    }


    void EditorCanvas::selectEntity(ungod::Entity e) const
    {
        mEntityEditState->selectEntity(e);
    }


    void EditorCanvas::OnIdle(wxIdleEvent& idleEvent)
    {
        // Send a paint message when the control is idle, to ensure maximum framerate
        Refresh();
    }

    void EditorCanvas::OnPaint(wxPaintEvent& paintEvent)
    {
        // Prepare the control to be repainted
        if ( isRunning() )  //underlying application is running
        {
            wxPaintDC Dc(this);
            processEvents();
            update();
            render();
        }
    }

    void EditorCanvas::onSizeChanged(wxSizeEvent& sizeEvent)
    {
        //recreate the window
        //the version below is bugged, e.g. the light system does not work properly
        //reason is, that the default view of the underlying window is incorrect and setSize does not
        //alter it
        //creating a new window will also reset the default view, which is fine
        createWindow(GetHandle());
        //getWindow().setSize( sf::Vector2u{ (unsigned)sizeEvent.GetSize().x, (unsigned)sizeEvent.GetSize().y } );
        mEditorState->getCamera().setViewSize(sf::Vector2f{(float)sizeEvent.GetSize().x, (float)sizeEvent.GetSize().y});
    }

    EditorCanvas::~EditorCanvas()
    {
    }
}
