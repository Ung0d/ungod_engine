#ifndef EDITOR_UTILITY_H
#define EDITOR_UTILITY_H

#include <tuple>
#include <string>
#include "wx/window.h"
#include <SFML/Graphics/Color.hpp>

namespace uedit
{
    wxString fileDialogHeader(const std::string& fileExt);

    wxString fileDialogWildcard(const std::string& fileExt);

    //helper method that gets a path via dialog or and returns (true, path) or
    //return (false, "") if something goes wrong
    std::pair<bool, std::string> getPathViaDialog(wxWindow * parent, const std::string& fileExt);

    sf::Color convertColor(const wxColor& color);
    wxColor convertColor(const sf::Color& color);
}

#endif // UTILITY_H
