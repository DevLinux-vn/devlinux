/**
 * @file main.c
 * @brief Platform driver controlling an LED via GPIO Descriptor API and Device Tree.
 * @date 2026-08-06
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#include <linux/slab.h>

#define DRIVER_NAME     "led_blink_driver"
#define DEVICE_NAME     "led_blink"
#define CLASS_NAME      "led_blink_class"
#define COMPATIBLE_STR  "phuctai,led-blink"

#define LED_CMD_ON      '1'
#define LED_CMD_OFF     '0'
#define MINOR_BASE      0
#define DEV_COUNT       1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Phuc Tai");
MODULE_DESCRIPTION("Platform Driver for LED Control using GPIO Descriptors");
MODULE_VERSION("1.0");

struct led_dev_priv {
	struct gpio_desc *gpiod;
	dev_t dev_num;
	struct cdev cdev;
	struct class *class;
	struct device *device;
};

static int led_open(struct inode *inode, struct file *filep)
{
	struct led_dev_priv *priv;

	priv = container_of(inode->i_cdev, struct led_dev_priv, cdev);
	filep->private_data = priv;

	pr_info("[%s] Device opened successfully\n", DEVICE_NAME);
	return 0;
}

static int led_release(struct inode *inode, struct file *filep)
{
	(void)inode;
	(void)filep;
	pr_info("[%s] Device closed successfully\n", DEVICE_NAME);
	return 0;
}

static ssize_t led_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset)
{
	struct led_dev_priv *priv = filep->private_data;
	char state_char;
	int gpio_val;

	if (*offset > 0) {
		return 0; /* EOF check */
	}

	if (len < 1) {
		return -EINVAL;
	}

	/* Direct physical hardware state acquisition via Descriptor API */
	gpio_val = gpiod_get_value(priv->gpiod);
	if (gpio_val < 0) {
		dev_err(priv->device, "Failed to read hardware GPIO status: %d\n", gpio_val);
		return gpio_val;
	}

	state_char = (gpio_val == 1) ? LED_CMD_ON : LED_CMD_OFF;

	if (copy_to_user(buffer, &state_char, 1) != 0) {
		dev_err(priv->device, "Failed to copy GPIO status to user space\n");
		return -EFAULT;
	}

	*offset += 1;
	return 1;
}

static ssize_t led_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset)
{
	struct led_dev_priv *priv = filep->private_data;
	char cmd_char;

	(void)offset;

	if (len == 0) {
		return 0;
	}

	if (copy_from_user(&cmd_char, buffer, 1) != 0) {
		dev_err(priv->device, "Failed to copy command from user space\n");
		return -EFAULT;
	}

	if (cmd_char == LED_CMD_ON) {
		gpiod_set_value(priv->gpiod, 1);
		dev_info(priv->device, "LED state set to HIGH (ON)\n");
	} else if (cmd_char == LED_CMD_OFF) {
		gpiod_set_value(priv->gpiod, 0);
		dev_info(priv->device, "LED state set to LOW (OFF)\n");
	} else {
		dev_err(priv->device, "Invalid command character received: 0x%02x\n", cmd_char);
		return -EINVAL;
	}

	return len;
}

static const struct file_operations led_fops = {
	.owner   = THIS_MODULE,
	.open    = led_open,
	.release = led_release,
	.read    = led_read,
	.write   = led_write,
};

static int led_probe(struct platform_device *pdev)
{
	struct led_dev_priv *priv;
	int ret;

	dev_info(&pdev->dev, "Initializing platform device matching DT node\n");

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv) {
		return -ENOMEM;
	}

	/* Acquire GPIO Descriptor mapped from Device Tree (Initial state: LOW) */
	priv->gpiod = devm_gpiod_get(&pdev->dev, NULL, GPIOD_OUT_LOW);
	if (IS_ERR(priv->gpiod)) {
		ret = PTR_ERR(priv->gpiod);
		dev_err(&pdev->dev, "Failed to acquire GPIO descriptor: %d\n", ret);
		return ret;
	}

	ret = alloc_chrdev_region(&priv->dev_num, MINOR_BASE, DEV_COUNT, DEVICE_NAME);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to allocate character device region\n");
		return ret;
	}

	cdev_init(&priv->cdev, &led_fops);
	priv->cdev.owner = THIS_MODULE;

	ret = cdev_add(&priv->cdev, priv->dev_num, DEV_COUNT);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to add cdev structure to system\n");
		goto fail_unregister_region;
	}

	priv->class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(priv->class)) {
		ret = PTR_ERR(priv->class);
		dev_err(&pdev->dev, "Failed to create sysfs class\n");
		goto fail_cdev_del;
	}

	priv->device = device_create(priv->class, &pdev->dev, priv->dev_num, NULL, DEVICE_NAME);
	if (IS_ERR(priv->device)) {
		ret = PTR_ERR(priv->device);
		dev_err(&pdev->dev, "Failed to create device node /dev/%s\n", DEVICE_NAME);
		goto fail_class_destroy;
	}

	platform_set_drvdata(pdev, priv);

	dev_info(&pdev->dev, "LED Platform Driver probed successfully. Major: %d, Minor: %d\n",
	         MAJOR(priv->dev_num), MINOR(priv->dev_num));
	return 0;

fail_class_destroy:
	class_destroy(priv->class);
fail_cdev_del:
	cdev_del(&priv->cdev);
fail_unregister_region:
	unregister_chrdev_region(priv->dev_num, DEV_COUNT);
	return ret;
}

static int led_remove(struct platform_device *pdev)
{
	struct led_dev_priv *priv = platform_get_drvdata(pdev);

	dev_info(&pdev->dev, "Removing LED Platform Driver\n");

	if (priv) {
		device_destroy(priv->class, priv->dev_num);
		class_destroy(priv->class);
		cdev_del(&priv->cdev);
		unregister_chrdev_region(priv->dev_num, DEV_COUNT);
	}

	dev_info(&pdev->dev, "LED Platform Driver removed cleanly\n");
	return 0;
}

static const struct of_device_id led_of_match[] = {
	{ .compatible = COMPATIBLE_STR },
	{ /* Sentinel */ }
};
MODULE_DEVICE_TABLE(of, led_of_match);

static struct platform_driver led_platform_driver = {
	.probe  = led_probe,
	.remove = led_remove,
	.driver = {
		.name           = DRIVER_NAME,
		.of_match_table = led_of_match,
	},
};

module_platform_driver(led_platform_driver);