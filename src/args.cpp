#include "args.h"
#include "log.h"
#include "third_party/argparse.hpp"

Args::Args(int argc, char *argv[]) {
    argparse::ArgumentParser parser("Lanmai");

    parser.add_argument("-c", "--config")
        .help("config file path, default: /etc/lanmai.json")
        .default_value(std::string("/etc/lanmai.json"));
    parser.add_argument("-l", "--log-level")
        .help("log-level, DEBUG/INFO/ERROR, default: ERROR")
        .default_value(std::string("ERROR"));

    try {
        parser.parse_args(argc, argv);
    } catch (const std::runtime_error &err) {
        printf("%s\n", err.what());
        printf("%s\n", parser.help().str().c_str());
        exit(1);
    }

    configPath = parser.get<std::string>("-c");
    std::string ll  = parser.get<std::string>("-l");
    if (ll == "DEBUG") {
        log_level = LL_DEBUG;
    } else if (ll == "INFO") {
        log_level = LL_INFO;
    } else {
        log_level = LL_ERROR;
    }
}
