#ifndef UEDIT_MULTI_COMP_PANEL_H
#define UEDIT_MULTI_COMP_PANEL_H

#include "ungod/base/Entity.h"
#include "wx/textctrl.h"
#include "wx/choice.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/button.h"

namespace uedit
{
    //helper class that defines a editing panel for a multi-component given
    //a method to create content for a single component
    template<typename T, typename EDIT_PANEL, typename ... OPT>
    class MultiComponentPanel : public wxPanel
    {
    public:
        MultiComponentPanel(wxWindow* parent, ungod::Entity e, WorldActionWrapper& waw);

        void showComponent(unsigned i);

        void refreshStats();

        void onSetCount(wxCommandEvent& event);

        int getComponentActive() const;

        T const* getComponent();

    private:
        ungod::Entity mEntity;
        WorldActionWrapper& mWorldAction;
        unsigned mCurMultiSelect;
        wxTextCtrl* mCompCountCtrl;
        wxChoice* mChoice;
        EDIT_PANEL* mPanel;
        wxBoxSizer* mSizer;

    private:
        void onSetupChoice(unsigned cc);
    };




    template<typename T, typename EDIT_PANEL, typename ... OPT>
    MultiComponentPanel<T, EDIT_PANEL, OPT...>::MultiComponentPanel(wxWindow* parent, ungod::Entity e, WorldActionWrapper& waw) : wxPanel(parent), mEntity(e), mWorldAction(waw), mPanel(nullptr)
    {
        const ungod::MultiComponent<T>& multi = mEntity.get<ungod::MultiComponent<T>>();
        mSizer = new wxBoxSizer(wxVERTICAL);

        {
            mCompCountCtrl = new wxTextCtrl(this, -1, _(std::to_string(multi.getComponentCount())));
            wxBoxSizer* hbox0 = new wxBoxSizer(wxHORIZONTAL);
            hbox0->Add(mCompCountCtrl, 1, wxCENTER);

            wxButton* countButton = new wxButton(this, -1, "Set light count");
            countButton->Bind(wxEVT_BUTTON, &MultiComponentPanel<T, EDIT_PANEL, OPT...>::onSetCount, this);
            hbox0->Add(countButton, 1, wxCENTER);
            mSizer->Add(hbox0,0,wxALIGN_CENTER_HORIZONTAL);

            std::size_t compCount = mEntity.get<ungod::MultiComponent<T>>().getComponentCount();

            wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
            mChoice = new wxChoice(this,-1);
            hbox1->Add(mChoice, 1, wxALL, 5);
            onSetupChoice(compCount);

            mSizer->Add(hbox1,0,wxALIGN_CENTER_HORIZONTAL);
        }

        SetSizer(mSizer);
        Fit();
    }


    template<typename T, typename EDIT_PANEL, typename ... OPT>
    void MultiComponentPanel<T, EDIT_PANEL, OPT...>::onSetupChoice(unsigned cc)
    {
        wxString keys[cc];
        for (std::size_t i = 0; i < cc; ++i)
            keys[i] = std::to_string(i);
        mChoice->Set(cc, keys);

        if (cc > 0)
        {
            mChoice->SetSelection(0);
            showComponent(0);
        }
    }


    template<typename T, typename EDIT_PANEL, typename ... OPT>
    void MultiComponentPanel<T, EDIT_PANEL, OPT...>::onSetCount(wxCommandEvent& event)
    {
        try
        {
            std::size_t cc = (std::size_t)(std::stoi( std::string(mCompCountCtrl->GetValue().mb_str()) ));
            mEntity.initMulti<ungod::MultiComponent<T>>(cc);
            onSetupChoice(cc);
        }
        catch(const std::exception& e)
        {
            auto err = wxMessageDialog(this, _("Text field must contain a valid number."));
            err.ShowModal();
        }
    }


    template<typename T, typename EDIT_PANEL, typename ... OPT>
    int MultiComponentPanel<T, EDIT_PANEL, OPT...>::getComponentActive() const
    {
        return mChoice->GetSelection();
    }


    template<typename T, typename EDIT_PANEL, typename ... OPT>
    T const* MultiComponentPanel<T, EDIT_PANEL, OPT...>::getComponent()
    {
        if (mPanel)
            return mPanel->getComponent();
        return nullptr;
    }


    template<typename T, typename EDIT_PANEL, typename ... OPT>
    void MultiComponentPanel<T, EDIT_PANEL, OPT...>::showComponent(unsigned i)
    {
        T& component = mEntity.modify<ungod::MultiComponent<T>>().getComponent(i);
        mCurMultiSelect = i;
        if (mPanel)
        {
            RemoveChild(mPanel);
            mPanel->Destroy();
        }
        mPanel = new EDIT_PANEL(mEntity, mWorldAction, this, component, mEntity.modify<ungod::MultiComponent<OPT>>().getComponent(i)...);
        mSizer->Add(mPanel,0,wxALIGN_CENTER_HORIZONTAL);
        Fit();
    }


    template<typename T, typename EDIT_PANEL, typename ... OPT>
    void MultiComponentPanel<T, EDIT_PANEL, OPT...>::refreshStats()
    {
        if (mPanel)
            mPanel->refreshStats();
    }
}

#endif // UEDIT_MULTI_COMP_PANEL_H
