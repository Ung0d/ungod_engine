/*
* This file is part of the ungod - framework.
* Copyright (C) 2016 Felix Becker - fb132550@uni-greifswald.de
*
* This software is provided 'as-is', without any express or
* implied warranty. In no event will the authors be held
* liable for any damages arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute
* it freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented;
*    you must not claim that you wrote the original software.
*    If you use this software in a product, an acknowledgment
*    in the product documentation would be appreciated but
*    is not required.
*
* 2. Altered source versions must be plainly marked as such,
*    and must not be misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any
*    source distribution.
*/
#include "ungod/base/Logger.h"

namespace ungod
{
    bool Logger::lEnableConsoleOutput = true;
    bool Logger::lEnableLogfileOutput = true;
    std::ofstream Logger::lFile;
    bool Logger::lEmptyLine = true;
    Logger::Priority_Level Logger::lCurrentLevel = Priority_Level::PRIORITY_INFO;

    void Logger::login()
    {
        lFile.open("log.txt");

        log("____________________________________________________", "", Priority_Level::PRIORITY_HEADER);
        endl();
        log("Ungod-Engine log.", "", Priority_Level::PRIORITY_HEADER);
        endl();
        log("____________________________________________________", "", Priority_Level::PRIORITY_HEADER);
        endl();
    }

    void Logger::logout()
    {
        log("____________________________________________________", "", Priority_Level::PRIORITY_HEADER);
        endl();
        log("Log closed.", "", Priority_Level::PRIORITY_HEADER);
        endl();
        log("____________________________________________________", "", Priority_Level::PRIORITY_HEADER);
        endl();

        lFile.close();
    }

    bool Logger::assertion(bool expr, const std::string& message)
    {
        if (!expr)
            log(message, "Assertion failed", Priority_Level::PRIORITY_ERROR);
        return expr;
    }

    void Logger::toggleConsoleOutput()
    {
        lEnableConsoleOutput = !lEnableConsoleOutput;
    }

    void Logger::toggleLogfileOutput()
    {
        lEnableLogfileOutput = !lEnableLogfileOutput;
    }

    void Logger::endl()
    {
        std::cout << std::endl;
        lFile << std::endl;
        lEmptyLine = true;
    }
}
