#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <asm/msr.h>

#define NUM_CHA 28
#define NUM_COUNTERS 4
#define NUM_CONTROLLERS 4
#define NUM_ITERATIONS 100000

static const u32 msr_addresses[NUM_COUNTERS + NUM_CONTROLLERS][NUM_CHA] = {
    // ctr0 => CHA 0-27
    {0x0E08, 0x0E18, 0x0E28, 0x0E38, 0x0E48, 0x0E58, 0x0E68, 0x0E78, 0x0E88, 0x0E98, 0x0EA8, 0x0EB8, 0x0EC8, 0x0ED8, 0x0EE8, 0x0EF8, 0x0F08, 0x0F18, 0x0F28, 0x0F38, 0x0F48, 0x0F58, 0x0F68, 0x0F78, 0x0F88, 0x0F98, 0x0FA8, 0x0FB8},
    // ctr1 => CHA 0-27
    {0x0E09, 0x0E19, 0x0E29, 0x0E39, 0x0E49, 0x0E59, 0x0E69, 0x0E79, 0x0E89, 0x0E99, 0x0EA9, 0x0EB9, 0x0EC9, 0x0ED9, 0x0EE9, 0x0EF9, 0x0F09, 0x0F19, 0x0F29, 0x0F39, 0x0F49, 0x0F59, 0x0F69, 0x0F79, 0x0F89, 0x0F99, 0x0FA9, 0x0FB9},
    // ctr2 => CHA 0-27
    {0x0E0A, 0x0E1A, 0x0E2A, 0x0E3A, 0x0E4A, 0x0E5A, 0x0E6A, 0x0E7A, 0x0E8A, 0x0E9A, 0x0EAA, 0x0EBA, 0x0ECA, 0x0EDA, 0x0EEA, 0x0EFA, 0x0F0A, 0x0F1A, 0x0F2A, 0x0F3A, 0x0F4A, 0x0F5A, 0x0F6A, 0x0F7A, 0x0F8A, 0x0F9A, 0x0FAA, 0x0FBA},
    // ctr3 => CHA 0-27
    {0x0E0B, 0x0E1B, 0x0E2B, 0x0E3B, 0x0E4B, 0x0E5B, 0x0E6B, 0x0E7B, 0x0E8B, 0x0E9B, 0x0EAB, 0x0EBB, 0x0ECB, 0x0EDB, 0x0EEB, 0x0EFB, 0x0F0B, 0x0F1B, 0x0F2B, 0x0F3B, 0x0F4B, 0x0F5B, 0x0F6B, 0x0F7B, 0x0F8B, 0x0F9B, 0x0FAB, 0x0FBB},
    // ctrl0 => CHA 0-27
    {0x0E01, 0x0E11, 0x0E21, 0x0E31, 0x0E41, 0x0E51, 0x0E61, 0x0E71, 0x0E81, 0x0E91, 0x0EA1, 0x0EB1, 0x0EC1, 0x0ED1, 0x0EE1, 0x0EF1, 0x0F01, 0x0F11, 0x0F21, 0x0F31, 0x0F41, 0x0F51, 0x0F61, 0x0F71, 0x0F81, 0x0F91, 0x0FA1, 0x0FB1},
    // ctrl1 => CHA 0-27
    {0x0E02, 0x0E12, 0x0E22, 0x0E32, 0x0E42, 0x0E52, 0x0E62, 0x0E72, 0x0E82, 0x0E92, 0x0EA2, 0x0EB2, 0x0EC2, 0x0ED2, 0x0EE2, 0x0EF2, 0x0F02, 0x0F12, 0x0F22, 0x0F32, 0x0F42, 0x0F52, 0x0F62, 0x0F72, 0x0F82, 0x0F92, 0x0FA2, 0x0FB2},
    // ctrl2 => CHA 0-27
    {0x0E03, 0x0E13, 0x0E23, 0x0E33, 0x0E43, 0x0E53, 0x0E63, 0x0E73, 0x0E83, 0x0E93, 0x0EA3, 0x0EB3, 0x0EC3, 0x0ED3, 0x0EE3, 0x0EF3, 0x0F03, 0x0F13, 0x0F23, 0x0F33, 0x0F43, 0x0F53, 0x0F63, 0x0F73, 0x0F83, 0x0F93, 0x0FA3, 0x0FB3},
    // ctrl3 => CHA 0-27
    {0x0E04, 0x0E14, 0x0E24, 0x0E34, 0x0E44, 0x0E54, 0x0E64, 0x0E74, 0x0E84, 0x0E94, 0x0EA4, 0x0EB4, 0x0EC4, 0x0ED4, 0x0EE4, 0x0EF4, 0x0F04, 0x0F14, 0x0F24, 0x0F34, 0x0F44, 0x0F54, 0x0F64, 0x0F74, 0x0F84, 0x0F94, 0x0FA4, 0x0FB4}
};

static void setup_cha_controller(void)
{
    int i;
    u64 event_select = 0x34; // LLC_LOOKUP event
    u64 umask = 0x11; // DATA_READ filter
    u64 config_value = (umask << 8) | event_select;

    printk(KERN_INFO "Setting up CHA controllers to monitor LLC_LOOKUP event\n");

    for (i = 0; i < NUM_CHA; i++) {
        // Configure the CHA controller
        native_write_msr(msr_addresses[4][i], (u32)config_value, (u32)(config_value >> 32));
        // Reset and enable the counter
        native_write_msr(msr_addresses[0][i], 0, 0); // Reset counter
        native_write_msr(msr_addresses[5][i], (1ULL << 22), 0); // Enable counter
        // hmm might need to set up FILTER0 and FILTER1 here as well
    }

    printk(KERN_INFO "Event Code: 0x%llX, Umask: 0x%llX\n", (unsigned long long)event_select, (unsigned long long)umask);
    printk(KERN_INFO "Umask - DATA_READ: 0x%llX, ANY: 0x%llX, LOCAL: 0x%llX\n", 0x01ULL, 0x01ULL, 0x01ULL);
    printk(KERN_INFO "Umask - REMOTE_SNOOP: 0x%llX, REMOTE: 0x%llX\n", 0x01ULL, 0x01ULL);
    printk(KERN_INFO "Max Threshold: %u, Register Restriction: 0-3 (Intel SDM, Volume 4, Table 2-95)\n", 0);
    printk(KERN_INFO "Filter Registers: FILTER0: 0x%llX, FILTER1: 0x%llX (Intel SDM, Volume 4, Table 2-96)\n", 0x0000000101ULL, 0x0000000101ULL);
}

static void perform_memory_task(void)
{
    volatile long long *ptr;
    int i;

    ptr = (volatile long long *)kmalloc(sizeof(long long), GFP_KERNEL);
    if (!ptr) {
        printk(KERN_ERR "Failed to allocate memory\n");
        return;
    }

    printk(KERN_INFO "Performing memory-related task\n");

    for (i = 0; i < NUM_ITERATIONS; i++) {
        ++(*ptr);
        asm volatile("mfence" ::: "memory");
        asm volatile("clflush (%0)" :: "r"(ptr) : "memory");
        asm volatile("mfence" ::: "memory");
    }

    kfree((void *)ptr);
}

static void read_cha_counters(void)
{
    int i, j;
    u64 msr_value;

    printk(KERN_INFO "Reading CHA counters for LLC_LOOKUP event\n");

    for (i = 0; i < NUM_COUNTERS; i++) {
        printk(KERN_INFO "Counter %d (ctr%d):\n", i, i);
        for (j = 0; j < NUM_CHA; j++) {
            msr_value = native_read_msr(msr_addresses[i][j]);
            printk(KERN_INFO "CHA %2d | MSR 0x%04X: 0x%016llX\n", j, msr_addresses[i][j], msr_value);
        }
        printk(KERN_INFO "\n");
    }
}

static int __init cha_msr_reader_init(void)
{
    printk(KERN_INFO "CHA MSR Reader: Initializing\n");
    printk(KERN_INFO "=================================\n");

    setup_cha_controller();
    perform_memory_task();
    read_cha_counters();

    printk(KERN_INFO "=================================\n");
    printk(KERN_INFO "CHA MSR Reader: Completed\n");

    return 0;
}

static void __exit cha_msr_reader_exit(void)
{
    printk(KERN_INFO "CHA MSR Reader: Exiting\n");
}

module_init(cha_msr_reader_init);
module_exit(cha_msr_reader_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mani Tofigh");
MODULE_DESCRIPTION("A kernel module to read Cache/Home Agent MSRs on Intel processors");
