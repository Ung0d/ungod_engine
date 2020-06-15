#include "LayerDisplay.h"
#include "EntityDialog.h"
#include "EditorFrame.h"
#include "wx/sizer.h"

namespace uedit
{
    enum LayerDisplayKeys
    {
        LAYER_LIST,
        ENTITY_LIST,
        BUTTON_MV_UP,
        BUTTON_MV_DOWN,
        BUTTON_TOGGLE,
        BUTTON_PROPERTIES
    };

    LayerDisplay::LayerDisplay(EditorCanvas* canvas,
                     EditorFrame* eframe,
                     wxWindow* parent, wxWindowID id,
                  const wxPoint& cPosition, const wxSize& cSize, long cStyle) :
                      wxWindow(parent, id, cPosition, cSize, cStyle),
                      mCanvas(canvas),
                      mEditorFrame(eframe),
                      mLayerlist(nullptr),
                      mLpw(nullptr)
    {
        setup();
    }

    void LayerDisplay::setup()
    {
		ungod::WorldGraphNode * node = mCanvas->getEditorState()->getWorldGraph().getActiveNode();
		if (!node) return;

        DestroyChildren();
        SetSizer(NULL);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

        wxButton* buttonDown = new wxButton(this, BUTTON_MV_DOWN, _("move down"));
        wxButton* buttonUp = new wxButton(this, BUTTON_MV_UP, _("move up"));
        wxButton* buttonToggle = new wxButton(this, BUTTON_TOGGLE, _("enable"));

        buttonDown->Bind(wxEVT_BUTTON, &LayerDisplay::onMoveDown, this);
        buttonUp->Bind(wxEVT_BUTTON, &LayerDisplay::onMoveUp, this);
        buttonToggle->Bind(wxEVT_BUTTON, &LayerDisplay::toggleSelection, this);

        hbox->Add(buttonDown, 1, wxCENTER);
        hbox->Add(buttonUp, 1, wxCENTER);
        hbox->Add(buttonToggle, 1, wxCENTER);

        wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);

        wxButton* buttonEntityCreate = new wxButton(this, BUTTON_MV_DOWN, _("create entity"));
        wxButton* buttonEntityEdit = new wxButton(this, BUTTON_MV_UP, _("edit mode"));
        wxButton* buttonDeselect = new wxButton(this, BUTTON_TOGGLE, _("deselect"));

        buttonEntityCreate->Bind(wxEVT_BUTTON, &LayerDisplay::onEntityCreate, this);
        buttonEntityEdit->Bind(wxEVT_BUTTON, &LayerDisplay::onEntityEdit, this);
        buttonDeselect->Bind(wxEVT_BUTTON, &LayerDisplay::onEntityDeselect, this);

        hbox2->Add(buttonEntityCreate, 1, wxCENTER);
        hbox2->Add(buttonEntityEdit, 1, wxCENTER);
        hbox2->Add(buttonDeselect, 1, wxCENTER);


        wxBoxSizer* hbox3 = new wxBoxSizer(wxHORIZONTAL);

        wxButton* buttonLayerProperties = new wxButton(this, BUTTON_PROPERTIES, _("properties"));

        buttonLayerProperties->Bind(wxEVT_BUTTON, &LayerDisplay::onLayerPropterties, this);

        hbox3->Add(buttonLayerProperties, 1, wxCENTER);


        wxArrayInt order;
        wxArrayString items;

        for (std::size_t i = 0; i < node->getLayers().getVector().size(); ++i)
        {
            if (node->getLayers().getVector()[i].second)
                order.push_back(i);
            else
                order.push_back(~i);
            items.push_back(node->getLayers().getVector()[i].first->getName());
        }

        mLayerlist = new wxRearrangeList(this, LAYER_LIST, wxDefaultPosition, wxDefaultSize, order, items);
        mLayerlist->Bind(wxEVT_LISTBOX, &LayerDisplay::onLayerSelect, this);

        mEntityList = new wxListCtrl(this, ENTITY_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
        mEntityList->Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &LayerDisplay::onEntityRightClick, this);
        mEntityList->Bind(wxEVT_LIST_ITEM_ACTIVATED, &LayerDisplay::onEntityListSelect, this);
        {
            wxListItem enName;
            enName.SetId(0);
            enName.SetText( _("Name") );
            enName.SetWidth(60);
            mEntityList->InsertColumn(0, enName);

            wxListItem enId;
            enId.SetId(1);
            enId.SetText( _("ID") );
            enId.SetWidth(30);
            mEntityList->InsertColumn(1, enId);

            wxListItem enInst;
            enInst.SetId(2);
            enInst.SetText( _("Instantiation") );
            enInst.SetWidth(200);
            mEntityList->InsertColumn(2, enInst);
        }

        vbox->Add(mLayerlist, 2, wxEXPAND);
        vbox->Add(mEntityList, 5, wxEXPAND);
        vbox->Add(hbox, 1, wxEXPAND);
        vbox->Add(hbox2, 1, wxEXPAND);
        vbox->Add(hbox3, 1, wxEXPAND);

        SetSizer(vbox);
        Fit();
		GetParent()->Fit();
    }

    int LayerDisplay::getSelection() const
    {
        return mLayerlist->GetSelection(); //if this is true, there is an active world node selected and it is safe to access it
    }

    ungod::World* LayerDisplay::getSelectedWorld() const
    {
		if (getSelection() != wxNOT_FOUND)
			return static_cast<ungod::World*>(mCanvas->getEditorState()->getWorldGraph().getActiveNode()->getLayers().getVector()[getSelection()].first.get());
        else
            return nullptr;
    }

    void LayerDisplay::onMoveDown(wxCommandEvent & event)
    {
        if (getSelection() != wxNOT_FOUND)
        {
            mLayerlist->MoveCurrentDown();
			mCanvas->getEditorState()->getWorldGraph().getActiveNode()->moveLayerDown( (std::size_t) mLayerlist->GetSelection() );
        }
    }

    void LayerDisplay::onMoveUp(wxCommandEvent & event)
    {
        if (getSelection() != wxNOT_FOUND)
        {
            mLayerlist->MoveCurrentUp();
			mCanvas->getEditorState()->getWorldGraph().getActiveNode()->moveLayerUp( (std::size_t) mLayerlist->GetSelection() );
        }
    }

    void LayerDisplay::toggleSelection(wxCommandEvent & event)
    {
        if (getSelection() != wxNOT_FOUND)
        {
            std::size_t i = (std::size_t) mLayerlist->GetSelection();
			mCanvas->getEditorState()->getWorldGraph().getActiveNode()->setActive(i, 
				!mCanvas->getEditorState()->getWorldGraph().getActiveNode()->getLayers().isActive(i));
            mLayerlist->Check(i, mCanvas->getEditorState()->getWorldGraph().getActiveNode()->getLayers().isActive(i));
        }
    }

    void LayerDisplay::onEntityCreate(wxCommandEvent& event)
    {
        ungod::World* world = getSelectedWorld();

        if (!world) return;

        EntityCreateDialog createDia( world, this, -1, _("Create entity"), wxPoint(100, 100) );
        if ( createDia.ShowModal() == wxID_OK )
        {
            ungod::Entity e = createDia.getEntity();
            mEditorFrame->addEntity(e);
        }
    }

    void LayerDisplay::onEntityEdit(wxCommandEvent& event)
    {
        if (mEditorFrame->getCanvas()->getStateManager().getForegroundStateID() != ENTITY_EDIT_STATE)
        {
            mCanvas->getStateManager().moveToForeground(ENTITY_EDIT_STATE);
        }
        else
        {
            mCanvas->getStateManager().moveToForeground(EDITOR_STATE);
        }
    }


    void LayerDisplay::onEntityDeselect(wxCommandEvent& event)
    {
        mCanvas->getEntityEditState()->clearEntitySelection();
    }


    void LayerDisplay::onLayerPropterties(wxCommandEvent& event)
    {
        if (mLpw) return;
        ungod::World* world = getSelectedWorld();
        if (!world) return;
        mLpw = new LayerPropertiesWindow(world, this, -1);
        mLpw->Bind(wxEVT_CLOSE_WINDOW, &LayerDisplay::onPropertiesClose, this);
        mLpw->Show();
    }


    void LayerDisplay::onPropertiesClose(wxCloseEvent& event)
    {
        mLpw->Destroy();
        mLpw = nullptr;
    }


    void LayerDisplay::onLayerSelect(wxCommandEvent& event)
    {
        mEnCreationLink.disconnect();
        mEnDestructionLink.disconnect();

        mEntityList->DeleteAllItems();

        mIDMapping.clear();
        mEntities.clear();

        ungod::World* world = getSelectedWorld();
        if (!world)
            return;


        quad::PullResult<ungod::Entity> res;
        world->getQuadTree().getContent(res);

        for (auto const& e : res.getList())
        {
            listEntity(e);
        }

        mEnCreationLink = world->onEntityCreation([this] (ungod::Entity e)
        {
            listEntity(e);
        });
        mEnDestructionLink = world->onEntityDestruction([this] (ungod::Entity e)
        {
            mEntityList->DeleteItem(mIDMapping[e.getID()]);
            mIDMapping.erase(e.getID());
            mEntities.erase(std::remove(mEntities.begin(), mEntities.end(), e), mEntities.end());
        });

        mEditorFrame->registerWorld(world);
    }


    void LayerDisplay::listEntity(ungod::Entity e)
    {
        wxListItem entityItem;
        entityItem.SetId(mIDMapping.size());
        mEntityList->InsertItem( entityItem );

        mEntityList->SetItem(mIDMapping.size(), 0, e.getWorld().getName(e));
        mEntityList->SetItem(mIDMapping.size(), 1, std::to_string(e.getID()));
        mEntityList->SetItem(mIDMapping.size(), 2, e.getInstantiation()->getSerialIdentifier());

        mIDMapping[e.getID()] = mIDMapping.size();
        mEntities.push_back(e);
    }


    void LayerDisplay::onEntityRightClick(wxListEvent& event)
    {
        wxMenu mnu;
        mnu.SetClientData(reinterpret_cast<void *>(&event));
        mnu.Append(INSPECT, 	"inspect");
        mnu.Append(SHOW, 	"show");
        mnu.Append(RENAME, 	"rename");
        mnu.Append(DELETE_EN, "delete");
        mnu.Bind(wxEVT_COMMAND_MENU_SELECTED, &LayerDisplay::onEntityMenuSelect, this);
        PopupMenu(&mnu);
    }


    void LayerDisplay::onEntityMenuSelect(wxCommandEvent& event)
    {
        ungod::World* world = getSelectedWorld();

        wxListEvent *clickevt =static_cast<wxListEvent*>(static_cast<wxMenu *>(event.GetEventObject())->GetClientData());
        switch(event.GetId())
        {
 		case INSPECT:
            mEditorFrame->openEntityDesigner(mEntities[clickevt->GetIndex()]);
 			break;
 		case SHOW:
            mCanvas->lookAt(
                mEntities[clickevt->GetIndex()].getWorld().getNode().mapToGlobalPosition(
					mEntities[clickevt->GetIndex()].get<ungod::TransformComponent>().getCenterPosition())
						* mEntities[clickevt->GetIndex()].getWorld().getRenderDepth());
 			break;
 		case RENAME:
        {
 		    wxTextEntryDialog dlg(this, "Enter entity name");
            if (dlg.ShowModal() == wxID_OK)
            {
                auto name = std::string(dlg.GetValue().mb_str());
                world->tagWithName(mEntities[clickevt->GetIndex()], name);
                mEntityList->SetItem(mIDMapping[mEntities[clickevt->GetIndex()].getID()], 0, name);
            }
 			break;
        }
        case DELETE_EN:
        {
            ungod::Entity e = mEntities[clickevt->GetIndex()];
            e.getWorld().destroy(e);
            break;
        }
        default:
            break;
        }
    }


    void LayerDisplay::onEntityListSelect(wxListEvent& event)
    {
        mCanvas->selectEntity(mEntities[event.GetIndex()]);
    }
}


namespace ungod
{
    //serialization code
    void SerialBehavior<uedit::LayerDisplay>::serialize(const uedit::LayerDisplay& data, MetaNode serializer, SerializationContext& context)
    {

    }

    void DeserialBehavior<uedit::LayerDisplay>::deserialize(uedit::LayerDisplay& data, MetaNode deserializer, DeserializationContext& context)
    {

    }
}
