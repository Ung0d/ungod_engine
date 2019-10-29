#ifndef BASE_APP_H
#define BASE_APP_H

#include "EditorFrame.h"
#include <wx/app.h>

namespace uedit
{
    /**
    * \brief The main application for the editor.
    */
    class EditorApplication : public wxApp
    {
        public:
            virtual bool OnInit() override;
            virtual int OnExit() override;
    };
}

#endif // BASE_APP_H

