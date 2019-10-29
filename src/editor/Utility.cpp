#include "Utility.h"
#include <sstream>
#include "wx/filedlg.h"
#include "wx/wfstream.h"
#include "wx/log.h"

namespace uedit
{
    wxString fileDialogHeader(const std::string& fileExt)
    {
        std::stringstream header;
        header << "Open " << fileExt << " file";
        return _(header.str());
    }

    wxString fileDialogWildcard(const std::string& fileExt)
    {
        std::stringstream files;
        files << fileExt << " files (*." << fileExt << ")|*." << fileExt;
        return _(files.str());
    }

    std::pair<bool, std::string> getPathViaDialog(wxWindow * parent, const std::string& fileExt)
    {
        wxFileDialog openFileDialog(parent, fileDialogHeader(fileExt), "", "",
                                    fileDialogWildcard(fileExt), wxFD_OPEN|wxFD_FILE_MUST_EXIST);

        if (openFileDialog.ShowModal() == wxID_CANCEL)
            return {false, ""};

        wxFileInputStream input(openFileDialog.GetPath());
        if (!input.IsOk())
        {
            wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
            return {false, ""};
        }
        else
            return {true,  openFileDialog.GetPath().ToStdString()};
    }

    sf::Color convertColor(const wxColor& color)
    {
        return sf::Color(color.Red(), color.Green(), color.Blue(), color.Alpha());
    }

    wxColor convertColor(const sf::Color& color)
    {
        return wxColor(color.r, color.g, color.b, color.a);
    }
}
