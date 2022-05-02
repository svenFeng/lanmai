#include "args.h"
#include "common.h"
#include "config.h"
#include "log.h"
#include "mapper.h"
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

void send(const libevdev_uinput* uinput_dev, unsigned int type, unsigned int code, int value) {
    LLOG(LL_DEBUG, "send: type:%d, code:%d, value:%d", type, code, value);
    libevdev_uinput_write_event(uinput_dev, type, code, value);
    libevdev_uinput_write_event(uinput_dev, EV_SYN, SYN_REPORT, 0);
}

void send(const libevdev_uinput* uinput_dev, input_event e) { send(uinput_dev, e.type, e.code, e.value); }

void handle_input(const std::string& path, SingleMapper sm, DoubleMapper dm, MetaMapper mm) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        LLOG(LL_ERROR, "open file:%s failed.", path.c_str());
        return;
    }
    Defer fd_defer{[&]() { close(fd); }};

    libevdev* dev = nullptr;
    Defer dev_defer{[&]() { libevdev_free(dev); }};
    if (libevdev_new_from_fd(fd, &dev) < 0) {
        LLOG(LL_ERROR, "create dev failed");
        return;
    }
    sleep(1);
    Defer grab_defer{[&]() { libevdev_grab(dev, LIBEVDEV_UNGRAB); }};
    if (libevdev_grab(dev, LIBEVDEV_GRAB) < 0) {
        LLOG(LL_ERROR, "grab dev failed");
        return;
    }

    int uifd = open("/dev/uinput", O_RDWR);
    if (uifd < 0) {
        LLOG(LL_ERROR, "open uinput file failed");
        return;
    }
    Defer uifd_defer{[&]() { close(uifd); }};

    struct libevdev_uinput* uidev = nullptr;
    Defer uidev_defer{[&]() { libevdev_uinput_destroy(uidev); }};
    if (libevdev_uinput_create_from_device(dev, uifd, &uidev) != 0) {
        return;
    }

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

        if (input.type != EV_KEY) {
            send(uidev, input);
            continue;
        }
        LLOG(LL_DEBUG, "accept key: type:%d, code:%d, value:%d", input.type, input.code, input.value);
        auto si = sm.map(input);
        for (auto di : dm.map(si)) {
            for (auto mi : mm.map(di)) {
                send(uidev, mi);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    Args args(argc, argv);
    GLOBAL_LOG_LEVEL  = args.log_level;
    json cfg          = readConfig(args.config_path);
    auto [sm, dm, mm] = get_mappers(cfg);
    if (!args.device.empty()) {
        handle_input(args.device, sm, dm, mm);
        return 0;
    }
    auto devices = get_kbd_devices();
    if (devices.size() == 0) {
        LLOG(LL_ERROR, "can't find out any key board device");
        return 1;
    }
    for (auto&& d : devices) {
        LLOG(LL_INFO, "keyboard device:%s", d.c_str());
    }
    handle_input(devices[0], sm, dm, mm);
    return 0;
}
