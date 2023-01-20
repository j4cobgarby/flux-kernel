#include "flux.h"

#include "generic/scheduler.h"
#include "generic/debug.h"
#include "generic/printk.h"

struct task tasks[N_TASKS];
task_id current_task;

static void switch_to_task(task_id next_task) {

}

void scheduler_init() {
    current_task = -1;

    // Set all entries in the task table to not present, since at this point
    // there are no available.
    for (task_id i = 0; i < N_TASKS; i++) {
        tasks[i].present = 0;
    }

    printk(SCHED "Initialised task scheduler.\n");
}


void scheduler_preempt() {
    
}