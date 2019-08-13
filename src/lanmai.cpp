#include <fcntl.h>
#include <functional>
#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>
#include <libudev.h>
#include <map>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <vector>

#define MAX_KBD_DEVICES 100

static int8_t global_log_level = 0;
enum LogLevel : int8_t { LL_ERROR = 0, LL_INFO = 1, LL_DEBUG = 2 };

template <typename... T> void log(int8_t log_level, T... args) {
    if (log_level <= global_log_level) {
        printf(args...);
        printf("\n");
    }
}

int map(int code) {
    switch (code) {
        case KEY_H:
            return KEY_LEFT;
        case KEY_J:
            return KEY_DOWN;
        case KEY_K:
            return KEY_UP;
        case KEY_L:
            return KEY_RIGHT;
        case KEY_E:
            return KEY_ESC;
        default:
            return -1;
    }
}

void send(const struct libevdev_uinput *uinput_dev, unsigned int type, unsigned int code, int value) {
    log(LL_DEBUG, "send: type:%d, code:%d, value:%d", type, code, value);
    libevdev_uinput_write_event(uinput_dev, type, code, value);
    libevdev_uinput_write_event(uinput_dev, EV_SYN, SYN_REPORT, 0);
}

void send(const struct libevdev_uinput *uinput_dev, input_event e) { send(uinput_dev, e.type, e.code, e.value); }

// based on https://gist.github.com/mmn80/4337126
std::vector<std::string> get_kbd_devices() {
    std::vector<std::string> devnodes;
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;

    udev = udev_new();
    if (!udev) {
        log(LL_ERROR, "Can't create udev.");
        exit(1);
    }
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_property(enumerate, "ID_INPUT_KEYBOARD", "1");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);
    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path, *devnode;
        path    = udev_list_entry_get_name(dev_list_entry);
        dev     = udev_device_new_from_syspath(udev, path);
        devnode = udev_device_get_devnode(dev);
        if (devnode) {
            devnodes.push_back(std::string(devnode));
        }
        udev_device_unref(dev);
    }
    udev_enumerate_unref(enumerate);
    udev_unref(udev);
    return devnodes;
}

void print_all_kbd_devices() {
    for (auto &&d : get_kbd_devices()) {
        log(LL_INFO, "kbd: %s", d.c_str());
    }
}

struct Defer {
    std::function<void()> f;
    Defer(std::function<void()> f) : f(f) {}
    ~Defer() { f(); }
};

void grab(const std::string &path) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        log(LL_ERROR, "open file:%s failed.", path.c_str());
    }
    Defer fd_defer{[&]() { close(fd); }};

    struct libevdev *dev = nullptr;
    Defer dev_defer{[&]() { libevdev_free(dev); }};
    if (libevdev_new_from_fd(fd, &dev) < 0) {
        log(LL_ERROR, "create dev failed");
        return;
    }
    sleep(1);
    Defer grab_defer{[&]() { libevdev_grab(dev, LIBEVDEV_UNGRAB); }};
    if (libevdev_grab(dev, LIBEVDEV_GRAB) < 0) {
        log(LL_ERROR, "grab dev failed");
        return;
    }

    int uifd = open("/dev/uinput", O_RDWR);
    if (uifd < 0) {
        log(LL_ERROR, "open uinput file failed");
        return;
    }
    Defer uifd_defer{[&]() { close(uifd); }};

    struct libevdev_uinput *uidev = nullptr;
    Defer uidev_defer{[&]() { libevdev_uinput_destroy(uidev); }};
    if (libevdev_uinput_create_from_device(dev, uifd, &uidev) != 0) {
        return;
    }

    bool space_key_pressed = false;
    bool space_as_mk       = false;
    int pressed_count      = 0;

    while (true) {
        struct input_event input;
        int rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL | LIBEVDEV_READ_FLAG_BLOCKING, &input);
        while (rc == LIBEVDEV_READ_STATUS_SYNC) {
            rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_SYNC, &input);
        }
        if (rc == -EAGAIN) {
            continue;
        }
        if (rc != LIBEVDEV_READ_STATUS_SUCCESS) {
            break;
        }

        log(LL_DEBUG, "accept: type:%d, code:%d, value:%d", input.type, input.code, input.value);
        if (input.type != EV_KEY) {
            continue;
        }
        if (input.value == 1) {
            pressed_count++;
        } else if (input.value == 0) {
            if (pressed_count > 0) {
                pressed_count--;
            }
        }
        // Caps as Ctrl
        if (input.type == EV_KEY && input.code == KEY_CAPSLOCK) {
            input.code = KEY_LEFTCTRL;
            send(uidev, input);
            continue;
        }
        // space
        log(LL_DEBUG, "space_key_pressed: %d, pressed_count:%d, space_as_mk:%d", space_key_pressed, pressed_count,
            space_as_mk);
        if (input.code == KEY_SPACE) {
            if (space_key_pressed) {
                if (input.value == 0) {
                    space_key_pressed = false;
                    if (space_as_mk) {
                        space_as_mk = false;
                    } else {
                        log(LL_INFO, "SPACE");
                        send(uidev, EV_KEY, KEY_SPACE, 1);
                        send(uidev, EV_KEY, KEY_SPACE, 0);
                    }
                }
                continue;
            }
            if (input.value == 1 && pressed_count == 1) {
                space_key_pressed = true;
                continue;
            }
        }
        int mc = map(input.code);
        if (mc != -1) {
            static std::set<unsigned int> set{};
            if (space_key_pressed) {
                if (input.value == 1) {
                    space_as_mk = true;
                    set.insert(input.code);
                } else if (input.value == 0) {
                    set.erase(input.code);
                }
                send(uidev, EV_KEY, mc, input.value);
                continue;
            } else if (set.find(input.code) != set.end()) {
                if (input.value == 0) {
                    set.erase(input.code);
                }
                send(uidev, EV_KEY, mc, input.value);
                continue;
            }
        }
        if (mc == -1 && space_key_pressed && !space_as_mk) {
            send(uidev, EV_KEY, KEY_SPACE, 1);
            send(uidev, EV_KEY, KEY_SPACE, 0);
            space_key_pressed = false;
        }
        send(uidev, input);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        printf("sudo %s <log_level>\n", argv[0]);
        return 1;
    } else if (argc == 2) {
        global_log_level = std::stoi(argv[1]);
    }
    auto devices = get_kbd_devices();
    if (devices.size() == 0) {
        log(LL_DEBUG, "can't find out any key board devices");
        return 0;
    }
    for (auto &&d : devices) {
        log(LL_INFO, "keyboard device:%s", d.c_str());
    }
    grab(devices[0]);
    return 0;
}
