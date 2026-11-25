// SPDX-License-Identifier: GPL-2.0
/*
 * counter_chardev.c - A shared, nonnegative counter exposed as /dev/counter.
 * Each write is one complete command, optionally followed by one newline.
 * Reads expose the current value; concurrent writes may change it between
 * read calls. Reopen, or write a command on the same fd, to restart reading.
 * API references: upstream Linux 6.1 and 6.4; no board/kernel is specified
 * by this review-only assignment. No hardware-specific API is used.
 */

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/limits.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#define DRIVER_NAME "counter_chardev"
#define DEVICE_NAME "counter"
#define CLASS_NAME "counter_class"
#define DEVICE_COUNT 1
#define COMMAND_SIZE sizeof("reset\n")
#define COUNTER_TEXT_SIZE 16

static dev_t counter_dev;
static struct cdev counter_cdev;
static struct class *counter_class;
static DEFINE_MUTEX(counter_lock);
static int counter_value;
static bool counter_ready;

static int counter_open(struct inode *inode, struct file *file)
{
	int ret;

	ret = mutex_lock_interruptible(&counter_lock);
	if (ret)
		return ret;
	/* cdev_add() publishes callbacks before class/device creation finishes. */
	ret = counter_ready ? nonseekable_open(inode, file) : -ENODEV;
	mutex_unlock(&counter_lock);
	pr_info(DRIVER_NAME ": open: status=%d\n", ret);
	return ret;
}

static int counter_release(struct inode *inode, struct file *file)
{
	pr_info(DRIVER_NAME ": release\n");
	return 0;
}

static ssize_t counter_read(struct file *file, char __user *buf,
			    size_t count, loff_t *ppos)
{
	char text[COUNTER_TEXT_SIZE];
	ssize_t ret;
	int len;

	if (!count)
		return 0;
	ret = mutex_lock_interruptible(&counter_lock);
	if (ret)
		return ret;
	len = scnprintf(text, sizeof(text), "%d\n", counter_value);
	/* Handles negative offsets, short copies, partial reads and EOF. */
	ret = simple_read_from_buffer(buf, count, ppos, text, len);
	mutex_unlock(&counter_lock);
	return ret;
}

static ssize_t counter_write(struct file *file, const char __user *buf,
			     size_t count, loff_t *ppos)
{
	char command[COMMAND_SIZE];
	size_t len = count;
	ssize_t ret;

	if (!count)
		return 0;
	if (count >= sizeof(command)) {
		pr_err_ratelimited(DRIVER_NAME ": command too long\n");
		return -EINVAL;
	}
	/* Do not change shared state if even part of the copy fails. */
	if (copy_from_user(command, buf, count))
		return -EFAULT;
	if (memchr(command, '\0', count)) {
		pr_err_ratelimited(DRIVER_NAME ": embedded NUL in command\n");
		return -EINVAL;
	}
	if (command[len - 1] == '\n')
		len--;
	command[len] = '\0';

	ret = mutex_lock_interruptible(&counter_lock);
	if (ret)
		return ret;
	if (!strcasecmp(command, "inc")) {
		if (counter_value == INT_MAX) {
			pr_err_ratelimited(DRIVER_NAME ": counter overflow\n");
			ret = -EOVERFLOW;
			goto out_unlock;
		}
		counter_value++;
	} else if (!strcasecmp(command, "dec")) {
		if (counter_value > 0)
			counter_value--;
	} else if (!strcasecmp(command, "reset")) {
		counter_value = 0;
	} else {
		pr_err_ratelimited(DRIVER_NAME ": invalid command\n");
		ret = -EINVAL;
		goto out_unlock;
	}
	*ppos = 0;
	pr_info(DRIVER_NAME ": counter=%d\n", counter_value);
	ret = count;

out_unlock:
	mutex_unlock(&counter_lock);
	return ret;
}

static const struct file_operations counter_fops = {
	.owner = THIS_MODULE,
	.open = counter_open,
	.release = counter_release,
	.read = counter_read,
	.write = counter_write,
};

static int __init counter_init(void)
{
	struct device *device;
	int ret;

	ret = alloc_chrdev_region(&counter_dev, 0, DEVICE_COUNT, DRIVER_NAME);
	if (ret) {
		pr_err(DRIVER_NAME ": alloc_chrdev_region failed: %d\n", ret);
		return ret;
	}
	cdev_init(&counter_cdev, &counter_fops);
	counter_cdev.owner = THIS_MODULE;
	ret = cdev_add(&counter_cdev, counter_dev, DEVICE_COUNT);
	if (ret)
		goto err_region;

	/* Upstream 6.4 removed the module argument from class_create(). */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
	counter_class = class_create(CLASS_NAME);
#else
	counter_class = class_create(THIS_MODULE, CLASS_NAME);
#endif
	if (IS_ERR(counter_class)) {
		ret = PTR_ERR(counter_class);
		goto err_cdev;
	}
	device = device_create(counter_class, NULL, counter_dev, NULL,
			       DEVICE_NAME);
	if (IS_ERR(device)) {
		ret = PTR_ERR(device);
		goto err_class;
	}
	mutex_lock(&counter_lock);
	counter_ready = true;
	mutex_unlock(&counter_lock);
	pr_info(DRIVER_NAME ": registered /dev/" DEVICE_NAME "\n");
	return 0;

err_class:
	class_destroy(counter_class);
err_cdev:
	cdev_del(&counter_cdev);
err_region:
	unregister_chrdev_region(counter_dev, DEVICE_COUNT);
	pr_err(DRIVER_NAME ": initialization failed: %d\n", ret);
	return ret;
}

/* Kernel module_exit callbacks must return void. */
static void __exit counter_exit(void)
{
	/* .owner pins the module while any open file/operation still uses it.
	 * Normal unload therefore cannot race with a successful open or I/O;
	 * forced module removal is unsupported.
	 */
	mutex_lock(&counter_lock);
	counter_ready = false;
	mutex_unlock(&counter_lock);
	device_destroy(counter_class, counter_dev);
	class_destroy(counter_class);
	cdev_del(&counter_cdev);
	unregister_chrdev_region(counter_dev, DEVICE_COUNT);
	pr_info(DRIVER_NAME ": unregistered\n");
}

module_init(counter_init);
module_exit(counter_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tran Danh Quang");
MODULE_DESCRIPTION("Mutex-protected character-device counter");
