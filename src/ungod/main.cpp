#include "ungod/application/Application.h"
#include "ungod/application/ScriptedGameState.h"

int main()
{
    ungod::Application app{};
    app.initApplication();
    return static_cast<std::underlying_type_t<ungod::ErrorCode>>(app.runApplication());
}
