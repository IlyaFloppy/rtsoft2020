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

#include "penguin.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ilya Popovnin");
MODULE_DESCRIPTION("penguin buffer");
MODULE_VERSION("1.0");

static char buffer[PENGUIN_BUFFER_SIZE];
static ssize_t current_read_idx = 0;
static ssize_t current_write_idx = 0;
static const char *DEVICE_NAME = "penguin";

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static long int device_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations file_ops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
    .unlocked_ioctl = device_ioctl
};
static int device_open_count = 0;

static dev_t penguin_region;
static struct cdev penguin_device;
static struct class *penguin_class;

static int __init penguin_init(void)
{
    if (alloc_chrdev_region(&penguin_region, 0, 1, DEVICE_NAME) < 0)
    {
        printk(KERN_ALERT "Failed to allocate char device region\n");
        return -1;
    }

    penguin_class = class_create(THIS_MODULE, "penguin_char_device");
    if (penguin_class == NULL)
    {
        printk(KERN_ALERT "Failed to create penguin device class\n");
        unregister_chrdev_region(penguin_region, 1);
        return -1;
    }

    if (device_create(penguin_class, NULL, penguin_region, NULL, DEVICE_NAME) == NULL)
    {
        printk(KERN_ALERT "Failed to create penguin device\n");
        class_destroy(penguin_class);
        unregister_chrdev_region(penguin_region, 1);
        return -1;
    }

    cdev_init(&penguin_device, &file_ops);
    if (cdev_add(&penguin_device, penguin_region, 1) < 0)
    {
        printk(KERN_ALERT "Failed to add penguin device\n");
        device_destroy(penguin_class, penguin_region);
        class_destroy(penguin_class);
        unregister_chrdev_region(penguin_region, 1);
        return -1;
    }

    printk(KERN_INFO "Penguin device initialized");

    return 0;
}

static void __exit penguin_exit(void)
{
    cdev_del(&penguin_device);
    device_destroy(penguin_class, penguin_region);
    class_destroy(penguin_class);
    unregister_chrdev_region(penguin_region, 1);
    printk(KERN_INFO "Penguin device unregistered\n");
}

static ssize_t device_read(struct file *flip, char *user_buffer, size_t len, loff_t *offset)
{
    ssize_t bytes_read = 0;
    while (len > 0)
    {
        put_user(buffer[current_read_idx++], user_buffer++);
        --len;
        ++bytes_read;
        if (current_read_idx >= PENGUIN_BUFFER_SIZE)
        {
            current_read_idx = 0;
        }
    }

    return bytes_read;
}

static ssize_t device_write(struct file *flip, const char *user_buffer, size_t len, loff_t *offset)
{
    ssize_t bytes_written = 0;
    while (len > 0)
    {
        get_user(buffer[current_write_idx++], user_buffer++);
        --len;
        ++bytes_written;
        if (current_write_idx >= PENGUIN_BUFFER_SIZE)
        {
            current_write_idx = 0;
        }
    }
    return bytes_written;
}

static int device_open(struct inode *inode, struct file *file)
{
    ++device_open_count;
    try_module_get(THIS_MODULE);

    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    --device_open_count;
    module_put(THIS_MODULE);

    return 0;
}

static long int device_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    if (cmd == IOCTL_RESET_PENGUIN_BUFFER) {
        int idx = 0;
        while (idx < PENGUIN_BUFFER_SIZE)
        {
            buffer[idx] = 0;
            ++idx;
        }
        current_read_idx = 0;
        current_write_idx = 0;
    }
    return 0;
}

module_init(penguin_init);
module_exit(penguin_exit);
