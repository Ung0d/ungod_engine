#include "WorldGraphState.h"
#include "Utility.h"
#include "Canvas.h"
#include "EditorFrame.h"

namespace uedit
{
	const sf::Color WorldGraphState::NODE_DEFAULT_COLOR = sf::Color{ 112,128,144 };

	WorldGraphState::WorldGraphState(ungod::Application& app, ungod::StateID id, EditorState& editorState, EditorFrame* editorframe) :
		ungod::State(app, id), mEditorframe(editorframe), mEditorState(editorState), mCamera(app.getWindow()), mCamContrl(mCamera), mFont("data/arial.ttf"),
		mSelectedNode(nullptr), mClicked(false), mMouseLastPos(sf::Mouse::getPosition(app.getWindow())), mConnect(nullptr), mCornerSelected(-1),
		mNodeChangeDialog(new NodeChangeDialog(editorframe, -1, wxPoint(100, 100)))
	{
		mClickChecker.onClick([this](const sf::Vector2i& pos)
			{
				sf::Vector2f mouseWorldPos = mApp.getWindow().mapPixelToCoords(pos, mCamera.getView())/SCALE;
				mEditorState.getWorldGraph().updateReferencePosition(mouseWorldPos);
				mEditorState.getWorldGraph().getCamera().lookAt(mouseWorldPos);
				closeState();
				mApp.getStateManager().moveToForeground(EDITOR_STATE);
			});
		mNodeChangeDialog->Hide();
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
                {
                    mnu.Append(1, "Connect to here");
                }
                mnu.Append(2, "Set position or size");
                mnu.Append(3, "Set color");
				mnu.Bind(wxEVT_COMMAND_MENU_SELECTED, [this, clickedNode](wxCommandEvent& event)
					{
						switch (event.GetId()) {
						case 0:
							mConnect = clickedNode;
							break;
						case 1:
							mEditorState.getWorldGraph().connect(*mConnect, *clickedNode);
							break;
                        case 2:
							mNodeChangeDialog->setNode(clickedNode);
							mNodeChangeDialog->Show();
                            break;
                        case 3:
						{
							sf::Color prev = NODE_DEFAULT_COLOR;
							auto res = mColorMapper.find(clickedNode);
							if (res != mColorMapper.end())
								prev = res->second;
							mColorData.SetColour(convertColor(prev));
							wxColourDialog dlg(mEditorframe->getCanvas(), &mColorData);
							dlg.Bind(wxEVT_COLOUR_CHANGED, [this, clickedNode](wxColourDialogEvent& event)
							{
								mColorMapper[clickedNode] = convertColor(event.GetColour());
							});
							if (dlg.ShowModal() != wxID_OK)
								mColorMapper[clickedNode] = prev;
							break;
						}
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
			sf::Color rectColor = NODE_DEFAULT_COLOR;
			auto res = mColorMapper.find(node);
			if (res != mColorMapper.end())
				rectColor = res->second;

			sf::RectangleShape rect{ {SCALE * bounds.width, SCALE * bounds.height} };
			rect.setFillColor(rectColor);
			rect.setOutlineThickness(100*SCALE);
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


	NodeChangeDialog::NodeChangeDialog(wxWindow* parent, wxWindowID id, const wxPoint& pos)
		: wxDialog(parent, id, _("_ properties"), pos, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP), mNode(nullptr)
	{
		wxSizer* boxsizer = new wxBoxSizer(wxVERTICAL);

		{
			mPosX = new StatDisplay<float>("position x:", this, -1);
			mPosX->connectSetter([this](float x)
				{
					if (!mNode) return;
					mNode->setPosition({ x, mNode->getPosition().y });
				});
			mPosX->connectGetter([this]() 
				{
					if (!mNode) return 0.0f;
					return mNode->getPosition().x;
				});
			boxsizer->Add(mPosX, 0, wxALIGN_CENTER_HORIZONTAL);
		}
		{
			mPosY = new StatDisplay<float>("position y:", this, -1);
			mPosY->connectSetter([this](float y)
				{
					if (!mNode) return;
					mNode->setPosition({ mNode->getPosition().x, y });
				});
			mPosY->connectGetter([this]()
				{
					if (!mNode) return 0.0f;
					return mNode->getPosition().y;
				});
			boxsizer->Add(mPosY, 0, wxALIGN_CENTER_HORIZONTAL);
		}
		{
			mSizeX = new StatDisplay<float>("size x:", this, -1);
			mSizeX->connectSetter([this](float sx)
				{
					if (!mNode) return;
					mNode->setSize({ sx, mNode->getSize().y });
				});
			mSizeX->connectGetter([this]() 
				{
					if (!mNode) return 0.0f;
					return mNode->getSize().x;
				});
			boxsizer->Add(mSizeX, 0, wxALIGN_CENTER_HORIZONTAL);
		}
		{
			mSizeY = new StatDisplay<float>("size y:", this, -1);
			mSizeY->connectSetter([this](float sy)
				{
					if (!mNode) return;
					mNode->setSize({ mNode->getSize().x, sy });
				});
			mSizeY->connectGetter([this]() 
				{
					if (!mNode) return 0.0f;
					return mNode->getSize().y;
				});
			boxsizer->Add(mSizeY, 0, wxALIGN_CENTER_HORIZONTAL);
		}

		SetSizer(boxsizer);
		Fit();
	}

	void NodeChangeDialog::setNode(ungod::WorldGraphNode* node)
	{
		mNode = node;
		SetTitle(_(node->getIdentifier() + " properties"));
		mNodeChangedLink.disconnect();
		mNodeChangedLink = node->onNodeChanged([this]() { refresh(); });
		refresh();
	}

	void NodeChangeDialog::refresh()
	{
		mPosX->refreshValue();
		mPosY->refreshValue();
		mSizeX->refreshValue();
		mSizeY->refreshValue();
	}
}

namespace ungod
{
	//serialization code
	void SerialBehavior<uedit::WorldGraphState>::serialize(const uedit::WorldGraphState& data, MetaNode serializer, SerializationContext& context)
	{
		std::vector<sf::Color> colors;
		for (unsigned i = 0; i < data.mEditorState.getWorldGraph().getALlists().getVertexCount(); i++)
		{
			ungod::WorldGraphNode* node = data.mEditorState.getWorldGraph().getNode(i);
			auto res = data.mColorMapper.find(node);
			if (res != data.mColorMapper.end())
				colors.emplace_back(res->second);
			else
				colors.emplace_back(uedit::WorldGraphState::NODE_DEFAULT_COLOR);
		}
		context.serializePropertyContainer("nodeColorMap", colors, serializer);
	}

	void DeserialBehavior<uedit::WorldGraphState>::deserialize(uedit::WorldGraphState& data, MetaNode deserializer, DeserializationContext& context)
	{
		std::vector<sf::Color> colors;
		auto init = [&colors](std::size_t num) { colors.reserve(num); };
		auto ref = [&colors](const sf::Color& col) { colors.emplace_back(col); };
		context.first(context.deserializeContainer<sf::Color>(init, ref), "nodeColorMap", deserializer);
		for (int i = 0; i < colors.size(); i++)
		{
			ungod::WorldGraphNode* node = data.mEditorState.getWorldGraph().getNode(i);
			if (node)
				data.mColorMapper[node] = colors[i];
		}
	}
}