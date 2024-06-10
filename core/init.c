/********************************************************
 * Filename: core/init.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 04/07/24
 *
 * Description: Perform OS initialization
 ********************************************************/

#include <core/eos.h>

static void _os_idle_task(void *arg);	// idle task
static eos_tcb_t idle_task;		// tcb for the idle task
static int8u_t idle_stack[8096];	// stack for the idle task


/*
 * This function is called by HAL after initializing H/W
 */
void _os_init() {
    // Interrupts and preemption must be disabled during initialziation
    hal_disable_interrupt();
    _os_scheduler_lock = LOCKED;

    // Initializes subsystems
    _os_init_hal();
    _os_init_icb_table();
    _os_init_scheduler();
    _os_init_task();
    _os_init_timer();

    // Creates an idle task
    PRINT("Creating an idle task\n");
    eos_create_task(&idle_task, (int32u_t *)idle_stack, 8096,
		    _os_idle_task, NULL, LOWEST_PRIORITY);

    // After finishing initialization, calls eos_user_main()
    void eos_user_main();
    eos_user_main();

    // Starts multitasking by enabling preemption and interrupts
    PRINT("Starts multitasking\n");
    _os_scheduler_lock = UNLOCKED;
    hal_enable_interrupt();

    // Permanently gives control to the tasks in the ready queue
    // The idle task runs when the ready queue is effectively empty
    eos_schedule();

    // Control never reaches here except Project 1
    while (1) { }
}


static void _os_idle_task(void *arg)
{
    while (1) { }
}
