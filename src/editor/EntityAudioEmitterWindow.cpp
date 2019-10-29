#include "EntityAudioEmitterWindow.h"

namespace uedit
{
    EntityAudioEmitterWindow::EntityAudioEmitterWindow(ungod::Entity e, WorldActionWrapper& waw, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz) :
        wxWindow(parent, id, pos, siz), mEntity(e), mWorldAction(waw)
    {
        wxSizer* boxsizer = new wxBoxSizer(wxVERTICAL);

        {
            mFilepicker = new wxFilePickerCtrl( this, -1, fileDialogHeader("wav"), wxFileSelectorPromptStr, fileDialogWildcard("wav")  );
            mFilepicker->Bind(wxEVT_FILEPICKER_CHANGED, &EntityAudioEmitterWindow::onFilepick, this);
            boxsizer->Add(mFilepicker,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mVolume = new StatDisplay<float>("volume:", this, -1);
            mVolume->connectSetter( [this](float vol)
            {
                mWorldAction.setAudioEmitterVolume(mEntity, vol);
            } );
            mVolume->connectGetter( [this]()
            {
                return mEntity.get<ungod::MusicEmitterComponent>().getVolume();
            } );
            boxsizer->Add(mVolume,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mDistanceCap = new StatDisplay<float>("distance cap:", this, -1);
            mDistanceCap->connectSetter( [this](float cap)
            {
                mWorldAction.setAudioEmitterDistanceCap(mEntity, cap);
            } );
            mDistanceCap->connectGetter( [this]()
            {
                return mEntity.get<ungod::MusicEmitterComponent>().getDistanceCap();
            } );
            boxsizer->Add(mDistanceCap,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mActive = new wxCheckBox(this, -1, _("active"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
            mActive->SetValue(mEntity.get<ungod::MusicEmitterComponent>().isActive());
            mActive->Bind(wxEVT_CHECKBOX, &EntityAudioEmitterWindow::onActiveChecked, this);
            boxsizer->Add(mActive,0,wxALIGN_CENTER_HORIZONTAL);
        }

        refreshStats();

        SetSizer(boxsizer);
        Fit();
    }


    void EntityAudioEmitterWindow::onActiveChecked(wxCommandEvent& event)
    {
        mWorldAction.setAudioEmitterActive(mEntity, mActive->GetValue());
    }


    void EntityAudioEmitterWindow::refreshStats()
    {
        mVolume->setValue(mEntity.get<ungod::MusicEmitterComponent>().getVolume());
        mDistanceCap->setValue(mEntity.get<ungod::MusicEmitterComponent>().getDistanceCap());
    }


    void EntityAudioEmitterWindow::onFilepick(wxFileDirPickerEvent& event)
    {
        mWorldAction.loadAudioEmitterTrack(mEntity, std::string(event.GetPath().mb_str()));
    }
}
