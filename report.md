# Project 1 report

Student id: 1651045
Student name: HOANG DINH HIEU

Student id: 1651051
Student name: TRINH TUAN KIET

## Code review

Necessary headers

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

Author and description

    #define DRIVER_AUTHOR "Hoang Dinh Hieu <hdhieu@apcs.vn>, Trinh Tuan Kiet <ttkiet16@apcs.vn>"
    #define DRIVER_DESC "Random-number generator kernel module"


    static dev_t first;
    static struct cdev c_dev;
    static struct class *cl;
    static int r;
    static char my_buffer[20];

Function to print log at `INFO` level when open and close.

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

Function used when reading the device character, generating a random number via `get_random_bytes()` and then copy it to user space using `copy_to_user()`

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

Function when writing to the device

    static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
    {
        printk(KERN_INFO "Driver: write()\n");
        return len;
    }

Operations on the device

    static struct file_operations pugs_fops =
    {
        .owner = THIS_MODULE,
        .open = my_open,
        .release = my_close,
        .read = my_read,
        .write = my_write
    };

Constructor for the device

    static int __init init_random_generator(void)
    {

Logging

        printk(KERN_INFO "random-number generator registered");

Register a range of char device number, it will return negative code when error.

        if(alloc_chrdev_region(&first, 0, 1, "hdhieu") < 0 )
        {
            return -1;
        }

Create a class pointer

        if((cl = class_create(THIS_MODULE, "random_generator")) == NULL)
        {

If it fails then unregister the char device number and return error

            unregister_chrdev_region(first, 1);
            return -1;
        }

Create the device with the class pointer

        if(device_create(cl, NULL, first, NULL, "random_generator") == NULL)
        {

If unsuccessfull then destroy the class and unregister the device number

            class_destroy(cl);
            unregister_chrdev_region(first, 1);
            return -1;
        }

Create a `cdev` structure to add to the system.

        cdev_init(&c_dev, &pugs_fops);

Add the `cdev` to the system

        if(cdev_add(&c_dev, first, 1) == -1)
        {

If the adding is unsuccessfull, destroy the device, the class and unregister the char device number

            device_destroy(cl, first);
            class_destroy(cl);
            unregister_chrdev_region(first, 1);
            return -1;
        }
        return 0;
    }

Deconstructor for the device

    static void __exit exit_random_generator(void)
    {

Logging

        printk("Uninstalling random-number generator...");

Delete the `cdev` added to the system

        cdev_del(&c_dev);

Free the device class pointer

        device_destroy(cl, first);

Destroy the device class

        class_destroy(cl);

Unregister the char device number

        unregister_chrdev_region(first, 1);

Logging

        printk(KERN_INFO "Finished");
    }

Register the function to initialize and exit the module

    module_init(init_random_generator);
    module_exit(exit_random_generator);

Set the module license, author, description and supported devices

    MODULE_LICENSE("GPL");
    MODULE_AUTHOR(DRIVER_AUTHOR);
    MODULE_DESCRIPTION(DRIVER_DESC);
    MODULE_SUPPORTED_DEVICE("testdevice");

## Usage

README.md