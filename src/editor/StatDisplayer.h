#ifndef UEDIT_STAT_DISPLAYER_H
#define UEDIT_STAT_DISPLAYER_H

#include "wx/panel.h"
#include "wx/textctrl.h"
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/msgdlg.h"
#include <type_traits>
#include <string>

namespace uedit
{
    namespace detail
    {
        template <typename T>
        class StatDisplayImpl : public wxPanel
        {
        public:
            StatDisplayImpl(const std::string& text, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz);

            //sets the text, does not call the setter
            void setValue(T t);

            //refreshes the text by calling the getter
            void refreshValue();

            //connects a setter function to the display that is invoked, when the value is changed
            void connectSetter(const std::function<void(T)>& setter);

            //connects a getter function to the display that is invoked, when an invalid value (not a number) is entered to reset the device
            void connectGetter(const std::function<T()>& getter);

        protected:
            wxTextCtrl* mTextCtrl;
            std::function<void(T)> mSetter;
            std::function<T()> mGetter;
        };
    }

    /** \brief A class that combines a static text with an input field, where a numeric
    * value can the entered and displayed. */
    template <typename T, typename ENABLE = void>
    class StatDisplay : public detail::StatDisplayImpl<T> {};

    /** \brief Partial spec for integral numbers. */
    template <typename T>
    class StatDisplay<T> : public detail::StatDisplayImpl<T>
    {
    public:
        StatDisplay(const std::string& text, wxWindow * parent, wxWindowID id, const wxPoint & pos = wxDefaultPosition, const wxSize& siz = wxDefaultSize);

        void onTextChange(wxCommandEvent& event);

    private:
        wxTextCtrl* mTextCtrl;
    };




    namespace detail
    {
        template<typename T>
        StatDisplayImpl<T>::StatDisplayImpl(const std::string& text, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz)
        : wxPanel(parent, id, pos, siz)
        {
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
            hbox->Add(new wxStaticText(this, -1, _(text)), 1, wxALIGN_LEFT | wxALL, 3);
            mTextCtrl = new wxTextCtrl(this, 0, _(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
            hbox->Add(mTextCtrl, 1, wxALL, 3);
            SetSizer(hbox);
            Fit();
        }

        template<typename T>
        void StatDisplayImpl<T>::setValue(T t)
        {
            mTextCtrl->SetLabel(std::to_string(t));
        }

        template<typename T>
        void StatDisplayImpl<T>::refreshValue()
        {
            mTextCtrl->SetLabel(std::to_string(mGetter()));
        }

        template<typename T>
        void StatDisplayImpl<T>::connectSetter(const std::function<void(T)>& setter)
        {
            mSetter = setter;
        }

        template<typename T>
        void StatDisplayImpl<T>::connectGetter(const std::function<T()>& getter)
        {
            mGetter = getter;
        }
    }

    template<typename T>
    StatDisplay<T>::StatDisplay(const std::string& text, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz)
    : detail::StatDisplayImpl<T>(text, parent, id, pos, siz)
    {
        wxPanel::Bind(wxEVT_TEXT_ENTER, &StatDisplay<T>::onTextChange, this, 0);
    }


    template<typename T>
    void StatDisplay<T>::onTextChange(wxCommandEvent& event)
    {
        if (!detail::StatDisplayImpl<T>::mSetter || !detail::StatDisplayImpl<T>::mGetter)
            return;

        try
        {
			T t;
			if constexpr (std::is_integral_v<T>)
				t = static_cast<T>(std::stoi( std::string(detail::StatDisplayImpl<T>::mTextCtrl->GetValue().mb_str()) ));
			else
				t = static_cast<T>(std::stof(std::string(detail::StatDisplayImpl<T>::mTextCtrl->GetValue().mb_str())));
            detail::StatDisplayImpl<T>::mSetter(t);
        }
        catch(const std::exception& e)
        {
            auto err = wxMessageDialog(this, _("Text field must contain a valid number."));
            err.ShowModal();
            detail::StatDisplayImpl<T>::setValue(detail::StatDisplayImpl<T>::mGetter());
        }
    }
}

#endif // UEDIT_STAT_DISPLAYER_H
