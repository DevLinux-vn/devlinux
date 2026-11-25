// SPDX-License-Identifier: GPL-2.0
/*
 * GPIO LED platform driver, API baseline: upstream Linux 6.1.
 * Commands: one '0'/'1', optionally followed by LF for echo(1).
 * Read: current GPIO logical value plus LF, with partial reads and EOF.
 */

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DRIVER_NAME "led_blink"
#define CLASS_NAME "led_blink_class"
#define LED_COMPATIBLE "tran-danh-quang,led-blink"
#define DEVICE_COUNT 1

struct led_device {
	struct device device;
	struct cdev cdev;
	struct class *class;
	struct gpio_desc *gpio;
	struct mutex lock;
	bool ready;
};

/* Device-core release callbacks must return void. */
static void led_device_release(struct device *device)
{
	struct led_device *led = container_of(device, struct led_device, device);

	kfree(led);
}

static int led_open(struct inode *inode, struct file *file)
{
	struct led_device *led = container_of(inode->i_cdev,
					    struct led_device, cdev);
	int ret;

	/* cdev's parent reference keeps led alive even while open is entering. */
	ret = mutex_lock_interruptible(&led->lock);
	if (ret)
		return ret;
	if (!led->ready) {
		ret = -ENODEV;
		goto out_unlock;
	}
	ret = nonseekable_open(inode, file);
	if (ret)
		goto out_unlock;
	if (!get_device(&led->device)) {
		ret = -ENODEV;
		goto out_unlock;
	}
	file->private_data = led;
out_unlock:
	mutex_unlock(&led->lock);
	return ret;
}

static int led_release(struct inode *inode, struct file *file)
{
	struct led_device *led = file->private_data;

	put_device(&led->device);
	return 0;
}

static ssize_t led_read(struct file *file, char __user *buf,
			size_t count, loff_t *ppos)
{
	struct led_device *led = file->private_data;
	char text[2];
	ssize_t ret;
	int value;

	if (!count)
		return 0;
	ret = mutex_lock_interruptible(&led->lock);
	if (ret)
		return ret;
	if (!led->ready) {
		ret = -ENODEV;
		goto out_unlock;
	}
	value = gpiod_get_value(led->gpio);
	if (value < 0) {
		dev_err(&led->device, "GPIO read failed: %d\n", value);
		ret = value;
		goto out_unlock;
	}
	text[0] = value ? '1' : '0';
	text[1] = '\n';
	/* A snapshot may finish copying after unbind. Do not make removal wait
	 * for a userspace page fault; only GPIO access needs the device lock.
	 */
	mutex_unlock(&led->lock);
	return simple_read_from_buffer(buf, count, ppos, text, sizeof(text));
out_unlock:
	mutex_unlock(&led->lock);
	return ret;
}

static ssize_t led_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *ppos)
{
	struct led_device *led = file->private_data;
	char command[2];
	ssize_t ret;

	if (!count)
		return 0;
	if (count > sizeof(command))
		return -EINVAL;
	if (copy_from_user(command, buf, count))
		return -EFAULT;
	if ((command[0] != '0' && command[0] != '1') ||
	    (count == sizeof(command) && command[1] != '\n'))
		return -EINVAL;
	ret = mutex_lock_interruptible(&led->lock);
	if (ret)
		return ret;
	if (!led->ready) {
		ret = -ENODEV;
		goto out_unlock;
	}
	gpiod_set_value(led->gpio, command[0] == '1');
	*ppos = 0;
	ret = count;
out_unlock:
	mutex_unlock(&led->lock);
	return ret;
}

static const struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.open = led_open,
	.release = led_release,
	.read = led_read,
	.write = led_write,
};

static int led_probe(struct platform_device *pdev)
{
	struct gpio_desc *gpio;
	struct led_device *led;
	dev_t number;
	int ret;

	/* NULL con_id matches the unprefixed "gpios" property in the overlay. */
	gpio = devm_gpiod_get(&pdev->dev, NULL, GPIOD_OUT_LOW);
	if (IS_ERR(gpio))
		return dev_err_probe(&pdev->dev, PTR_ERR(gpio), "GPIO acquisition failed\n");
	/* The exercise requires gpiod_get_value(), so reject sleeping expanders. */
	if (gpiod_cansleep(gpio))
		return dev_err_probe(&pdev->dev, -EOPNOTSUPP,
				     "a non-sleeping GPIO controller is required\n");
	dev_info(&pdev->dev, "GPIO acquired, LED initially off\n");

	/* Not devm memory: software state must outlive unbind and old open FDs. */
	led = kzalloc(sizeof(*led), GFP_KERNEL);
	if (!led)
		return dev_err_probe(&pdev->dev, -ENOMEM, "state allocation failed\n");
	mutex_init(&led->lock);
	led->gpio = gpio;
	device_initialize(&led->device);
	led->device.release = led_device_release;
	led->device.parent = &pdev->dev;
	ret = dev_set_name(&led->device, DRIVER_NAME);
	if (ret)
		goto err_device;
	ret = alloc_chrdev_region(&number, 0, DEVICE_COUNT, DRIVER_NAME);
	if (ret)
		goto err_device;
	led->device.devt = number;
	cdev_init(&led->cdev, &led_fops);
	led->cdev.owner = THIS_MODULE;
	/* Pins the containing device for pending opens and cdev references. */
	cdev_set_parent(&led->cdev, &led->device.kobj);
	ret = cdev_add(&led->cdev, number, DEVICE_COUNT);
	if (ret)
		goto err_region;
	led->class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(led->class)) {
		ret = PTR_ERR(led->class);
		goto err_cdev;
	}
	led->device.class = led->class;
	/* device_add exports this refcounted object, including its /dev node. */
	ret = device_add(&led->device);
	if (ret)
		goto err_class;
	platform_set_drvdata(pdev, led);
	mutex_lock(&led->lock);
	led->ready = true;
	mutex_unlock(&led->lock);
	dev_info(&pdev->dev, "created /dev/" DRIVER_NAME "\n");
	return 0;

err_class:
	class_destroy(led->class);
err_cdev:
	cdev_del(&led->cdev);
err_region:
	unregister_chrdev_region(number, DEVICE_COUNT);
err_device:
	put_device(&led->device);
	return dev_err_probe(&pdev->dev, ret, "character device setup failed\n");
}

static int led_remove(struct platform_device *pdev)
{
	struct led_device *led = platform_get_drvdata(pdev);
	dev_t number = led->device.devt;

	/* Drain the current hardware operation before devm releases the GPIO.
	 * Old FDs retain software state but all later I/O returns -ENODEV.
	 */
	mutex_lock(&led->lock);
	led->ready = false;
	gpiod_set_value(led->gpio, 0);
	led->gpio = NULL;
	mutex_unlock(&led->lock);
	device_del(&led->device);
	class_destroy(led->class);
	cdev_del(&led->cdev);
	unregister_chrdev_region(number, DEVICE_COUNT);
	platform_set_drvdata(pdev, NULL);
	put_device(&led->device);
	dev_info(&pdev->dev, "removed device, LED off\n");
	return 0;
}

static const struct of_device_id led_of_match[] = {
	{ .compatible = LED_COMPATIBLE },
	{ }
};
MODULE_DEVICE_TABLE(of, led_of_match);

static struct platform_driver led_driver = {
	.probe = led_probe,
	.remove = led_remove,
	.driver = {
		.name = DRIVER_NAME,
		.of_match_table = led_of_match,
	},
};
module_platform_driver(led_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tran Danh Quang");
MODULE_DESCRIPTION("Descriptor-based GPIO LED with a character interface");
