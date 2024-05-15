/* * Device driver for the VGA video generator
 *
 * A Platform device implemented using the misc subsystem
 * 
 * Alex Yu, Stephen A. Edwards
 * Columbia University
 *
 * References:
 * Linux source: Documentation/driver-model/platform.txt
 *               drivers/misc/arm-charlcd.c
 * http://www.linuxforu.com/tag/linux-device-drivers/
 * http://free-electrons.com/docs/
 *
 * "make" to build
 * insmod vga_zylo.ko
 *
 * Check code style with
 * checkpatch.pl --file --no-tree vga_zylo.c
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "vga_zylo.h"

#define DRIVER_NAME "vga_zylo"

/* Device registers */
//#define AUD_AMP(x) (x)       //5 0
//#define MEM_LMT(x) ((x)+4)   //6 1
//#define MEM_ADDR(x) ((x)+8)  //7 2
//#define MEM_MODE(x) ((x)+12) //8 3
#define SCORE_N(x) ((x)+16) //8
#define COMBO_N(x) ((x)+20) //8
#define DATA_N(x) ((x)+24) //8

/*
 * Information about our device
 */
struct vga_zylo_dev {
	struct resource res; /* Resource: our registers */
	void __iomem *virtbase; /* Where registers can be accessed in memory */
	vga_zylo_data_t packet;
} dev;

/*
 * Write segments of a single digit
 * Assumes digit is in range and the device information has been set up
 */
static void write_packet(vga_zylo_data_t *packet)
{
	int i;
	for (i = 0; i < SIZE; i++) {
		iowrite32(packet->data[i], DATA_N(dev.virtbase) );
	}
	dev.packet = *packet;
}

static void write_score(vga_zylo_data_t *packet)
{
	iowrite32(packet->score, SCORE_N(dev.virtbase));
	dev.packet = *packet;
}

static void write_combo(vga_zylo_data_t *packet)
{
	iowrite32(packet->combo, COMBO_N(dev.virtbase));
	dev.packet = *packet;
}
/*
 * Handle ioctl() calls from userspace:
 * Read or write the segments on single digits.
 * Note extensive error checking of arguments
 */
static long vga_zylo_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	vga_zylo_arg_t vla;

	switch (cmd) {
		case VGA_ZYLO_WRITE_PACKET:
			if (copy_from_user(&vla, (vga_zylo_arg_t *) arg, sizeof(vga_zylo_arg_t)))
				return -EACCES;
			write_packet(&vla.packet);
			break;
		case VGA_ZYLO_WRITE_SCORE:
			if (copy_from_user(&vla, (vga_zylo_arg_t *) arg, sizeof(vga_zylo_arg_t)))
				return -EACCES;
			write_score(&vla.packet);
			break;
		case VGA_ZYLO_WRITE_COMBO:
			if (copy_from_user(&vla, (vga_zylo_arg_t *) arg, sizeof(vga_zylo_arg_t)))
				return -EACCES;
			write_combo(&vla.packet);
			break;
		case VGA_ZYLO_READ_PACKET:
			vla.packet = dev.packet;
			if (copy_to_user((vga_zylo_arg_t *) arg, &vla, sizeof(vga_zylo_arg_t)))
				return -EACCES;
			break;
		default:
			return -EINVAL;
	}

	return 0;
}

/* The operations our device knows how to do */
static const struct file_operations vga_zylo_fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl = vga_zylo_ioctl,
};

/* Information about our device for the "misc" framework -- like a char dev */
static struct miscdevice vga_zylo_misc_device = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= DRIVER_NAME,
	.fops		= &vga_zylo_fops,
};

/*
 * Initialization code: get resources (registers) and display
 * a welcome message
 */
static int __init vga_zylo_probe(struct platform_device *pdev)
{
	int ret;

	/* Register ourselves as a misc device: creates /dev/vga_zylo */
	ret = misc_register(&vga_zylo_misc_device);

	/* Get the address of our registers from the device tree */
	ret = of_address_to_resource(pdev->dev.of_node, 0, &dev.res);
	if (ret) {
		ret = -ENOENT;
		goto out_deregister;
	}

	/* Make sure we can use these registers */
	if (request_mem_region(dev.res.start, resource_size(&dev.res),
				DRIVER_NAME) == NULL) {
		ret = -EBUSY;
		goto out_deregister;
	}

	/* Arrange access to our registers */
	dev.virtbase = of_iomap(pdev->dev.of_node, 0);
	if (dev.virtbase == NULL) {
		ret = -ENOMEM;
		goto out_release_mem_region;
	}

	return 0;

out_release_mem_region:
	release_mem_region(dev.res.start, resource_size(&dev.res));
out_deregister:
	misc_deregister(&vga_zylo_misc_device);
	return ret;
}

/* Clean-up code: release resources */
static int vga_zylo_remove(struct platform_device *pdev)
{
	iounmap(dev.virtbase);
	release_mem_region(dev.res.start, resource_size(&dev.res));
	misc_deregister(&vga_zylo_misc_device);
	return 0;
}

/* Which "compatible" string(s) to search for in the Device Tree */
#ifdef CONFIG_OF
static const struct of_device_id vga_zylo_of_match[] = {
	{ .compatible = "csee4840,vga_zylo-1.2" },
	{},
};
MODULE_DEVICE_TABLE(of, vga_zylo_of_match);
#endif

/* Information for registering ourselves as a "platform" driver */
static struct platform_driver vga_zylo_driver = {
	.driver	= {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(vga_zylo_of_match),
	},
	.remove	= __exit_p(vga_zylo_remove),
};

/* Called when the module is loaded: set things up */
static int __init vga_zylo_init(void)
{
	pr_info(DRIVER_NAME ": init\n");
	return platform_driver_probe(&vga_zylo_driver, vga_zylo_probe);
}

/* Calball when the module is unloaded: release resources */
static void __exit vga_zylo_exit(void)
{
	platform_driver_unregister(&vga_zylo_driver);
	pr_info(DRIVER_NAME ": exit\n");
}

module_init(vga_zylo_init);
module_exit(vga_zylo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexander Yu, Columbia University");
MODULE_DESCRIPTION("VGA zylo driver");
