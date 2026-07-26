#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define DEVICE_NAME "counter"
#define CLASS_NAME "counter_class"
#define BUF_SIZE 64

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pham Anh Quan");
MODULE_DESCRIPTION("A simple counter character device driver");
MODULE_VERSION("1.0");

static int major_number;
static static_int counter_value = 0;
static struct class *counter_class = NULL;
static struct device *counter_device = NULL;
static struct cdev counter_cdev;

static int dev_open(struct inode *inodep, struct file *filep) {
    pr_info("counter_chardev: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    pr_info("counter_chardev: Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    char kbuf[BUF_SIZE];
    int bytes_to_copy;
    int err;

    if (*offset > 0) {
        return 0; // EOF
    }

    bytes_to_copy = snprintf(kbuf, sizeof(kbuf), "%d\n", counter_value);

    err = copy_to_user(buffer, kbuf, bytes_to_copy);
    if (err) {
        pr_err("counter_chardev: Failed to send data to user space\n");
        return -EFAULT;
    }

    *offset += bytes_to_copy;
    return bytes_to_copy;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    char kbuf[BUF_SIZE];
    size_t copy_len;

    if (len == 0) return 0;

    copy_len = (len < sizeof(kbuf) - 1) ? len : (sizeof(kbuf) - 1);

    if (copy_from_user(kbuf, buffer, copy_len)) {
        pr_err("counter_chardev: Failed to copy data from user space\n");
        return -EFAULT;
    }

    kbuf[copy_len] = '\0';

    // Bỏ qua ký tự xuống dòng nếu có
    if (copy_len > 0 && kbuf[copy_len - 1] == '\n') {
        kbuf[copy_len - 1] = '\0';
    }

    // Xử lý các lệnh inc, dec, reset (không phân biệt hoa thường / hỗ trợ lệnh chuẩn)
    if (strncasecmp(kbuf, "inc", 3) == 0) {
        counter_value++;
        pr_info("counter_chardev: Executed 'inc'. New value = %d\n", counter_value);
    } else if (strncasecmp(kbuf, "dec", 3) == 0) {
        if (counter_value > 0) {
            counter_value--;
        }
        pr_info("counter_chardev: Executed 'dec'. New value = %d\n", counter_value);
    } else if (strncasecmp(kbuf, "reset", 5) == 0) {
        counter_value = 0;
        pr_info("counter_chardev: Executed 'reset'. New value = %d\n", counter_value);
    } else {
        pr_err("counter_chardev: Invalid command '%s'\n", kbuf);
        return -EINVAL;
    }

    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

static int __init counter_init(void) {
    dev_t dev;
    int ret;

    pr_info("counter_chardev: Initializing driver\n");

    // 1. Dynamic allocation of major/minor numbers
    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("counter_chardev: Failed to allocate major number\n");
        return ret;
    }
    major_number = MAJOR(dev);

    // 2. Initialize cdev struct
    cdev_init(&counter_cdev, &fops);
    counter_cdev.owner = THIS_MODULE;

    // 3. Add cdev to Kernel
    ret = cdev_add(&counter_cdev, dev, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev, 1);
        pr_err("counter_chardev: Failed to add cdev\n");
        return ret;
    }

    // 4. Create class
    counter_class = class_create(CLASS_NAME);
    if (IS_ERR(counter_class)) {
        cdev_del(&counter_cdev);
        unregister_chrdev_region(dev, 1);
        pr_err("counter_chardev: Failed to create class\n");
        return PTR_ERR(counter_class);
    }

    // 5. Create device node /dev/counter
    counter_device = device_create(counter_class, NULL, dev, NULL, DEVICE_NAME);
    if (IS_ERR(counter_device)) {
        class_destroy(counter_class);
        cdev_del(&counter_cdev);
        unregister_chrdev_region(dev, 1);
        pr_err("counter_chardev: Failed to create device\n");
        return PTR_ERR(counter_device);
    }

    pr_info("counter_chardev: Driver loaded successfully (/dev/%s created)\n", DEVICE_NAME);
    return 0;
}

static void __exit counter_exit(void) {
    dev_t dev = MKDEV(major_number, 0);

    // Dọn dẹp theo thứ tự ngược lại
    device_destroy(counter_class, dev);
    class_destroy(counter_class);
    cdev_del(&counter_cdev);
    unregister_chrdev_region(dev, 1);

    pr_info("counter_chardev: Driver unloaded\n");
}

module_init(counter_init);
module_exit(counter_exit);