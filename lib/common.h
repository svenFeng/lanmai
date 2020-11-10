#include <functional>
#include <string>
#include <vector>

struct Defer {
    std::function<void()> f;
    Defer(std::function<void()> f) : f(f) {}
    ~Defer() { f(); }
};

// uinput and udev
std::vector<std::string> get_devices(const char *dt);
