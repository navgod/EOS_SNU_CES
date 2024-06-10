/********************************************************
 * Filename: core/sync.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/31/24
 *
 * Description: Routines for semaphores and condition variables
 ********************************************************/

#include <core/eos.h>

void eos_init_semaphore(eos_semaphore_t *sem, int32u_t initial_count, int8u_t queue_type)
{
    // To be filled by students: Project 4
    sem->count = initial_count;
    sem->wait_queue = NULL;
    sem->queue_type = queue_type;
}

// 0 for fail 1 for success
int32u_t eos_acquire_semaphore(eos_semaphore_t *sem, int32s_t timeout)
{
    // To be filled by students: Project 4
    int32u_t flag = hal_disable_interrupt();
    eos_tcb_t *current_task = eos_get_current_task();
    while (1)
    {
        if (sem->count > 0) {
        sem->count--;
        hal_restore_interrupt(flag);
        return 1;
        }
        else {
            if (timeout == -1)
            {
                hal_restore_interrupt(flag);
                return 0;
            }
            else if (timeout == 0)
            {
                if (sem->queue_type == FIFO)
                    _os_add_node_tail(&sem->wait_queue, &current_task->wait_queue);
                else
                    _os_add_node_priority(&sem->wait_queue, &current_task->wait_queue);
                hal_restore_interrupt(flag);
                eos_sleep(0);
            }
            else {
                if (sem->queue_type == FIFO)
                    _os_add_node_tail(&sem->wait_queue, &current_task->wait_queue);
                else
                    _os_add_node_priority(&sem->wait_queue, &current_task->wait_queue);
                hal_restore_interrupt(flag);
                eos_sleep(timeout);
            }
        }
    }
}


void eos_release_semaphore(eos_semaphore_t *sem)
{
    // To be filled by students: Project 4
    int32u_t flag = hal_disable_interrupt();
    eos_tcb_t *next_task = NULL;
    sem->count++;
    if(sem->wait_queue != NULL) {
        next_task = sem->wait_queue->ptr_data;
    }
    if (next_task != NULL) {
        _os_remove_node(&sem->wait_queue, &next_task->wait_queue);
        if (next_task->task_state != 1) 
            _os_wakeup_sleeping_task(next_task);
    }
    hal_restore_interrupt(flag);
}


/**
 * Condition variables are not covery in the OS course
 */

void eos_init_condition(eos_condition_t *cond, int32u_t queue_type)
{
    cond->wait_queue = NULL;
    cond->queue_type = queue_type;
}


void eos_wait_condition(eos_condition_t *cond, eos_semaphore_t *mutex)
{
    /* Releases acquired semaphore */
    eos_release_semaphore(mutex);
    /* Waits on condition's wait_queue */
    _os_wait(&cond->wait_queue);
    /* Acquires semaphore before returns */
    eos_acquire_semaphore(mutex, 0);
}


void eos_notify_condition(eos_condition_t *cond)
{
    /* Selects a task in wait_queue and wake it up */
    _os_wakeup_single(&cond->wait_queue, cond->queue_type);
}

int8u_t eos_lock_scheduler() {
    int32u_t flag = hal_disable_interrupt();
    int8u_t temp = _os_scheduler_lock;
    _os_scheduler_lock = LOCKED;
    hal_restore_interrupt(flag);
    return temp;
}

void eos_restore_scheduler(int8u_t scheduler_state) {
    int32u_t flag = hal_disable_interrupt();
    _os_scheduler_lock = scheduler_state;
    hal_restore_interrupt(flag);
    eos_schedule();
}

int8u_t eos_get_scheduler_lock() {
    return _os_scheduler_lock;
}