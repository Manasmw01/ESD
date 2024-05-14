/* * Device driver for the VGA video generator
 *
 * A Platform device implemented using the misc subsystem
 *
 * Stephen A. Edwards
 * Columbia University
 *
 * References:
 * Linux source: Documentation/driver-model/platform.txt
 *               drivers/misc/arm-charlcd.c
 * http://www.linuxforu.com/tag/linux-device-drivers/
 * http://free-electrons.com/docs/
 *
 * "make" to build
 * insmod vga_ball.ko
 *
 * Check code style with
 * checkpatch.pl --file --no-tree vga_ball.c
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
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>
#include "vga_ball.h"

#define DRIVER_NAME "audio"

/* Device registers */
#define L_SAMPLES(x) ((x)+8)
#define WRITE(x) ((x))

DECLARE_WAIT_QUEUE_HEAD(wq);


/*
 * Information about our device
 */
struct vga_ball_dev {
	struct resource res; /* Resource: our registers */
	void __iomem *virtbase; /* Where registers can be accessed in memory */
	audio_samples_t samples;
	audio_ready_t ready;
	audio_data_t data;
	int irq_num;
} dev;
static void read_samples(audio_samples_t *samples)
{
	samples->l = ioread32(L_SAMPLES(dev.virtbase));
	//ioread32(RESET_IRQ(dev.virtbase));
	dev.samples = *samples;
}

static int read_samples_simple(void)
{
	int output = ioread8(L_SAMPLES(dev.virtbase));
	return output;
}

/*
 * Handle interrupts raised by our device. Read samples,
 * clear the interrupt, and wake the user level program.
 */
irq_handler_t irq_handler(int irq, void *dev_id, struct pt_regs *reg)
{
	// Read samples from the device
	audio_samples_t samples;
	read_samples(&samples);

	// Wake the user level process
	audio_ready_t ready = { .audio_ready = 1 };
	dev.ready = ready;
	wake_up_interruptible(&wq);

	return IRQ_RETVAL(1);
}
/*
 * Write segments of a single digit
 * Assumes digit is in range and the device information has been set up
 */
static void write_background(audio_data_t* data)
{
	iowrite8(data->write, WRITE(dev.virtbase) );
	dev.data = *data;
}

// static void write_hv(vga_ball_hv_t *hv){
// 	iowrite16(hv->h, H(dev.virtbase));
// 	iowrite16(hv->v, V(dev.virtbase));
// 	dev.hv = *hv;
// }
/*
 * Handle ioctl() calls from userspace:
 * Read or write the segments on single digits.
 * Note extensive error checking of arguments
 */
static long vga_ball_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	vga_ball_arg_t vla;
	switch (cmd) {
		case AUDIO_READ_SAMPLES:
			// Sleep the process until woken by the interrupt handler, and the data is ready
				printk("111\n");
			wait_event_interruptible_exclusive(wq, dev.ready.audio_ready);
				printk("113\n");

			// The data is now ready, send them to the user space
			vla.samples = dev.samples;
			audio_ready_t ready = { .audio_ready = 0 };
			dev.ready = ready;
			// Copy the data to the user space
			if (copy_to_user((vga_ball_arg_t *) arg, &vla,
					sizeof(vga_ball_arg_t))){
						return -EACCES;
					}
			break;
		case AUDIO_READ_SAMPLES_X:
			// Sleep the process until woken by the interrupt handler, and the data is ready
			
			// The data is now ready, send them to the user space
			vla.samples.l = read_samples_simple();
			// Copy the data to the user space
			if (copy_to_user((vga_ball_arg_t *) arg, &vla,
					sizeof(vga_ball_arg_t))){
						return -EACCES;
					}
			break;

	case VGA_BALL_WRITE_BACKGROUND:
		if (copy_from_user(&vla, (vga_ball_arg_t *) arg,
				   sizeof(vga_ball_arg_t)))
			return -EACCES;
		write_background(&vla.data);
		break;

	// case VGA_BALL_READ_BACKGROUND:
	//   	vla.background = dev.background;
	// 	if (copy_to_user((vga_ball_arg_t *) arg, &vla,
	// 			 sizeof(vga_ball_arg_t)))
	// 		return -EACCES;
	// 	break;
	// case VGA_BALL_WRITE_HV:
    //             if (copy_from_user(&vla, (vga_ball_arg_t *) arg,
    //                                sizeof(vga_ball_arg_t)))
    //                     return -EACCES;
    //             write_hv(&vla.hv);
    //             break;

    //     case VGA_BALL_READ_HV:
    //             vla.hv = dev.hv;
    //             if (copy_to_user((vga_ball_arg_t *) arg, &vla,
    //                              sizeof(vga_ball_arg_t)))
    //                     return -EACCES;
    //             break;


	default:
		return -EINVAL;
	}

	return 0;
}

/* The operations our device knows how to do */
static const struct file_operations vga_ball_fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl = vga_ball_ioctl,
};

/* Information about our device for the "misc" framework -- like a char dev */
static struct miscdevice vga_ball_misc_device = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= DRIVER_NAME,
	.fops		= &vga_ball_fops,
};

/*
 * Initialization code: get resources (registers) and display
 * a welcome message
 */
static int __init audio_probe(struct platform_device *pdev)
{
    //     vga_ball_color_t beige = { 0xf9, 0xe4, 0xb7 };
	// vga_ball_hv_t initial = {0x6, 0x6};
	int ret;
	printk(KERN_EMERG "Hello");
	/* Register ourselves as a misc device: creates /dev/vga_ball */
	ret = misc_register(&vga_ball_misc_device);

	/* Get the address of our registers from the device tree */
	ret = of_address_to_resource(pdev->dev.of_node, 0, &dev.res);
	if (ret) {
		ret = -ENOENT;
		goto out_deregister;
	}
	printk(KERN_EMERG "191\n");

	/* Make sure we can use these registers */
	if (request_mem_region(dev.res.start, resource_size(&dev.res),
			       DRIVER_NAME) == NULL) {
		ret = -EBUSY;
		goto out_deregister;
	}
	printk(KERN_EMERG "198\n");

	/* Arrange access to our registers */
	dev.virtbase = of_iomap(pdev->dev.of_node, 0);
	if (dev.virtbase == NULL) {
		ret = -ENOMEM;
		goto out_release_mem_region;
	}
	printk(KERN_EMERG "207\n");

	int irq = irq_of_parse_and_map(pdev->dev.of_node, 0);
	dev.irq_num = irq;
	printk(KERN_EMERG  "211: %d\n", irq);

	ret = request_irq(irq, (irq_handler_t) irq_handler, 0, "csee4840_audio", NULL);
	printk(KERN_EMERG "212: %d\n", ret);

	if (ret) {
		printk("request_irq err: %d", ret);
		ret = -ENOENT;
		goto out_deregister;
	}
		printk(KERN_EMERG "207\n");

        
	/* Set an initial color */
    //     write_background(&beige);
	// write_hv(&initial);

	return 0;

out_release_mem_region:
	release_mem_region(dev.res.start, resource_size(&dev.res));
out_deregister:
	misc_deregister(&vga_ball_misc_device);
	return ret;
}

/* Clean-up code: release resources */
static int audio_remove(struct platform_device *pdev)
{
	iounmap(dev.virtbase);
	release_mem_region(dev.res.start, resource_size(&dev.res));
	free_irq(dev.irq_num, NULL);
	misc_deregister(&vga_ball_misc_device);
	return 0;
}

/* Which "compatible" string(s) to search for in the Device Tree */
#ifdef CONFIG_OF
static const struct of_device_id audio_of_match[] = {
	{ .compatible = "csee4840,audio-1.0" },
	{},
};
MODULE_DEVICE_TABLE(of, audio_of_match);
#endif

/* Information for registering ourselves as a "platform" driver */
static struct platform_driver audio_driver = {
	.driver	= {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(audio_of_match),
	},
	.remove	= __exit_p(audio_remove),
};

/* Called when the module is loaded: set things up */
static int __init audio_init(void)
{
	pr_info(DRIVER_NAME ": init\n");
	return platform_driver_probe(&audio_driver, audio_probe);
}

/* Calball when the module is unloaded: release resources */
static void __exit audio_exit(void)
{
	platform_driver_unregister(&audio_driver);
	pr_info(DRIVER_NAME ": exit\n");
}

module_init(audio_init);
module_exit(audio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stephen A. Edwards, Columbia University");
MODULE_DESCRIPTION("VGA ball driver");