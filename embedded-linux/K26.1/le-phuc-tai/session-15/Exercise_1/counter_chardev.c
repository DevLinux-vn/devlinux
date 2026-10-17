/**
 * @file counter_chardev.c
 * @brief Character device driver managing a thread-safe kernel space counter.
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

#define DEVICE_NAME       "counter"
#define CLASS_NAME        "counter_class"
#define CMD_BUF_LEN       16
#define RESP_BUF_LEN      16
#define MIN_COUNTER_VAL   0
#define BASE_DECIMAL      10

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Embedded Senior Engineer");
MODULE_DESCRIPTION("Thread-safe Counter Character Device Driver");
MODULE_VERSION("1.0");

static dev_t dev_num;
static struct cdev counter_cdev;
static struct class *counter_class = NULL;
static struct device *counter_device = NULL;

static int counter_value = MIN_COUNTER_VAL;
static DEFINE_MUTEX(counter_lock);

static int dev_open(struct inode *inodep, struct file *filep)
{
	pr_info("[%s] Device opened successfully\n", DEVICE_NAME);
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	pr_info("[%s] Device closed successfully\n", DEVICE_NAME);
	return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset)
{
	char resp_str[RESP_BUF_LEN];
	int resp_len;
	int current_val;

	/* End of File (EOF) check if user already read the content */
	if (*offset > 0) {
		return 0;
	}

	mutex_lock(&counter_lock);
	current_val = counter_value;
	mutex_unlock(&counter_lock);

	resp_len = snprintf(resp_str, sizeof(resp_str), "%d\n", current_val);
	if (resp_len < 0) {
		return -EFAULT;
	}

	if (len < (size_t)resp_len) {
		return -EINVAL;
	}

	if (copy_to_user(buffer, resp_str, resp_len) != 0) {
		pr_err("[%s] Failed to copy counter value to user space\n", DEVICE_NAME);
		return -EFAULT;
	}

	*offset += resp_len;
	return resp_len;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset)
{
	char kbuf[CMD_BUF_LEN];
	size_t copy_len;

	if (len == 0) {
		return 0;
	}

	copy_len = (len < (sizeof(kbuf) - 1)) ? len : (sizeof(kbuf) - 1);
	if (copy_from_user(kbuf, buffer, copy_len) != 0) {
		pr_err("[%s] Failed to copy command from user space\n", DEVICE_NAME);
		return -EFAULT;
	}
	kbuf[copy_len] = '\0';

	/* Strip trailing newline and carriage return characters */
	while (copy_len > 0 && (kbuf[copy_len - 1] == '\n' || kbuf[copy_len - 1] == '\r' || kbuf[copy_len - 1] == ' ')) {
		kbuf[--copy_len] = '\0';
	}

	mutex_lock(&counter_lock);

	if (strncasecmp(kbuf, "inc", 3) == 0 && kbuf[3] == '\0') {
		counter_value++;
		pr_info("[%s] Command INC executed. New Value: %d\n", DEVICE_NAME, counter_value);
	} else if (strncasecmp(kbuf, "dec", 3) == 0 && kbuf[3] == '\0') {
		if (counter_value > MIN_COUNTER_VAL) {
			counter_value--;
			pr_info("[%s] Command DEC executed. New Value: %d\n", DEVICE_NAME, counter_value);
		} else {
			pr_warn("[%s] Command DEC ignored. Counter already at minimum (%d)\n", DEVICE_NAME, MIN_COUNTER_VAL);
		}
	} else if (strncasecmp(kbuf, "reset", 5) == 0 && kbuf[5] == '\0') {
		counter_value = MIN_COUNTER_VAL;
		pr_info("[%s] Command RESET executed. Value reset to %d\n", DEVICE_NAME, MIN_COUNTER_VAL);
	} else {
		mutex_unlock(&counter_lock);
		pr_err("[%s] Invalid command received: '%s'\n", DEVICE_NAME, kbuf);
		return -EINVAL;
	}

	mutex_unlock(&counter_lock);

	*offset = 0; /* Reset position offset after write to allow immediate read */
	return len;
}

static struct file_operations fops = {
	.owner   = THIS_MODULE,
	.open    = dev_open,
	.release = dev_release,
	.read    = dev_read,
	.write   = dev_write,
};

static int __init counter_init(void)
{
	int ret;

	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if (ret < 0) {
		pr_err("[%s] Failed to allocate major/minor number region\n", DEVICE_NAME);
		return ret;
	}

	cdev_init(&counter_cdev, &fops);
	counter_cdev.owner = THIS_MODULE;

	ret = cdev_add(&counter_cdev, dev_num, 1);
	if (ret < 0) {
		pr_err("[%s] Failed to add cdev structure to kernel\n", DEVICE_NAME);
		goto fail_unregister;
	}

	counter_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(counter_class)) {
		ret = PTR_ERR(counter_class);
		pr_err("[%s] Failed to create sysfs class\n", DEVICE_NAME);
		goto fail_cdev_del;
	}

	counter_device = device_create(counter_class, NULL, dev_num, NULL, DEVICE_NAME);
	if (IS_ERR(counter_device)) {
		ret = PTR_ERR(counter_device);
		pr_err("[%s] Failed to create device node in /dev/\n", DEVICE_NAME);
		goto fail_class_destroy;
	}

	pr_info("[%s] Driver initialized. Major: %d, Minor: %d\n",
	        DEVICE_NAME, MAJOR(dev_num), MINOR(dev_num));
	return 0;

fail_class_destroy:
	class_destroy(counter_class);
fail_cdev_del:
	cdev_del(&counter_cdev);
fail_unregister:
	unregister_chrdev_region(dev_num, 1);
	return ret;
}

static void __exit counter_exit(void)
{
	device_destroy(counter_class, dev_num);
	class_destroy(counter_class);
	cdev_del(&counter_cdev);
	unregister_chrdev_region(dev_num, 1);
	pr_info("[%s] Driver unloaded cleanly\n", DEVICE_NAME);
}

module_init(counter_init);
module_exit(counter_exit);