#include "args.h"
#include "common.h"
#include "log.h"
#include "third_party/argparse.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>

Args::Args(int argc, char* argv[]) {
    argparse::ArgumentParser parser("Lanmai");

    parser.add_argument("-c", "--config")
        .help("config file path, default: /etc/lanmai.json")
        .default_value(std::string("/etc/lanmai.json"));
    parser.add_argument("-l", "--log-level")
        .help("log-level, DEBUG/INFO/ERROR, default: ERROR")
        .default_value(std::string("ERROR"));
    parser.add_argument("--list-kbd-devices")
        .help("list all keyboard devices")
        .default_value(false)
        .implicit_value(true);
    // clang-format off
    parser.add_argument("-d", "--device")
        .help("specify which one you want to grab")
        .default_value(std::string());
    // clang-format off
    parser.add_argument("-v", "--version")
        .help("lanmai version")
        .default_value(false)
        .implicit_value(true);
    parser.add_argument("--keys")
        .help("display all key names")
        .default_value(false)
        .implicit_value(true);

    try {
        parser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        printf("%s\n", err.what());
        printf("%s\n", parser.help().str().c_str());
        exit(1);
    }

    if (parser.get<bool>("-v")) {
        printf("%s\n", LANMAI_VERSION);
        exit(0);
    }

    if (parser.get<bool>("--list-kbd-devices")) {
        print_all_kbd_devices();
        exit(0);
    }

    if (parser.get<bool>("--keys")) {
        print_all_keys();
        exit(0);
    }

    config_path     = parser.get<std::string>("-c");
    device         = parser.get<std::string>("-d");
    std::string ll = parser.get<std::string>("-l");
    if (ll == "DEBUG") {
        log_level = LL_DEBUG;
    } else if (ll == "INFO") {
        log_level = LL_INFO;
    } else {
        log_level = LL_ERROR;
    }
}
