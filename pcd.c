
#include<linux/module.h>
#include <linux/cdev.h>   // struct cdev, cdev_init, cdev_add
#include <linux/fs.h>     // struct file_operations
#include <linux/device.h>
#include <linux/kdev_t.h>

#define DEV_MEM_SIZE 512

struct cdev pcd_cdev;               // one per device

#undef pr_fmt
#define pr_fmt(fmt) "%s:" fmt, __func__

loff_t pcd_lseek (struct file *flip, loff_t off, int whence)
{
  pr_info("lseek requested \n");
  return 0;
}
ssize_t pcd_write (struct file *flip, const char __user *Buff, size_t count, loff_t *f_pos)
{
  pr_info("write request for %zu bytes\n", count);
  return 0; 
}

ssize_t pcd_read (struct file *flip, char __user *Buff, size_t count, loff_t *f_pos)
{
  pr_info("read request for %zu bytes\n", count);
  return 0;
}

int pcd_open(struct inode *inode, struct file *flip)
{
  pr_info("open was succesfull\n");
  return 0;
}

int pcd_release(struct inode *inode, struct file *flip)
{
  pr_info("close was succesfull\n");
  return 0;
}

struct class *class_pcd;
struct device *device_pcd;

// Pseudo device's memory
char device_buffer[DEV_MEM_SIZE];

dev_t device_number;

struct file_operations pcd_fops =
{
  .open = pcd_open,
  .write = pcd_write,
  .llseek = pcd_lseek,
  .read = pcd_read,
  .release = pcd_release,
  .owner = THIS_MODULE
};

static int __init pcd_driver_init(void){

    /* 1. Dynamically allocate a device number */
    alloc_chrdev_region(&device_number, 0, 1, "pcd");
    pr_info("Device number <major>:<minor> = %d:%d\n",MAJOR(device_number), MINOR(device_number));

    /* 2. initialize cdev structure with fops*/
    cdev_init(&pcd_cdev, &pcd_fops);
    pcd_cdev.owner = THIS_MODULE;

    /* 3. Register a device with VFS*/
    cdev_add(&pcd_cdev, device_number, 1);

    /* 4. Create device class under /sys/class/ */
    class_pcd = class_create("pcd_class");

    /* 5. Populate sysfs with device information*/
    device_pcd = device_create(class_pcd, NULL, device_number, NULL, "pcd");

    pr_info("init was successfull\n");

    return 0;

}

static void __exit pcd_driver_cleanup(void){

    device_destroy(class_pcd, device_number);
    class_destroy(class_pcd);
    cdev_del(&pcd_cdev);
    unregister_chrdev_region(device_number, 1);
    pr_info("module unloaded\n");

}

module_init(pcd_driver_init);
module_exit(pcd_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Swapnil");
MODULE_DESCRIPTION("Pseudo driver");
