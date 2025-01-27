#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/timer.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Klemen Parkelj");
MODULE_VERSION("1.0");

#define DEVICE_NAME "modul"
#define CLASS_NAME "modul_class"

static int major_number;
static struct class *modul_class = NULL;
static struct device *modul_device = NULL;

static struct timer_list led_timer;
static int LEDon = 0;
static int SWITCHon = 0;
static int user_interval = 2000; // Privzeto 2 sekundi

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

    modul_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(modul_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(modul_class);
    }

    modul_device = device_create(modul_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(modul_device)) {
        class_destroy(modul_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(modul_device);
    }

    timer_setup(&led_timer, timer_callback, 0);

    printk(KERN_INFO "Modul initialized: /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit modul_exit(void) {
    del_timer(&led_timer);
    device_destroy(modul_class, MKDEV(major_number, 0));
    class_unregister(modul_class);
    class_destroy(modul_class);
    unregister_chrdev(major_number, DEVICE_NAME);

    printk(KERN_INFO "Modul exited\n");
}

module_init(modul_init);
module_exit(modul_exit);

