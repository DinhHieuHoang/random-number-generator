# Random number generator module

## Build

Build the kernel object module

    $ make
    $ modinfo random_generator.ko

## Install/Uninstall module

Install the module

    $ sudo insmod random_generator.ko
    $ dmesg

Uninstall the module

    $ sudo rmmod random_generator.ko
    $ dmesg

Check the installation status

    $ lsmod | grep "random_generator"