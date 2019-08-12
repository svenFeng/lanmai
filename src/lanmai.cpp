#include <fcntl.h>
#include <functional>
#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>
#include <libudev.h>
#include <map>
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
        default:
            return -1;
    }
}

struct Group {
    bool has_msc = false;
    input_event msc;
    input_event kbd;
};

void send(const struct libevdev_uinput *uinput_dev, unsigned int type, unsigned int code, int value) {
    log(LL_DEBUG, "send: type:%d, code:%d, value:%d", type, code, value);
    libevdev_uinput_write_event(uinput_dev, type, code, value);
}

void send_group(const struct libevdev_uinput *uinput_dev, Group group) {
    if (group.has_msc) {
        send(uinput_dev, group.msc.type, group.msc.code, group.msc.value);
    }
    send(uinput_dev, group.kbd.type, group.kbd.code, group.kbd.value);
    send(uinput_dev, EV_SYN, SYN_REPORT, 0);
}

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
    Group group;

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
        if (input.type == EV_MSC) {
            group.has_msc = true;
            group.msc     = input;
            continue;
        }
        if (input.type == EV_KEY) {
            group.kbd = input;
            continue;
        }
        if (input.type != EV_SYN) {
            log(LL_ERROR, "unexpected event type:%d", input.type);
            exit(1);
        }
        input = group.kbd;
        // Caps as Ctrl
        if (input.type == EV_KEY && input.code == KEY_CAPSLOCK) {
            group.kbd.code = KEY_LEFTCTRL;
            send_group(uidev, group);
            group.has_msc = false;
            continue;
        }
        // space
        if (input.code == KEY_SPACE) {
            if (!space_key_pressed) {
                if (input.value != 0) {
                    space_key_pressed = true;
                }
            } else {
                if (input.value == 0) {
                    space_key_pressed = false;
                    if (space_as_mk) {
                        space_as_mk = false;
                    } else {
                        log(LL_INFO, "SPACE");
                        send(uidev, EV_MSC, MSC_SCAN, KEY_SPACE);
                        send(uidev, EV_KEY, KEY_SPACE, 1);
                        send(uidev, EV_SYN, SYN_REPORT, 0);
                        send(uidev, EV_MSC, MSC_SCAN, KEY_SPACE);
                        send(uidev, EV_KEY, KEY_SPACE, 0);
                        send(uidev, EV_SYN, SYN_REPORT, 0);
                    }
                }
            }
            group.has_msc = false;
            continue;
        }
        int mc = map(input.code);
        if (mc != -1 && space_key_pressed) {
            space_as_mk = true;
            log(LL_INFO, "Arror: %d", input.code);
            send(uidev, EV_MSC, MSC_SCAN, mc);
            send(uidev, EV_KEY, mc, input.value);
            send(uidev, EV_SYN, SYN_REPORT, 0);
            group.has_msc = false;
            continue;
        }
        if (mc == -1 && space_key_pressed) {
            send(uidev, EV_MSC, MSC_SCAN, KEY_SPACE);
            send(uidev, EV_KEY, KEY_SPACE, 1);
            send(uidev, EV_SYN, SYN_REPORT, 0);
            send(uidev, EV_MSC, MSC_SCAN, KEY_SPACE);
            send(uidev, EV_KEY, KEY_SPACE, 0);
            send(uidev, EV_SYN, SYN_REPORT, 0);
            space_key_pressed = false;
            space_as_mk       = false;
        }
        send_group(uidev, group);
        group.has_msc = false;
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
