#ifndef ENTITY_DIALOG_H
#define ENTITY_DIALOG_H

#include "ungod/base/World.h"
#include "ungod/content/EntityTypes.h"
#include "ActionManager.h"
#include "wx/dialog.h"
#include "wx/radiobut.h"

namespace uedit
{
    class EntityVisualsWindow;

    class EntityCreateDialog : public wxDialog
    {
    public:
        EntityCreateDialog ( ungod::World* world,
                            wxWindow * parent, wxWindowID id, const wxString & title,
                            const wxPoint & pos = wxDefaultPosition );

        ungod::Entity getEntity();

    private:
        void onOk( wxCommandEvent & event );

        wxDECLARE_EVENT_TABLE();

    private:
        static const wxSize DEFAULT_SIZE;

        ungod::World* mWorld;
        wxRadioButton* mChoiceEntity;
        wxRadioButton* mChoiceActor;
        wxRadioButton* mChoiceMetaObject;
        wxRadioButton* mChoiceLight;
        wxRadioButton* mChoiceTrigger;
        wxRadioButton* mChoiceWorldObject;
        wxRadioButton* mChoiceParticleSystem;
        wxRadioButton* mChoiceAudioEmitter;
    };
}

#endif // ENTITY_DIALOG_H

