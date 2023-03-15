#ifndef __GENERIC_SCHEDULER_H__
#define __GENERIC_SCHEDULER_H__

#include "flux.h"

#include "arch/x64/structures.h"
#include <stddef.h>

#ifdef __ARCH_X64__
    #include "arch/x64/thread.h"
#endif

enum sched_state {
    RUNNING,
    BLOCKED,
    READY,
};

struct task {
    int present; // Is this entry present in the tasks table?

    /* Scheduling Data */
    enum sched_state state; // The running state of the schedulable
    long long unsigned int elapsed_total; // The total amount of CPU time this schedulable has got
    long long unsigned int elapsed_since_run; // The time elapsed since it was last running

#ifdef __ARCH_X64__
    struct processor_regs regs;
    pml4_entry_t *pml4_table;
    unsigned short int privilege_level;
#endif

    // This is where in the future, other values such as the user that ran it, its working directory,
    // and its open file descriptors, will go.
};

typedef int task_id;

#define N_TASKS 128

extern struct task tasks[N_TASKS];

/**
 * @brief The currently executing task.
 * -1 if no task is executing at the moment, otherwise an index into the task
 * table, `tasks`.
 * Of course just because this is not -1, the task may not currently be "on the
 * cpu", since kernel code might be running, handling an interrupt or something.
 */
extern task_id current_task;

size_t new_task();

size_t copy_task();

void scheduler_init();

/**
 * @brief Call the scheduler to preempt the running task.
 * This often will be called, for example, by the timer interrupt, so that
 * tasks can be preempted frequently, for multiprocessing. Also will be called
 * by the kernel logic when resources are made available that processes may be
 * waiting for.
 * When this is called, the scheduler will decide which task should get to run
 * next, unload the last task, switch to the new task, and also update the 
 * relevant task structures to reflect their running times and state.
 */
void scheduler_preempt();

#endif /* __GENERIC_SCHEDULER_H__ */