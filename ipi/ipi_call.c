
#include <linux/mm.h>
#include <linux/debugfs.h>
#include <linux/timer.h>
#include <linux/percpu.h>
#include <linux/version.h>
#include <linux/relay.h>
#include <linux/module.h>


static int loop_n = 100000;
static int64_t last_time;
static int last_target_cpu;
static int last_source_cpu;

module_param(loop_n, int, 0644);

static void ini_callback(void *info)
{
	return;
}

static ssize_t ipi_call_handle(struct file *file, const char __user *buff,
				 size_t count, loff_t *offset)
{
	struct timespec64 start, end;
	char cpu_num[20];
	int cpu;
	int64_t i;

	ssize_t len;

	len = simple_write_to_buffer(cpu_num, sizeof(cpu_num) - 1, offset,
				     buff, count);

	if (len < 0)
		return len;

	cpu_num[len] = 0;

	cpu = simple_strtol(cpu_num, NULL, 10);

	ktime_get_real_ts64(&start);
	for (i = 0; i < loop_n; ++i) {
		smp_call_function_single(cpu, ini_callback, NULL, true);
	}
	ktime_get_real_ts64(&end);

	last_time = (end.tv_sec - start.tv_sec) * 1000 * 1000 * 1000 + end.tv_nsec - start.tv_nsec;

	last_target_cpu = cpu;
	last_source_cpu = smp_processor_id();

	return count;
}


static ssize_t ipi_call_read_handle(struct file *f, char __user *ubuf, size_t count, loff_t *ppos)
{
	ssize_t n;
	char buf[50];

	n = snprintf(buf, sizeof(buf), "%d -> %d: %lld\n", last_source_cpu, last_target_cpu, last_time);

	return simple_read_from_buffer(ubuf, count, ppos, buf, n);
}



static const struct file_operations fops = {
	.owner      = THIS_MODULE,
	.write      = ipi_call_handle,
	.read       = ipi_call_read_handle,
};

struct dentry *d;

static int __init ipi_call_init(void)
{
	d = debugfs_create_file("ipi_call", 0600, NULL, NULL, &fops);
	if (!d) {
		pr_err("ipi_call init: register ipi_call fail!\n");
		return -1;
	}

	return 0;
}

static void __exit ipi_call_exit(void)
{
	debugfs_remove(d);
}

module_init(ipi_call_init);
module_exit(ipi_call_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("IPI Call");
