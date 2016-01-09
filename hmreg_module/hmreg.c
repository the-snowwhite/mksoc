#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
//#include <asm/uaccess.h>
#include <linux/ioport.h>
#include <linux/io.h>

#define hmreg_BASE 0xff200000
//#define PAGE_SIZE 4096
#define hmreg_SIZE PAGE_SIZE
#define hmreg_OFFSET 0x04000

void *hmreg_mem;

static struct device_driver hmreg_driver = {
	.name = "hmreg",
	.bus = &platform_bus_type,
};

ssize_t hmreg_show(struct device_driver *drv, char *buf)
{
	u16 index;
	u8 data;
	for (index=0;index < 32767;index++)
	{
		data = ioread8(hmreg_mem + index);
		buf[index] = data;
	}
//	return scnprintf(buf, PAGE_SIZE, "%s\n", drv->name);
	return 32767;
}

ssize_t hmreg_store(struct device_driver *drv, const char *buf, size_t count)
{
	unsigned long indata;
	u8 data;
	u8 adr;

	if (buf == NULL) {
		pr_err("Error, string must not be NULL\n");
		return -EINVAL;
	}

	if (kstrtoul(buf, 10, &indata) < 0) {
		pr_err("Could not convert string to integer\n");
		return -EINVAL;
	}

	data = indata & 0xFF;
	if (data > 8191) {
		pr_err("Invalid data value %d\n", data);
		return -EINVAL;
	}

	adr = indata >> 8;
	if (adr > 255) {
		pr_err("Invalid address value %d\n", adr);
		return -EINVAL;
	}


	iowrite8(data, hmreg_mem + adr);

	return count;
}

static DRIVER_ATTR(hmreg, (S_IWUSR | S_IRUSR), hmreg_show, hmreg_store);

MODULE_LICENSE("Dual BSD/GPL");

static int __init hmreg_init(void)
{
	int ret;
	struct resource *res;

	ret = driver_register(&hmreg_driver);
	if (ret < 0)
		return ret;

	ret = driver_create_file(&hmreg_driver, &driver_attr_hmreg);
	if (ret < 0) {
		driver_unregister(&hmreg_driver);
		return ret;
	}

	res = request_mem_region((hmreg_BASE + hmreg_OFFSET), hmreg_SIZE, "hmreg");
	if (res == NULL) {
		driver_remove_file(&hmreg_driver, &driver_attr_hmreg);
		driver_unregister(&hmreg_driver);
		return -EBUSY;
	}

	hmreg_mem = ioremap((hmreg_BASE + hmreg_OFFSET), hmreg_SIZE);
	if (hmreg_mem == NULL) {
		driver_remove_file(&hmreg_driver, &driver_attr_hmreg);
		driver_unregister(&hmreg_driver);
		release_mem_region(hmreg_BASE, hmreg_SIZE);
		return -EFAULT;
	}

	return 0;
}

static void __exit hmreg_exit(void)
{
	driver_remove_file(&hmreg_driver, &driver_attr_hmreg);
	driver_unregister(&hmreg_driver);
	release_mem_region((hmreg_BASE + hmreg_OFFSET), hmreg_SIZE);
	iounmap(hmreg_mem);
}

module_init(hmreg_init);
module_exit(hmreg_exit);

MODULE_LICENSE("Dual BSD/GPL");
