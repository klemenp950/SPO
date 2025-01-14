#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "gonilnik"
#define BUFFER_SIZE 256

// sudo make -C /usr/src/linux-headers-$(uname -r) M=$(pwd) modules
// sudo insmod ./gonilnik.ko
// cat /proc/devices | grep gonilnik
// sudo mknod -m 666 /dev/gonilnik c MAJOR MINOR


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple character device driver");
MODULE_VERSION("1.0");

static int major_number;
static int open_count = 0;
static char device_buffer[BUFFER_SIZE];
static int device_open(struct inode *inode, struct file *file);
static int device_release(struct inode *inode, struct file *file);
static ssize_t device_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset);

static struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .read = device_read,
};

static int __init char_device_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register a major number\n");
        return major_number;
    }
    printk(KERN_INFO "Registered char device with major number %d\n", major_number);
    return 0;
}

static void __exit char_device_exit(void) {
    open_count = 0;
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Char device unregistered\n");
}

static int device_open(struct inode *inode, struct file *file) {
    open_count++;
    printk(KERN_INFO "%s: Device opened %d times\n", DEVICE_NAME, open_count);
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "%s: Device closed\n", DEVICE_NAME);
    return 0;
}

static ssize_t device_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset) {
    char msg[BUFFER_SIZE];
    int msg_length;

    msg_length = snprintf(msg, BUFFER_SIZE, "Device opened %d times\n", open_count);

    if (*offset >= msg_length) {
        return 0;
    }

    if (size + *offset > msg_length) {
        size = msg_length - *offset;
    }

    if (copy_to_user(user_buffer, msg + *offset, size) != 0) {
        return -EFAULT;
    }

    *offset += size;
    return size;
}

module_init(char_device_init);
module_exit(char_device_exit);

/* Instructions to compile the module:
 * 1. Create a Makefile in the same directory as this source file:
 *    obj-m += my_char_device.o
 *
 * 2. Run the following commands to compile and load the module:
 *    $ make -C /lib/modules/$(uname -r)/build M=$(pwd) modules
 *    $ sudo insmod my_char_device.ko
 *
 * 3. To interact with the device:
 *    - Create a device file: sudo mknod /dev/my_char_device c <major_number> 0
 *    - Replace <major_number> with the major number printed in dmesg.
 *
 * 4. Check kernel logs for output using: dmesg
 *
 * 5. To remove the module:
 *    $ sudo rmmod my_char_device
 */
