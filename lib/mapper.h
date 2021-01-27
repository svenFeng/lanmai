#ifndef LANMAI_MAPPER_H
#define LANMAI_MAPPER_H

#include <cstdio>
#include <map>
#include <set>
#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>
#include <linux/input.h>
#include <sys/types.h>
#include <list>
#include <tuple>
#include "common.h"
#include "third_party/json.hpp"

class SingleMapper {
public:
    input_event map(input_event);
    bool empty() const { return keys.empty(); };
    void add_key(uint k1, uint k2) { keys[k1] = k2; }

private:
    std::map<uint, uint> keys;
};

class DoubleMapper {
public:
    std::list<input_event> map(input_event);
    void add_key(uint k, uint k1, uint k2) { keys.emplace(k, Info(k1, k2)); }
    bool empty() const { return keys.empty(); };

private:
    struct Info {
        bool pressed      = false;
        bool as_press_key = false;
        uint click_key;
        uint press_key;
        Info(uint k1, uint k2) : click_key(k1), press_key(k2) {}
    };
    std::map<uint, Info> keys;
    std::set<uint> pressed_set;
};

class MetaMapper {
public:
    MetaMapper() = default;
    MetaMapper(uint key, uint click_key, std::map<uint, uint> keys) : key(key), click_key(click_key), keys(keys) {}
    std::list<input_event> map(input_event input);
    bool empty() const { return keys.empty(); };

private:
    bool pressed     = false;
    bool as_meta_key = false;
    uint key;
    uint click_key;
    std::map<uint, uint> keys;
    std::list<input_event> mapped_inputs;
};

std::tuple<SingleMapper, DoubleMapper, MetaMapper> get_mappers(const nlohmann::json &cfg);

#endif
