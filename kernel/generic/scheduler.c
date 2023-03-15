#include "arch/x64/mem.h"
#include "arch/x64/structures.h"
#include "arch/x64/thread.h"
#include "arch/x64/paging.h"
#include "flux.h"

#include "generic/scheduler.h"
#include "generic/debug.h"
#include "generic/printk.h"
#include <stddef.h>

struct task tasks[N_TASKS];
task_id current_task;

static struct processor_regs default_regs_userspace = {
    .rip = 0,
    .rax = 0, .rbx = 0, .rcx = 0, .rdx = 0,
    .rsi = 0, .rdi = 0, 
    .rsp = 0, .rbp = 0, 
    .r8  = 0, .r9  = 0, .r10 = 0, .r11 = 0, 
    .r12 = 0, .r13 = 0, .r14 = 0, .r15 = 0, 
    
    .cs = INIT_SEGMENT_SELECTOR(3, 0, 3),
    .ds = INIT_SEGMENT_SELECTOR(3, 0, 4),
    .ss = INIT_SEGMENT_SELECTOR(3, 0, 4),
    .es = INIT_SEGMENT_SELECTOR(3, 0, 4),
    .fs = INIT_SEGMENT_SELECTOR(3, 0, 4),
    .gs = INIT_SEGMENT_SELECTOR(3, 0, 4),

    .rflags = 0,
    .cr3 = 0,
};

static struct processor_regs default_regs_kernel = {
    .rip = 0,
    .rax = 0, .rbx = 0, .rcx = 0, .rdx = 0,
    .rsi = 0, .rdi = 0, 
    .rsp = 0, .rbp = 0, 
    .r8  = 0, .r9  = 0, .r10 = 0, .r11 = 0, 
    .r12 = 0, .r13 = 0, .r14 = 0, .r15 = 0, 
    
    .cs = INIT_SEGMENT_SELECTOR(0, 0, 3),
    .ds = INIT_SEGMENT_SELECTOR(0, 0, 4),
    .ss = INIT_SEGMENT_SELECTOR(0, 0, 4),
    .es = INIT_SEGMENT_SELECTOR(0, 0, 4),
    .fs = INIT_SEGMENT_SELECTOR(0, 0, 4),
    .gs = INIT_SEGMENT_SELECTOR(0, 0, 4),

    .rflags = 0,
    .cr3 = 0,
};

static void switch_to_task(task_id next_task) {
    /*  To switch to a task we need to do the following:
            - Save the current registers into the currently running task's
                struct.
            - Set the current task variable to the next task, and load its
                registers into the CPU.
            - */
    
}

size_t new_task() {
    struct task *tsk = NULL;

    for (size_t i = 0; i < N_TASKS; i++) {
        if (!tasks[i].present) {
            tsk = &tasks[i];
            break;
        }
    }

    if (!tsk) {
        WARN("Failed to find free space for new task!\n");
        return -1;
    }

    tsk->present = 1;
    tsk->state = READY;
    tsk->elapsed_total = 0;
    tsk->elapsed_since_run = 0;

#ifdef __ARCH_X64__
    tsk->pml4_table = (pml4_entry_t*)get_phys_block();
    tsk->privilege_level = 3;
    tsk->regs = default_regs_userspace;
#endif
}

void scheduler_init() {
    default_regs_kernel.cr3 = CR3_BASE_ADDR((flux_phyaddr)kernel_pml4_table);

    current_task = -1;

    // Set all entries in the task table to not present, since at this point
    // there are no available.
    for (task_id i = 0; i < N_TASKS; i++) {
        tasks[i].present = 0;
    }

    printk(SYSTEM_SCHED "Initialised task scheduler.\n");
}


void scheduler_preempt() {
    
}