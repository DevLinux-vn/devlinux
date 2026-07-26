#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define DEVICE_NAME "reverse"
#define CLASS_NAME "reverse_class"
#define MAX_LEN 256

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pham Anh Quan");
MODULE_DESCRIPTION("A character device driver that reverses input strings");
MODULE_VERSION("1.0");

static int major_number;
static char raw_buf[MAX_LEN];
static char reversed_buf[MAX_LEN];
static size_t data_len = 0;

static struct class *reverse_class = NULL;
static struct device *reverse_device = NULL;
static struct cdev reverse_cdev;

// Hàm tự viết đảo ngược chuỗi trong Kernel
static void reverse_string(const char *src, char *dst, size_t len) {
    size_t i;
    for (i = 0; i < len; i++) {
        dst[i] = src[len - 1 - i];
    }
    dst[len] = '\0';
}

static int dev_open(struct inode *inodep, struct file *filep) {
    pr_info("reverse_chardev: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    pr_info("reverse_chardev: Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    int bytes_to_copy;
    int err;

    if (*offset >= (loff_t)data_len || data_len == 0) {
        return 0; // EOF hoặc chưa ghi dữ liệu
    }

    bytes_to_copy = data_len - *offset;
    if ((size_t)bytes_to_copy > len) {
        bytes_to_copy = len;
    }

    err = copy_to_user(buffer, reversed_buf + *offset, bytes_to_copy);
    if (err) {
        pr_err("reverse_chardev: Failed to copy data to user space\n");
        return -EFAULT;
    }

    *offset += bytes_to_copy;
    return bytes_to_copy;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    size_t copy_len;

    if (len == 0) return 0;

    copy_len = (len < MAX_LEN - 1) ? len : (MAX_LEN - 1);

    if (copy_from_user(raw_buf, buffer, copy_len)) {
        pr_err("reverse_chardev: Failed to copy data from user space\n");
        return -EFAULT;
    }

    // Bỏ qua ký tự xuống dòng '\n' ở cuối nếu người dùng dùng echo
    if (copy_len > 0 && raw_buf[copy_len - 1] == '\n') {
        copy_len--;
    }

    raw_buf[copy_len] = '\0';

    // Đảo ngược chuỗi
    reverse_string(raw_buf, reversed_buf, copy_len);
    
    // Thêm ký tự '\n' vào cuối chuỗi đảo ngược để cat ra đẹp hơn
    reversed_buf[copy_len] = '\n';
    reversed_buf[copy_len + 1] = '\0';
    data_len = copy_len + 1;

    pr_info("reverse_chardev: Original: '%s', Reversed: '%s'\n", raw_buf, reversed_buf);

    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

static int __init reverse_init(void) {
    dev_t dev;
    int ret;

    pr_info("reverse_chardev: Initializing driver\n");

    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("reverse_chardev: Failed to allocate major number\n");
        return ret;
    }
    major_number = MAJOR(dev);

    cdev_init(&reverse_cdev, &fops);
    reverse_cdev.owner = THIS_MODULE;

    ret = cdev_add(&reverse_cdev, dev, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev, 1);
        pr_err("reverse_chardev: Failed to add cdev\n");
        return ret;
    }

    reverse_class = class_create(CLASS_NAME);
    if (IS_ERR(reverse_class)) {
        cdev_del(&reverse_cdev);
        unregister_chrdev_region(dev, 1);
        pr_err("reverse_chardev: Failed to create class\n");
        return PTR_ERR(reverse_class);
    }

    reverse_device = device_create(reverse_class, NULL, dev, NULL, DEVICE_NAME);
    if (IS_ERR(reverse_device)) {
        class_destroy(reverse_class);
        cdev_del(&reverse_cdev);
        unregister_chrdev_region(dev, 1);
        pr_err("reverse_chardev: Failed to create device\n");
        return PTR_ERR(reverse_device);
    }

    pr_info("reverse_chardev: Driver loaded (/dev/%s created)\n", DEVICE_NAME);
    return 0;
}

static void __exit reverse_exit(void) {
    dev_t dev = MKDEV(major_number, 0);

    device_destroy(reverse_class, dev);
    class_destroy(reverse_class);
    cdev_del(&reverse_cdev);
    unregister_chrdev_region(dev, 1);

    pr_info("reverse_chardev: Driver unloaded\n");
}

module_init(reverse_init);
module_exit(reverse_exit);