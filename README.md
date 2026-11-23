# Pseudo Character Driver (pcd)

A simple Linux character driver that I am building while learning from the LDD1 course (FastBit Embedded Brain Academy).
The "hardware" is memory areas in kernel RAM, so no real device is needed.

## Current version: multiple devices (`pcd_n.c`)

One driver now handles 4 devices.

| Device file    | Size       | Permission  |
|----------------|------------|-------------|
| `/dev/pcdev-1` | 1024 bytes | Read only   |
| `/dev/pcdev-2` | 512 bytes  | Write only  |
| `/dev/pcdev-3` | 1024 bytes | Read-write  |
| `/dev/pcdev-4` | 512 bytes  | Read-write  |

What it does right now

* Gets 4 device numbers from the kernel (`alloc_chrdev_region`) and tells devices apart by minor number
* Creates one class, then registers each device with the VFS and creates its `/dev` file (`cdev_init`, `cdev_add`, `device_create`)
* Keeps per-device data (buffer, size, serial number, permission, `cdev`) in a device private data struct, and all devices in a driver private data struct
* `open` finds the device using `container_of` and saves it in `filp->private_data` for `read`, `write` and `lseek`
* `open` checks the access mode against the device permission and returns `-EPERM` if it is not allowed
* Real `read`, `write` and `lseek` logic using `copy_to_user` and `copy_from_user`
* If init fails partway, it cleans up everything done so far in reverse order

Build

```
make host
```

This creates `pcd_n.ko`. Clean with `make clean`.

Run

```
sudo insmod pcd_n.ko
ls -l /dev/pcdev*
sudo dmesg | tail
sudo rmmod pcd_n
```

Test

```
echo hello | sudo tee /dev/pcdev-1    # Operation not permitted (read only)
sudo cat /dev/pcdev-2                 # Operation not permitted (write only)
echo hello | sudo tee /dev/pcdev-3
sudo cat /dev/pcdev-3                 # prints hello
```

Planned

* Platform driver
* Device tree
* Locking

## Version 1: single device (`pcd.c`)

The first version of the driver. The "hardware" is a 512-byte memory area in kernel RAM.

What it does

* Gets a device number from the kernel (`alloc_chrdev_region`)
* Registers the char device with the VFS (`cdev_init`, `cdev_add`)
* Creates `/dev/pcd` automatically (`class_create`, `device_create`)
* Has `open`, `release`, `read`, `write` and `lseek` methods (for now they only print messages)
* Cleans up everything in reverse order on `rmmod`

Build

```
make host
```

This creates `pcd.ko`. Clean with `make clean`.

Run

```
sudo insmod pcd.ko
ls -l /dev/pcd
sudo dmesg | tail
sudo rmmod pcd
```

## Files

* `pcd.c` - the single-device driver
* `pcd_n.c` - the multi-device driver
* `Makefile` - builds the module

## Tested on

Fedora, x86_64, kernel 7.x
