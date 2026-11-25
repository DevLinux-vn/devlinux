// SPDX-License-Identifier: GPL-2.0
/*
 * reverse_chardev.c - Store up to 255 bytes and expose their reverse order.
 * Writes replace the shared value atomically; reads use the latest value
 * on each call. Newlines and embedded NUL bytes are data, not delimiters.
 * API references: upstream Linux 6.1 and 6.4, not a specified board SDK.
 * This is a review-only submission; target build/runtime remain unverified.
 */

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#define DRIVER_NAME "reverse_chardev"
#define DEVICE_NAME "reverse"
#define CLASS_NAME "reverse_class"
#define DEVICE_COUNT 1
#define MAX_LEN 256

static dev_t reverse_dev;
static struct cdev reverse_cdev;
static struct class *reverse_class;
static DEFINE_MUTEX(reverse_lock);
static char raw_buf[MAX_LEN];
static char reversed_buf[MAX_LEN];
static size_t data_len;
static bool reverse_ready;

static int reverse_open(struct inode *inode, struct file *file)
{
	int ret;

	ret = mutex_lock_interruptible(&reverse_lock);
	if (ret)
		return ret;
	/* Reject opens while a later initialization step can still fail. */
	ret = reverse_ready ? nonseekable_open(inode, file) : -ENODEV;
	mutex_unlock(&reverse_lock);
	pr_info(DRIVER_NAME ": open: status=%d\n", ret);
	return ret;
}

static int reverse_release(struct inode *inode, struct file *file)
{
	pr_info(DRIVER_NAME ": release\n");
	return 0;
}

static ssize_t reverse_read(struct file *file, char __user *buf,
			    size_t count, loff_t *ppos)
{
	ssize_t ret;

	if (!count)
		return 0;
	ret = mutex_lock_interruptible(&reverse_lock);
	if (ret)
		return ret;
	/* data_len starts at zero: reading before the first write yields EOF.
	 * A sleepable mutex keeps the buffer stable even if usercopy faults.
	 */
	ret = simple_read_from_buffer(buf, count, ppos, reversed_buf, data_len);
	mutex_unlock(&reverse_lock);
	return ret;
}

static ssize_t reverse_write(struct file *file, const char __user *buf,
			     size_t count, loff_t *ppos)
{
	char input[MAX_LEN];
	size_t i;
	int ret;

	if (!count)
		return 0;
	/* Reject oversized writes rather than silently truncating the value. */
	if (count >= MAX_LEN)
		return -EMSGSIZE;
	if (copy_from_user(input, buf, count))
		return -EFAULT;
	input[count] = '\0';

	ret = mutex_lock_interruptible(&reverse_lock);
	if (ret)
		return ret;
	memcpy(raw_buf, input, count + 1);
	for (i = 0; i < count; i++)
		reversed_buf[i] = raw_buf[count - 1 - i];
	reversed_buf[count] = '\0';
	data_len = count;
	/* Allow read-after-write on this fd, even after it previously hit EOF. */
	*ppos = 0;
	/* Escaped, length-bounded output handles arbitrary user bytes safely. */
	pr_debug(DRIVER_NAME ": input=%*pE reversed=%*pE\n",
		 (int)count, raw_buf, (int)count, reversed_buf);
	mutex_unlock(&reverse_lock);
	return count;
}

static const struct file_operations reverse_fops = {
	.owner = THIS_MODULE,
	.open = reverse_open,
	.release = reverse_release,
	.read = reverse_read,
	.write = reverse_write,
};

static int __init reverse_init(void)
{
	struct device *device;
	int ret;

	ret = alloc_chrdev_region(&reverse_dev, 0, DEVICE_COUNT, DRIVER_NAME);
	if (ret) {
		pr_err(DRIVER_NAME ": alloc_chrdev_region failed: %d\n", ret);
		return ret;
	}
	cdev_init(&reverse_cdev, &reverse_fops);
	reverse_cdev.owner = THIS_MODULE;
	ret = cdev_add(&reverse_cdev, reverse_dev, DEVICE_COUNT);
	if (ret)
		goto err_region;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
	reverse_class = class_create(CLASS_NAME);
#else
	reverse_class = class_create(THIS_MODULE, CLASS_NAME);
#endif
	if (IS_ERR(reverse_class)) {
		ret = PTR_ERR(reverse_class);
		goto err_cdev;
	}
	device = device_create(reverse_class, NULL, reverse_dev, NULL,
			       DEVICE_NAME);
	if (IS_ERR(device)) {
		ret = PTR_ERR(device);
		goto err_class;
	}
	mutex_lock(&reverse_lock);
	reverse_ready = true;
	mutex_unlock(&reverse_lock);
	pr_info(DRIVER_NAME ": registered /dev/" DEVICE_NAME "\n");
	return 0;

err_class:
	class_destroy(reverse_class);
err_cdev:
	cdev_del(&reverse_cdev);
err_region:
	unregister_chrdev_region(reverse_dev, DEVICE_COUNT);
	pr_err(DRIVER_NAME ": initialization failed: %d\n", ret);
	return ret;
}

/* Required kernel callback signature; normal unload waits for module users. */
static void __exit reverse_exit(void)
{
	mutex_lock(&reverse_lock);
	reverse_ready = false;
	mutex_unlock(&reverse_lock);
	device_destroy(reverse_class, reverse_dev);
	class_destroy(reverse_class);
	cdev_del(&reverse_cdev);
	unregister_chrdev_region(reverse_dev, DEVICE_COUNT);
	pr_info(DRIVER_NAME ": unregistered\n");
}

module_init(reverse_init);
module_exit(reverse_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tran Danh Quang");
MODULE_DESCRIPTION("Mutex-protected byte-reversing character device");
