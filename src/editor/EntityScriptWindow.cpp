#include "EntityScriptWindow.h"

namespace uedit
{
    EntityScriptWindow::EntityScriptWindow(ungod::Entity e, WorldActionWrapper& waw, ScriptManager& sm, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz)
        : wxWindow(parent, id, pos, siz), mEntity(e), mWorldAction(waw), mScriptManager(sm)
    {
        wxSizer* boxsizer = new wxBoxSizer(wxVERTICAL);

        {
            wxPanel* scriptLoadPanel = new wxPanel(this);
            wxBoxSizer* scriptLoadSizer = new wxBoxSizer(wxVERTICAL);

            //build an environment to load a texture for the entity and a button, that uses the currently in the sheet-manager selected texture as the entity texture
            wxStaticText* texlabel = new wxStaticText(scriptLoadPanel, -1, _("Selected script: "));
            wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
            hbox1->Add(texlabel, 1, wxALL, 8);
            scriptLoadSizer->Add(hbox1,0,wxALIGN_CENTER_HORIZONTAL);

            mScriptsComboBox = new wxComboBox( scriptLoadPanel, SCRIPT_WINDOW_COMBOBOX );
            Bind(wxEVT_COMBOBOX, EntityScriptWindow::onScriptSelected, this, SCRIPT_WINDOW_COMBOBOX);
            wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
            hbox2->Add(mScriptsComboBox,1);
            scriptLoadSizer->Add(hbox2,0,wxALIGN_CENTER_HORIZONTAL);

            scriptLoadPanel->SetSizer(scriptLoadSizer);
            scriptLoadPanel->Fit();

            boxsizer->Add(scriptLoadPanel,0,wxALIGN_CENTER | wxALL, 8);
        }

        //if the entity already has a script loaded, display it here
        //if (e.get<ungod::EntityBehaviorComponent>().get)

        SetSizer(boxsizer);
        Fit();

        //catch script loaded events from the script manager
        sm.Bind(EVT_SCRIPT_LOADED, &EntityScriptWindow::onScriptLoad, this);

        //put already loaded scripts into the combo-box
        for (const auto& e : mScriptManager.getEditors())
        {
            mScriptsComboBox->Append(e->getPath().filename().stem().string());
        }

        if (mEntity.get<ungod::EntityBehaviorComponent>().valid())
        {
            int i = mScriptsComboBox->FindString(mEntity.get<ungod::EntityBehaviorComponent>().getScriptName());
            if (i != wxNOT_FOUND)
            {
                mScriptsComboBox->SetSelection( i );
            }
            else
            {
                mScriptsComboBox->SetSelection(  mScriptsComboBox->Append( mEntity.get<ungod::EntityBehaviorComponent>().getScriptName() )  );
            }
        }
    }

    void EntityScriptWindow::onScriptLoad(wxCommandEvent& event)
    {
        boost::filesystem::path path(event.GetString());
        mScriptsComboBox->Append(path.filename().stem().string());
    }

    void EntityScriptWindow::onScriptSelected(wxCommandEvent& event)
    {
        mWorldAction.attachScript(mEntity, std::string(mScriptsComboBox->GetValue().mb_str()));
    }
}
