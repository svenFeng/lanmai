#pragma once

#include "common.h"
#include "err.h"
#include "third_party/json.hpp"

using json = nlohmann::json;

std::string read_file(const std::string& path);
json readConfig(const std::string& path);
