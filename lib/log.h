#ifndef LANMAI_LOG_H
#define LANMAI_LOG_H

#include <string>

inline int8_t GLOBAL_LOG_LEVEL = 0;
enum LogLevel : int8_t { LL_ERROR = 0, LL_INFO = 1, LL_DEBUG = 2 };

inline std::string ll_name(LogLevel log_level) {
    switch (log_level) {
    case LL_ERROR:
        return "ERROR";
    case LL_INFO:
        return "INFO";
    case LL_DEBUG:
        return "DEBUG";
    default:
        return "NONE";
    }
}

#define LLOG(log_level, fmt, args...)                                                                 \
    do {                                                                                              \
        if (log_level <= GLOBAL_LOG_LEVEL) {                                                          \
            printf("[%s:%d, %s][%s] ", __FILE__, __LINE__, __FUNCTION__, ll_name(log_level).c_str()); \
            printf(fmt, ##args);                                                                      \
            printf("\n");                                                                             \
        }                                                                                             \
    } while (false)

#endif
