#include "EntityDialog.h"
#include "wx/button.h"
#include "wx/log.h"
#include "wx/sizer.h"
#include "Utility.h"
#include "EntityVisualsWindow.h"

namespace uedit
{
    //connect events
    wxBEGIN_EVENT_TABLE(EntityCreateDialog, wxDialog)
        EVT_BUTTON(wxID_OK, EntityCreateDialog::onOk)
    wxEND_EVENT_TABLE()

    const wxSize EntityCreateDialog::DEFAULT_SIZE = {200, 200};

    EntityCreateDialog::EntityCreateDialog (ungod::World* world,
                                            wxWindow * parent, wxWindowID id, const wxString & title,
                           const wxPoint & position )
    : wxDialog( parent, id, title, position, DEFAULT_SIZE, wxDEFAULT_DIALOG_STYLE), mWorld(world)
    {
        wxBoxSizer* masterSizer = new wxBoxSizer(wxVERTICAL);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
        mChoiceEntity = new wxRadioButton(this, -1, "Entity", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
        mChoiceActor = new wxRadioButton(this, -1, "Actor", wxDefaultPosition, wxDefaultSize);
        mChoiceMetaObject = new wxRadioButton(this, -1, "MetaObject", wxDefaultPosition, wxDefaultSize);
        mChoiceLight = new wxRadioButton(this, -1, "Light", wxDefaultPosition, wxDefaultSize);
        mChoiceTrigger = new wxRadioButton(this, -1, "Trigger", wxDefaultPosition, wxDefaultSize);
        mChoiceWorldObject = new wxRadioButton(this, -1, "WorldObject", wxDefaultPosition, wxDefaultSize);
        mChoiceParticleSystem = new wxRadioButton(this, -1, "ParticleSystem", wxDefaultPosition, wxDefaultSize);
        mChoiceTilemap = new wxRadioButton(this, -1, "TileMap", wxDefaultPosition, wxDefaultSize);
        mChoiceWater = new wxRadioButton(this, -1, "Water", wxDefaultPosition, wxDefaultSize);
        mChoiceAudioEmitter = new wxRadioButton(this, -1, "AudioEmitter", wxDefaultPosition, wxDefaultSize);
        vbox->Add(mChoiceEntity, 1, wxALIGN_LEFT);
        vbox->Add(mChoiceActor, 1, wxALIGN_LEFT);
        vbox->Add(mChoiceMetaObject, 1, wxALIGN_LEFT);
        vbox->Add(mChoiceLight, 1, wxALIGN_LEFT);
        vbox->Add(mChoiceTrigger, 1, wxALIGN_LEFT);
        vbox->Add(mChoiceWorldObject, 1, wxALIGN_LEFT);
        vbox->Add(mChoiceParticleSystem, 1, wxALIGN_LEFT);
        vbox->Add(mChoiceTilemap, 1, wxALIGN_LEFT);
        vbox->Add(mChoiceWater, 1, wxALIGN_LEFT);
        vbox->Add(mChoiceAudioEmitter, 1, wxALIGN_LEFT);

        masterSizer->Add(vbox, 1, wxEXPAND);

        wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
        hbox->Add(new wxButton( this, wxID_OK, _("Accept"), wxDefaultPosition, wxDefaultSize ), 1, wxCENTER);
        hbox->Add(new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize ), 1, wxCENTER);

        masterSizer->Add(hbox, 1, wxEXPAND);

        SetSizer(masterSizer);
        Fit();
        Center();
    }

    ungod::Entity EntityCreateDialog::getEntity()
    {
        if (mChoiceEntity->GetValue())
        {
            return mWorld->create(ungod::EntityBaseComponents(), ungod::EntityOptionalComponents());
        }

        if (mChoiceActor->GetValue())
        {
            return mWorld->create(ungod::ActorBaseComponents(), ungod::ActorOptionalComponents());
        }

        if (mChoiceLight->GetValue())
        {
            return mWorld->create(ungod::LightBaseComponents(), ungod::LightOptionalComponents());
        }

        if (mChoiceMetaObject->GetValue())
        {
            return mWorld->create(ungod::MetaObjectBaseComponents(), ungod::MetaObjectOptionalComponents());
        }

        if (mChoiceTrigger->GetValue())
        {
            return mWorld->create(ungod::TriggerBaseComponents(), ungod::TriggerOptionalComponents());
        }

        if (mChoiceParticleSystem->GetValue())
        {
            return mWorld->create(ungod::ParticleSystemBaseComponents(), ungod::ParticleSystemOptionalComponents());
        }

        if (mChoiceTilemap->GetValue())
        {
            return mWorld->create(ungod::TileMapBaseComponents(), ungod::TileMapOptionalComponents());
        }

        if (mChoiceWater->GetValue())
        {
            return mWorld->create(ungod::WaterBaseComponents(), ungod::WaterOptionalComponents());
        }
        if (mChoiceAudioEmitter->GetValue())
        {
            return mWorld->create(ungod::AudioEmitterBaseComponents(), ungod::AudioEmitterOptionalComponents());
        }

        //if (mChoiceWorldObject->GetValue())
        //{
            return mWorld->create(ungod::WorldObjectBaseComponents(), ungod::WorldObjectOptionalComponents());
        //}
    }

    void EntityCreateDialog::onOk( wxCommandEvent & event )
    {
        EndModal(wxID_OK);
    }
}

