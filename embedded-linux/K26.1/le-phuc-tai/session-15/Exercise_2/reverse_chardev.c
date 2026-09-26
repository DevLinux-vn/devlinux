/**
 * @file reverse_chardev.c
 * @brief Character device driver returning user-written string in reversed order.
 * @date 2026-07-26
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/string.h>

#define DEVICE_NAME   "reverse"
#define CLASS_NAME    "reverse_class"
#define MAX_BUF_LEN   256

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Embedded Senior Engineer");
MODULE_DESCRIPTION("String Reversal Character Device Driver");
MODULE_VERSION("1.0");

static dev_t dev_num;
static struct cdev reverse_cdev;
static struct class *reverse_class = NULL;
static struct device *reverse_device = NULL;

static char raw_buf[MAX_BUF_LEN];
static char reversed_buf[MAX_BUF_LEN];
static size_t stored_data_len = 0;
static DEFINE_MUTEX(reverse_lock);

static void invert_string_buffer(const char *src, char *dst, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++) {
		dst[i] = src[len - 1 - i];
	}
	dst[len] = '\0';
}

static int dev_open(struct inode *inodep, struct file *filep)
{
	pr_info("[%s] Device node opened\n", DEVICE_NAME);
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	pr_info("[%s] Device node closed\n", DEVICE_NAME);
	return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset)
{
	size_t bytes_to_copy;

	mutex_lock(&reverse_lock);

	/* Return 0 (EOF) if no data written or offset reached end of buffer */
	if (*offset >= stored_data_len || stored_data_len == 0) {
		mutex_unlock(&reverse_lock);
		return 0;
	}

	bytes_to_copy = stored_data_len - *offset;
	if (bytes_to_copy > len) {
		bytes_to_copy = len;
	}

	if (copy_to_user(buffer, reversed_buf + *offset, bytes_to_copy) != 0) {
		mutex_unlock(&reverse_lock);
		pr_err("[%s] Failed copying reversed string to user space\n", DEVICE_NAME);
		return -EFAULT;
	}

	*offset += bytes_to_copy;
	mutex_unlock(&reverse_lock);

	return bytes_to_copy;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset)
{
	size_t copy_len;

	if (len == 0) {
		return 0;
	}

	copy_len = (len < (MAX_BUF_LEN - 1)) ? len : (MAX_BUF_LEN - 1);

	mutex_lock(&reverse_lock);

	memset(raw_buf, 0, sizeof(raw_buf));
	memset(reversed_buf, 0, sizeof(reversed_buf));

	if (copy_from_user(raw_buf, buffer, copy_len) != 0) {
		mutex_unlock(&reverse_lock);
		pr_err("[%s] Failed copying raw string from user space\n", DEVICE_NAME);
		return -EFAULT;
	}
	raw_buf[copy_len] = '\0';

	/* Exclude trailing newline character from string reversal if present */
	if (copy_len > 0 && raw_buf[copy_len - 1] == '\n') {
		raw_buf[copy_len - 1] = '\0';
		copy_len--;
	}

	stored_data_len = copy_len;
	if (stored_data_len > 0) {
		invert_string_buffer(raw_buf, reversed_buf, stored_data_len);
		/* Re-append newline for clean terminal output formatting */
		reversed_buf[stored_data_len] = '\n';
		reversed_buf[stored_data_len + 1] = '\0';
		stored_data_len++; /* Count newline in total length */
	}

	pr_info("[%s] Raw input: '%s' | Reversed output: '%s'\n",
	        DEVICE_NAME, raw_buf, reversed_buf);

	mutex_unlock(&reverse_lock);

	*offset = 0; /* Reset read offset for subsequent reads */
	return len;
}

static struct file_operations fops = {
	.owner   = THIS_MODULE,
	.open    = dev_open,
	.release = dev_release,
	.read    = dev_read,
	.write   = dev_write,
};

static int __init reverse_init(void)
{
	int ret;

	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if (ret < 0) {
		pr_err("[%s] Failed allocating major/minor number region\n", DEVICE_NAME);
		return ret;
	}

	cdev_init(&reverse_cdev, &fops);
	reverse_cdev.owner = THIS_MODULE;

	ret = cdev_add(&reverse_cdev, dev_num, 1);
	if (ret < 0) {
		pr_err("[%s] Failed adding cdev structure\n", DEVICE_NAME);
		goto fail_unregister;
	}

	reverse_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(reverse_class)) {
		ret = PTR_ERR(reverse_class);
		pr_err("[%s] Failed creating sysfs class\n", DEVICE_NAME);
		goto fail_cdev_del;
	}

	reverse_device = device_create(reverse_class, NULL, dev_num, NULL, DEVICE_NAME);
	if (IS_ERR(reverse_device)) {
		ret = PTR_ERR(reverse_device);
		pr_err("[%s] Failed creating device node /dev/%s\n", DEVICE_NAME, DEVICE_NAME);
		goto fail_class_destroy;
	}

	pr_info("[%s] Driver initialized. Major: %d, Minor: %d\n",
	        DEVICE_NAME, MAJOR(dev_num), MINOR(dev_num));
	return 0;

fail_class_destroy:
	class_destroy(reverse_class);
fail_cdev_del:
	cdev_del(&reverse_cdev);
fail_unregister:
	unregister_chrdev_region(dev_num, 1);
	return ret;
}

static void __exit reverse_exit(void)
{
	device_destroy(reverse_class, dev_num);
	class_destroy(reverse_class);
	cdev_del(&reverse_cdev);
	unregister_chrdev_region(dev_num, 1);
	pr_info("[%s] Driver unloaded cleanly\n", DEVICE_NAME);
}

module_init(reverse_init);
module_exit(reverse_exit);