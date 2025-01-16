#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

// sudo make -C /usr/src/linux-headers-$(uname -r) M=$(pwd) modules
// sudo insmod ./gonilnik.ko
// cat /proc/devices | grep gonilnik
// sudo mknod -m 666 /dev/gonilnik c MAJOR MINOR

#define DEVICE_NAME "qread"
#define QUANTUM_SIZE 10
#define QSET_SIZE 5

MODULE_LICENSE("GPL");

struct my_dev {
    void **data;
    unsigned Q;
    unsigned S;
    unsigned size;
};

ssize_t read(struct file *file, char __user *buf, size_t count, loff_t *f_pos);
ssize_t write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos);
int release(struct inode *inode, struct file *file);
int open(struct inode *inode, struct file *file);
int init(void);
void cleanup(void);
int trim(struct my_dev *dev);


struct file_operations fops = {
    .read = read,
    .write = write,
    .release = release,
    .open = open
};

int major_number;
int minor_number;
struct my_dev *dev_ptr;

int init(void){
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register a major number\n");
        return major_number;
    }
    minor_number = 0;
    printk(KERN_ALERT "Registered char device with major number %d\n", major_number);

    dev_ptr = kmalloc(sizeof(struct my_dev), GFP_KERNEL);
    
    if(!dev_ptr){
        printk(KERN_ALERT "Failed to allocate memory for device\n");
        return -ENOMEM;
    }

    dev_ptr->Q = QUANTUM_SIZE;
    dev_ptr->S = QSET_SIZE;

    return 0;
}

void cleanup(void){
    if(dev_ptr){
        trim(dev_ptr);
        kfree(dev_ptr);
    }
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Char device unregistered\n");
}

int trim(struct my_dev *dev){
    int i;
    if(dev->data){
        for(i = 0; i < dev->Q; i++){
            if(dev->data[i]){
                kfree(dev->data[i]);
            }
        }
        kfree(dev->data);
        dev->data = NULL;
    }
    dev->size = 0;
    return 0;
}

int open(struct inode *inode, struct file *file){
    file->private_data = dev_ptr;
    if(file->f_flags & O_ACCMODE == O_WRONLY){
        trim(dev_ptr);
    }
    return 0;
}

int release(struct inode *inode, struct file *file){
    return 0;
}

ssize_t read(struct file *file, char __user *buf, size_t count, loff_t *f_pos){
    struct my_dev *dev = file->private_data;
    int Q = dev->Q;
    int S = dev->S;
    int s_pos, q_pos;
    ssize_t return_value = 0;

    printk(KERN_INFO "Read called\n");

    if(*f_pos >= dev->size){
        return 0;
    }

    if(count > dev->size - *f_pos){
        count = dev->size - *f_pos;
    }

    s_pos = (long)*f_pos / Q;
    q_pos = (long)*f_pos % Q;

    if(count > Q - q_pos){
        count = Q - q_pos;
    }

    if(copy_to_user(buf, dev->data[s_pos] + q_pos, count)){
        return -EFAULT;
    }
    *f_pos += count;
    return_value = count;
    printk(KERN_INFO "Read %d bytes\n", count);
    return return_value;
}

ssize_t write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos){
    struct my_dev *dev = file->private_data;
    int Q = dev->Q;
    int S = dev->S;
    int s_pos, q_pos;
    ssize_t return_value = -ENOMEM;
    
    printk(KERN_INFO "Write called\n");

    s_pos = (long)*f_pos / Q;
    q_pos = (long)*f_pos % Q;

    if(s_pos >= S){
        return -ENOMEM;
    }

    if(!dev->data){
        dev->data = kmalloc(S * sizeof(char *), GFP_KERNEL);
        if(!dev->data){
            goto out;
        }
        memset(dev->data, 0, S * sizeof(char *));
    }

    if(!dev->data[s_pos]){
        dev->data[s_pos] = kmalloc(Q, GFP_KERNEL);
        if(!dev->data[s_pos]){
            goto out;
        }
    }

    if(count > Q - q_pos){
        count = Q - q_pos;
    }

    if(copy_from_user(dev->data[s_pos] + q_pos, buf, count)){
        return -EFAULT;
    }

    *f_pos += count;
    return_value = count;
    if(dev->size < *f_pos){
        dev->size = *f_pos;
    }
    printk(KERN_INFO "Wrote %d bytes\n", count);
    out:
    return return_value;
}