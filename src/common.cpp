#include "../lib/log.h"
#include "../lib/common.h"
#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>
#include <libudev.h>

std::vector<std::string> get_devices(const char *dt) {
    std::vector<std::string> devnodes;
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;

    udev = udev_new();
    if (!udev) {
        LLOG(LL_ERROR, "Can't create udev.");
        exit(1);
    }
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_property(enumerate, dt, "1");
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
