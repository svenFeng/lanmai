#include <bits/stdint-uintn.h>
#include <fcntl.h>
#include <string.h>
#include <linux/input-event-codes.h>
#include <unistd.h>
#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>
#include <libudev.h>
#include "../lib/log.h"
#include "../lib/common.h"

uint16_t map(uint16_t code) {
    switch (code) {
        case BTN_A:
            return BTN_B;
        case BTN_B:
            return BTN_A;
        case BTN_X:
            return BTN_Y;
        case BTN_Y:
            return BTN_X;
        default:
            return code;
    }
}

void grab(const std::string &path) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        log(LL_ERROR, "open file:%s failed.", path.c_str());
    }
    Defer fd_defer{[&]() { close(fd); }};

    struct libevdev *dev = nullptr;
    Defer dev_defer{[&]() { libevdev_free(dev); }};
    if (auto res = libevdev_new_from_fd(fd, &dev); res < 0) {
        log(LL_ERROR, "create dev failed, res:%d, str:%s", res, strerror(-res));
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

        if (input.code >= BTN_A && input.code <= BTN_Y) {
            log(LL_DEBUG, "accept: type:%d, code:%d, value:%d", input.type, input.code, input.value);
        }

        uint16_t code = input.code;
        if (input.type == 1) {
            code = map(input.code);
            log(LL_DEBUG, "send: type:%d, code:%d, value:%d", input.type, code, input.value);
        }
        libevdev_uinput_write_event(uidev, input.type, code, input.value);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        printf("sudo %s <log_level>\n", argv[0]);
        return 1;
    } else if (argc == 2) {
        global_log_level = std::stoi(argv[1]);
    }
    auto devices = get_devices("ID_INPUT_JOYSTICK");
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
