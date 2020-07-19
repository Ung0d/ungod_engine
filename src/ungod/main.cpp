#include "ungod/application/Application.h"
#include "ungod/application/ScriptedGameState.h"

int main()
{
    ungod::Logger::login();
    ungod::Application app{};
    app.initApplication();
    auto err = static_cast<std::underlying_type_t<ungod::ErrorCode>>(app.runApplication());
    ungod::Logger::logout();
    return err;
}
