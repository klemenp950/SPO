#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/timer.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "modul"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Avtor");
MODULE_VERSION("1.0");

static int major_number;
static struct timer_list led_timer;
static int LEDon = 0;
static int SWITCHon = 0;
static int user_interval = 2000; // Default 2 seconds

static void timer_callback(struct timer_list *timer) {
    if (SWITCHon == 1) {
        LEDon = !LEDon;
        mod_timer(&led_timer, jiffies + msecs_to_jiffies(user_interval));
    }
}

static ssize_t device_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    char command;
    if (copy_from_user(&command, buffer, 1)) {
        return -EFAULT;
    }

    switch (command) {
        case 0x00:
            user_interval = 0;
            break;
        case 0x01:
            user_interval = 500;
            break;
        case 0x02:
            SWITCHon = 0;
            break;
        case 0x03:
            SWITCHon = 1;
            break;
        default:
            printk(KERN_WARNING "Invalid command\n");
            return -EINVAL;
    }

    printk(KERN_INFO "SWITCHon: %d, LEDon: %d\n", SWITCHon, LEDon);
    if (SWITCHon == 1) {
        mod_timer(&led_timer, jiffies + msecs_to_jiffies(user_interval));
    }

    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = device_write,
};

static int __init modul_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register a major number\n");
        return major_number;
    }

    timer_setup(&led_timer, timer_callback, 0);

    printk(KERN_INFO "Modul initialized. Major number: %d\n", major_number);
    printk(KERN_INFO "Create the device node with: mknod /dev/%s c %d 0\n", DEVICE_NAME, major_number);
    return 0;
}

static void __exit modul_exit(void) {
    del_timer(&led_timer);
    unregister_chrdev(major_number, DEVICE_NAME);

    printk(KERN_INFO "Modul exited\n");
}

module_init(modul_init);
module_exit(modul_exit);


