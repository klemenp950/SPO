#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define DEVICE_NAME "gonilnik"
#define BUFFER_SIZE 128
#define QUANTUM_SIZE 10
#define QSET_SIZE 5

struct dev {
    void **data;
    unsigned Q;
    unsigned S;
    unsigned size;
};

// sudo make -C /usr/src/linux-headers-$(uname -r) M=$(pwd) modules
// sudo insmod ./gonilnik.ko
// cat /proc/devices | grep gonilnik
// sudo mknod -m 666 /dev/gonilnik c MAJOR MINOR


MODULE_LICENSE("GPL");

int major_number;
int device_open(struct inode *inode, struct file *file);
int device_release(struct inode *inode, struct file *file);
ssize_t device_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset);
ssize_t device_write(struct file *file, const char __user *user_buffer, size_t size, loff_t *offset);
struct dev *dev_ptr;

struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
};

int __init char_device_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    dev_ptr = kmalloc(sizeof(struct dev), GFP_KERNEL);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register a major number\n");
        return major_number;
    }
    printk(KERN_INFO "Registered char device with major number %d\n", major_number);
    memset(dev_ptr, 0, sizeof(struct dev));
    dev_ptr->Q = QUANTUM_SIZE;
    dev_ptr->S = QSET_SIZE;
    return 0;
}

void __exit char_device_exit(void) {
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Char device unregistered\n");
}

int device_open(struct inode *inode, struct file *file) {
    file->private_data = dev_ptr;
    return 0;
}

int device_release(struct inode *inode, struct file *file) {
    return 0;
}

ssize_t device_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset) {
    struct dev *dev_data = file->private_data;
    int Q = dev_data->Q;
    int S = dev_data->S;
    int s_pos, q_pos;
    ssize_t retval = 0;

    if (*offset >= dev_data->size)
        goto out;

    if ( size > dev_data->size - *offset)
        size = dev_data->size - *offset;
    
    s_pos = (long) *offset / Q;
    q_pos = (long) *offset % Q;

    if (size > Q - q_pos)
        size = Q - q_pos;

    if ( copy_to_user(user_buffer, dev_data->data[s_pos] + q_pos, size)) {
        retval = -EFAULT;
        goto out;
    }
    *offset += size;
    retval = size;

    out:
    return retval;
}

ssize_t device_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos){
    struct dev *dev_data = filp->private_data;
    int Q = dev_data->Q;
    int S = dev_data->S;
    int s_pos, q_pos;
    ssize_t retval = -ENOMEM;

    s_pos = (long)*f_pos / Q; // pozicija v kvantni množici
    q_pos = (long)*f_pos % Q; // pozicija v kvantu
    if ( s_pos >= S)
        return count;// št. kvantov je omejeno na S (qset_size)

    // alokacija kvantne mnozice (qset)
    if (!dev_data->data) {
        dev_data->data = kmalloc(S * sizeof(char *), GFP_KERNEL);
        if (!dev_data->data)
            goto out;
        memset(dev_data->data, 0, S * sizeof(char *));
    }

    if (!dev_data->data[s_pos]) {
        dev_data->data[s_pos] = kmalloc(Q * sizeof(char), GFP_KERNEL);
        if (!dev_data->data[s_pos])
            goto out;
    }

    if (count > Q - q_pos) // pišemo le do konca trenutnega kvanta
        count = Q - q_pos;

    if (copy_from_user( dev_data->data[s_pos] + q_pos, buf, count)) {
        retval = -EFAULT;
        goto out;
    }

    *f_pos += count;
    retval = count;
    if (dev_data->size < *f_pos) 
        dev_data->size = *f_pos;

    out:
    return retval;
}
module_init(char_device_init);
module_exit(char_device_exit);
