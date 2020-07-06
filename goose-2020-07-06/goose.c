#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ilya Popovnin");
MODULE_DESCRIPTION("ASCII goose as a kernel module");
MODULE_VERSION("1.0");

static const char *ascii_goose = "\
     ▄▀▀▀▄\n\
▄███▀ *   ▌\n\
    ▌     ▐\n\
    ▐     ▐\n\
    ▌     ▐▄▄\n\
    ▌    ▄▀##▀▀▀▀▄\n\
   ▐    ▐########▀▀▄\n\
   ▐    ▐▄##########▀▄\n\
    ▀▄    ▀▄##########▀▄\n\
      ▀▄▄▄▄▄█▄▄▄▄▄▄▄▄▄▄▄▀▄\n\
           ▌▌ ▌▌\n\
           ▌▌ ▌▌\n\
         ▄▄▌▌▄▌▌\n\
";
static char *ascii_goose_ptr = NULL;
static const char *DEVICE_NAME = "goose";

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations file_ops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};
static int device_open_count = 0;

static dev_t goose_region;
static struct cdev goose_device;
static struct class *goose_class;

static int __init goose_init(void)
{
    if (alloc_chrdev_region(&goose_region, 0, 1, DEVICE_NAME) < 0)
    {
        printk(KERN_ALERT "Failed to allocate char device region\n");
        return -1;
    }

    goose_class = class_create(THIS_MODULE, "goose_char_device");
    if (goose_class == NULL)
    {
        printk(KERN_ALERT "Failed to create goose device class\n");
        unregister_chrdev_region(goose_region, 1);
        return -1;
    }

    if (device_create(goose_class, NULL, goose_region, NULL, DEVICE_NAME) == NULL)
    {
        printk(KERN_ALERT "Failed to create goose device\n");
        class_destroy(goose_class);
        unregister_chrdev_region(goose_region, 1);
        return -1;
    }

    cdev_init(&goose_device, &file_ops);
    if (cdev_add(&goose_device, goose_region, 1) < 0)
    {
        printk(KERN_ALERT "Failed to add goose device\n");
        device_destroy(goose_class, goose_region);
        class_destroy(goose_class);
        unregister_chrdev_region(goose_region, 1);
        return -1;
    }

    printk(KERN_INFO "Goose device initialized");

    return 0;
}

static void __exit goose_exit(void)
{
    cdev_del(&goose_device);
    device_destroy(goose_class, goose_region);
    class_destroy(goose_class);
    unregister_chrdev_region(goose_region, 1);
    printk(KERN_INFO "Goose device unregistered\n");
}

static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) {
    int bytes_read = 0;

    while (len > 0 && *ascii_goose_ptr != 0) {
        put_user(*(ascii_goose_ptr++), buffer++);
        --len;
        ++bytes_read;
    }

    return bytes_read;
}

static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset) {
    printk(KERN_ALERT "You can feed geese with anything but it doesn't make sense\n");
    return -EINVAL; // writing into geese is not supported
}

static int device_open(struct inode *inode, struct file *file) {
    if (device_open_count > 0) {
        printk(KERN_ALERT "You want too much from me\n");
        return -EBUSY; // only one reader is supported
    }

    ascii_goose_ptr = (char *)ascii_goose; 

    ++device_open_count;
    try_module_get(THIS_MODULE);

    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    --device_open_count;
    module_put(THIS_MODULE);

    return 0;
}

module_init(goose_init);
module_exit(goose_exit);
