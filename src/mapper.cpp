#include <linux/input.h>
#include "mapper.h"
#include "log.h"
#include "common.h"

input_event SingleMapper::map(input_event input) {
    if (auto it = keys.find(input.code); it != keys.end()) {
        input.code = it->second;
    }
    return input;
}

// pressed_set as used to check a double key whether is a press key,
//     when a double key and other keys pressed, the key will be treated as a press key.
std::list<input_event> DoubleMapper::map(input_event input) {
    std::list<input_event> res;
    if (input.value == 1 && !pressed_set.empty()) {
        for (auto v : pressed_set) {
            auto ni           = input;
            Info &info        = keys.at(v);
            info.as_press_key = true;
            ni.code           = info.press_key;
            ni.value          = 1;
            res.push_back(ni);
        }
        pressed_set.clear();
    }
    auto it = keys.find(input.code);
    if (it == keys.end()) {
        res.push_back(input);
        return res;
    }
    if (pressed_set.size() > 1) {
        for (auto v : pressed_set) {
            Info &info        = keys.at(v);
            info.as_press_key = true;
            input.code        = info.press_key;
            input.value       = 1;
            res.push_front(input);
        }
        pressed_set.clear();
    }
    Info &info = it->second;
    if (input.value == 1) {
        info.pressed = true;
        pressed_set.insert(it->first);
    } else if (input.value == 2) {
        if (info.as_press_key) {
            input.code = info.press_key;
            res.push_back(input);
        }
    } else if (!info.as_press_key) {
        input.code = info.click_key;
        res.push_back(input);
        input.value = 1;
        res.push_front(input);
        info.pressed = false;
        pressed_set.clear();
    } else {
        input.code  = info.press_key;
        input.value = 0;
        res.push_back(input);
        info.as_press_key = false;
        info.pressed      = false;
    }
    return res;
}

std::list<input_event> MetaMapper::map(input_event input) {
    std::list<input_event> res;
    if (input.code == key) {
        if (!as_meta_key) {
            if (!pressed && input.value == 1) {
                pressed = true;
            } else if (pressed && input.value == 0) {
                input.code = click_key;
                res.push_back(input);
                input.value = 1;
                res.push_front(input);
                pressed = false;
            }
        } else if (input.value == 0) {
            pressed     = false;
            as_meta_key = false;
            for (auto i : mapped_inputs) {
                i.value = 0;
                res.push_back(i);
            }
            mapped_inputs.clear();
        }
        return res;
    }
    if (pressed) {
        if (input.value == 1) {
            as_meta_key = true;
        }
        if (as_meta_key) {
            auto it = keys.find(input.code);
            if (it != keys.end()) {
                input.code = it->second;
                if (input.value == 1) {
                    mapped_inputs.push_back(input);
                }
            }
        }
    }
    res.push_back(input);
    return res;
}

std::tuple<SingleMapper, DoubleMapper, MetaMapper> get_mappers(const nlohmann::json &cfg) {
    SingleMapper sm;
    DoubleMapper dm;
    MetaMapper mm;
    if (auto it = cfg.find("mapping"); it != cfg.end()) {
        for (auto &&[m_name, v] : it->items()) {
            auto typ = v.at("type").get<std::string>();
            if (!v.at("enable").get<bool>()) {
                continue;
            }
            if (typ == "single") {
                uint from = TABLE.at(v.at("from").get<std::string>());
                uint to   = TABLE.at(v.at("to").get<std::string>());
                sm.add_key(from, to);
            } else if (typ == "double") {
                uint key = TABLE.at(v.at("key").get<std::string>());
                uint k1  = TABLE.at(v.at("click").get<std::string>());
                uint k2  = TABLE.at(v.at("press").get<std::string>());
                dm.add_key(key, k1, k2);
            } else if (typ == "meta") {
                if (!mm.empty()) {
                    LLOG(LL_INFO, "don't support multi meta key yet, ignore %s", m_name.c_str());
                    continue;
                }
                uint key   = TABLE.at(v.at("key").get<std::string>());
                uint click = TABLE.at(v.at("click").get<std::string>());
                std::map<uint, uint> meta_keys;
                for (auto &&[from, to] : v.at("mapping").items()) {
                    uint fv       = TABLE.at(from);
                    uint tv       = TABLE.at(to.get<std::string>());
                    meta_keys[fv] = tv;
                }
                mm = MetaMapper(key, click, meta_keys);
            } else {
                LLOG(LL_INFO, "unknown type:%s", typ.c_str());
            }
        }
    }
    return {sm, dm, mm};
}
