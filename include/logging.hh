// #ifndef OPEN_GL_hpp 
// #define OPEN_GL_hpp

#include <cstdarg>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <vector>

#include <global.hh>

namespace LOG 
{
    // Used for array indexes!  Don't change the numbers!
    enum LogLevel 
    {
        DEBUG = 0,
        INFO = 1,
        INFO_2 = 2,
        INFO_3 = 3,
        INFO_4 = 4,
        WARN = 5,
        WARN_2 = 6,
        WARN_3 = 7,
        ERROR = 8,
        FATAL = 9
    };

    // Used for array indexes!  Don't change the numbers!
    enum SystemLocation 
    {
        DEFAULT = 0,
        CPU,
        GPU,
        RENDERER,
        APP,
        LOG,
        MEM,
        IO,
        DMA,
        PSX,
        OPEN_GL
    };

    struct LOG_MESSAGE
    {
        LogLevel logLevel;
        std::string logLevel_text;
        SystemLocation location;
        std::string location_text;
        std::string message;
    };

    std::string LogLevel_to_string(LogLevel level);
    std::string SystemLocation_to_string(SystemLocation loc);

    std::vector<LOG_MESSAGE>& getLogRecords();

    void log(LogLevel level, SystemLocation location, const char* format);

    template <typename T>
    void log(LogLevel level, SystemLocation location, const char* format, const T& value)
    {
        std::string str(format);
        size_t loc = str.find_first_of("%");
        if (std::string::npos == loc)
        {
            //CRASH?
            return;
        }

        char type = format[loc+1];
        str.erase(loc, 2);
        if ('s' == type) {
            std::stringstream ss;
            ss << value;
            std::string res ( ss.str() );
            str.insert(loc, res);
        } else if ('i' == type) {
            std::stringstream ss;
            ss << value;
            std::string res ( ss.str() );
            str.insert(loc, res);
        } else if ('f' == type) {
            std::stringstream ss;
            ss << std::fixed << value;
            std::string res ( ss.str() );
            str.insert(loc, res);
        } else if ('x' == type) {
            std::stringstream ss;
            ss << std::hex << value;
            std::string res ( ss.str() );
            str.insert(loc, res);
        }

        // printf("%s", str.c_str());
        LOG_MESSAGE message;
        message.location = location;
        message.location_text = SystemLocation_to_string(location);
        message.logLevel = level;
        message.logLevel_text = LogLevel_to_string(level);
        message.message = str;

        getLogRecords().push_back(message);
    }

    template <typename T, typename... Args>
    void log(LogLevel level, SystemLocation location, const char* format, const T& value, const Args&... args)
    {
        std::string str(format);
        size_t loc = str.find_first_of("%");
        if (std::string::npos == loc)
        {
            // log("< ???? Print() Args error ???? >", args...);
        }

        char type = format[loc+1];
        str.erase(loc, 2);
        if ('s' == type) {
            std::stringstream ss;
            ss << value;
            std::string res ( ss.str() );
            str.insert(loc, res);
        } else if ('i' == type) {
            std::stringstream ss;
            ss << value;
            std::string res ( ss.str() );
            str.insert(loc, res);
        } else if ('f' == type) {
            std::stringstream ss;
            ss << std::fixed << value;
            std::string res ( ss.str() );
            str.insert(loc, res);
        } else if ('x' == type) {
            std::stringstream ss;
            ss << std::hex << value;
            std::string res ( ss.str() );
            str.insert(loc, res);
        }

        log(level, location, str.c_str(), args...);
    }

    template <typename T, typename... Args>
    void debug(SystemLocation location, const char* format, const T& value, const Args&... args)
    {
        log(LogLevel::DEBUG, location, format, value, args...);
    }
    template <typename T, typename... Args>
    void info(SystemLocation location, const char* format, const T& value, const Args&... args)
    {
        log(LogLevel::INFO, location, format, value, args...);
    }
    template <typename T, typename... Args>
    void warn(SystemLocation location, const char* format, const T& value, const Args&... args)
    {
        log(LogLevel::WARN, location, format, value, args...);
    }
    template <typename T, typename... Args>
    void error(SystemLocation location, const char* format, const T& value, const Args&... args)
    {
        log(LogLevel::ERROR, location, format, value, args...);
    }
    template <typename T, typename... Args>
    void fatal(SystemLocation location, const char* format, const T& value, const Args&... args)
    {
        log(LogLevel::FATAL, location, format, value, args...);
    }

    void debug(SystemLocation location, const char* format);
    void info(SystemLocation location, const char* format);
    void warn(SystemLocation location, const char* format);
    void error(SystemLocation location, const char* format);
    void fatal(SystemLocation location, const char* format);
}