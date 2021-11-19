#pragma once

#include "log.h"
#include <string>

struct Args {
    std::string config_path;
    int8_t log_level;
    std::string device;
    Args(int argc, char* argv[]);
};
