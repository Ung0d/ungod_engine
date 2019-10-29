#ifndef UEDIT_LABELED_CHOICE
#define UEDIT_LABELED_CHOICE

#include <functional>
#include "wx/window.h"
#include "wx/choice.h"
#include "wx/stattext.h"

namespace uedit
{
    /** \brief Convieniece class that combines a static info text with a choice. */
    class LabeledChoice : public wxWindow
    {
    public:
        LabeledChoice(wxWindow *parent, wxWindowID id, const wxArrayString &choices, const wxString& text, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize);

        wxChoice* getChoice() { return mChoice; }
        wxChoice* getChoice() const { return mChoice; }

        void bindChoice(const std::function<void(wxCommandEvent&)>& callback);

    private:
        wxChoice* mChoice;
    };
}

#endif // UEDIT_LABELED_CHOICE
