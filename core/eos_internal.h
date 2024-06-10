/********************************************************
 * Filename: core/eos_internal.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 04/07/24
 *
 * Description: Internal header for eOS
 ********************************************************/

#ifndef EOS_INTERNAL_H
#define EOS_INTERNAL_H

#include <stdio.h>
#include <stdarg.h>
#include <hal/linux/type.h>
#include <hal/linux/emulator.h>


/********************************************************
 * Initialization module
 ********************************************************/

void _os_init();		// Initialize OS
void _os_init_hal();		// Initialize HAL
void _os_init_icb_table();	// Initialize ICB table structure
void _os_init_scheduler();	// Initialize bitmap scheduler module
void _os_init_task();		// Initialize task management module
void _os_init_timer();		// Initialize timer management module


/********************************************************
 * Common utility module
 ********************************************************/

/* Common structure for list element */
typedef struct _os_node {
    struct _os_node *previous;
    struct _os_node *next;
    void *ptr_data;
    int32u_t priority;
} _os_node_t;

/* Adds the specified node at the end (tail) of the list */
void _os_add_node_tail(_os_node_t **head, _os_node_t *new_node);

/* Adds the specified node by its priority */
void _os_add_node_priority(_os_node_t **head, _os_node_t *new_node);

/* Removes a node from the list */
int32u_t _os_remove_node(_os_node_t **head, _os_node_t *node);

/* Formatted output conversion */
int32s_t vsprintf(char *buf, const char *fmt, va_list args);

void _os_serial_puts(const char *s);


/********************************************************
 * Interrupt management module
 ********************************************************/

/* Maximum number of IRQs */
#define IRQ_MAX 32

/* The common interrupt handler:
 * 	Invoked by HAL whenever an interrupt occurrs.
 */
void _os_common_interrupt_handler(int32u_t);


/********************************************************
 * Timer management module
 ********************************************************/

void _os_init_timer();


/********************************************************
 * Task management module
 ********************************************************/

void _os_wait(_os_node_t **wait_queue);
void _os_wakeup_single(_os_node_t **wait_queue, int32u_t suspend_type);
void _os_wakeup_all(_os_node_t **wait_queue, int32u_t suspend_type);
void _os_wakeup_sleeping_task(void *arg);


/********************************************************
 * Scheduler module
 ********************************************************/

#define LOWEST_PRIORITY		63
#define MEDIUM_PRIORITY		32
#define READY_TABLE_SIZE	(LOWEST_PRIORITY / 8 + 1)
#define LOCKED			0
#define UNLOCKED		1

/* Scheduler lock */
extern int8u_t _os_scheduler_lock;

int8u_t _os_lock_scheduler(void);

void _os_restore_scheduler(int8u_t);

/* Gets the highest-prioity task from the ready list */
int32u_t _os_get_highest_priority();

/* Sets priority bit in the ready list to 0 */
void _os_unset_ready(int8u_t priority);

/* Sets priority bit in the ready list to 1 */
void _os_set_ready(int8u_t priority);


/********************************************************
 * Hardware abstraction module
 ********************************************************/

/**
 * Creates an initial context on the stack
 * Its stack_pointer is the highest memory address of the stack area
 * "stack_size" is the size of the stack area
 * This function returns task context that it created
 * 
 * When this context is resumed, "entry" is called with the argument "arg"
 */
addr_t _os_create_context(addr_t stack_base, size_t stack_size, void (*entry)(void *arg), void *arg);

/**
 * Saves the context of the current task onto stack
 * This method returns in two different ways:
 *     First, it returns after saving the context onto the stack;
 *         it returns the address of the saved context
 *     Second, it returns via the saved task
 *         it returns 0 (NULL)
 */
addr_t _os_save_context();

/**
 * Restores CPU registers of a given context
 */
void _os_restore_context(addr_t sp);

#endif /* EOS_INTERNAL_H */
