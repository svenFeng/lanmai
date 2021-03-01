# lanmai
lanmai is a key mapping software on Linux.

# features
+ three mapping modes:
    + single mapping
    + double mapping, e.g., click CAPSLOCK => ESC, CAPSLOCK + other keys => CTRL + other keys
    + meta mapping, it can be used to implement [SpaceFn](https://geekhack.org/index.php?topic=51069.0), e.g., click SPACE => SPACE, SPACE + h/j/k/l => Left/Down/Up/Right
+ don't depend on XWindow

# dependencies
+ libevdev
+ libudev

# build and install
```
git clone https://github.com/svenFeng/lanmai.git
cd lanmai
mkdir build
cd build
cmake ..
make
sudo make install
```

# usage
## configuration
the default config file is /etc/lanmai.json, there existed some useful mappings, you can choose what you want, and change the `enable` to `true`.

## run
lanmai need run by root user.

you can use `lanmai --help` to see some options.

when you installed correctly, the etc/lanmai.service has been installed to /usr/lib/systemd/systemd/, so you can manage it by systemd.
```
# start
sudo systemctl start lanmai.service
# stop
sudo systemctl stop lanmai.service
# enable
sudo systemctl enable lanmai.service
```

## tips
1. multi-devices
lanmai don't support grab multi-devices now, if you have more than one keyboards, you can use `--list-kbd-devices` option to find which device you want to grab, and use `-d` option to specify it.

2. grab wrong device
sometimes mouse(or others) may register as a keyboard, so lanmai may grab it, you can also use `--list-kbd-devices` and `-d` options to avoid that trouble like above.

# TODO
## P0
+ real-time derives detecting and mapping
+ better logger

## P1
+ verify it in Coq/TLA+
