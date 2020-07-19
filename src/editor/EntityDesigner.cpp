#include "EntityDesigner.h"
#include "TransformEditState.h"
#include "VisualsEditState.h"
#include "TilemapEditState.h"
#include "CollidersEditState.h"
#include "ParticleEditState.h"
#include "LightEditState.h"
#include "EditorFrame.h"

namespace uedit
{
    enum DesignerKeys
    {
        COMP_CHECKBOX = 177,
        COMP_TABS = 178,
        EDIT_UNDO = 555,
        EDIT_REDO = 556
    };

//connect events
    wxBEGIN_EVENT_TABLE(EntityDesigner, wxFrame)
        EVT_CHECKLISTBOX(COMP_CHECKBOX, EntityDesigner::onComponentCheck)
        EVT_CLOSE(EntityDesigner::onWindowClose)
        EVT_NOTEBOOK_PAGE_CHANGED(COMP_TABS, EntityDesigner::onTabChanged)
        EVT_MENU(EDIT_UNDO, EntityDesigner::onEditUndo)
        EVT_MENU(EDIT_REDO, EntityDesigner::onEditRedo)
    wxEND_EVENT_TABLE()

    const wxSize EntityDesigner::DEFAULT_SIZE = {1600, 1000};

    EntityDesigner::EntityDesigner ( EditorCanvas& canvas, ActionManager& actionManager, ungod::Entity e,
                                wxWindow * parent, wxWindowID id,
                           const wxPoint & position )
    : wxFrame( parent, id, _("Entity Designer"), position, DEFAULT_SIZE, wxDEFAULT_FRAME_STYLE|wxFRAME_FLOAT_ON_PARENT), mCanvas(canvas), mActionManager(actionManager), mEntity(e)
    {
        reset();

        {
            wxMenu* menuEdit = new wxMenu;
            menuEdit->Append(EDIT_UNDO, "&undo\tCtrl-Z");
            menuEdit->Append(EDIT_REDO, "&redo\tCtrl-Shift-Z");

            wxMenuBar *menuBar = new wxMenuBar;
            menuBar->Append( menuEdit, "&Edit");
            SetMenuBar( menuBar );
            CreateStatusBar();
        }

        wxPanel *panel = new wxPanel(this, -1);

        wxBoxSizer* bigbox = new wxBoxSizer(wxHORIZONTAL);

        {
            wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

            mTabs = new wxNotebook(panel, COMP_TABS, wxDefaultPosition, wxDefaultSize, wxNB_TOP);
            wxWindow* componentOrganiser = new wxWindow(mTabs,-1,wxDefaultPosition,wxDefaultSize);
            buildComponentWindow(componentOrganiser);
            mTabs->AddPage(componentOrganiser, "Components");
            //mTabs->Layout();

            hbox->Add(mTabs,1, wxEXPAND);
            vbox->Add(hbox,1, wxEXPAND);

            bigbox->Add(vbox, 1, wxEXPAND);
        }

        {
            wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

            mEntityPreview = new EntityPreview(mCanvas, mEntity, mActionManager, panel, -1, wxDefaultPosition, wxSize(DEFAULT_SIZE.x - mTabs->GetSize().x, DEFAULT_SIZE.y));

            hbox->Add(mEntityPreview,1, wxEXPAND);
            vbox->Add(hbox,1, wxEXPAND);

            bigbox->Add(vbox, 5, wxEXPAND);
        }

        buildSpecWindows();

        panel->SetSizer(bigbox);

        Centre();
    }

    void EntityDesigner::onEditUndo(wxCommandEvent& event)
    {
        mActionManager.getEditorFrame()->getActionManager().undo();
    }

    void EntityDesigner::onEditRedo(wxCommandEvent& event)
    {
        mActionManager.getEditorFrame()->getActionManager().redo();
    }

    void EntityDesigner::onEntityContentsChanged(ungod::Entity e, ungod::World& world)
    {
        if (e != mEntity)
            return;

        if (mTransformOrganizer)
        {
            mTransformOrganizer->onEntityContentsChanged(e);
        }

        if (mVisualsOrganizer)
        {
            mVisualsOrganizer->onEntityContentsChanged(e);
        }

        if (mLightOrganizer)
        {
            mLightOrganizer->onEntityContentsChanged(e);
        }
    }

    void EntityDesigner::onEntityPositionChanged(ungod::Entity e, ungod::World& world, const sf::Vector2f& position)
    {
        if (e != mEntity)
            return;

        if (mTransformOrganizer)
        {
            mTransformOrganizer->onEntityPositionChanged(e, position);
        }
    }

    void EntityDesigner::onEntitySizeChanged(ungod::Entity e, ungod::World& world, const sf::Vector2f& siz)
    {
        if (e != mEntity)
            return;

        if (mTransformOrganizer)
        {
            mTransformOrganizer->onEntitySizeChanged(e, siz);
        }
    }

    void EntityDesigner::onParticleEmitterChanged(ungod::Entity e, ungod::World& world, const std::string& key, const ungod::PSData& data)
    {
        if (e != mEntity)
            return;

        if (mParticlesOrganizer)
        {
            mParticlesOrganizer->onParticleEmitterChanged(key, data);
        }
    }

    void EntityDesigner::onTexrectInitChanged(ungod::Entity e, ungod::World& world, const std::string& key, const ungod::PSData& data)
    {
        if (e != mEntity)
            return;

        if (mParticlesOrganizer)
        {
            mParticlesOrganizer->onTexrectInitChanged(key, data);
        }
    }

    void EntityDesigner::onAffectorsChanged(ungod::Entity e, ungod::World& world, const std::string& key, const ungod::PSData& data)
    {
        if (e != mEntity)
            return;

        if (mParticlesOrganizer)
        {
            mParticlesOrganizer->onAffectorsChanged(key, data);
        }
    }


    void EntityDesigner::onColliderChanged(ungod::Entity e, ungod::World& world)
    {
        if (e != mEntity)
            return;

        if (mMovementCollidersOrganizer)
            mMovementCollidersOrganizer->onColliderChanged(e);

        if (mSemanticsCollidersOrganizer)
            mSemanticsCollidersOrganizer->onColliderChanged(e);
    }


    void EntityDesigner::buildComponentWindow(wxWindow* window)
    {
        wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

        wxStaticText *st1 =  new wxStaticText(window, -1, wxT("Optional components of the entity: "));
        vbox->Add(st1, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP | wxBOTTOM, 14);

        std::vector<std::size_t> checkLater;

        checkOptional<ungod::TransformComponent>("Transform", checkLater);
        checkOptional<ungod::VisualsComponent>("Visuals", checkLater);
        checkOptional<ungod::SpriteComponent>("Sprite", checkLater);
        checkOptional<ungod::MultiSpriteComponent>("MultiSprite", checkLater);
        checkOptional<ungod::VertexArrayComponent>("VertexArray", checkLater);
        checkOptional<ungod::VisualAffectorComponent>("VisualAffector", checkLater);
        checkOptional<ungod::MultiVisualAffectorComponent>("MultiVisualAffector", checkLater);
        checkOptional<ungod::SpriteMetadataComponent>("SpriteMetadata", checkLater);
        checkOptional<ungod::AnimationComponent>("Animation", checkLater);
        checkOptional<ungod::MultiAnimationComponent>("MultiAnimation", checkLater);
        checkOptional<ungod::EntityBehaviorComponent>("EntityBehavior[Script support]", checkLater);
        checkOptional<ungod::RigidbodyComponent<ungod::MOVEMENT_COLLISION_CONTEXT>>("Rigidbody[Movement]", checkLater);
        checkOptional<ungod::RigidbodyComponent<ungod::SEMANTICS_COLLISION_CONTEXT>>("Rigidbody[Semantics]", checkLater);
        checkOptional<ungod::MultiRigidbodyComponent<ungod::MOVEMENT_COLLISION_CONTEXT>>("MultiRigidbody[Movement]", checkLater);
        checkOptional<ungod::MultiRigidbodyComponent<ungod::SEMANTICS_COLLISION_CONTEXT>>("MultiRigidbody[Semantics]", checkLater);
        checkOptional<ungod::MovementComponent>("Movement", checkLater);
        checkOptional<ungod::SteeringComponent<ungod::script::Environment>>("Steering", checkLater);
        checkOptional<ungod::EntityUpdateTimer>("EntityUpdateTimer", checkLater);
        checkOptional<ungod::SoundEmitterComponent>("SoundEmitterComponent", checkLater);
        checkOptional<ungod::PathFinderComponent>("PathFinder", checkLater);
        checkOptional<ungod::ShadowEmitterComponent>("ShadowEmitter", checkLater);
        checkOptional<ungod::LightEmitterComponent>("LightEmitter", checkLater);
        checkOptional<ungod::LightAffectorComponent>("LightAffector", checkLater);
        checkOptional<ungod::MultiShadowEmitter>("MultiShadowEmitter", checkLater);
        checkOptional<ungod::MultiLightEmitter>("MultiLightEmitter", checkLater);
        checkOptional<ungod::MultiLightAffector>("MultiLightAffector", checkLater);
        checkOptional<ungod::TileMapComponent>("TileMap", checkLater);
        checkOptional<ungod::WaterComponent>("Water", checkLater);
        checkOptional<ungod::ParticleSystemComponent>("ParticleSystem", checkLater);
        checkOptional<ungod::MusicEmitterComponent>("MusicEmitter", checkLater);

        mChecker = new wxCheckListBox(window, COMP_CHECKBOX, wxDefaultPosition, wxDefaultSize, mComponentNames.size(), mComponentNames.data());
        mChecker->Layout();
        mChecker->Fit();

        for (const auto& c : checkLater)
        {
            mChecker->Check(c);
        }

        wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
        hbox->Add(mChecker, 0, wxEXPAND);
        vbox->Add(hbox, 0, wxCENTER);
        window->SetSizer(vbox);
        window->Fit();
    }

    void EntityDesigner::buildSpecWindows()
    {
        if (mEntity.has<ungod::TransformComponent>())
        {
            mTransformOrganizer = new EntityTransformWindow(mEntity, mActionManager, mTabs,-1,wxDefaultPosition,wxDefaultSize);
            mTabs->AddPage(mTransformOrganizer, "Transform");
        }
        if (mEntity.has<ungod::VisualsComponent>())
        {
            mVisualsOrganizer = new EntityVisualsWindow(mEntity, mActionManager, mTabs,-1,wxDefaultPosition,wxDefaultSize);
            mTabs->AddPage(mVisualsOrganizer, "Visuals");
        }
        if (mEntity.has<ungod::RigidbodyComponent<ungod::MOVEMENT_COLLISION_CONTEXT>>() ||
            mEntity.has<ungod::MultiRigidbodyComponent<ungod::MOVEMENT_COLLISION_CONTEXT>>())
        {
            mMovementCollidersOrganizer = new EntityCollidersWindow<ungod::MOVEMENT_COLLISION_CONTEXT>(mEntity, mActionManager, mTabs,-1,wxDefaultPosition,wxDefaultSize);
            mTabs->AddPage(mMovementCollidersOrganizer, "MovementCollision");
        }
        if (mEntity.has<ungod::RigidbodyComponent<ungod::SEMANTICS_COLLISION_CONTEXT>>() ||
            mEntity.has<ungod::MultiRigidbodyComponent<ungod::SEMANTICS_COLLISION_CONTEXT>>())
        {
            mSemanticsCollidersOrganizer = new EntityCollidersWindow<ungod::SEMANTICS_COLLISION_CONTEXT>(mEntity, mActionManager, mTabs,-1,wxDefaultPosition,wxDefaultSize);
            mTabs->AddPage(mSemanticsCollidersOrganizer, "SemanticsCollision");
        }
        if (mEntity.has<ungod::EntityBehaviorComponent>())
        {
            mScriptOrganizer = new EntityScriptWindow(mEntity, mActionManager, *mActionManager.getEditorFrame()->getScriptManager(), mTabs,-1,wxDefaultPosition,wxDefaultSize);
            mTabs->AddPage(mScriptOrganizer, "Script");
        }
        if (mEntity.has<ungod::TileMapComponent>())
        {
            mTileMapOrganizer = new EntityTileMapWindow(mEntity, this, mActionManager, mTabs,-1,wxDefaultPosition,wxDefaultSize);
            mTabs->AddPage(mTileMapOrganizer, "TileMap");
        }
        if (mEntity.has<ungod::WaterComponent>())
        {
            mWaterOrganizer = new EntityWaterWindow(mCanvas, mEntity, this, mActionManager, mTabs,-1,wxDefaultPosition,wxDefaultSize);
            mTabs->AddPage(mWaterOrganizer, "Water");
        }
        if (mEntity.has<ungod::LightEmitterComponent>() || mEntity.has<ungod::MultiLightEmitter>() ||
            mEntity.has<ungod::ShadowEmitterComponent>() || mEntity.has<ungod::MultiShadowEmitter>() ||
            mEntity.has<ungod::LightAffectorComponent>() || mEntity.has<ungod::MultiLightAffector>())
        {
            mLightOrganizer = new EntityLightWindow(mEntity, mActionManager, mTabs,-1,wxDefaultPosition,wxDefaultSize);
            mTabs->AddPage(mLightOrganizer, "Light");
        }
        if (mEntity.has<ungod::ParticleSystemComponent>())
        {
            mParticlesOrganizer = new EntityParticleSystemWindow(mEntity, mActionManager, *mEntityPreview, mTabs,-1,wxDefaultPosition,wxDefaultSize);
            mTabs->AddPage(mParticlesOrganizer, "ParticleSystem");
        }
        if (mEntity.has<ungod::MusicEmitterComponent>())
        {
            mAudioEmitterOrganizer = new EntityAudioEmitterWindow(mEntity, mActionManager, mTabs,-1,wxDefaultPosition,wxDefaultSize);
            mTabs->AddPage(mAudioEmitterOrganizer, "MusicEmitter");
        }
    }

    void EntityDesigner::reset()
    {
        mTransformOrganizer = nullptr;
        mVisualsOrganizer = nullptr;
        mScriptOrganizer = nullptr;
        mTileMapOrganizer = nullptr;
        mWaterOrganizer = nullptr;
        mLightOrganizer = nullptr;
        mParticlesOrganizer = nullptr;
        mAudioEmitterOrganizer = nullptr;
        mMovementCollidersOrganizer = nullptr;
        mSemanticsCollidersOrganizer = nullptr;
    }

    void EntityDesigner::onWindowClose(wxCloseEvent& event)
    {
        mActionManager.getEditorFrame()->clearEntityDesigner(this);
        event.Skip();
    }

    void EntityDesigner::onTabChanged(wxBookCtrlEvent& event)
    {
        if (event.GetSelection() == 0)
        {
            mEntityPreview->toggleDefault();
        }
        else if (mTabs->GetPageText(event.GetSelection()) == _("Transform"))
        {
            mEntityPreview->toggle<TransformEditState>();
        }
        else if (mTabs->GetPageText(event.GetSelection()) == _("Visuals"))
        {
            mEntityPreview->toggle<VisualsEditState>();
        }
        else if (mTabs->GetPageText(event.GetSelection()) == _("TileMap"))
        {
            mEntityPreview->toggle<TileMapEditState>();
        }
        else if (mTabs->GetPageText(event.GetSelection()) == _("Water"))
        {
           // mEntityPreview->toggle<TileMapEditState>();
        }
        else if (mTabs->GetPageText(event.GetSelection()) == _("ParticleSystem"))
        {
            mEntityPreview->toggle<ParticleEditState>(mEntity);
        }
        else if (mTabs->GetPageText(event.GetSelection()) == _("MovementCollision"))
        {
            mEntityPreview->toggle<CollidersEditState<ungod::MOVEMENT_COLLISION_CONTEXT>>(*mMovementCollidersOrganizer);
        }
        else if (mTabs->GetPageText(event.GetSelection()) == _("SemanticsCollision"))
        {
            mEntityPreview->toggle<CollidersEditState<ungod::SEMANTICS_COLLISION_CONTEXT>>(*mSemanticsCollidersOrganizer);
        }
        else if (mTabs->GetPageText(event.GetSelection()) == _("Light"))
        {
            mEntityPreview->toggle<LightEditState>(*mLightOrganizer);
        }
    }

    void EntityDesigner::onComponentCheck( wxCommandEvent& event )
    {
        if (mChecker->IsChecked(event.GetInt()))
        {
            mAddCallbacks[event.GetInt()]();
        }
        else
        {
            mRemoveCallbacks[event.GetInt()]();
        }
        reset();
        std::size_t pc = mTabs->GetPageCount();
        for (std::size_t i = 1; i < pc; ++i)
            mTabs->DeletePage(mTabs->GetPageCount()-1);
        buildSpecWindows();
    }
}
