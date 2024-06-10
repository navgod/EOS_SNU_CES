/********************************************************
 * Filename: core/task.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 04/06/24
 *
 * Description: task creation and scheduling
 ********************************************************/

#include <core/eos.h>

#define READY		1
#define RUNNING		2
#define WAITING		3
#define SUSPENDED       4

/**
 * Runqueue of ready tasks
 */
static _os_node_t *_os_ready_queue[LOWEST_PRIORITY + 1];

/**
 * Pointer to TCB of the running task
 */
static eos_tcb_t *_os_current_task;


int32u_t eos_create_task(eos_tcb_t *task, addr_t sblock_start, size_t sblock_size, void (*entry)(void *arg), void *arg, int32u_t priority)
{
    PRINT("task: 0x%x, priority: %d\n", (int32u_t)task, priority);
    // To be filled by students: Projects 2 and 3
    addr_t context_ptr = _os_create_context(sblock_start, sblock_size, entry, arg);
    // to check
    //print_context(context_ptr);

    task->task_state = READY;
    task->priority = priority;
    task->sp = context_ptr;

    task->node = (_os_node_t *)malloc(sizeof(_os_node_t));
    task->node->ptr_data = task;
    task->node->priority = priority;

    task->alarm.alarm_queue_node.ptr_data = &task->alarm;
    task->alarm.handler = entry;
    task->alarm.arg = arg;

    _os_add_node_priority(&_os_ready_queue[priority], task->node);
    _os_set_ready(priority);
    
    return 0;
}


int32u_t eos_destroy_task(eos_tcb_t *task) {
    // To be filled by students: not covered
}


void eos_schedule() {
    // To be filled by students: Projects 2 and 3
    eos_tcb_t *current_task = eos_get_current_task();

    if (current_task != NULL)
    {
        addr_t old_task_sp = _os_save_context();

        if (old_task_sp == NULL) return;
        current_task->sp = old_task_sp;
        if (current_task->task_state == RUNNING) {
            current_task->task_state = READY;
            _os_add_node_priority(&_os_ready_queue[current_task->priority], current_task->node);
            _os_set_ready(current_task->priority);
        }
    }
    int32u_t next_priority = _os_get_highest_priority();
    
    _os_node_t *next_node = _os_ready_queue[next_priority];
    if (next_node != NULL) {
        eos_tcb_t *next_task = next_node->ptr_data; //get tcb
        _os_remove_node(&_os_ready_queue[next_priority], next_node);
        next_task->task_state = RUNNING;
        if (_os_ready_queue[next_task->priority] == NULL) {
            _os_unset_ready(next_task->priority);
        }
        _os_current_task = next_task;
        _os_restore_context(next_task->sp);
    }
    else {
        PRINT("No task to run\n");
    }
}


eos_tcb_t *eos_get_current_task() {
	return _os_current_task;
}


void eos_change_priority(eos_tcb_t *task, int32u_t priority) {
    // To be filled by students: not covered
}


int32u_t eos_get_priority(eos_tcb_t *task) {
    // To be filled by students: not covered
}


void eos_set_period(eos_tcb_t *task, int32u_t period){
    // To be filled by students: Project 3
    task->period = period;
}


int32u_t eos_get_period(eos_tcb_t *task) {
    // To be filled by students: not covered
}


int32u_t eos_suspend_task(eos_tcb_t *task) {
    // To be filled by students: not covered
}


int32u_t eos_resume_task(eos_tcb_t *task) {
    // To be filled by students: not covered
}


void eos_sleep(int32u_t tick) {
    // To be filled by students: Project 3
    eos_tcb_t *current_task = eos_get_current_task();
    eos_counter_t *timer = eos_get_system_timer();

    if (tick == 0) {
        if (current_task->period > 0)
                {
                    eos_set_alarm(timer,&current_task->alarm, current_task->period, _os_wakeup_sleeping_task, current_task);
                }
                else {
                    eos_set_alarm(timer,&current_task->alarm, __INT_MAX__, _os_wakeup_sleeping_task, current_task);
                }
    }
    else {
        eos_set_alarm(timer,&current_task->alarm, tick, _os_wakeup_sleeping_task, current_task);
    }
    current_task->task_state = WAITING;
    eos_schedule();
}


void _os_init_task() {
    PRINT("Initializing task module\n");

    /* Initializes current_task */
    _os_current_task = NULL;

    /* Initializes multi-level ready_queue */
    for (int32u_t i = 0; i < LOWEST_PRIORITY; i++) {
        _os_ready_queue[i] = NULL;
    }
}


void _os_wait(_os_node_t **wait_queue) {
    // To be filled by students: not covered
}


void _os_wakeup_single(_os_node_t **wait_queue, int32u_t queue_type) {
    // To be filled by students: not covered
}


void _os_wakeup_all(_os_node_t **wait_queue, int32u_t queue_type) {
    // To be filled by students: not covered
}


void _os_wakeup_sleeping_task(void *arg) {
    // To be filled by students: Project 3
    eos_tcb_t* task = (eos_tcb_t*)arg;
    task->task_state = READY;
    _os_add_node_priority(&_os_ready_queue[task->priority], task->node);
    _os_set_ready(task->priority);
    eos_schedule();
}
