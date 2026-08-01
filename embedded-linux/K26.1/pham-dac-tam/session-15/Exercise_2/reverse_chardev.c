#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/errno.h>

#define DRIVER_NAME "reverse_chardev"
#define CLASS_NAME "reverse_class"
#define DEVICE_NAME "reverse"
#define MAX_LEN 256

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DevLinux");
MODULE_DESCRIPTION("Character device driver that reverses a string");
MODULE_VERSION("1.0");

static dev_t dev_num;
static struct cdev reverse_cdev;
static struct class *reverse_class;
static struct device *reverse_device;

static char raw_buf[MAX_LEN];
static char reversed_buf[MAX_LEN];
static size_t data_len = 0;

static int reverse_open(struct inode *inode, struct file *file);
static int reverse_release(struct inode *inode, struct file *file);
static ssize_t reverse_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos);
static ssize_t reverse_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos);

static const struct file_operations reverse_fops = {
    .owner = THIS_MODULE,
    .open = reverse_open,
    .release = reverse_release,
    .read = reverse_read,
    .write = reverse_write,
};

static int reverse_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "[reverse_chardev] open() called\n");
    return 0;
}

static int reverse_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "[reverse_chardev] release() called\n");
    return 0;
}

static ssize_t reverse_read(struct file *file, char __user *ubuf,
                            size_t count, loff_t *ppos)
{
    size_t to_copy;

    if (*ppos >= data_len)
        return 0;

    to_copy = count;
    if (to_copy > data_len - (size_t)*ppos)
        to_copy = data_len - (size_t)*ppos;

    if (copy_to_user(ubuf, reversed_buf + *ppos, to_copy)) {
        printk(KERN_ERR "[reverse_chardev] copy_to_user failed\n");
        return -EFAULT;
    }

    *ppos += to_copy;
    return to_copy;
}

static ssize_t reverse_write(struct file *file, const char __user *ubuf,
                             size_t count, loff_t *ppos)
{
    size_t len;
    size_t i;

    len = count;
    if (len > MAX_LEN - 1)
        len = MAX_LEN - 1;

    if (copy_from_user(raw_buf, ubuf, len)) {
        printk(KERN_ERR "[reverse_chardev] copy_from_user failed\n");
        return -EFAULT;
    }

    raw_buf[len] = '\0';

    /* Bỏ ký tự '\n' hoặc '\r' do echo gửi */
    while (len > 0 && (raw_buf[len - 1] == '\n' || raw_buf[len - 1] == '\r'))
     raw_buf[--len] = '\0';

    data_len = len;

    /* Đảo chuỗi */
    for (i = 0; i < data_len; ++i)
     reversed_buf[i] = raw_buf[data_len - 1 - i];

    /* Thêm xuống dòng để cat hiển thị đẹp */
    reversed_buf[data_len] = '\n';
    reversed_buf[data_len + 1] = '\0';
    data_len++;
    printk(KERN_INFO "[reverse_chardev] raw='%s' reversed='%s'\n", raw_buf, reversed_buf);
    *ppos = 0;
    return count;
}

static int __init reverse_init(void)
{
    int ret;

    printk(KERN_INFO "[reverse_chardev] initializing\n");

    ret = alloc_chrdev_region(&dev_num, 0, 1, DRIVER_NAME);
    if (ret < 0) {
        printk(KERN_ERR "[reverse_chardev] alloc_chrdev_region failed: %d\n", ret);
        return ret;
    }

    cdev_init(&reverse_cdev, &reverse_fops);
    reverse_cdev.owner = THIS_MODULE;

    ret = cdev_add(&reverse_cdev, dev_num, 1);
    if (ret < 0) {
        printk(KERN_ERR "[reverse_chardev] cdev_add failed: %d\n", ret);
        goto err_cdev;
    }

    reverse_class = class_create(CLASS_NAME);
    if (IS_ERR(reverse_class)) {
        ret = PTR_ERR(reverse_class);
        printk(KERN_ERR "[reverse_chardev] class_create failed: %d\n", ret);
        goto err_class;
    }

    reverse_device = device_create(reverse_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(reverse_device)) {
        ret = PTR_ERR(reverse_device);
        printk(KERN_ERR "[reverse_chardev] device_create failed: %d\n", ret);
        goto err_device;
    }

    printk(KERN_INFO "[reverse_chardev] initialized successfully -> /dev/%s\n", DEVICE_NAME);
    return 0;

err_device:
    class_destroy(reverse_class);
err_class:
    cdev_del(&reverse_cdev);
err_cdev:
    unregister_chrdev_region(dev_num, 1);
    return ret;
}

static void __exit reverse_exit(void)
{
    device_destroy(reverse_class, dev_num);
    class_destroy(reverse_class);
    cdev_del(&reverse_cdev);
    unregister_chrdev_region(dev_num, 1);

    printk(KERN_INFO "[reverse_chardev] removed\n");
}

module_init(reverse_init);
module_exit(reverse_exit);
