/* Assignment 1. on page P-7
 * A module that creates a proc entry jiffies that reports the current value of jiffies when read, such as:
 * cat /proc/jiffies
 * 24386
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <asm/uaccess.h>
#include <asm/param.h>


#define BUFFER_SIZE 128
#define PROC_NAME "jiffies"

static ssize_t jiffies_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

static const struct proc_ops jiffies_proc_ops = {
    .proc_read = jiffies_read
};

static int jiffies_init(void) {
    printk(KERN_INFO "Creating /proc/%s\n", PROC_NAME);

    proc_create(PROC_NAME, 0666, NULL, &jiffies_proc_ops);

    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
    return 0;
}

static void jiffies_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);

    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

static ssize_t jiffies_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int rv = 0;
    static int completed = 0;
    static unsigned long jiffies_last = 0;
    char buffer[BUFFER_SIZE];
    unsigned long jiffies_now;
    unsigned long seconds_since_last;

    if (completed) {
        completed = 0;
        return 0;
    }
    completed = 1;

    jiffies_now = jiffies;
    seconds_since_last = (jiffies_now - jiffies_last) / HZ;
    if (jiffies_last == 0) {
        seconds_since_last = ~((long)0);
    }

    rv = sprintf(buffer, "jiffies: %lu\nHZ: %d\nSeconds since last call: %lu sec\n", jiffies_now, HZ, seconds_since_last);

    copy_to_user(usr_buf, buffer, rv);

    jiffies_last = jiffies_now;

    return rv;
    
}


module_init(jiffies_init);
module_exit(jiffies_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello Module");
MODULE_AUTHOR("SGG");

