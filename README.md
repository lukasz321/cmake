## powerusb
A very simple wrapper for controlling PwrUSB on a repeating schedule.

See https://pwrusb.com

## requirements
```sh
sudo apt-get install cmake
```

## build
```sh
mkdir -p build && cd build
cmake ..
make
```

## usage
Power cycle the entire strip every 300 seconds:
```sh
powercycler 300
```

Don't forget to update your udev rules. See `src/powerstrip/udev`.
