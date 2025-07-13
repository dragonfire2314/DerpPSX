#include <logging.hh>
#include <algorithm>

namespace LOG 
{
    #define MAX_LOG_LENGTH 4096

    std::string LogLevel_to_string(LogLevel level);
    std::string SystemLocation_to_string(SystemLocation loc);

    std::vector<LOG_MESSAGE> log_messages;

    void log(LogLevel level, SystemLocation location, const char* format)
    {
        std::string str(format);

        LOG_MESSAGE message;
        message.location = location;
        message.location_text = SystemLocation_to_string(location);
        message.logLevel = level;
        message.logLevel_text = LogLevel_to_string(level);
        message.message = str;

        getLogRecords().push_back(message);
    }

    void debug(SystemLocation location, const char* format)
    {
        log(LogLevel::DEBUG, location, format);
    }
    void info(SystemLocation location, const char* format)
    {
        log(LogLevel::INFO, location, format);
    }
    void warn(SystemLocation location, const char* format)
    {
        log(LogLevel::WARN, location, format);
    }
    void error(SystemLocation location, const char* format)
    {
        log(LogLevel::ERROR, location, format);
    }
    void fatal(SystemLocation location, const char* format)
    {
        log(LogLevel::FATAL, location, format);
    }

    std::vector<LOG_MESSAGE>& getLogRecords()
    {
        return log_messages;
    }

    std::string LogLevel_to_string(LogLevel level)
    {
        switch (level)
        {
            case DEBUG:     return "DEBUG ";
            case INFO:      return "INFO";
            case INFO_2:    return "INFO_2";
            case INFO_3:    return "INFO_3";
            case INFO_4:    return "INFO_4";
            case WARN:      return "WARN";
            case WARN_2:    return "WARN_2";
            case WARN_3:    return "WARN_3";
            case ERROR:     return "ERROR";
            case FATAL:     return "FATAL";
            default:
                return "      ";
        }
    }

    std::string SystemLocation_to_string(SystemLocation loc) 
    {
        switch (loc)
        {
            case DEFAULT:       return "DEFAULT";
            case CPU:         return "CPU";
            case GPU:           return "GPU";
            case RENDERER:      return "RENDERER";
            case APP:           return "APP";
            case LOG:           return "LOG";
            case MEM:           return "MEM";
            case IO:           return "IO";
            case DMA:           return "DMA";
            case PSX:           return "PSX";
            case OPEN_GL:           return "OPEN_GL";
            default:
                    return "        ";
        }
    }
}