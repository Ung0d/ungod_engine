#include <wx/wxprec.h>
#include "EditorApplication.h"
#include "ungod/base/Logger.h"

namespace uedit
{
    IMPLEMENT_APP(EditorApplication);

    bool EditorApplication::OnInit()
    {
        ungod::Logger::login();
        EditorFrame *frame = new EditorFrame();
        frame->Show( true );
        return true;
    }

    int EditorApplication::OnExit()
    {
        ungod::Logger::logout();
        return 0;
    }
}
