// SPDX-License-Identifier: GPL-2.0
/* SSD1306 128x64 character driver; kernel API baseline: Linux 6.1. */
#include <linux/cdev.h>
#include <linux/compat.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include "ssd1306.h"

#define DRIVER_NAME "ssd1306_i2c"
#define CLASS_NAME "ssd1306_char"
#define COMMAND_PREFIX 0x00
#define DATA_PREFIX 0x40
#define DISPLAY_OFF 0xae
#define DISPLAY_ON 0xaf
#define SET_COLUMN_RANGE 0x21
#define SET_PAGE_RANGE 0x22
#define TRANSFER_SIZE 32
#define STATUS_SIZE 256
#define TEXT_PREFIX "TEXT "
#define MAX_COMMAND (sizeof(TEXT_PREFIX) - 1 + SSD1306_TEXT_MAX + 1)

struct ssd1306_device {
	struct device device;
	struct cdev cdev;
	struct class *class;
	struct i2c_client *client;
	struct mutex lock;
	u8 framebuffer[SSD1306_BITMAP_SIZE];
	char last_text[SSD1306_TEXT_MAX + 1];
	bool ready;
	bool display_on;
	bool power_known;
	bool synced;
};

struct ssd1306_file {
	struct ssd1306_device *display;
	struct mutex lock;
	char status[STATUS_SIZE];
	size_t status_len;
};

static int ssd1306_error(int error, const char *operation)
{
	pr_err_ratelimited("[ssd1306] %s failed: %d\n", operation, error);
	return error;
}

/* Caller serializes complete command/frame transactions with display->lock. */
static int ssd1306_send(struct ssd1306_device *display, const u8 *data, size_t size)
{
	int ret = i2c_master_send(display->client, data, size);

	if (ret != size) {
		pr_err("[ssd1306] I2C write failed: result=%d expected=%zu\n", ret, size);
		return ret == -ENXIO || ret == -ENODEV ? -ENODEV : -EIO;
	}
	return 0;
}

static int ssd1306_command(struct ssd1306_device *display, u8 command)
{
	u8 message[] = { COMMAND_PREFIX, command };

	return ssd1306_send(display, message, sizeof(message));
}

static int ssd1306_flush(struct ssd1306_device *display, const u8 *frame)
{
	const u8 address[] = {
		SET_COLUMN_RANGE, 0, SSD1306_WIDTH - 1,
		SET_PAGE_RANGE, 0, SSD1306_PAGES - 1,
	};
	u8 packet[TRANSFER_SIZE];
	size_t offset;
	size_t length;
	int ret;
	int i;

	/* Every frame resets the address window, including after failed writes. */
	for (i = 0; i < ARRAY_SIZE(address); i++) {
		ret = ssd1306_command(display, address[i]);
		if (ret)
			goto failed;
	}
	packet[0] = DATA_PREFIX;
	for (offset = 0; offset < SSD1306_BITMAP_SIZE; offset += length) {
		/* 32-byte wire limit includes the control byte: 31 payload bytes. */
		length = min_t(size_t, TRANSFER_SIZE - 1, SSD1306_BITMAP_SIZE - offset);
		memcpy(packet + 1, frame + offset, length);
		ret = ssd1306_send(display, packet, length + 1);
		if (ret)
			goto failed;
	}
	memmove(display->framebuffer, frame, SSD1306_BITMAP_SIZE);
	display->synced = true;
	return 0;
failed:
	/* Hardware may contain a partial frame; do not pretend rollback occurred. */
	display->synced = false;
	return ret;
}

static int ssd1306_render(u8 *frame, const char *text)
{
	unsigned int page = 0;
	unsigned int column = 0;
	unsigned int i;
	unsigned char ch;

	while ((ch = *text++)) {
		if (ch == '\n') {
			page++;
			column = 0;
			continue;
		}
		if (column == SSD1306_CHARS_PER_LINE) {
			page++;
			column = 0;
		}
		if (page >= SSD1306_PAGES || !ssd1306_font_supported(ch))
			return ssd1306_error(-EINVAL, "text bounds/character validation");
		/* Transparent foreground overlay; zero glyph bits preserve bitmap. */
		for (i = 0; i < SSD1306_FONT_WIDTH; i++)
			frame[page * SSD1306_WIDTH + column * SSD1306_CELL_WIDTH + i] |=
				ssd1306_font[ch - SSD1306_FONT_FIRST][i];
		column++;
	}
	return page < SSD1306_PAGES ? 0 : ssd1306_error(-EINVAL, "text row validation");
}

static int ssd1306_initialize(struct ssd1306_device *display)
{
	/* Internal charge pump, horizontal addressing, 128x64 multiplex. */
	static const u8 sequence[] = {
		DISPLAY_OFF, 0xd5, 0x80, 0xa8, 0x3f, 0xd3, 0x00, 0x40,
		0x8d, 0x14, 0x20, 0x00, 0xa1, 0xc8, 0xda, 0x12,
		0x81, 0x7f, 0xd9, 0xf1, 0xdb, 0x40, 0xa4, 0xa6, 0x2e,
	};
	int i;
	int ret;

	for (i = 0; i < ARRAY_SIZE(sequence); i++) {
		ret = ssd1306_command(display, sequence[i]);
		if (ret)
			return ret;
	}
	ret = ssd1306_flush(display, display->framebuffer);
	if (ret)
		return ret;
	ret = ssd1306_command(display, DISPLAY_ON);
	if (ret)
		return ret;
	display->display_on = true;
	display->power_known = true;
	pr_info("[ssd1306] I2C device initialized at address 0x3c; display cleared\n");
	return 0;
}

static void ssd1306_device_release(struct device *device)
{
	struct ssd1306_device *display = container_of(device, struct ssd1306_device, device);

	kfree(display);
}

static int ssd1306_open(struct inode *inode, struct file *file)
{
	struct ssd1306_device *display = container_of(inode->i_cdev, struct ssd1306_device, cdev);
	struct ssd1306_file *context;
	int ret;

	/* The cdev parent already pins display while this callback is entering. */
	context = kzalloc(sizeof(*context), GFP_KERNEL);
	if (!context)
		return ssd1306_error(-ENOMEM, "open allocation");
	mutex_init(&context->lock);
	ret = mutex_lock_interruptible(&display->lock);
	if (ret)
		goto err_context;
	if (!display->ready || !display->client) {
		ret = -ENODEV;
		goto out_unlock;
	}
	ret = nonseekable_open(inode, file);
	if (ret)
		goto out_unlock;
	if (!get_device(&display->device)) {
		ret = -ENODEV;
		goto out_unlock;
	}
	context->display = display;
	file->private_data = context;
	pr_info("[ssd1306] Device opened by PID %d\n", task_pid_nr(current));
out_unlock:
	mutex_unlock(&display->lock);
	if (!ret)
		return 0;
err_context:
	kfree(context);
	return ssd1306_error(ret, "open");
}

static int ssd1306_release(struct inode *inode, struct file *file)
{
	struct ssd1306_file *context = file->private_data;

	pr_info("[ssd1306] Device closed by PID %d\n", task_pid_nr(current));
	put_device(&context->display->device);
	kfree(context);
	return 0;
}

static ssize_t ssd1306_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	struct ssd1306_file *context = file->private_data;
	struct ssd1306_device *display = context->display;
	ssize_t ret;

	if (!count)
		return 0;
	ret = mutex_lock_interruptible(&context->lock);
	if (ret)
		return ret;
	ret = mutex_lock_interruptible(&display->lock);
	if (ret)
		goto out_context;
	if (!display->ready) {
		ret = -ENODEV;
		goto out_display;
	}
	if (!*ppos)
		context->status_len = scnprintf(context->status, sizeof(context->status),
			"Status: %s, Last text: %s, Synced: %s\n",
			display->power_known ? (display->display_on ? "ON" : "OFF") : "UNKNOWN",
			display->last_text, display->synced ? "yes" : "no");
	mutex_unlock(&display->lock);
	/* Snapshot stays stable over partial reads. Removal never waits on usercopy. */
	ret = simple_read_from_buffer(buf, count, ppos, context->status, context->status_len);
	goto out_context;
out_display:
	mutex_unlock(&display->lock);
out_context:
	mutex_unlock(&context->lock);
	return ret < 0 ? ssd1306_error(ret, "read") : ret;
}

static ssize_t ssd1306_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	struct ssd1306_file *context = file->private_data;
	struct ssd1306_device *display = context->display;
	char command[MAX_COMMAND + 1];
	u8 frame[SSD1306_BITMAP_SIZE];
	char *text = NULL;
	size_t len = count;
	ssize_t ret;

	if (!count)
		return 0;
	if (count > MAX_COMMAND)
		return ssd1306_error(-EINVAL, "command size validation");
	if (copy_from_user(command, buf, count))
		return ssd1306_error(-EFAULT, "command usercopy");
	if (memchr(command, '\0', count))
		return ssd1306_error(-EINVAL, "command NUL validation");
	if (command[len - 1] == '\n')
		len--;
	command[len] = '\0';
	if (!strncmp(command, TEXT_PREFIX, sizeof(TEXT_PREFIX) - 1)) {
		text = command + sizeof(TEXT_PREFIX) - 1;
		if (!*text || strlen(text) > SSD1306_TEXT_MAX)
			return ssd1306_error(-EINVAL, "text length validation");
	} else if (strcmp(command, "CLEAR") && strcmp(command, "ON") && strcmp(command, "OFF")) {
		pr_err("[ssd1306] Invalid command\n");
		return -EINVAL;
	}
	ret = mutex_lock_interruptible(&context->lock);
	if (ret)
		return ret;
	ret = mutex_lock_interruptible(&display->lock);
	if (ret)
		goto out_context;
	if (!display->ready) {
		ret = -ENODEV;
		goto out_display;
	}
	if (text || !strcmp(command, "CLEAR")) {
		if (text) {
			memcpy(frame, display->framebuffer, sizeof(frame));
			ret = ssd1306_render(frame, text);
			if (ret)
				goto out_display;
		} else {
			memset(frame, 0, sizeof(frame));
		}
		ret = ssd1306_flush(display, frame);
		if (ret)
			goto out_display;
		if (text)
			memcpy(display->last_text, text, strlen(text) + 1);
		else
			display->last_text[0] = '\0';
		if (text)
			pr_info("[ssd1306] Text rendered: %*pE\n", (int)strlen(text), text);
		else
			pr_info("[ssd1306] Display cleared\n");
	} else {
		bool on = !strcmp(command, "ON");

		ret = ssd1306_command(display, on ? DISPLAY_ON : DISPLAY_OFF);
		if (ret) {
			display->power_known = false;
			goto out_display;
		}
		display->display_on = on;
		display->power_known = true;
		pr_info("[ssd1306] Display command: %s\n", command);
	}
	*ppos = 0;
	ret = count;
out_display:
	mutex_unlock(&display->lock);
out_context:
	mutex_unlock(&context->lock);
	return ret < 0 ? ssd1306_error(ret, "write") : ret;
}

static long ssd1306_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	struct ssd1306_file *context = file->private_data;
	struct ssd1306_device *display = context->display;
	struct ssd1306_bitmap *bitmap;
	int ret;

	if (!(file->f_mode & FMODE_WRITE))
		return ssd1306_error(-EBADF, "bitmap upload requires a writable fd");
	if (command != SSD1306_IOC_UPLOAD_BITMAP) {
		pr_err("[ssd1306] Invalid bitmap ioctl: %#x\n", command);
		return _IOC_TYPE(command) == SSD1306_IOC_MAGIC ? -EINVAL : -ENOTTY;
	}
	bitmap = memdup_user((void __user *)arg, sizeof(*bitmap));
	if (IS_ERR(bitmap))
		return ssd1306_error(PTR_ERR(bitmap), "bitmap usercopy");
	ret = mutex_lock_interruptible(&context->lock);
	if (ret)
		goto out_bitmap;
	ret = mutex_lock_interruptible(&display->lock);
	if (ret)
		goto out_context;
	if (!display->ready) {
		ret = -ENODEV;
		goto out_display;
	}
	ret = ssd1306_flush(display, bitmap->pixels);
	if (!ret) {
		display->last_text[0] = '\0';
		file->f_pos = 0;
		pr_info("[ssd1306] Bitmap uploaded: %u bytes\n", SSD1306_BITMAP_SIZE);
	}
out_display:
	mutex_unlock(&display->lock);
out_context:
	mutex_unlock(&context->lock);
out_bitmap:
	kfree(bitmap);
	return ret < 0 ? ssd1306_error(ret, "bitmap upload") : ret;
}

static const struct file_operations ssd1306_fops = {
	.owner = THIS_MODULE,
	.open = ssd1306_open,
	.release = ssd1306_release,
	.read = ssd1306_read,
	.write = ssd1306_write,
	.unlocked_ioctl = ssd1306_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = compat_ptr_ioctl,
#endif
};

static int ssd1306_probe(struct i2c_client *client)
{
	struct ssd1306_device *display;
	dev_t number;
	int ret;

	if (client->addr != SSD1306_I2C_ADDR || !i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		pr_err("[ssd1306] Expected I2C address 0x3c and raw I2C adapter\n");
		return -ENODEV;
	}
	display = kzalloc(sizeof(*display), GFP_KERNEL);
	if (!display)
		return ssd1306_error(-ENOMEM, "device allocation");
	mutex_init(&display->lock);
	display->client = client;
	device_initialize(&display->device);
	display->device.release = ssd1306_device_release;
	display->device.parent = &client->dev;
	ret = dev_set_name(&display->device, DRIVER_NAME);
	if (ret)
		goto err_device;
	ret = ssd1306_initialize(display);
	if (ret)
		goto err_power;
	ret = alloc_chrdev_region(&number, 0, 1, DRIVER_NAME);
	if (ret)
		goto err_power;
	display->device.devt = number;
	cdev_init(&display->cdev, &ssd1306_fops);
	display->cdev.owner = THIS_MODULE;
	cdev_set_parent(&display->cdev, &display->device.kobj);
	ret = cdev_add(&display->cdev, number, 1);
	if (ret)
		goto err_region;
	display->class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(display->class)) {
		ret = PTR_ERR(display->class);
		goto err_cdev;
	}
	display->device.class = display->class;
	ret = device_add(&display->device);
	if (ret)
		goto err_class;
	i2c_set_clientdata(client, display);
	mutex_lock(&display->lock);
	display->ready = true;
	mutex_unlock(&display->lock);
	pr_info("[ssd1306] Character device created: /dev/" DRIVER_NAME "\n");
	return 0;
err_class:
	class_destroy(display->class);
err_cdev:
	cdev_del(&display->cdev);
err_region:
	unregister_chrdev_region(number, 1);
err_power:
	if (ssd1306_command(display, DISPLAY_OFF))
		pr_err("[ssd1306] Could not turn display off during probe rollback\n");
err_device:
	put_device(&display->device);
	pr_err("[ssd1306] Probe failed: %d\n", ret);
	return ret;
}

/* Linux 6.1 I2C remove and device-release callbacks have void return types. */
static void ssd1306_remove(struct i2c_client *client)
{
	struct ssd1306_device *display = i2c_get_clientdata(client);
	dev_t number = display->device.devt;

	mutex_lock(&display->lock);
	display->ready = false;
	if (ssd1306_command(display, DISPLAY_OFF))
		pr_err("[ssd1306] Display OFF failed during removal\n");
	display->client = NULL;
	mutex_unlock(&display->lock);
	device_del(&display->device);
	class_destroy(display->class);
	cdev_del(&display->cdev);
	unregister_chrdev_region(number, 1);
	put_device(&display->device);
	pr_info("[ssd1306] I2C device removed\n");
}

static const struct of_device_id ssd1306_of_match[] = {
	{ .compatible = "tran-danh-quang,ssd1306" },
	{ }
};
MODULE_DEVICE_TABLE(of, ssd1306_of_match);

static const struct i2c_device_id ssd1306_ids[] = {
	{ "ssd1306_char", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ssd1306_ids);

static struct i2c_driver ssd1306_driver = {
	.driver = {
		.name = "ssd1306_i2c_driver",
		.of_match_table = ssd1306_of_match,
	},
	.probe_new = ssd1306_probe,
	.remove = ssd1306_remove,
	.id_table = ssd1306_ids,
};
module_i2c_driver(ssd1306_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tran Danh Quang");
MODULE_DESCRIPTION("SSD1306 I2C display with text and bitmap character interface");
