#ifndef LANMAI_ARGS_H
#define LANMAI_ARGS_H

#include "log.h"
#include <string>

struct Args {
    std::string configPath;
    int8_t log_level;
    Args(int argc, char *argv[]);
};

#endif
