#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/of.h>

#define DEVICE_NAME "led_blink"
#define CLASS_NAME "led_blink_class"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pham Anh Quan");
MODULE_DESCRIPTION("Platform Driver for LED Control using GPIO Descriptor API");
MODULE_VERSION("1.0");

static struct gpio_desc *led_gpio;
static dev_t dev_num;
static struct cdev led_cdev;
static struct class *led_class;
static struct device *led_device;

static int led_open(struct inode *inode, struct file *file) {
    return 0;
}

static int led_release(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t led_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    int val;
    char state_char;

    if (*ppos > 0) return 0; // EOF

    if (!led_gpio) {
        return -EIO;
    }

    // Đọc trạng thái thực tế từ phần cứng qua GPIO descriptor
    val = gpiod_get_value(led_gpio);
    state_char = (val > 0) ? '1' : '0';

    if (copy_to_user(buf, &state_char, 1)) {
        return -EFAULT;
    }

    *ppos += 1;
    return 1;
}

static ssize_t led_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    char kbuf;

    if (count == 0) return 0;

    if (copy_from_user(&kbuf, buf, 1)) {
        return -EFAULT;
    }

    if (!led_gpio) {
        return -EIO;
    }

    if (kbuf == '1') {
        gpiod_set_value(led_gpio, 1);
    } else if (kbuf == '0') {
        gpiod_set_value(led_gpio, 0);
    } else {
        return -EINVAL;
    }

    return count;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_release,
    .read = led_read,
    .write = led_write,
};

static int led_probe(struct platform_device *pdev) {
    struct device *dev = &pdev->dev;
    int ret;

    dev_info(dev, "Probing led_blink platform driver...\n");

    // Lấy GPIO descriptor từ Device Tree (tên phím GPIO là "led")
    led_gpio = devm_gpiod_get(dev, "led", GPIOD_OUT_LOW);
    if (IS_ERR(led_gpio)) {
        ret = PTR_ERR(led_gpio);
        dev_err(dev, "Failed to get GPIO descriptor: %d\n", ret);
        return ret;
    }

    // Cấp phát Major/Minor device number
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        dev_err(dev, "Failed to allocate chrdev region\n");
        return ret;
    }

    cdev_init(&led_cdev, &fops);
    led_cdev.owner = THIS_MODULE;

    ret = cdev_add(&led_cdev, dev_num, 1);
    if (ret < 0) {
        dev_err(dev, "Failed to add cdev\n");
        goto fail_unregister;
    }

    led_class = class_create(CLASS_NAME);
    if (IS_ERR(led_class)) {
        ret = PTR_ERR(led_class);
        dev_err(dev, "Failed to create class\n");
        goto fail_cdev_del;
    }

    led_device = device_create(led_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(led_device)) {
        ret = PTR_ERR(led_device);
        dev_err(dev, "Failed to create device /dev/%s\n", DEVICE_NAME);
        goto fail_class_destroy;
    }

    dev_info(dev, "Driver probed successfully. Created /dev/%s\n", DEVICE_NAME);
    return 0;

fail_class_destroy:
    class_destroy(led_class);
fail_cdev_del:
    cdev_del(&led_cdev);
fail_unregister:
    unregister_chrdev_region(dev_num, 1);
    return ret;
}

static int led_remove(struct platform_device *pdev) {
    struct device *dev = &pdev->dev;

    dev_info(dev, "Removing led_blink platform driver...\n");

    device_destroy(led_class, dev_num);
    class_destroy(led_class);
    cdev_del(&led_cdev);
    unregister_chrdev_region(dev_num, 1);

    // devm_gpiod_get sẽ tự động giải phóng tài nguyên GPIO
    return 0;
}

static const struct of_device_id led_of_match[] = {
    { .compatible = "pham-anh-quan,led-blink" },
    { /* Sentinel */ }
};
MODULE_DEVICE_TABLE(of, led_of_match);

static struct platform_driver led_platform_driver = {
    .probe = led_probe,
    .remove = led_remove,
    .driver = {
        .name = "led_blink_driver",
        .of_match_table = led_of_match,
    },
};

module_platform_driver(led_platform_driver);
