#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <fstream>
#include <istream>

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
        template <class T>
        static void log(const T& message, const std::string& prefix, const Priority_Level level);
    public:
        //called once at program start
        static void login();
        //called once when shutdown the program
        static void logout();
        //logs
        template <class T>
        static void info(const T& message);
        template <class T>
        static void warning(const T& message);
        template <class T>
        static void error(const T& message);

        static void assertion(bool expr, const std::string& message);
        //toggle output streams
        static void toggleConsoleOutput();
        static void toggleLogfileOutput();
        //mark the end of a line
        static void endl();
    };

    template <class T>
    void Logger::log(const T& message, const std::string& prefix,  const Priority_Level level)
    {
        if(level >= lCurrentLevel)
        {
            //grant console output
            if(lEnableConsoleOutput)
            {
                if(lEmptyLine)
                {
                    std::cout << prefix << message;
                }
                else
                    std::cout << message;
            }
            //grant logfile output
            if(lEnableLogfileOutput)
            {
                if(lEmptyLine)
                {
                    lFile << prefix << message;
                }
                else
                    lFile << message;
            }

            lEmptyLine = false;
        }
    }

    template <class T>
    void Logger::info(const T& message)
    {
        log(message, "Info: ", Priority_Level::PRIORITY_INFO);
    }

    template <class T>
    void Logger::warning(const T& message)
    {
        log(message, "Warning: ", Priority_Level::PRIORITY_WARNING);
    }

    template <class T>
    void Logger::error(const T& message)
    {
        log(message, "Error: ", Priority_Level::PRIORITY_ERROR);
    }
}

#endif // LOGGER_H
