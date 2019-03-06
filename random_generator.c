#include <linux/module.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/random.h>
#include <linux/uaccess.h>

#define DRIVER_AUTHOR "Hoang Dinh Hieu <hdhieu@apcs.vn>, Trinh Tuan Kiet <ttkiet16@apcs.vn>"
#define DRIVER_DESC "Random-number generator kernel module"

static dev_t first;
static struct cdev c_dev;
static struct class *cl;
static int r;
static char my_buffer[20];

static int my_open(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Driver: open()\n");
	return 0;
}

static int my_close(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Driver: close()\n");
	return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "Driver: read()\n");
	get_random_bytes(&r, sizeof(int));
	sprintf(my_buffer, "%d\n", r);
	if(*off > 0)
	{
		return 0;
	}
	if(copy_to_user(buf, &my_buffer, strlen(my_buffer)))
	{
		return -EFAULT;
	}
	*off+=strlen(my_buffer);
	printk("%d\n", r);
	return strlen(my_buffer);
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "Driver: write()\n");
	return len;
}

static struct file_operations pugs_fops =
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
	.write = my_write
};

static int __init init_random_generator(void)
{
	printk(KERN_INFO "random-number generator registered");
	if(alloc_chrdev_region(&first, 0, 1, "hdhieu") < 0 )
	{
		return -1;
	}
	if((cl = class_create(THIS_MODULE, "random_generator")) == NULL)
	{
		unregister_chrdev_region(first, 1);
		return -1;
	}
	if(device_create(cl, NULL, first, NULL, "random_generator") == NULL)
	{
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return -1;
	}
	cdev_init(&c_dev, &pugs_fops);
	if(cdev_add(&c_dev, first, 1) == -1)
	{
		device_destroy(cl, first);
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return -1;
	}
	return 0;
}

static void __exit exit_random_generator(void)
{
	printk("Uninstalling random-number generator...");
	cdev_del(&c_dev);
	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first, 1);
	printk(KERN_INFO "Finished");
}

module_init(init_random_generator);
module_exit(exit_random_generator);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("testdevice");
