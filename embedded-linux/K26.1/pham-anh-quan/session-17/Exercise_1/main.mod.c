#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const char ____versions[]
__used __section("__versions") =
	"\x14\x00\x00\x00\x5a\xd4\x32\xd3"
	"cdev_del\0\0\0\0"
	"\x24\x00\x00\x00\x33\xb3\x91\x60"
	"unregister_chrdev_region\0\0\0\0"
	"\x18\x00\x00\x00\xc2\x9c\xc4\x13"
	"_copy_from_user\0"
	"\x18\x00\x00\x00\x26\xc0\x4b\xd4"
	"gpiod_set_value\0"
	"\x1c\x00\x00\x00\xcb\xf6\xfd\xf0"
	"__stack_chk_fail\0\0\0\0"
	"\x18\x00\x00\x00\xf1\xba\x6d\xb7"
	"devm_gpiod_get\0\0"
	"\x1c\x00\x00\x00\x2b\x2f\xec\xe3"
	"alloc_chrdev_region\0"
	"\x14\x00\x00\x00\xc5\x88\xf4\xcc"
	"cdev_init\0\0\0"
	"\x14\x00\x00\x00\xa2\xcc\x0f\x48"
	"cdev_add\0\0\0\0"
	"\x18\x00\x00\x00\xf1\x96\xcb\x17"
	"class_create\0\0\0\0"
	"\x18\x00\x00\x00\xae\x5e\xe1\x39"
	"device_create\0\0\0"
	"\x14\x00\x00\x00\xf7\x3c\xd7\x9a"
	"_dev_err\0\0\0\0"
	"\x18\x00\x00\x00\xb4\xff\xfb\xe2"
	"gpiod_get_value\0"
	"\x18\x00\x00\x00\xe1\xbe\x10\x6b"
	"_copy_to_user\0\0\0"
	"\x24\x00\x00\x00\x9c\x01\x79\x88"
	"platform_driver_unregister\0\0"
	"\x14\x00\x00\x00\xbb\x6d\xfb\xbd"
	"__fentry__\0\0"
	"\x1c\x00\x00\x00\xca\x39\x82\x5b"
	"__x86_return_thunk\0\0"
	"\x24\x00\x00\x00\x55\x3d\x5d\x96"
	"__platform_driver_register\0\0"
	"\x14\x00\x00\x00\xe5\xa7\xe4\x39"
	"_dev_info\0\0\0"
	"\x18\x00\x00\x00\xbd\x45\x7d\x2e"
	"device_destroy\0\0"
	"\x18\x00\x00\x00\xe3\x2e\x87\x04"
	"class_destroy\0\0\0"
	"\x18\x00\x00\x00\x30\xcf\x11\x80"
	"module_layout\0\0\0"
	"\x00\x00\x00\x00\x00\x00\x00\x00";

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Cpham-anh-quan,led-blink");
MODULE_ALIAS("of:N*T*Cpham-anh-quan,led-blinkC*");

MODULE_INFO(srcversion, "BDEB4E09F19C4CF3981B284");
