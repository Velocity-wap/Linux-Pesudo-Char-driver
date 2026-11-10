#include<linux/module.h>
#include <linux/cdev.h>   // struct cdev, cdev_init, cdev_add
#include <linux/fs.h>     // struct file_operations
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>

#define NO_OF_DEVICES 4

#define MEM_SIZE_MAX_PCDEV1 1024
#define MEM_SIZE_MAX_PCDEV2 512
#define MEM_SIZE_MAX_PCDEV3 1024
#define MEM_SIZE_MAX_PCDEV4 512

char device_buffer_pcdev1[MEM_SIZE_MAX_PCDEV1];
char device_buffer_pcdev2[MEM_SIZE_MAX_PCDEV2];
char device_buffer_pcdev3[MEM_SIZE_MAX_PCDEV3];
char device_buffer_pcdev4[MEM_SIZE_MAX_PCDEV4];

/* device private Data structure*/
struct pcdev_private_data
{
  char *buffer;
  unsigned size;
  const char *serial_number;
  int perm;
  struct cdev cdev;
};

/* driver private data structure */
struct pcdrv_private_data
{
  int total_devices;
  struct pcdev_private_data pcdev_data[NO_OF_DEVICES];
  dev_t device_number;
  struct class *class_pcd;
  struct device *device_pcd;
};

struct pcdrv_private_data pcdrv_data = 
{
    .total_devices = NO_OF_DEVICES,
    .pcdev_data = {
            [0] = {
                  .buffer = device_buffer_pcdev1,
                  .size = MEM_SIZE_MAX_PCDEV1,
                  .serial_number = "PCDEV1XY123",
                  .perm = 0x1 /*read only*/
            },
            [1] = {
                  .buffer = device_buffer_pcdev2,
                  .size = MEM_SIZE_MAX_PCDEV2,
                  .serial_number = "PCDEV1XY123",
                  .perm = 0x10 /*Write only*/
            },
            [2] = {
                  .buffer = device_buffer_pcdev3,
                  .size = MEM_SIZE_MAX_PCDEV3,
                  .serial_number = "PCDEV1XY123",
                  .perm = 0x11 /*RDWR*/
            },
            [3] = {
                  .buffer = device_buffer_pcdev4,
                  .size = MEM_SIZE_MAX_PCDEV4,
                  .serial_number = "PCDEV1XY123",
                  .perm = 0x11 /*RDWR*/
            } 

    }
};

#undef pr_fmt
#define pr_fmt(fmt) "%s:" fmt, __func__

loff_t pcd_lseek (struct file *flip, loff_t off, int whence)
{

  struct pcdev_private_data *pcdev_data = (struct pcdev_private_data*) flip->private_data;
  int max_size = pcdev_data->size;

  loff_t temp;
  pr_info("lseek requested \n");
  pr_info("current file position:%lld\n", flip->f_pos);

  switch (whence){
    
    case SEEK_SET:
      
      if((off > max_size) || (off < 0)){
        return -EINVAL;
      }
      flip -> f_pos = off;
      break;
    
    case SEEK_CUR:
     
      temp = flip->f_pos + off;
      if ((temp > max_size) || (temp < 0)){
        return -EINVAL;
      }
      flip -> f_pos += off;
      break;

    case SEEK_END:
      
      temp = max_size + off;
      if((temp > max_size) || (temp < 0)){
        return -EINVAL;
      }
      flip -> f_pos = max_size + off;
      
      break;
    
    default:
      return -EINVAL;  /* Invalid Whence */
  }
  return flip->f_pos; /* Returns the new file position*/

  return 0;
}

ssize_t pcd_write (struct file *flip, const char __user *buff, size_t count, loff_t *f_pos)
{

  struct pcdev_private_data *pcdev_data = (struct pcdev_private_data*) flip->private_data;
  int max_size = pcdev_data->size;

  pr_info("write request for %zu bytes\n", count);
  pr_info("Current file position: %lld\n", *f_pos);

  /* Check n' adjust count */
  if((*f_pos + count) > max_size){
    count = max_size - *f_pos;
  }

  /* special case for no write */
  if(!count){
    return -ENOMEM;
  }

  /* Copy count to kernel */
  if(copy_from_user(pcdev_data->buffer+(*f_pos), buff, count)){
    return -EFAULT;       /* If copy_from_user throws non zero that means error i.e throws EFAULT */
  }

  /* update f_pos */
  *f_pos += count;

  pr_info("Number of bytes succesfully written: %zu\n", count);
  pr_info("Latest file position: %lld\n", *f_pos);

  /* Return the number of bytes that have been written */
  return count; 
  return -ENOMEM;

}

ssize_t pcd_read (struct file *flip, char __user *buff, size_t count, loff_t *f_pos)
{

  struct pcdev_private_data *pcdev_data = (struct pcdev_private_data*) flip->private_data;
  int max_size = pcdev_data->size;

  pr_info("read request for %zu bytes\n", count);

  /* Check n' adjust count */
  if((*f_pos + count) > max_size){
    count = max_size - *f_pos;
  }

  /* Copy count to user */
  if(copy_to_user(buff, pcdev_data->buffer+(*f_pos), count)){
    return -EFAULT;       /* If copy_to_user throws non zero that means error i.e throws EFAULT */
  }

  /* update f_pos */
  *f_pos += count;

  pr_info("Number of bytes succesfully read: %zu\n", count);
  pr_info("Latest file position: %lld\n", *f_pos);

  /* Return the number of bytes that have been read */
  return count;
  return 0;

}

int pcd_open(struct inode *inode, struct file *flip)
{

  int minor_n;
  struct pcdev_private_data *pcdev_data;

  /* find out on which device file open was attempted by the user space*/
  minor_n = MINOR(inode ->i_rdev);
  pr_info("minor access = %d\n", minor_n);

  /* get device's private data structure*/
  pcdev_data = container_of(inode -> i_cdev, struct pcdev_private_data, cdev);

  /* to supply devicep private data to other mehtods of the driver*/
  flip-> private_data = pcdev_data; 

  return 0;

}

int pcd_release(struct inode *inode, struct file *flip)
{

  pr_info("close was succesfull\n");
  return 0;

}

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
    int i;
    int ret;
    /* Dynamically allocate a device number */
    ret = alloc_chrdev_region(&pcdrv_data.device_number, 0, NO_OF_DEVICES, "pcd");
    if(ret < 0){
      pr_err("Alloc chrdev failed\n");
      goto out;
    }
 
    /* Create device class under /sys/class/ (moved cuz it happens only once) */
    pcdrv_data.class_pcd = class_create("pcd_class");
    if(IS_ERR(pcdrv_data.class_pcd)){
      pr_err("class creation failed\n");
      ret = PTR_ERR(pcdrv_data.class_pcd);
      goto unreg_chardev;
    }

    for (i = 0; i<NO_OF_DEVICES; i++){
      pr_info("Device number <major>:<minor> = %d:%d\n",MAJOR(pcdrv_data.device_number + i), MINOR(pcdrv_data.device_number + i));

      /* Initialize cdev structure with fops*/
      cdev_init(&pcdrv_data.pcdev_data[i].cdev, &pcd_fops);
      pcdrv_data.pcdev_data[i].cdev.owner = THIS_MODULE;

      /* Register a device with VFS*/
      ret = cdev_add(&pcdrv_data.pcdev_data[i].cdev, pcdrv_data.device_number + i, 1);
      if(ret<0){
        pr_err("Cdev add failed\n");
        goto cdev_del; /*we can use goto class_del as well*/
      }

      /* Populate sysfs with device information*/
      pcdrv_data.device_pcd = device_create(pcdrv_data.class_pcd, NULL, pcdrv_data.device_number + i, NULL, "pcdev - %d", i);
      if(IS_ERR(pcdrv_data.device_pcd)){
        pr_err("device create failed\n");
        ret = PTR_ERR(pcdrv_data.device_pcd);
        goto class_del;
      } 
    }

    pr_info("init was successfull\n");
    return 0;

cdev_del: 
  /* cdev_del is empty because cdev_add and device_create failures need the same cleanup, which sits under class_del.
   * Hence once code reaches cdev_del it will instantly go to class_del.
   * The label can be replaced for a failure caused in cdev_add i.e instead of using cdev_del and goto cdev_del we can directly give goto class_del*/

class_del:
    for(;i>=0;i--){
      device_destroy(pcdrv_data.class_pcd, pcdrv_data.device_number+i);
      cdev_del(&pcdrv_data.pcdev_data[i].cdev);
    }
    class_destroy(pcdrv_data.class_pcd);

unreg_chardev:
    unregister_chrdev_region(pcdrv_data.device_number, NO_OF_DEVICES);

out:
    return ret;
}

static void __exit pcd_driver_cleanup(void){
  int i;
  for(i=0; i<NO_OF_DEVICES; i++){
      device_destroy(pcdrv_data.class_pcd, pcdrv_data.device_number + i);
      cdev_del(&pcdrv_data.pcdev_data[i].cdev);
  } 

  class_destroy(pcdrv_data.class_pcd);
  unregister_chrdev_region(pcdrv_data.device_number, NO_OF_DEVICES);
  pr_info("module unloaded\n");
}

module_init(pcd_driver_init);
module_exit(pcd_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Swapnil");
MODULE_DESCRIPTION(" a Pseudo driver that handles N number of devices");
