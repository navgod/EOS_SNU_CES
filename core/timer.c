/********************************************************
 * Filename: core/timer.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/30/24
 *
 * Description: User interface for handling interrupts
 ********************************************************/

#include <core/eos.h>

static eos_counter_t system_timer;


int8u_t eos_init_counter(eos_counter_t *counter, int32u_t init_value)
{
    counter->tick = init_value;
    counter->alarm_queue = NULL;

    return 0;
}


void eos_set_alarm(eos_counter_t* counter, eos_alarm_t* alarm, int32u_t timeout, void (*entry)(void *arg), void *arg)
{
    // To be filled by students: Project 3
    // 1. Remove the alarm from the counter's alarm queue if it exists
    _os_node_t* current_node = counter->alarm_queue;
    _os_node_t* first_node = counter->alarm_queue;
    if (current_node != NULL && alarm == current_node->ptr_data)
    {
        _os_remove_node(&counter->alarm_queue, current_node);
    }
    while (current_node != NULL || current_node != 0)
    {   
        if (current_node->next != NULL)
        {
            current_node = current_node->next;
        }
        
        if (current_node->ptr_data != NULL) {
            eos_alarm_t* current_alarm = current_node->ptr_data;
            if (current_alarm == alarm)
            {
                _os_remove_node(&counter->alarm_queue, current_node);
                break;
            }
        }
        if (current_node != NULL) {
            if (current_node == first_node)
            {
            break;
            }
        }
    }
    // 2. If timeout is 0 or entry is NULL, return
    if (timeout == 0 || entry == NULL) {
        return;
    }

    alarm->timeout = timeout;
    alarm->handler = entry;
    alarm->arg = arg;
    alarm->alarm_queue_node.ptr_data = alarm;
    alarm->alarm_queue_node.priority = timeout;

    _os_add_node_priority(&counter->alarm_queue, &alarm->alarm_queue_node);
}


eos_counter_t* eos_get_system_timer()
{
    return &system_timer;
}


void eos_trigger_counter(eos_counter_t* counter)
{
    PRINT("tick\n");
    // To be filled by students: Project 3
    counter->tick++;

    _os_node_t *current_node = counter->alarm_queue;
    eos_alarm_t *current_alarm = current_node->ptr_data;
    while (current_node != NULL)
    {
        current_alarm->timeout--;
        if (current_alarm->timeout == 0)
        {
            current_alarm->handler(current_alarm->arg);
        }
        current_node = current_node->next;
        current_alarm = current_node->ptr_data;
        if (current_node != NULL){
            if (current_node == counter->alarm_queue)
            {
                break;
            }
        }
    }
}


/* Timer interrupt handler */
static void timer_interrupt_handler(int8s_t irqnum, void *arg)
{
    /* Triggers alarms */
    eos_trigger_counter(&system_timer);
}


void _os_init_timer()
{
    eos_init_counter(&system_timer, 0);

    /* Registers timer interrupt handler */
    eos_set_interrupt_handler(IRQ_INTERVAL_TIMER0,
		    timer_interrupt_handler, NULL);
}
