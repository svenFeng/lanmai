#include <string>

static int8_t global_log_level = 0;
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

#define log(log_level, fmt, args...)                                                                  \
    do {                                                                                              \
        if (log_level <= global_log_level) {                                                          \
            printf("[%s:%d, %s][%s] ", __FILE__, __LINE__, __FUNCTION__, ll_name(log_level).c_str()); \
            printf(fmt, ##args);                                                                      \
            printf("\n");                                                                             \
        }                                                                                             \
    } while (false)
