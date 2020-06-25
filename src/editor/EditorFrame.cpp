#include "EditorFrame.h"
#include "Utility.h"
#include "WaterDialog.h"
#include "StateProperties.h"
#include "wx/sizer.h"

namespace uedit
{
    //define an enum that holds unique identifiers for the menu items
    enum MenuItems
    {
        NEW_PROJECT,
        LOAD_PROJECT,
        SAVE_PROJECT,
        NEW_WORLD,
		NEW_NODE,
        STATE_PROPERTIES,
        EDIT_UNDO,
        EDIT_REDO,
        SHEET_LISTBOX,
        KEY_LISTBOX,
        SHEET_PREVIEW,
        PROCESS_CHECKBOX,
        SHEET_DIALOG,
        ENTITY_DESIGNER,
        SCRIPT_MANAGER,
        LAYER_DISPLAY,
        SCRIPT_TOGGLE
    };

    //connect events
    wxBEGIN_EVENT_TABLE(EditorFrame, wxFrame)
        EVT_MENU(NEW_PROJECT, EditorFrame::onFileNew)
        EVT_MENU(LOAD_PROJECT, EditorFrame::onFileLoad)
        EVT_MENU(SAVE_PROJECT, EditorFrame::onFileSave)
        EVT_MENU(NEW_WORLD, EditorFrame::onWorldNew)
		EVT_MENU(NEW_NODE, EditorFrame::onNodeNew)
        EVT_MENU(STATE_PROPERTIES, EditorFrame::onStateProperties)
        EVT_MENU(EDIT_UNDO, EditorFrame::onEditUndo)
        EVT_MENU(EDIT_REDO, EditorFrame::onEditRedo)
        EVT_MENU(SCRIPT_TOGGLE, EditorFrame::onScriptToggle)
    wxEND_EVENT_TABLE()


    EditorFrame::EditorFrame() :
        wxFrame(nullptr,
                wxID_ANY,
                "Ungod Editor",
                wxDefaultPosition,
                wxSize(1500, 700)),
                mContentSaved(true),
                mParent(nullptr),
                mCanvas(nullptr),
                mSheetPreview(nullptr),
                mScriptManager(nullptr),
                mCurrentState(EDITOR_STATE),
                mEditorTabs(nullptr),
                mActionManager(this)
    {
        mActionManager.onSomethingChanged([this]() { notifyProjectChanged(); });

        Maximize();

        wxMenu* menuFile = new wxMenu;
        menuFile->Append(NEW_PROJECT, "&new",
                         "Creates a new project file.");
        menuFile->Append(LOAD_PROJECT, "&load\tCtrl-L",
                         "Loads a existing project file.");
        menuFile->AppendSeparator();
        menuFile->Append(SAVE_PROJECT, "&save\tCtrl-S",
                         "Saves the current project file.");

        wxMenu* menuEdit = new wxMenu;
        menuEdit->Append(EDIT_UNDO, "&undo\tCtrl-Z");
        menuEdit->Append(EDIT_REDO, "&redo\tCtrl-Shift-Z");

        wxMenu* menuLayers = new wxMenu;
		menuLayers->Append(NEW_NODE, "&add node\tCtrl-N",
			"Creates a new node in the world graph");
		menuLayers->Append(NEW_WORLD, "&add world\tCtrl-W",
			"Creates a new world an adds it to the project.");
        menuLayers->Append(STATE_PROPERTIES, "&state properties",
                         "Opens a dialog to manage the basic properties of the gamestate.");

        wxMenu* menuScript = new wxMenu;
        menuScript->Append(SCRIPT_TOGGLE, "&toggles the script menu",
                         "Opens or cloces the script editor subwindow.");

        wxMenuBar *menuBar = new wxMenuBar;
        menuBar->Append( menuFile, "&Project" );
        menuBar->Append( menuEdit, "&Edit");
        menuBar->Append( menuLayers, "&WorldGraph" );
        menuBar->Append( menuScript, "&Scripts" );
        SetMenuBar( menuBar );
        CreateStatusBar();

        mParent = new wxPanel(this, wxID_ANY);
        mEditorTabs = new wxNotebook(mParent, wxID_ANY);
        mCanvas = new EditorCanvas(this, mParent, wxID_ANY);
		mCanvas->getEditorState()->getWorldGraph().onActiveNodeChanged([this](ungod::WorldGraph& wg, ungod::WorldGraphNode& oldNode, ungod::WorldGraphNode& newNode)
			{
				mLayerDisplay->setup();
			});
        mLayerDisplay = new LayerDisplay(mCanvas, this, mEditorTabs, LAYER_DISPLAY);
        mSheetPreview = new SheetPreview(mEditorTabs, SHEET_PREVIEW);

        wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

        hbox->Add(mEditorTabs, 1, wxEXPAND);

        mEditorTabs->AddPage(mLayerDisplay, "layers");
        mEditorTabs->AddPage(mSheetPreview, "sheets");

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
        vbox->Add(mCanvas, 30, wxEXPAND);
		{
			wxButton* graphViewButton = new wxButton(mParent, -1, "graph view");
			graphViewButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) 
				{
					mCanvas->getStateManager().getState(WORLD_GRAPH_STATE)->initState();
					mCanvas->getStateManager().moveToForeground(WORLD_GRAPH_STATE);
				});
			vbox->Add(graphViewButton, 1, wxCENTER);
		}
        hbox->Add(vbox, 8, wxEXPAND);

        mScriptManager = new ScriptManager("C:/Users/Felix/Desktop/GameProgramming/lota_final/res", *mCanvas, mActionManager, this,SCRIPT_MANAGER, wxPoint(200, 240));

        mParent->SetSizer(hbox);

		ungod::DeserializationContext context;
		if (context.read("meta_info.xml"))
		{
			context.deserializeRootObject(mMetaInfo);
			if (mMetaInfo.lastProject.size() > 0)
				loadProject(mMetaInfo.lastProject);
		}

		Fit();
    }

    void EditorFrame::onFileNew(wxCommandEvent& event)
    {
        ProjectCreateDialog dia (this, -1, ("Create a new project"), wxPoint(100, 100) );
        if (dia.ShowModal() == wxID_OK)
        {
            mCanvas->setMasterFile(std::string{dia.getGameMasterFilepath().mb_str()});
            mProjectFilePath = dia.getProjectFilepath();
            mLayerDisplay->setup();
            mMetaInfo.lastProject = dia.getProjectFilepath().mb_str();
            //Fit();
            saveProject();
        }
    }

    void EditorFrame::onFileLoad(wxCommandEvent& event)
    {
        auto path = getPathViaDialog(this, "xml");
        if (path.first)
        {
            loadProject(path.second);
            /*for (const auto& l : mCanvas->getEditorState()->getLayers().getVector())
            {
                //TODO TODO TODO sloppy hack, because World is currently the only render layers
                registerWorld(static_cast<ungod::World*>(l.first.get()));
            }
            mLayerDisplay->setup();*/
        }
    }

    void EditorFrame::onFileSave(wxCommandEvent& event)
    {
        saveProject();
    }

	void EditorFrame::onNodeNew(wxCommandEvent& event)
	{
		WorldGraphNodeCreateDialog graphDia(this, -1, _("Create a new world graph node"), wxPoint(100, 100));
		if (graphDia.ShowModal() != wxID_OK) return;
		WorldCreateDialog worldDia(this, -1, _("Create a new world"), wxPoint(100, 100));
		if (worldDia.ShowModal() != wxID_OK) return;
		ungod::WorldGraphNode& node = mCanvas->mEditorState->getWorldGraph().createNode(*mCanvas->mEditorState, graphDia.getIdentifier(), graphDia.getDatafile());
		node.setPosition({ graphDia.getPosX(), graphDia.getPosY() });
		node.setSize({ graphDia.getSizeX(), graphDia.getSizeY() });
		ungod::World* world = node.addWorld();
		world->setRenderDepth(worldDia.getDepth());
		world->setName(std::string{ worldDia.getName().mb_str() });
		mCanvas->mEditorState->getWorldGraph().updateReferencePosition({ graphDia.getPosX() + graphDia.getSizeX()/2, graphDia.getPosY() + graphDia.getSizeY()/2});
		registerWorld(world);
		mLayerDisplay->setup();
	}

    void EditorFrame::onWorldNew(wxCommandEvent& event)
    {
		ungod::WorldGraphNode* node = mCanvas->mEditorState->getWorldGraph().getActiveNode();
		if (!node)
			wxMessageBox(wxT("No world node selected."));
		else
		{ 
			WorldCreateDialog dia (this, -1, _("Create a new world"), wxPoint(100, 100) );
			if (dia.ShowModal() == wxID_OK)
			{
				ungod::World* world = node->addWorld();
				world->setRenderDepth(dia.getDepth());
				world->setName(std::string{ dia.getName().mb_str() });
				registerWorld(world);
				mLayerDisplay->setup();
			}
        }
    }

    void EditorFrame::onStateProperties(wxCommandEvent& event)
    {
        StateProperties statedia (*mCanvas->getEditorState(), this, -1, wxPoint(100, 100) );
        statedia.ShowModal();
    }

    void EditorFrame::onEditUndo(wxCommandEvent& event)
    {
        mActionManager.undo();
    }

    void EditorFrame::onEditRedo(wxCommandEvent& event)
    {
        mActionManager.redo();
    }


    bool EditorFrame::saveBeforeProceed()
    {
        if (!mContentSaved)
            {
                if (wxMessageBox(_("Current content has not been saved! Proceed?"), _("Please confirm"),
                                 wxICON_QUESTION | wxYES_NO, this) == wxNO )
                    return false;
            }
        return true;
    }

    void EditorFrame::registerWorld(ungod::World* world)
    {
        world->getVisualsHandler().onContentsChanged([this, world] (ungod::Entity e, const sf::FloatRect& rect)
        {
            for (auto d : mActiveDesigners)
               d->onEntityContentsChanged(e, *world);
        });
        world->getLightHandler().onContentsChanged([this, world] (ungod::Entity e, const sf::FloatRect& rect)
        {
            for (auto d : mActiveDesigners)
               d->onEntityContentsChanged(e, *world);
        });
        world->getSemanticsRigidbodyHandler().onContentsChanged([this, world] (ungod::Entity e, const sf::FloatRect& rect)
        {
            for (auto d : mActiveDesigners)
               d->onColliderChanged(e, *world);
        });
        world->getMovementRigidbodyHandler().onContentsChanged([this, world](ungod::Entity e, const sf::FloatRect& rect)
        {
            for (auto d : mActiveDesigners)
                d->onColliderChanged(e, *world);
        });
        world->getSemanticsRigidbodyHandler().onContentRemoved([this, world] (ungod::Entity e)
        {
            for (auto d : mActiveDesigners)
               d->onColliderChanged(e, *world);
        });
        world->getTransformHandler().onPositionChanged([this, world] (ungod::Entity e, const sf::Vector2f& pos)
        {
            for (auto d : mActiveDesigners)
               d->onEntityPositionChanged(e, *world, pos);
        });
        world->getTransformHandler().onSizeChanged([this, world] (ungod::Entity e, const sf::Vector2f& s)
        {
            for (auto d : mActiveDesigners)
               d->onEntitySizeChanged(e, *world, s);
        });
        world->getParticleSystemHandler().onEmitterChanged([this, world] (ungod::Entity e, const std::string& key, const ungod::PSData& data)
        {
            for (auto d : mActiveDesigners)
               d->onParticleEmitterChanged(e, *world, key, data);
        });
        world->getParticleSystemHandler().onTexrectInitChanged([this, world] (ungod::Entity e, const std::string& key, const ungod::PSData& data)
        {
            for (auto d : mActiveDesigners)
               d->onTexrectInitChanged(e, *world, key, data);
        });
        world->getParticleSystemHandler().onAffectorsChanged([this, world] (ungod::Entity e, const std::string& key, const ungod::PSData& data)
        {
            for (auto d : mActiveDesigners)
               d->onAffectorsChanged(e, *world, key, data);
        });
    }

    void EditorFrame::notifyProjectChanged()
    {
        if (mContentSaved)
        {
            mContentSaved = false;
            SetTitle(_("Ungod Editor*"));
        }
    }

    void EditorFrame::saveProject()
    {
        ungod::SerializationContext context;
        context.serializeRootObject(*this);
        context.save(mProjectFilePath);
        SetTitle(_("Ungod Editor"));
        mContentSaved = true;
		mCanvas->getEditorState()->getWorldGraph().save(*mCanvas->getEditorState());
    }

    void EditorFrame::loadProject(const std::string& filepath)
    {
        mProjectFilePath = filepath;
		mMetaInfo.lastProject = filepath;
        ungod::DeserializationContext context;
        if (context.read(mProjectFilePath))
			context.deserializeRootObject(*this); 
        SetTitle(_("Ungod Editor"));
        mContentSaved = true;
		Fit();
    }


    void EditorFrame::onScriptToggle(wxCommandEvent& event)
    {
        if (!mScriptManager)
        {
            mScriptManager->Show();
        }
        else
        {
            mScriptManager->Hide();
        }
    }


    void EditorFrame::openEntityDesigner(ungod::Entity e)
    {
        //dont open designer, if any one the existing desginers corresponds to the entity already
        for (auto d : mActiveDesigners)
        {
            if (d->getEntity() == e)
                return;
        }
        mActiveDesigners.emplace_back( new EntityDesigner(*mCanvas, mActionManager, e, this, ENTITY_DESIGNER) );
        mActiveDesigners.back()->Show();
    }

    void EditorFrame::clearEntityDesigner(EntityDesigner* ed)
    {
        mActiveDesigners.erase(std::remove(mActiveDesigners.begin(), mActiveDesigners.end(), ed), mActiveDesigners.end());
    }

    ungod::World* EditorFrame::getSelectedWorld()
    {
        if (mLayerDisplay->getSelection() == wxNOT_FOUND)
            return nullptr;
        else
        {
			ungod::WorldGraphNode* node = mCanvas->mEditorState->getWorldGraph().getActiveNode();
			if (!node)
				return nullptr;
            return node->getWorld(mLayerDisplay->getSelection());
        }
    }

    void EditorFrame::addEntity(ungod::Entity e)
    {
        if (e.has<ungod::TransformComponent>())
        {
            sf::Vector2f centerpos = mCanvas->getWindow().mapPixelToCoords(
                    {(int)mCanvas->getWindow().getSize().x/2, (int)mCanvas->getWindow().getSize().y/2}, mCanvas->getEditorState()->getWorldGraph().getCamera().getView() );
            centerpos.x *=  e.getWorld().getRenderDepth();
            centerpos.y *=  e.getWorld().getRenderDepth();
			centerpos = e.getWorld().getNode().mapToLocalPosition(centerpos);
            e.getWorld().getTransformHandler().setPosition(e, centerpos);
            e.getWorld().getQuadTree().insert(e);
        }
        openEntityDesigner(e);
    }

    void EditorFrame::addEntities(std::vector<ungod::Entity> entities, const sf::Vector2f& relativeTo)
    {
        sf::Vector2f centerpos = mCanvas->getWindow().mapPixelToCoords(
                {(int)mCanvas->getWindow().getSize().x/2, (int)mCanvas->getWindow().getSize().y/2}, mCanvas->getEditorState()->getWorldGraph().getCamera().getView() );
        sf::Vector2f offset = centerpos - relativeTo;

        for (auto& e : entities)
        {
            if (e.has<ungod::TransformComponent>())
            {
                e.getWorld().getTransformHandler().move(e, offset);
                e.getWorld().getQuadTree().insert(e);
            }
        }
    }

	EditorFrame::~EditorFrame()
	{
		ungod::SerializationContext context;
		context.serializeRootObject(mMetaInfo);
		context.save("meta_info.xml");
	}
}


namespace ungod
{
    //serialization code
    void SerialBehavior<uedit::EditorFrame>::serialize(const uedit::EditorFrame& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeObject("script_manager", *data.mScriptManager, serializer);
        context.serializeObject("sheet_preview", *data.mSheetPreview, serializer);
        context.serializeObject("canvas", *data.mCanvas, serializer);
    }

    void DeserialBehavior<uedit::EditorFrame>::deserialize(uedit::EditorFrame& data, MetaNode deserializer, DeserializationContext& context)
    {
        auto attr = context.first(context.deserializeObject(*data.mScriptManager), "script_manager", deserializer);
        attr = context.next(context.deserializeObject(*data.mSheetPreview), "sheet_preview", deserializer, attr);
        context.next(context.deserializeObject(*data.mCanvas), "canvas", deserializer, attr);
    }


	//serialization code
	void SerialBehavior<uedit::MetaInfo>::serialize(const uedit::MetaInfo& data, MetaNode serializer, SerializationContext& context)
	{
		context.serializeProperty("last_project", data.lastProject, serializer);
	}

	void DeserialBehavior<uedit::MetaInfo>::deserialize(uedit::MetaInfo& data, MetaNode deserializer, DeserializationContext& context)
	{
		auto attr = context.first(context.deserializeProperty(data.lastProject), "last_project", deserializer);
	}
}
