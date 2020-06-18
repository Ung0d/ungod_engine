#include "Canvas.h"
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

		auto bounds = mEditorState->getWorldGraph().getActiveNode()->getBounds();
        sf::RenderStates nodeStates = states;
        nodeStates.transform.translate({ bounds.left, bounds.top });

        for (const auto& e : mSelectedEntities)
        {
            mEditorState->getWorldGraph().getCamera().renderBegin(e.getWorld().getRenderDepth());
            sf::RenderStates local = nodeStates;
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


	WorldGraphState::WorldGraphState(ungod::Application& app, ungod::StateID id, EditorState& editorState, EditorFrame* editorframe) :
		ungod::State(app, id), mEditorframe(editorframe), mEditorState(editorState), mCamera(app.getWindow()), mCamContrl(mCamera), mFont("data/arial.ttf"),
		mSelectedNode(nullptr), mClicked(false), mMouseLastPos(sf::Mouse::getPosition(app.getWindow())), mConnect(nullptr), mCornerSelected(-1)
	{
		mClickChecker.onClick([this](const sf::Vector2i& pos)
			{
				sf::Vector2f mouseWorldPos = mApp.getWindow().mapPixelToCoords(pos, mCamera.getView())/SCALE;
				mEditorState.getWorldGraph().updateReferencePosition(mouseWorldPos);
				mEditorState.getWorldGraph().getCamera().lookAt(mouseWorldPos);
				closeState();
				mApp.getStateManager().moveToForeground(EDITOR_STATE);
			});
	}


	void WorldGraphState::handleEvent(const sf::Event& event)
	{
		mCamContrl.handleEvent(event);
		mClickChecker.handleEvent(event);

		if (event.type == sf::Event::MouseButtonPressed)
		{
			sf::Vector2i mousePos = sf::Mouse::getPosition(mApp.getWindow());
			sf::Vector2f mouseWorldPos = mApp.getWindow().mapPixelToCoords(mousePos, mCamera.getView()) / SCALE;
			ungod::WorldGraphNode* clickedNode = mEditorState.getWorldGraph().getNode(mouseWorldPos);
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				if (!mClicked)
				{
					mTotalMove = { 0.0f,0.0f };
					if (clickedNode)
					{
						auto bounds = clickedNode->getBounds();
						std::array<sf::Vector2f, 4> points{ sf::Vector2f{bounds.left, bounds.top},
															sf::Vector2f{bounds.left+bounds.width, bounds.top},
															sf::Vector2f{bounds.left + bounds.width, bounds.top + bounds.height},
															sf::Vector2f{bounds.left, bounds.top + bounds.height} };
						for (int i = 0; i < 4; i++)
							if (ungod::distance(points[i], mouseWorldPos) < (bounds.width+ bounds.height)*CORNER_CLICK_RANGE)
							{
								mCornerSelected = i;
								break;
							}
					}
					mSelectedNode = clickedNode;
				}
				mClicked = true;
			}
			else if (event.mouseButton.button == sf::Mouse::Right)
			{
				wxMenu mnu;
				mnu.Append(0, "Connect from here");
				if (mConnect && mConnect != clickedNode)
					mnu.Append(1, "Connect to here");
				mnu.Bind(wxEVT_COMMAND_MENU_SELECTED, [this, clickedNode](wxCommandEvent& event)
					{
						switch (event.GetId()) {
						case 0:
							mConnect = clickedNode;
							break;
						case 1:
							mEditorState.getWorldGraph().connect(*mConnect, *clickedNode);
							break;
						default:
							break;
						}
					});
				mEditorframe->PopupMenu(&mnu);
			}
		}
		else if (event.type == sf::Event::MouseMoved)
		{
			if (mSelectedNode)
			{
				sf::Vector2f mouseMove = (1 / SCALE) * mCamera.getZoom() * sf::Vector2f{ (float)(event.mouseMove.x - mMouseLastPos.x), (float)(event.mouseMove.y - mMouseLastPos.y) };
				if (mCornerSelected == -1)
					mSelectedNode->move(mouseMove);
				mTotalMove += mouseMove;
			}
			mMouseLastPos.x = event.mouseMove.x;
			mMouseLastPos.y = event.mouseMove.y;
		}
		else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
		{
			switch (mCornerSelected)
			{
			case 0:
				mSelectedNode->move(mTotalMove);
				mSelectedNode->setSize(sf::Vector2f{ mSelectedNode->getBounds().width, mSelectedNode->getBounds().height }-mTotalMove);
				break;
			case 1:
				mSelectedNode->move({ 0.0f, mTotalMove.y });
				mSelectedNode->setSize(sf::Vector2f{ mSelectedNode->getBounds().width, mSelectedNode->getBounds().height }+sf::Vector2f{ mTotalMove.x, -mTotalMove.y });
				break;
			case 2:
				mSelectedNode->setSize(sf::Vector2f{ mSelectedNode->getBounds().width, mSelectedNode->getBounds().height }+mTotalMove);
				break;
			case 3:
				mSelectedNode->move({ mTotalMove.x, 0.0f });
				mSelectedNode->setSize(sf::Vector2f{ mSelectedNode->getBounds().width, mSelectedNode->getBounds().height }+sf::Vector2f{ -mTotalMove.x, mTotalMove.y });
				break;
			default:
				break;
			}
			mClicked = false;
			mSelectedNode = nullptr;
			mCornerSelected = -1;
		}
	}

	void WorldGraphState::update(float delta)
	{
	}

	void WorldGraphState::render(sf::RenderTarget& target, sf::RenderStates states)
	{
		//states.transform.scale(SCALE, SCALE);
		mCamera.renderBegin();
		for (unsigned i = 0; i < mEditorState.getWorldGraph().getALlists().getVertexCount(); i++)
		{
			ungod::WorldGraphNode* node = mEditorState.getWorldGraph().getNode(i);
			sf::FloatRect bounds = node->getBounds();

			//draw rect
			sf::Color rectColor = sf::Color{ 112,128,144 };
			auto res = mColorMapper.find(node);
			if (res != mColorMapper.end())
				rectColor = res->second;

			sf::RectangleShape rect{ {SCALE * bounds.width, SCALE * bounds.height} };
			rect.setFillColor(rectColor);
			rect.setOutlineThickness(1000*SCALE);
			rect.setOutlineColor(sf::Color::White);
			rect.setPosition({ SCALE*bounds.left, SCALE * bounds.top});
			target.draw(rect, states);

			//draw incident edges
			for (const auto& n : mEditorState.getWorldGraph().getALlists().getNeighbors(i))
			{
				sf::FloatRect adjacentBounds = mEditorState.getWorldGraph().getNode(n.destination)->getBounds();
				sf::Vertex line[2];
				line[0].position = sf::Vector2f{ SCALE * (bounds.left+bounds.width/2), SCALE * (bounds.top + bounds.height / 2) };
				line[0].color = sf::Color::White;
				line[1].position = sf::Vector2f{ SCALE * (adjacentBounds.left + adjacentBounds.width / 2), SCALE * (adjacentBounds.top + adjacentBounds.height / 2) };
				line[1].color = sf::Color::White;
				target.draw(line, 2, sf::LineStrip, states);
			}

			//draw text
			if (mFont.isLoaded())
			{
				float resizeFactor = (rect.getSize().x + rect.getSize().y) / 10000.0f;
				sf::Text textID{ node->getIdentifier(), mFont.get(), unsigned(TEXTSIZE*SCALE* resizeFactor) };
                textID.setPosition(SCALE * (bounds.left + 0.25f*bounds.width), SCALE * (bounds.top + 0.25f*bounds.height));
                
                std::string posStr = "(" + std::to_string(node->getBounds().left) + "," +
                    std::to_string(node->getBounds().top) + ")";
                sf::Text textNodepos{ posStr, mFont.get(), unsigned(TEXTSIZE * SCALE * resizeFactor) };
                textNodepos.setPosition(SCALE * (bounds.left + 0.25f * bounds.width), SCALE * (bounds.top + 0.25f * bounds.height) + textID.getGlobalBounds().height);

                std::string sizeStr = "(" + std::to_string(node->getBounds().width) + "," +
                    std::to_string(node->getBounds().height) + ")";
                sf::Text textNodesize{ sizeStr, mFont.get(), unsigned(TEXTSIZE * SCALE * resizeFactor) };
                textNodesize.setPosition(SCALE * (bounds.left + 0.25f * bounds.width), SCALE * (bounds.top + 0.25f * bounds.height) + textID.getGlobalBounds().height + textNodepos.getGlobalBounds().height);


                target.draw(textID, states);
                target.draw(textNodepos, states);
                target.draw(textNodesize, states);
			}
		}
		mCamera.renderEnd();
	}

	void WorldGraphState::init()
	{
		mClicked = false;
		mSelectedNode = nullptr;
        mCamera.setZoom(0.25f);
	}

	void WorldGraphState::close()
	{

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
            ungod::Application()
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

		getStateManager().addState<WorldGraphState>(WORLD_GRAPH_STATE, *mEditorState, editorframe);
		mWorldGraphState = getStateManager().getState<WorldGraphState>(WORLD_GRAPH_STATE);
		mWorldGraphState->setTranscendency(false);
		mWorldGraphState->setTransparency(false);

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
        mEditorState->getWorldGraph().getCamera().setViewSize(sf::Vector2f{(float)sizeEvent.GetSize().x, (float)sizeEvent.GetSize().y});
		mWorldGraphState->mCamera.setViewSize(sf::Vector2f{ (float)sizeEvent.GetSize().x, (float)sizeEvent.GetSize().y });
    }

    EditorCanvas::~EditorCanvas()
    {
    }
}
