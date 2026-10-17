#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>



/* ------------------Metadata ------------------ */
#define DRIVER_NAME  "counter_chardev"
#define CLASS_NAME   "counter_class"
#define DEVICE_NAME  "counter"
#define CMD_BUF_SIZE 16

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TamPD");
MODULE_DESCRIPTION("Character device driver for a kernel-space counter");
MODULE_VERSION("1.0");


/* ------------------Global variables ------------------ */
static dev_t          dev_num;        // major:minor do kernel cấp
static struct cdev    counter_cdev;   // đối tượng character device
static struct class  *counter_class;  // class hiện trong /sys/class/
static struct device *counter_device; // device node /dev/counter


static int counter_value = 0;           // giá trị counter trong kernel
static char counter_buf[CMD_BUF_SIZE];  // buffer nội bộ kernel để lưu dữ liệu user ghi vào


/* ------------------Prototype ------------------ */
static int     counter_open   (struct inode *, struct file *);
static int     counter_release(struct inode *, struct file *);
static ssize_t counter_read   (struct file *, char __user *, size_t, loff_t *);
static ssize_t counter_write  (struct file *, const char __user *, size_t, loff_t *);

/* ------------------file_operations – "bảng hàm" của character device ------------------ */
static const struct file_operations counter_fops = {
    .owner   = THIS_MODULE,
    .open    = counter_open,
    .release = counter_release,
    .read    = counter_read,
    .write   = counter_write,
};

/* ============================================================
 * open() – gọi khi user-space mở /dev/counter
 * ============================================================ */
static int counter_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "[counter_chardev] open() called\n");
    return 0;   /* 0 = thành công */
}   

/* ============================================================
 * release() – gọi khi user-space đóng file descriptor
 * ============================================================ */
static int counter_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "[counter_chardev] release() called\n");
    return 0;
}

/* ============================================================
 * read() – user-space đọc từ /dev/counter
 *   → kernel gửi giá trị counter_value về user-space
 * ============================================================ */
static ssize_t counter_read(struct file *file, char __user *ubuf,
                            size_t count, loff_t *ppos)
{
    size_t len;
    size_t to_copy;

    len = snprintf(counter_buf, sizeof(counter_buf), "%d\n", counter_value);

    if (*ppos >= len)
        return 0;

    to_copy = count;
    if (to_copy > len - (size_t)*ppos)
        to_copy = len - (size_t)*ppos;

    if (copy_to_user(ubuf, counter_buf + *ppos, to_copy)) {
        printk(KERN_ERR "[counter_chardev] copy_to_user failed\n");
        return -EFAULT;
    }

    *ppos += to_copy;
    return to_copy;
}

/* ============================================================
 * write() – user-space ghi vào /dev/counter
 *   → kernel nhận dữ liệu vào counter_buf
 * ============================================================ */
static ssize_t counter_write(struct file *file, const char __user *ubuf,
                             size_t count, loff_t *ppos)
{
    char cmd_buf[CMD_BUF_SIZE];
    size_t len;
    int i;

    len = count;
    if (len > sizeof(cmd_buf) - 1)
        len = sizeof(cmd_buf) - 1;

    if (copy_from_user(cmd_buf, ubuf, len)) {
        printk(KERN_ERR "[counter_chardev] copy_from_user failed\n");
        return -EFAULT;
    }
    cmd_buf[len] = '\0';

    while (len > 0 && (cmd_buf[len - 1] == '\n' || cmd_buf[len - 1] == '\r')) {
        cmd_buf[--len] = '\0';
    }

    for (i = 0; i < len; ++i) {
        if (cmd_buf[i] >= 'A' && cmd_buf[i] <= 'Z')
            cmd_buf[i] += 32;
    }

    if (strncmp(cmd_buf, "inc", 3) == 0) {
        counter_value++;
    } else if (strncmp(cmd_buf, "dec", 3) == 0) {
        if (counter_value > 0)
            counter_value--;
    } else if (strncmp(cmd_buf, "reset", 5) == 0) {
        counter_value = 0;
    } else {
        printk(KERN_ERR "[counter_chardev] invalid command: %s\n", cmd_buf);
        return -EINVAL;
    }

    printk(KERN_INFO "[counter_chardev] counter_value = %d\n", counter_value);
    *ppos = 0;
    return len;
}

/* ============================================================
 * init() – hàm khởi tạo, chạy khi insmod
 * ============================================================ */
 static int __init counter_init(void)
 {
       int ret;

    printk(KERN_INFO "[counter_chardev] initializing\n");

    ret = alloc_chrdev_region(&dev_num, 0, 1, DRIVER_NAME);
    if (ret < 0) {
        printk(KERN_ERR "[counter_chardev] alloc_chrdev_region failed: %d\n", ret);
        return ret;
    }

    cdev_init(&counter_cdev, &counter_fops);
    counter_cdev.owner = THIS_MODULE;

    ret = cdev_add(&counter_cdev, dev_num, 1);
    if (ret < 0) {
        printk(KERN_ERR "[counter_chardev] cdev_add failed: %d\n", ret);
        goto err_cdev;
    }

    counter_class = class_create(CLASS_NAME);
    if (IS_ERR(counter_class)) {
        ret = PTR_ERR(counter_class);
        printk(KERN_ERR "[counter_chardev] class_create failed: %d\n", ret);
        goto err_class;
    }

    counter_device = device_create(counter_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(counter_device)) {
        ret = PTR_ERR(counter_device);
        printk(KERN_ERR "[counter_chardev] device_create failed: %d\n", ret);
        goto err_device;
    }

    printk(KERN_INFO "[counter_chardev] initialized successfully -> /dev/%s\n", DEVICE_NAME);
    return 0;

err_device:
    class_destroy(counter_class);
err_class:
    cdev_del(&counter_cdev);
err_cdev:
    unregister_chrdev_region(dev_num, 1);
    return ret;
 }

/* ============================================================
 * exit() – hàm cleanup, chạy khi rmmod
 * ============================================================ */
static void __exit counter_exit(void)
{
    device_destroy(counter_class, dev_num);
    class_destroy(counter_class);
    cdev_del(&counter_cdev);
    unregister_chrdev_region(dev_num, 1);

    printk(KERN_INFO "[counter_chardev] removed\n");
}
module_init(counter_init);
module_exit(counter_exit);