#ifndef UEDIT_ENTITY_SCRIPT_WINDOW_H
#define UEDIT_ENTITY_SCRIPT_WINDOW_H

#include "ActionManager.h"
#include "ScriptManager.h"
#include "Utility.h"
#include "StatDisplayer.h"
#include "wx/window.h"
#include "wx/stattext.h"
#include "wx/panel.h"
#include "wx/filepicker.h"
#include "wx/textctrl.h"
#include "wx/radiobut.h"
#include "wx/dialog.h"
#include "wx/msgdlg.h"

namespace uedit
{
    enum ScriptWindowIDs
    {
        SCRIPT_WINDOW_FILE_PICKER
    };

    namespace detail
    {
        class ParameterDisplayBase : public wxPanel
        {
        public:
            ParameterDisplayBase(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& siz = wxDefaultSize) :
                wxPanel(parent, id, pos, siz) {}

            virtual void addParameter(ungod::script::Environment env, ungod::Entity e) const = 0;
        };
    }


    template<typename T>
    class SimpleParameterDisplay : public detail::ParameterDisplayBase
    {
    public:
        SimpleParameterDisplay(const std::string& name, wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& siz = wxDefaultSize)
            : ParameterDisplayBase(parent, id, pos, siz), mStatDisplay(new StatDisplay<T>(name, this, -1))
        {}

        virtual void addParameter(ungod::script::Environment env, ungod::Entity e) const override
        {
            std::optional<T> t = mStatDisplay->getValue();
            if (t)
                env[mStatDisplay->getText()] = *t;
        }

    private:
        StatDisplay<T>* mStatDisplay;
    };



    class EntityParameterDisplay : public detail::ParameterDisplayBase
    {
    public:
        EntityParameterDisplay(const std::string& name, wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& siz = wxDefaultSize)
            : ParameterDisplayBase(parent, id, pos, siz)
        {}

        virtual void addParameter(ungod::script::Environment env, ungod::Entity e) const override
        {
        }
    };


    class Vec2fParameterDisplay : public detail::ParameterDisplayBase
    {
    public:
        Vec2fParameterDisplay(const std::string& name, wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& siz = wxDefaultSize)
            : ParameterDisplayBase(parent, id, pos, siz)
        {}

        virtual void addParameter(ungod::script::Environment env, ungod::Entity e) const override
        {
        }
    };


    class TableParameterDisplay : public detail::ParameterDisplayBase
    {
    public:
        TableParameterDisplay(const std::string& name, wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& siz = wxDefaultSize)
            : ParameterDisplayBase(parent, id, pos, siz), mStatDisplay(new StatDisplay<std::string>(name, this, -1))
        {}

        virtual void addParameter(ungod::script::Environment env, ungod::Entity e) const override
        {
            auto ss = e.getWorld().getGraph().getState().getEntityBehaviorManager().getBehaviorManager().getSharedState();
            std::optional<std::string> t = mStatDisplay->getValue();
            if (t)
            {
                auto result = ss->script("______TABLE=" + *t, [](lua_State*, sol::protected_function_result pfr) { return pfr; });
                if (result.valid())
                {
                    sol::object o = ss->get<ungod::script::Environment>("______TABLE");
                    ss->set("______TABLE", sol::nil);
                    if (o.is<ungod::script::Environment>())
                    {
                        env[mStatDisplay->getText()] = o.as<ungod::script::Environment>();
                        return;
                    }
                }
            }
            auto err = wxMessageDialog(GetParent(), _("Field does not contain code that defines a environment."));
            err.ShowModal();
        }

    private:
        StatDisplay<std::string>* mStatDisplay;
    };


    class EntityScriptWindow : public wxWindow
    {
    public:
        EntityScriptWindow(ungod::Entity e, ActionManager& actionManager, ScriptManager& sm, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz);

        ungod::Entity getEntity() const { return mEntity; }

        void appendParameterDisplay(detail::ParameterDisplayBase* dpb) { mParameterDisplays.emplace_back(dpb); }

        wxPanel* getScriptLoadPanel() { return mScriptLoadPanel; }

        void rebuild();

    private:
        ungod::Entity mEntity;
        ActionManager& mActionManager;
        ScriptManager& mScriptManager;
        wxFilePickerCtrl* mScriptFilePicker;
        std::vector< detail::ParameterDisplayBase* > mParameterDisplays;
        wxPanel* mScriptLoadPanel;
        wxStaticText* mTexlabel;
        wxButton* mNewParamButton;

    private:
        void onScriptPicked(wxFileDirPickerEvent& event);
        void onNewParamClicked(wxCommandEvent& event);
    };


    class ParameterCreateDialog : public wxDialog
    {
    public:
        ParameterCreateDialog(EntityScriptWindow& esw);

    private:
        void onOk(wxCommandEvent& event);

        wxDECLARE_EVENT_TABLE();

    private:
        static const wxSize DEFAULT_SIZE;

        EntityScriptWindow& mEntityScriptWindow;
        wxTextCtrl* mParameterNameCtrl;
        wxRadioButton* mChoiceFloat;
        wxRadioButton* mChoiceInt;
        wxRadioButton* mChoiceString;
        wxRadioButton* mChoiceBool;
        wxRadioButton* mChoiceEntity;
        wxRadioButton* mChoiceVec2f;
        wxRadioButton* mChoiceTable;
    };
}

#endif // UEDIT_ENTITY_SCRIPT_WINDOW
