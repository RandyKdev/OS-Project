# Kernel Modules

This part of the project has to do with loading kernel modules in to the linux system and unloading them.
Note: These are solely my opinions and not a hard rule to follow. Tested, Compiled and ran successfully on

- Ubuntu 20.04.3 LTS
- Kernel version 5.13.0-25-generic

# Requirements

- A linux/unix based OS
- root access
- and spare time to debug 🥲

# Steps to take note of when building a kernel module

1.  Your makefile should look similar to

> obj-m += hello.o
>
> all:\
>  make -C /lib/modules/\$(shell uname -r)/build M=$(PWD) modules
>
> clean:\
>  make -C /lib/modules/\$(shell uname -r)/build M=$(PWD) clean

Where "hello" is your file name

Note: there are tabs before the make command

2. Your directory should contain a "Kbuild" file, which contains the following line

> obj-m += hello.o

Where "hello" is your file name

3. To sign your modules (.ko file) before loading them with "insmod" command. Follow this [guide](https://wiki.debian.org/SecureBoot#MOK_-_Machine_Owner_Key)
