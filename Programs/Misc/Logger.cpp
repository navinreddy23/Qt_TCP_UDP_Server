#include "Logger.h"

Logger::Logger()
{

}

void Logger::GetLogMessage(log_level_t level, const QString moduleName, QString &message)
{
    message.prepend(" ");

    switch (level)
    {
        case LOG_NONE:
            message.prepend(moduleName);
        break;
        case LOG_ERROR:
            message.prepend(moduleName);
            message.prepend("[ERROR]   : ");
        break;
        case LOG_INFO:
            message.prepend(moduleName);
            message.prepend("[INFO]    : ");
        break;
        case LOG_DEBUG:
            message.prepend(moduleName);
            message.prepend("[DEBUG]   : ");
        break;
        case LOG_VERBOSE:
            message.prepend(moduleName);
            message.prepend("[VERBOSE] : ");
        break;
    }
}
