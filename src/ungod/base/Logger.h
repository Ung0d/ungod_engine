#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

namespace ungod
{
    /**____________________________________________________
    Logging object which is able to write to several output
    streams, possibly multible ones. There are 3 priority
    levels for more or less logging information.
    _____________________________________________________*/
    class Logger
    {
    //_____________________________________________________
    enum class Priority_Level
    {
        PRIORITY_NOLOG       = -1,
        PRIORITY_INFO        = 0,
        PRIORITY_WARNING     = 1,
        PRIORITY_ERROR       = 2,
        PRIORITY_HEADER      = 3
    };
    //_____________________________________________________
    private:
        //fixed output streams
        static bool lEnableConsoleOutput;
        static bool lEnableLogfileOutput;
        //logfile writer
        static std::ofstream lFile;
        //true if current output line is still empty
        static bool lEmptyLine;
        //current priority-level
        static Priority_Level lCurrentLevel;
        //internal log of a message
        template <typename M, typename... MS>
        static void log(const std::string& prefix, const Priority_Level level, M&& message, MS&&... messages);

        template <typename M, typename... MS>
        static void doPrint(std::stringstream& ss, M&& message, MS&&... messages);
    public:
        //called once at program start
        static void login();
        //called once when shutdown the program
        static void logout();
        //logs
        template <typename M, typename... MS>
        static void info(M&& message, MS&&... messages);
        template <typename M, typename... MS>
        static void warning(M&& message, MS&&... messages);
        template <typename M, typename... MS>
        static void error(M&& message, MS&&... messages);

        static bool assertion(bool expr, const std::string& message);
        //toggle output streams
        static void toggleConsoleOutput();
        static void toggleLogfileOutput();
    };

    template <typename M, typename... MS>
    void Logger::log(const std::string& prefix,  const Priority_Level level, M&& message, MS&&... messages)
    {
        if(level >= lCurrentLevel)
        {
            std::stringstream msg;
            doPrint(msg, std::forward<M>(message), std::forward<MS>(messages)...);
            msg << "\n";
            //grant console output
            if(lEnableConsoleOutput)
                std::cout << (prefix + msg.str());
            //grant logfile output
            if(lEnableLogfileOutput)
                lFile << (prefix + msg.str());
        }
    }

    template <typename M, typename... MS>
    void Logger::doPrint(std::stringstream& ss, M&& message, MS&&... messages)
    {
        ss << std::forward<M>(message);
        ((ss << ' ' << std::forward<MS>(messages)), ...);
    }

    template <typename M, typename... MS>
    void Logger::info(M&& message, MS&&... messages)
    {
        log("Info: ", Priority_Level::PRIORITY_INFO, std::forward<M>(message), std::forward<MS>(messages)...);
    }


    template <typename M, typename... MS>
    void Logger::warning(M&& message, MS&&... messages)
    {
        log("Warning: ", Priority_Level::PRIORITY_WARNING, std::forward<M>(message), std::forward<MS>(messages)...);
    }


    template <typename M, typename... MS>
    void Logger::error(M&& message, MS&&... messages)
    {
        log("Error: ", Priority_Level::PRIORITY_ERROR, std::forward<M>(message), std::forward<MS>(messages)...);
    }
}

#endif // LOGGER_H
