#include "LabeledChoice.h"
#include "wx/sizer.h"

namespace uedit
{
    LabeledChoice::LabeledChoice(wxWindow *parent, wxWindowID id, const wxArrayString &choices, const wxString& text, const wxPoint &pos, const wxSize &size)
        : wxWindow(parent, id, pos, size)
    {
        wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

        wxStaticText* statt = new wxStaticText(this, -1, text);

        hbox->Add(statt, 0, wxALIGN_CENTER_HORIZONTAL);

        mChoice = new wxChoice(this, -1, wxDefaultPosition, wxDefaultSize, choices);

        hbox->Add(mChoice, 0, wxALIGN_CENTER_HORIZONTAL);

        SetSizer(hbox);
        Layout();
        Fit();
    }

    void LabeledChoice::bindChoice(const std::function<void(wxCommandEvent&)>& callback)
    {
        mChoice->Bind(wxEVT_CHOICE, callback);
    }
}

