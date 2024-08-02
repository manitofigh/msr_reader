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



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x78161fc9, "__tracepoint_write_msr" },
	{ 0x1a103589, "kmalloc_caches" },
	{ 0xb236c6eb, "kmalloc_trace" },
	{ 0x37a0cba, "kfree" },
	{ 0x29193521, "__tracepoint_read_msr" },
	{ 0x39e3c030, "do_trace_read_msr" },
	{ 0x25db1577, "do_trace_write_msr" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x122c3a7e, "_printk" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x43581fb4, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "9911BB3D3054758F75C5C54");
