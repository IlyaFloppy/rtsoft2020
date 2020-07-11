#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/ioctl.h>

#define PENGUIN_BUFFER_SIZE 4096

#define IOCTL_RESET_PENGUIN_BUFFER 1
