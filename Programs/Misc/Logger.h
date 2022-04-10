#ifndef LOGGER_H
#define LOGGER_H

#include <QString>

typedef enum
{
    LOG_NONE = 0,
    LOG_ERROR,
    LOG_INFO,
    LOG_DEBUG,
    LOG_VERBOSE,
}log_level_t;

class Logger
{
public:
    Logger();

    void GetLogMessage(log_level_t level, const QString moduleName, QString& message);
};

#endif // LOGGER_H
