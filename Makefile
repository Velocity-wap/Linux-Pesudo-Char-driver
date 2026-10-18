obj-m := pcd.o

KERN_DIR = /lib/modules/$(shell uname -r)/build

host:
	make -C $(KERN_DIR) M=$(PWD) modules

clean:
	make -C $(KERN_DIR) M=$(PWD) clean
