#include "EntityScriptWindow.h"
#include "EditorFrame.h"
#include "wx/msgdlg.h"

namespace uedit
{
    EntityScriptWindow::EntityScriptWindow(ungod::Entity e, ActionManager& actionManager, ScriptManager& sm, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz)
        : wxWindow(parent, id, pos, siz), mEntity(e), mActionManager(actionManager), mScriptManager(sm), 
        mScriptLoadPanel(new wxPanel(this)),
        mTexlabel(new wxStaticText(mScriptLoadPanel, -1, _("Selected script: ")))
    {
        mScriptFilePicker = new wxFilePickerCtrl(mScriptLoadPanel, SCRIPT_WINDOW_FILE_PICKER, fileDialogHeader("lua"), wxFileSelectorPromptStr, fileDialogWildcard("lua"));
        Bind(wxEVT_FILEPICKER_CHANGED, &EntityScriptWindow::onScriptPicked, this, SCRIPT_WINDOW_FILE_PICKER);

        mNewParamButton = new wxButton(mScriptLoadPanel, -1, "add parameter");
        mNewParamButton->Bind(wxEVT_BUTTON, &EntityScriptWindow::onNewParamClicked, this);

        rebuild();
    }

    void EntityScriptWindow::onScriptPicked(wxFileDirPickerEvent& event)
    {
        ungod::script::Environment paramEnv = mActionManager.getEditorFrame()->getCanvas()->getEditorState()->getEntityBehaviorManager().getBehaviorManager().createEnvironment();
        for (auto p : mParameterDisplays)
            p->addParameter(paramEnv);
        auto file = mScriptFilePicker->GetFileName();
        file.MakeRelativeTo();
        mScriptManager.loadScript(std::string{ file.GetFullPath().mb_str() });
        mActionManager.behaviorActions().assignBehavior(mEntity, std::string{ file.GetName().mb_str() }, paramEnv);
    }

    void EntityScriptWindow::onNewParamClicked(wxCommandEvent& event)
    {
        ParameterCreateDialog pcd(*this);
        pcd.ShowModal();
    }

    void EntityScriptWindow::rebuild()
    {
        SetSizer(NULL);
        mScriptLoadPanel->SetSizer(NULL);

        wxSizer* boxsizer = new wxBoxSizer(wxVERTICAL);

        {
            wxBoxSizer* scriptLoadSizer = new wxBoxSizer(wxVERTICAL);

            wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
            hbox1->Add(mTexlabel, 1, wxALL, 8);
            scriptLoadSizer->Add(hbox1, 0, wxALIGN_CENTER_HORIZONTAL, 12);

            scriptLoadSizer->Add(mScriptFilePicker, 0, wxALIGN_CENTER_HORIZONTAL, 12);

            scriptLoadSizer->Add(mNewParamButton, 0, wxALIGN_CENTER_HORIZONTAL, 12);

            for (auto p : mParameterDisplays)
                scriptLoadSizer->Add(p, 0, wxALIGN_CENTER | wxALL, 8);

            mScriptLoadPanel->SetSizer(scriptLoadSizer);
            mScriptLoadPanel->Fit();

            boxsizer->Add(mScriptLoadPanel, 0, wxALIGN_CENTER | wxALL, 8);
        }

        //if the entity already has a script loaded, display it here
       // if (e.get<ungod::EntityBehaviorComponent>().get)

        SetSizer(boxsizer);
        Fit();
    }


    //connect events
    wxBEGIN_EVENT_TABLE(ParameterCreateDialog, wxDialog)
        EVT_BUTTON(wxID_OK, ParameterCreateDialog::onOk)
    wxEND_EVENT_TABLE()

    const wxSize ParameterCreateDialog::DEFAULT_SIZE = { 300, 400 };

    ParameterCreateDialog::ParameterCreateDialog(EntityScriptWindow& esw)
        : wxDialog(&esw, -1, _("add a parameter"), wxDefaultPosition, DEFAULT_SIZE, wxDEFAULT_DIALOG_STYLE), mEntityScriptWindow(esw)
    {
        wxBoxSizer* masterSizer = new wxBoxSizer(wxVERTICAL);

        wxBoxSizer* vboxTxt = new wxBoxSizer(wxVERTICAL);
        mParameterNameCtrl = new wxTextCtrl(this, 0, _(""), wxDefaultPosition, wxSize(64, 16), wxTE_PROCESS_ENTER);
        vboxTxt->Add(mParameterNameCtrl, 1, wxALIGN_LEFT, 10);
        masterSizer->Add(vboxTxt, 0, wxALIGN_CENTER_HORIZONTAL);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
        mChoiceFloat = new wxRadioButton(this, -1, "float", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
        mChoiceInt = new wxRadioButton(this, -1, "int", wxDefaultPosition, wxDefaultSize);
        mChoiceString = new wxRadioButton(this, -1, "string", wxDefaultPosition, wxDefaultSize);
        mChoiceBool = new wxRadioButton(this, -1, "bool", wxDefaultPosition, wxDefaultSize);
        mChoiceEntity = new wxRadioButton(this, -1, "entity", wxDefaultPosition, wxDefaultSize);
        mChoiceVec2f = new wxRadioButton(this, -1, "vec2f", wxDefaultPosition, wxDefaultSize);
        vbox->Add(mChoiceFloat, 1, wxALIGN_LEFT);
        vbox->Add(mChoiceInt, 1, wxALIGN_LEFT);
        vbox->Add(mChoiceString, 1, wxALIGN_LEFT);
        vbox->Add(mChoiceBool, 1, wxALIGN_LEFT);
        vbox->Add(mChoiceEntity, 1, wxALIGN_LEFT);
        vbox->Add(mChoiceVec2f, 1, wxALIGN_LEFT);

        masterSizer->Add(vbox, 1, wxALIGN_CENTER);

        wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
        hbox->Add(new wxButton(this, wxID_OK, _("Accept"), wxDefaultPosition, wxDefaultSize), 1, wxCENTER);
        hbox->Add(new wxButton(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize), 1, wxCENTER);

        masterSizer->Add(hbox, 1, wxALIGN_CENTER);

        SetSizer(masterSizer);
        Fit();
    }

    void ParameterCreateDialog::onOk(wxCommandEvent& event)
    {
        std::string name{ mParameterNameCtrl->GetValue().mb_str() };
        if (name == "")
        {
            auto err = wxMessageDialog(this, _("Choose a parameter name."));
            err.ShowModal();
            return;
        }

        if (mChoiceFloat->GetValue())
        {
            mEntityScriptWindow.getEntity().getWorld().getBehaviorHandler().serializeParameter<float>(mEntityScriptWindow.getEntity(), name);
            mEntityScriptWindow.appendParameterDisplay(new SimpleParameterDisplay<float>(name, mEntityScriptWindow.getScriptLoadPanel(), -1));
        }
        else if (mChoiceInt->GetValue())
        {
            mEntityScriptWindow.getEntity().getWorld().getBehaviorHandler().serializeParameter<int>(mEntityScriptWindow.getEntity(), name);
            mEntityScriptWindow.appendParameterDisplay(new SimpleParameterDisplay<int>(name, mEntityScriptWindow.getScriptLoadPanel(), -1));
        }
        else if (mChoiceString->GetValue())
        {
            mEntityScriptWindow.getEntity().getWorld().getBehaviorHandler().serializeParameter<std::string>(mEntityScriptWindow.getEntity(), name);
            mEntityScriptWindow.appendParameterDisplay(new SimpleParameterDisplay<std::string>(name, mEntityScriptWindow.getScriptLoadPanel(), -1));
        }
        else if (mChoiceBool->GetValue())
        {
            mEntityScriptWindow.getEntity().getWorld().getBehaviorHandler().serializeParameter<bool>(mEntityScriptWindow.getEntity(), name);
            mEntityScriptWindow.appendParameterDisplay(new SimpleParameterDisplay<bool>(name, mEntityScriptWindow.getScriptLoadPanel(), -1));
        }
        else if (mChoiceEntity->GetValue())
        { }
        else if (mChoiceVec2f->GetValue())
        { }

        mEntityScriptWindow.rebuild();

        EndModal(wxID_OK);
    }
}
