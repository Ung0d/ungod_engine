#include <wx/wxprec.h>
#include "EditorApplication.h"

namespace uedit
{
    IMPLEMENT_APP(EditorApplication);

    bool EditorApplication::OnInit()
    {
        EditorFrame *frame = new EditorFrame();
        frame->Show( true );
        return true;
    }

    int EditorApplication::OnExit()
    {
        return 0;
    }
}
