#include "Canvas.h"
#include "WorldGraphState.h"
#include "EditorFrame.h"

namespace uedit
{
    EditorState::EditorState(ungod::Application& app, ungod::StateID id) :
        ungod::ScriptedGameState(app,id),
        mUpdateFlag(true),
        mRenderFlag(true),
        mDebugRendering(false),
        mCamContrl(getWorldGraph().getCamera())
    {
		getWorldGraph().onActiveNodeChanged([this](ungod::WorldGraph& wg, ungod::WorldGraphNode& oldNode, ungod::WorldGraphNode& newNode)
			{
                wg.getCamera().lookAt(newNode.mapToGlobalPosition({0.0f, 0.0f}));
			});
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
                        mEFrame->getActionManager().transformActions().endEntityMoveSession(e);
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
                            mEFrame->getActionManager().transformActions().startEntityMoveSession(e);
                    }
                }

                for (const auto& e : mSelectedEntities)
                {
                    if (e.has<ungod::TransformComponent>())
                    {
                        sf::Vector2f mouseMove = mEditorState->getWorldGraph().getCamera().getZoom() * sf::Vector2f{(float)(curEvent.mouseMove.x - mMouseLastPos.x), (float)(curEvent.mouseMove.y - mMouseLastPos.y)};
                        mEFrame->getActionManager().transformActions().moveEntity(e, mouseMove);
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
                            mEFrame->getActionManager().removeEntity(e);
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
                              mEFrame->getCanvas()->getEditorState()->getWorldGraph().getCamera().getView() );
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
                            mEFrame->getActionManager().transformActions().startEntityMoveSession(e);
                            mEFrame->getActionManager().transformActions().moveEntity(e, sf::Vector2f{-1,0});
                            mEFrame->getActionManager().transformActions().endEntityMoveSession(e);
                        }
                        break;
                    }
                case sf::Keyboard::Right:
                    {
                        for (const auto& e : mSelectedEntities)
                        {
                            mEFrame->getActionManager().transformActions().startEntityMoveSession(e);
                            mEFrame->getActionManager().transformActions().moveEntity(e, sf::Vector2f{1,0});
                            mEFrame->getActionManager().transformActions().endEntityMoveSession(e);
                        }
                        break;
                    }
                case sf::Keyboard::Up:
                    {
                        for (const auto& e : mSelectedEntities)
                        {
                            mEFrame->getActionManager().transformActions().startEntityMoveSession(e);
                            mEFrame->getActionManager().transformActions().moveEntity(e, sf::Vector2f{-1,0});
                            mEFrame->getActionManager().transformActions().endEntityMoveSession(e);
                        }
                        break;
                    }
                case sf::Keyboard::Down:
                    {
                        for (const auto& e : mSelectedEntities)
                        {
                            mEFrame->getActionManager().transformActions().startEntityMoveSession(e);
                            mEFrame->getActionManager().transformActions().moveEntity(e, sf::Vector2f{1,0});
                            mEFrame->getActionManager().transformActions().endEntityMoveSession(e);
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

		/*auto bounds = mEditorState->getWorldGraph().getActiveNode()->getBounds();
        sf::RenderStates nodeStates = states;
        nodeStates.transform.translate({ bounds.left, bounds.top });*/

        for (const auto& e : mSelectedEntities)
        {
            mEditorState->getWorldGraph().getCamera().renderBegin(e.getWorld().getRenderDepth());
            sf::RenderStates local = states;
            sf::RectangleShape rect;
            if (e.has<ungod::TransformComponent>())
            {
                ungod::TransformComponent& transf = e.modify<ungod::TransformComponent>();
                local.transform *= transf.getTransform();  //apply the transform of the entity
                rect.setSize(transf.getLowerBounds() - transf.getUpperBounds());
                rect.setPosition(transf.getUpperBounds());
				rect.setFillColor(sf::Color(255, 0, 0, 70));
				rect.setOutlineThickness(2);
				rect.setOutlineColor(sf::Color::Red);
				target.draw(rect, local);
            }
            mEditorState->getWorldGraph().getCamera().renderEnd();
        }

        //render layer bounds
        quad::PullResult<ungod::WorldGraphNode*> pull;
        sf::Vector2f windowTop = target.mapPixelToCoords({}, mEditorState->getWorldGraph().getCamera().getView());
        sf::Vector2f windowBot = target.mapPixelToCoords(static_cast<sf::Vector2i>(target.getSize()), mEditorState->getWorldGraph().getCamera().getView());
        mEditorState->getWorldGraph().getQuadTree().retrieve(pull, {windowTop.x, windowTop.y, windowBot.x - windowTop.x, windowBot.y - windowTop.y});
        for (auto* n : pull.getList())
        {
            mEditorState->getWorldGraph().getCamera().renderBegin();
            auto nbounds = n->getBounds();
            nbounds.left = 0;
            nbounds.top = 0;
            if (sf::FloatRect{windowTop, windowBot - windowTop}.intersects(nbounds))
            {
                sf::RectangleShape rect;
                rect.setPosition(std::max(windowTop.x, nbounds.left), std::max(windowTop.y, nbounds.top));
                rect.setSize(sf::Vector2f{ std::min(windowBot.x, nbounds.left + nbounds.width), std::min(windowBot.y, nbounds.top + nbounds.height) } - rect.getPosition());
                rect.setFillColor(sf::Color::Transparent);
                rect.setOutlineColor(sf::Color::Green);
                rect.setOutlineThickness(10.0f);
                target.draw(rect, states);
            }
            mEditorState->getWorldGraph().getCamera().renderEnd();
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
            sf::Vector2f mouseWorldPos = mApp.getWindow().mapPixelToCoords( 
                                    {event.mouseButton.x, event.mouseButton.y}, 
                                    mEditorState->getWorldGraph().getCamera().getView(mEFrame->getSelectedWorld()->getRenderDepth()));
			mouseWorldPos = mEFrame->getSelectedWorld()->getNode().mapToLocalPosition(mouseWorldPos);
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
            ungod::Application(),
            mEditorframe(editorframe)
    {
        //set up the ungod application
        loadConfig();
        createWindow(GetHandle());
        init(false);

        getStateManager().addState<EditorState>(EDITOR_STATE);
        mEditorState = getStateManager().getState<EditorState>(EDITOR_STATE);
        mEditorState->setTranscendency(false);
        mEditorState->setTransparency(false);

        getStateManager().addState<EntityEditState>(ENTITY_EDIT_STATE, mEditorState, editorframe);
        mEntityEditState = getStateManager().getState<EntityEditState>(ENTITY_EDIT_STATE);
        mEntityEditState->setTranscendency(false);
        mEntityEditState->setTransparency(false);

		getStateManager().addState<WorldGraphState>(WORLD_GRAPH_STATE, *mEditorState, editorframe);
		mWorldGraphState = getStateManager().getState<WorldGraphState>(WORLD_GRAPH_STATE);
		mWorldGraphState->setTranscendency(false);
		mWorldGraphState->setTransparency(false);

        getStateManager().moveToForeground(EDITOR_STATE);
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
        if ( mEditorframe->entityDesginersActive()==0u && isRunning() )  //underlying application is running
        {
            //wxPaintDC Dc(this);
            processEvents();
            if (update())
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
        mEditorState->getWorldGraph().getCamera().setViewSize(sf::Vector2f{(float)sizeEvent.GetSize().x, (float)sizeEvent.GetSize().y});
		mWorldGraphState->mCamera.setViewSize(sf::Vector2f{ (float)sizeEvent.GetSize().x, (float)sizeEvent.GetSize().y });
    }

    EditorCanvas::~EditorCanvas()
    {
    }
}

namespace ungod
{
    //serialization code
    void SerialBehavior<uedit::EditorCanvas>::serialize(const uedit::EditorCanvas& data, MetaNode serializer, SerializationContext& context)
    {
        if (!data.getMasterFile().empty())
        {
            context.serializeProperty("gameMasterFile", data.getMasterFile(), serializer);
            data.getEditorState()->save(data.getMasterFile());
        }
        context.serializeObject("worldGraphState", *data.mWorldGraphState, serializer);
    }

    void DeserialBehavior<uedit::EditorCanvas>::deserialize(uedit::EditorCanvas& data, MetaNode deserializer, DeserializationContext& context)
    {
        std::string master;
        auto attr = context.first(context.deserializeProperty(master), "gameMasterFile", deserializer);
        data.setMasterFile(master);
        if (boost::filesystem::exists(boost::filesystem::path(master)))
        {
            data.getEditorState()->load(master);
        }
        attr = context.next(context.deserializeObject(*data.mWorldGraphState), "worldGraphState", deserializer, attr);
    }
}
