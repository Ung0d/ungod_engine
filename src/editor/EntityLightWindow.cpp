#include "EntityLightWindow.h"

namespace uedit
{
    LightComponentEdit::LightComponentEdit(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent, ungod::LightEmitterComponent& lightComponent)
     : wxPanel(parent), mEntity(e), mWorldAction(waw), mLightComponent(lightComponent)
    {
        wxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        {
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

            wxStaticText* texlabel = new wxStaticText(this, -1, _("Loaded texture: "));
            hbox->Add(texlabel, 1, wxALL | wxALIGN_CENTER_HORIZONTAL, 4);

            mTexPicker = new wxFilePickerCtrl( this, -1, fileDialogHeader("png"), wxFileSelectorPromptStr, fileDialogWildcard("png")  );
            mTexPicker->Bind(wxEVT_FILEPICKER_CHANGED, &LightComponentEdit::onLightTexturePicked, this);
            hbox->Add(mTexPicker,1, wxALL | wxALIGN_CENTER_HORIZONTAL, 4);

            vbox->Add(hbox,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mPosX = new StatDisplay<float>("position x:", this, -1);
            mPosX->connectSetter( [this, &lightComponent](float x)
            {
                mWorldAction.setLightPosition(mEntity, mLightComponent, { x, lightComponent.getLight().getPosition().y });
            } );
            mPosX->connectGetter( [this, &lightComponent]()
            {
                return lightComponent.getLight().getPosition().x;
            } );
            vbox->Add(mPosX,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mPosY = new StatDisplay<float>("position y:", this, -1);
            mPosY->connectSetter( [this, &lightComponent](float y)
            {
                mWorldAction.setLightPosition(mEntity, mLightComponent, { lightComponent.getLight().getPosition().x, y });
            } );
            mPosY->connectGetter( [this, &lightComponent]()
            {
                return lightComponent.getLight().getPosition().y;
            } );
            vbox->Add(mPosY,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mScaleX = new StatDisplay<float>("scale x:", this, -1);
            mScaleX->connectSetter( [this, &lightComponent](float x)
            {
                mWorldAction.setLightPosition(mEntity, mLightComponent, { x, lightComponent.getLight().getScale().y });
            } );
            mScaleX->connectGetter( [this, &lightComponent]()
            {
                return lightComponent.getLight().getScale().x;
            } );
            vbox->Add(mScaleX,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mScaleY = new StatDisplay<float>("scale y:", this, -1);
            mScaleY->connectSetter( [this, &lightComponent](float y)
            {
                mWorldAction.setLightPosition(mEntity, mLightComponent, { lightComponent.getLight().getScale().x, y });
            } );
            mScaleY->connectGetter( [this, &lightComponent]()
            {
                return lightComponent.getLight().getScale().y;
            } );
            vbox->Add(mScaleY,0,wxALIGN_CENTER_HORIZONTAL);
        }

        refreshStats();

        SetSizer(vbox);
        Fit();
    }

    void LightComponentEdit::refreshStats()
    {
        mPosX->setValue(mLightComponent.getLight().getPosition().x);
        mPosY->setValue(mLightComponent.getLight().getPosition().y);
        mScaleX->setValue(mLightComponent.getLight().getScale().x);
        mScaleY->setValue(mLightComponent.getLight().getScale().y);
    }

    void LightComponentEdit::onLightTexturePicked(wxFileDirPickerEvent& event)
    {
        mWorldAction.loadLightTexture(mEntity, mLightComponent, {mTexPicker->GetPath().mb_str()});
    }



    wxBEGIN_EVENT_TABLE(FlickeringDialog, wxDialog)
        EVT_BUTTON(wxID_OK, FlickeringDialog::onOk)
    wxEND_EVENT_TABLE()

    FlickeringDialog::FlickeringDialog (wxWindow * parent, wxWindowID id, const wxString & title, const wxPoint & pos)
    : wxDialog( parent, id, title, pos), mPeriod(DEFAULT_PERIOD), mStrength(DEFAULT_STRENGTH)
    {
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
        {
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
            hbox->Add(new wxStaticText(this, -1, "period:"), 1, wxCENTER);
            mPeriodCtrl = new wxTextCtrl(this, -1, std::to_string(DEFAULT_PERIOD));
            hbox->Add(mPeriodCtrl, 1, wxCENTER);
            vbox->Add(hbox, 1, wxCENTER);
        }
        {
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
            hbox->Add(new wxStaticText(this, -1, "strength:"), 1, wxCENTER);
            mStrengthCtrl = new wxTextCtrl(this, -1, std::to_string(DEFAULT_STRENGTH));
            hbox->Add(mStrengthCtrl, 1, wxCENTER);
            vbox->Add(hbox, 1, wxCENTER);
        }
        {
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
            hbox->Add(new wxButton( this, wxID_OK, _("Ok")), 1, wxCENTER);
            hbox->Add(new wxButton( this, wxID_CANCEL, _("Cancel")), 1, wxCENTER);
            vbox->Add(hbox, 1, wxCENTER);
        }
        SetSizer(vbox);
        Fit();
    }



    void FlickeringDialog::onOk( wxCommandEvent & event )
    {
        try
        {
            mPeriod = std::stof( std::string(mPeriodCtrl->GetLineText(0).mb_str()) );
            mStrength = std::stof( std::string(mStrengthCtrl->GetLineText(0).mb_str()) );

            EndModal(wxID_OK);
        }
        catch(const std::exception& e)
        {
            EndModal(wxID_CANCEL);
        }
    }


    LightAffectorEdit::LightAffectorEdit(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent, ungod::LightAffectorComponent& affector, ungod::LightEmitterComponent& emitter)
     : wxPanel(parent), mEntity(e), mWorldAction(waw), mAffectorComponent(affector), mEmitterComponent(emitter)
     {
        wxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        {
            mChoice = new wxChoice(this,-1);
            wxString keys[] = {"None", "Flickering", "Randomized Flickering"};
            mChoice->Set(3, keys);
            mChoice->Bind(wxEVT_CHOICE, &LightAffectorEdit::choiceChanged, this);
            vbox->Add(mChoice, 1, wxALIGN_CENTER_HORIZONTAL);
        }

        refreshStats();

        SetSizer(vbox);
        Fit();
     }


     void LightAffectorEdit::refreshStats()
     {
     }


    void LightAffectorEdit::choiceChanged(wxCommandEvent& event)
    {
        if (mChoice->GetSelection() == 1) //Flickering
        {
            FlickeringDialog fd(this, -1, "Flickering");
            if (fd.ShowModal() == wxID_OK)
                mWorldAction.setAffectorCallback(mEntity, mAffectorComponent, mEmitterComponent, ungod::LightFlickering(fd.getPeriod(), fd.getStrength()));
        }
        else if (mChoice->GetSelection() == 2) //Randomized Flickering
        {
            FlickeringDialog fd(this, -1, "RandomizedFlickering");
            if (fd.ShowModal() == wxID_OK)
                mWorldAction.setAffectorCallback(mEntity, mAffectorComponent, mEmitterComponent, ungod::RandomizedFlickering(fd.getPeriod(), fd.getStrength()));
        }
    }




    LightColliderEdit::LightColliderEdit(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent, ungod::ShadowEmitterComponent& shadowEmitter)
     : wxPanel(parent), mEntity(e), mWorldAction(waw), mShadowEmitter(shadowEmitter)
    {
        mSizer = new wxBoxSizer(wxVERTICAL);

        mLightOverShapeCheck = new wxCheckBox(this, -1, "light over shape");
        mLightOverShapeCheck->Bind(wxEVT_CHECKBOX, [this](wxCommandEvent&) {
                mWorldAction.setLightOverShape(mEntity, mShadowEmitter, mLightOverShapeCheck->IsChecked());
            });
        mSizer->Add(mLightOverShapeCheck,0,wxALIGN_CENTER_HORIZONTAL);

        wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
        wxButton* removeAll = new wxButton(this, -1, "Remove all");
        removeAll->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event)
            {
                mWorldAction.setPointCount(mEntity, mShadowEmitter, 0);
            });
        hbox->Add(removeAll,0,wxALIGN_CENTER_HORIZONTAL);
        wxButton* removeLast = new wxButton(this, -1, "Remove last");
        removeLast->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event)
            {
                if (mShadowEmitter.getCollider().getPointCount() > 0)
                    mWorldAction.setPointCount(mEntity, mShadowEmitter, mShadowEmitter.getCollider().getPointCount()-1);
            });
        hbox->Add(removeLast,0,wxALIGN_CENTER_HORIZONTAL);
        mSizer->Add(hbox,0,wxALIGN_CENTER_HORIZONTAL);

        mPosX.reserve(mShadowEmitter.getCollider().getPointCount());
        mPosY.reserve(mShadowEmitter.getCollider().getPointCount());
        for (unsigned i = 0; i < mShadowEmitter.getCollider().getPointCount(); i++)
            createPosPanel(i);

        refreshStats();

        SetSizerAndFit(mSizer);
        GetParent()->Fit();
    }


    void LightColliderEdit::refreshStats()
    {
        mLightOverShapeCheck->SetValue(mShadowEmitter.getCollider().getLightOverShape());

        while (mShadowEmitter.getCollider().getPointCount() < mPosX.size())
        {
            mPanels.back()->Destroy(); //also destroys child widgets
            mPanels.pop_back();
            mPosX.pop_back();
            mPosY.pop_back();
        }

        while (mShadowEmitter.getCollider().getPointCount() > mPosX.size())
            createPosPanel(mPosX.size());

        for (unsigned i = 0; i < mShadowEmitter.getCollider().getPointCount(); i++)
        {
            mPosX[i]->setValue(mShadowEmitter.getCollider().getPoint(i).x);
            mPosY[i]->setValue(mShadowEmitter.getCollider().getPoint(i).y);
        }

        SetSizerAndFit(mSizer);
        GetParent()->Fit();
    }


    void LightColliderEdit::createPosPanel(unsigned i)
    {
        mPanels.emplace_back(new wxPanel(this));
        wxSizer* vbox2 = new wxBoxSizer(wxVERTICAL);

        mPosX.emplace_back(new StatDisplay<float>("x"+std::to_string(i)+":", mPanels.back(), -1) );
        mPosX.back()->connectSetter( [this, i](float x)
        {
            mWorldAction.setPoint(mEntity, mShadowEmitter, { x, mShadowEmitter.getCollider().getPoint(i).y }, i);
        } );
        mPosX.back()->connectGetter( [this, i]()
        {
            return mShadowEmitter.getCollider().getPoint(i).x;
        } );
        vbox2->Add(mPosX.back(),0,wxALIGN_CENTER_HORIZONTAL);

        mPosY.emplace_back(new StatDisplay<float>("y"+std::to_string(i)+":", mPanels.back(), -1) );
        mPosY.back()->connectSetter( [this, i](float y)
        {
            mWorldAction.setPoint(mEntity, mShadowEmitter, { mShadowEmitter.getCollider().getPoint(i).x, y }, i);
        } );
        mPosY.back()->connectGetter( [this, i]()
        {
            return mShadowEmitter.getCollider().getPoint(i).y;
        } );
        vbox2->Add(mPosY.back(),0,wxALIGN_CENTER_HORIZONTAL);
        mPanels.back()->SetSizer(vbox2);
        mPanels.back()->Fit();
        mSizer->Add(mPanels.back(),0,wxALIGN_CENTER_HORIZONTAL);
    }




    EntityLightWindow::EntityLightWindow(ungod::Entity e, WorldActionWrapper& waw, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz)
        : wxWindow(parent, id, pos, siz), mEntity(e), mWorldAction(waw), mLightEdit(nullptr), mMultiLightPanel(nullptr)
    {
        wxSizer* boxsizer = new wxBoxSizer(wxVERTICAL);
        mNotebook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, wxNB_TOP);
        boxsizer->Add(mNotebook,0,wxEXPAND);

        if (e.has<ungod::LightEmitterComponent>())
        {
            mLightEdit = new LightComponentEdit(mEntity, mWorldAction, mNotebook, e.modify<ungod::LightEmitterComponent>());
            mNotebook->AddPage(mLightEdit,"Light");
        }

        if (e.has<ungod::MultiLightEmitter>())
        {
            mMultiLightPanel = new MultiComponentPanel<ungod::LightEmitterComponent, LightComponentEdit>(mNotebook, mEntity, mWorldAction);
            mNotebook->AddPage(mMultiLightPanel,"MultiLight");
        }

        if (e.has<ungod::LightAffectorComponent>() && e.has<ungod::LightEmitterComponent>())
        {
            mAffectorEdit = new LightAffectorEdit(mEntity, mWorldAction, mNotebook, e.modify<ungod::LightAffectorComponent>(), e.modify<ungod::LightEmitterComponent>());
            mNotebook->AddPage(mAffectorEdit,"Affect");
        }

        if (e.has<ungod::MultiLightAffector>() && e.has<ungod::MultiLightEmitter>())
        {
            mMultiAffectorPanel = new MultiComponentPanel<ungod::LightAffectorComponent, LightAffectorEdit, ungod::LightEmitterComponent>(mNotebook, mEntity, mWorldAction);
            mNotebook->AddPage(mMultiAffectorPanel,"MultiAffect");
        }

        if (e.has<ungod::ShadowEmitterComponent>())
        {
            mLightColliderEdit = new LightColliderEdit(mEntity, mWorldAction, mNotebook, e.modify<ungod::ShadowEmitterComponent>());
            mNotebook->AddPage(mLightColliderEdit,"Collider");
        }

        if (e.has<ungod::MultiShadowEmitter>())
        {
            mMultiLightColliderPanel = new MultiComponentPanel<ungod::ShadowEmitterComponent, LightColliderEdit>(mNotebook, mEntity, mWorldAction);
            mNotebook->AddPage(mMultiLightColliderPanel,"MultiCollider");
        }

        SetSizer(boxsizer);
        Fit();
    }

    void EntityLightWindow::onEntityContentsChanged(ungod::Entity e)
    {
        refreshStats();
    }

    void EntityLightWindow::refreshStats()
    {
        if (mEntity.has<ungod::LightEmitterComponent>())
            mLightEdit->refreshStats();
        if (mEntity.has<ungod::MultiLightEmitter>())
            mMultiLightPanel->refreshStats();
        if (mEntity.has<ungod::LightAffectorComponent>())
            mAffectorEdit->refreshStats();
        if (mEntity.has<ungod::MultiLightAffector>())
            mMultiAffectorPanel->refreshStats();
        if (mEntity.has<ungod::ShadowEmitterComponent>())
            mLightColliderEdit->refreshStats();
        if (mEntity.has<ungod::MultiShadowEmitter>())
            mMultiLightColliderPanel->refreshStats();
    }

    bool EntityLightWindow::singleColliderActive() const
    {
        int sel = mNotebook->GetSelection();
        return sel != wxNOT_FOUND && mNotebook->GetPageText(sel) == "Collider";
    }

    int EntityLightWindow::multiColliderActive() const
    {
        int sel = mNotebook->GetSelection();
        if (sel != wxNOT_FOUND && mNotebook->GetPageText(sel) == "MultiCollider")
            return mMultiLightColliderPanel->getComponentActive();
        return wxNOT_FOUND;
    }

    ungod::ShadowEmitterComponent const* EntityLightWindow::getActiveComponent() const
    {
        int sel = mNotebook->GetSelection();
        if (sel == wxNOT_FOUND)
            return nullptr;
        if (mNotebook->GetPageText(sel) == "Collider")
            return static_cast<LightColliderEdit*>(mNotebook->GetPage(sel))->getComponent();
        if (mNotebook->GetPageText(sel) == "MultiCollider")
            return static_cast<MultiComponentPanel<ungod::ShadowEmitterComponent, LightColliderEdit>*>(mNotebook->GetPage(sel))->getComponent();
        return nullptr;
    }
}
