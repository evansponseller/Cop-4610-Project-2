#include <linux/init.h> /* declares module initialization and cleanup macros. */
#include <linux/module.h> /* provides metadata and module lifecycle support. */
#include <linux/mutex.h> /* protects the saved timestamp from concurrent readers. */
#include <linux/proc_fs.h> /* provides the proc entry interface. */
#include <linux/timekeeping.h> /* provides ktime_get_real_ts64(). */
#include <linux/uaccess.h> /* provides the user-buffer types used by proc reads. */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cop4610");
MODULE_DESCRIPTION("reports current and elapsed real time through procfs");

#define TIMER_ENTRY_NAME "timer" /* names the proc entry created by this module. */
#define TIMER_ENTRY_MODE 0444 /* allows all users to read the timer entry. */
#define TIMER_BUFFER_SIZE 256 /* keeps the formatted response safely bounded. */

static struct proc_dir_entry *timer_entry; /* stores the proc entry for cleanup. */
static struct timespec64 previous_time; /* stores the timestamp from the prior read. */
static bool previous_time_valid; /* records whether an elapsed interval exists yet. */
static DEFINE_MUTEX(timer_lock); /* serializes timestamp access and response formatting. */

static ssize_t timer_read(struct file *file, char __user *user_buffer,
			  size_t count, loff_t *position)
{
	struct timespec64 current_time; /* holds the timestamp captured for this read. */
	struct timespec64 elapsed_time; /* holds the interval since the prior read. */
	char response[TIMER_BUFFER_SIZE]; /* receives the text copied to user space. */
	int response_length; /* stores the number of formatted response bytes. */

	if (*position != 0) /* makes the proc response end after one complete read. */
		return 0;

	mutex_lock(&timer_lock); /* prevents readers from racing over saved timestamps. */
	ktime_get_real_ts64(&current_time); /* captures seconds and nanoseconds since epoch. */

	if (previous_time_valid) { /* reports elapsed time only after the first read. */
		elapsed_time = timespec64_sub(current_time, previous_time); /* subtracts the two timestamps. */
		response_length = scnprintf(response, sizeof(response),
					    "current time: %lld.%09ld\nelapsed time: %lld.%09ld\n",
					    (long long)current_time.tv_sec,
					    current_time.tv_nsec,
					    (long long)elapsed_time.tv_sec,
					    elapsed_time.tv_nsec); /* formats current and elapsed time. */
	} else { /* gives the first read a current-time-only response. */
		response_length = scnprintf(response, sizeof(response),
					    "current time: %lld.%09ld\n",
					    (long long)current_time.tv_sec,
					    current_time.tv_nsec); /* formats the initial timestamp. */
	}

	previous_time = current_time; /* makes this read the baseline for the next interval. */
	previous_time_valid = true; /* enables elapsed-time output on later reads. */
	mutex_unlock(&timer_lock); /* releases the shared timestamp state after formatting. */

	return simple_read_from_buffer(user_buffer, count, position,
				       response, response_length); /* copies the response to user space. */
}

static const struct proc_ops timer_proc_ops = { /* defines operations for /proc/timer. */
	.proc_read = timer_read, /* connects proc reads to the timer handler. */
};

static int __init my_timer_init(void)
{
	previous_time_valid = false; /* starts without an elapsed interval. */
	timer_entry = proc_create(TIMER_ENTRY_NAME, TIMER_ENTRY_MODE, NULL,
				  &timer_proc_ops); /* creates /proc/timer. */
	if (timer_entry == NULL) /* reports allocation failure to the module loader. */
		return -ENOMEM;

	return 0; /* reports successful module initialization. */
}

static void __exit my_timer_exit(void)
{
	proc_remove(timer_entry); /* removes /proc/timer before unloading the module. */
}

module_init(my_timer_init); /* registers the module load callback. */
module_exit(my_timer_exit); /* registers the module unload callback. */
