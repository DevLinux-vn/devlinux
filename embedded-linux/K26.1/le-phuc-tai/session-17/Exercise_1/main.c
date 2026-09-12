/**
 * @file main.c
 * @brief Thread-safe platform driver for Linux Kernel 6.17+ using GPIO Descriptors.
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
#include <linux/mutex.h>

#define DRIVER_NAME     "led_blink_driver"
#define DEVICE_NAME     "led_blink"
#define CLASS_NAME      "led_blink_class"
#define COMPATIBLE_STR  "thaonguyen,led-blink"

#define LED_CMD_ON      '1'
#define LED_CMD_OFF     '0'
#define MINOR_BASE      0
#define DEV_COUNT       1
#define TRANSFER_SIZE   ((size_t)1)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Senior Embedded Systems Engineer");
MODULE_DESCRIPTION("Platform Driver compatible with Linux Kernel 6.17+");
MODULE_VERSION("2.0");

struct led_dev_priv {
	struct gpio_desc *gpiod;
	dev_t dev_num;
	struct cdev cdev;
	struct class *class;
	struct device *device;
	struct mutex lock;
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
		*offset = 0; /* Reset offset to allow continuous status polling */
		return 0;
	}

	if (len < TRANSFER_SIZE) {
		return -EINVAL;
	}

	mutex_lock(&priv->lock);
	gpio_val = gpiod_get_value(priv->gpiod);
	mutex_unlock(&priv->lock);

	if (gpio_val < 0) {
		dev_err(priv->device, "Failed to read hardware GPIO status: %d\n", gpio_val);
		return gpio_val;
	}

	state_char = (gpio_val == 1) ? LED_CMD_ON : LED_CMD_OFF;

	if (copy_to_user(buffer, &state_char, TRANSFER_SIZE) != 0) {
		dev_err(priv->device, "Failed to copy GPIO status to user space\n");
		return -EFAULT;
	}

	*offset += TRANSFER_SIZE;
	return (ssize_t)TRANSFER_SIZE;
}

static ssize_t led_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset)
{
	struct led_dev_priv *priv = filep->private_data;
	char cmd_char;

	(void)offset;

	if (len == 0) {
		return 0;
	}

	if (copy_from_user(&cmd_char, buffer, TRANSFER_SIZE) != 0) {
		dev_err(priv->device, "Failed to copy command from user space\n");
		return -EFAULT;
	}

	mutex_lock(&priv->lock);
	if (cmd_char == LED_CMD_ON) {
		gpiod_set_value(priv->gpiod, 1);
		dev_info(priv->device, "LED state set to HIGH (ON)\n");
	} else if (cmd_char == LED_CMD_OFF) {
		gpiod_set_value(priv->gpiod, 0);
		dev_info(priv->device, "LED state set to LOW (OFF)\n");
	} else {
		mutex_unlock(&priv->lock);
		dev_err(priv->device, "Invalid command character received: 0x%02x\n", cmd_char);
		return -EINVAL;
	}
	mutex_unlock(&priv->lock);

	return len;
}

static const struct file_operations led_fops = {
	.owner   = THIS_MODULE,
	.open    = led_open,
	.release = led_release,
	.read    = led_read,
	.write   = led_write,
};

static int led_probe_init_cdev(struct led_dev_priv *priv, struct platform_device *pdev)
{
	int ret;

	ret = alloc_chrdev_region(&priv->dev_num, MINOR_BASE, DEV_COUNT, DEVICE_NAME);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to allocate character device region\n");
		return ret;
	}

	cdev_init(&priv->cdev, &led_fops);
	priv->cdev.owner = THIS_MODULE;

	ret = cdev_add(&priv->cdev, priv->dev_num, DEV_COUNT);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to add cdev structure\n");
		unregister_chrdev_region(priv->dev_num, DEV_COUNT);
		return ret;
	}

	return 0;
}

static int led_probe_init_device(struct led_dev_priv *priv, struct platform_device *pdev)
{
	int ret;

	/* FIX: Kernel 6.4+ class_create takes only 1 argument (CLASS_NAME) */
	priv->class = class_create(CLASS_NAME);
	if (IS_ERR(priv->class)) {
		ret = PTR_ERR(priv->class);
		dev_err(&pdev->dev, "Failed to create sysfs class\n");
		return ret;
	}

	priv->device = device_create(priv->class, &pdev->dev, priv->dev_num, NULL, DEVICE_NAME);
	if (IS_ERR(priv->device)) {
		ret = PTR_ERR(priv->device);
		dev_err(&pdev->dev, "Failed to create device node /dev/%s\n", DEVICE_NAME);
		class_destroy(priv->class);
		return ret;
	}

	return 0;
}

static int led_probe(struct platform_device *pdev)
{
	struct led_dev_priv *priv;
	int ret;

	dev_info(&pdev->dev, "Initializing platform device matching DT node\n");

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv) {
		return -ENOMEM;
	}

	mutex_init(&priv->lock);

	priv->gpiod = devm_gpiod_get(&pdev->dev, NULL, GPIOD_OUT_LOW);
	if (IS_ERR(priv->gpiod)) {
		ret = PTR_ERR(priv->gpiod);
		dev_err(&pdev->dev, "Failed to acquire GPIO descriptor: %d\n", ret);
		return ret;
	}

	ret = led_probe_init_cdev(priv, pdev);
	if (ret < 0) {
		return ret;
	}

	ret = led_probe_init_device(priv, pdev);
	if (ret < 0) {
		cdev_del(&priv->cdev);
		unregister_chrdev_region(priv->dev_num, DEV_COUNT);
		return ret;
	}

	platform_set_drvdata(pdev, priv);
	dev_info(&pdev->dev, "LED Platform Driver probed successfully. Major: %d, Minor: %d\n",
	         MAJOR(priv->dev_num), MINOR(priv->dev_num));
	return 0;
}

/* FIX: Kernel 6.11+ platform_driver.remove callback signature returns void */
static void led_remove(struct platform_device *pdev)
{
	struct led_dev_priv *priv = platform_get_drvdata(pdev);

	dev_info(&pdev->dev, "Removing LED Platform Driver\n");

	if (priv) {
		device_destroy(priv->class, priv->dev_num);
		class_destroy(priv->class);
		cdev_del(&priv->cdev);
		unregister_chrdev_region(priv->dev_num, DEV_COUNT);
		mutex_destroy(&priv->lock);
	}

	dev_info(&pdev->dev, "LED Platform Driver removed cleanly\n");
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