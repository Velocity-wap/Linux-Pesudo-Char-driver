# Pseudo Character Driver (pcd)

A simple Linux character driver that I am building while learning from the LDD1 course (FastBit Embedded Brain Academy).

The "hardware" is a 512-byte memory area in kernel RAM, so no real device is needed.

## What it does right now
- Gets a device number from the kernel (`alloc_chrdev_region`)
- Registers the char device with the VFS (`cdev_init`, `cdev_add`)
- Creates `/dev/pcd` automatically (`class_create`, `device_create`)
- Has `open`, `release`, `read`, `write` and `lseek` methods (for now they only print messages)
- Cleans up everything in reverse order on `rmmod`

## Planned
- Real read, write and lseek logic using `copy_to_user` and `copy_from_user`
- A userspace test app
- Multiple device nodes

## Build
```
make host
```
This creates `pcd.ko`. Clean with `make clean`.

## Run
```
sudo insmod pcd.ko
ls -l /dev/pcd
sudo dmesg | tail
sudo rmmod pcd
```

## Files
- `pcd.c` - the driver
- `Makefile` - builds the module

## Tested on
Fedora, x86_64, kernel 7.x# Pseudo Character Driver (pcd)

A simple Linux character driver that I am building while learning from the LDD1 course (FastBit Embedded Brain Academy).

The "hardware" is a 512-byte memory area in kernel RAM, so no real device is needed.

## What it does right now
- Gets a device number from the kernel (`alloc_chrdev_region`)
- Registers the char device with the VFS (`cdev_init`, `cdev_add`)
- Creates `/dev/pcd` automatically (`class_create`, `device_create`)
- Has `open`, `release`, `read`, `write` and `lseek` methods (for now they only print messages)
- Cleans up everything in reverse order on `rmmod`

## Planned
- Real read, write and lseek logic using `copy_to_user` and `copy_from_user`
- A userspace test app
- Multiple device nodes

## Build
```
make host
```
This creates `pcd.ko`. Clean with `make clean`.

## Run
```
sudo insmod pcd.ko
ls -l /dev/pcd
sudo dmesg | tail
sudo rmmod pcd
```

## Files
- `pcd.c` - the driver
- `Makefile` - builds the module

## Tested on
Fedora, x86_64, kernel 7.x
