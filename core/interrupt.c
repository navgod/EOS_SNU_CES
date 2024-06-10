/********************************************************
 * Filename: core/interrupt.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/30/24
 *
 * Description: User interface for handling interrupts
 ********************************************************/

#include <core/eos.h>

/**
 * ICB structure that represents an in-kernel status of an irq
 */
typedef struct icb {
    int8s_t irqnum;				// irq number
    void (*handler)(int8s_t irqnum, void *arg);	// the handler function
    void *arg;   // argument given to the handler when interrupt occurs
} _os_icb_t;

/**
 * Table of ICBs for all interrupts
 */
_os_icb_t _os_icb_table[IRQ_MAX];


void _os_init_icb_table()
{
    PRINT("Initializing interrupt module\n");

    for (int8s_t i = 0; i < IRQ_MAX; i++) {
        _os_icb_t *p = &_os_icb_table[i];
        p->irqnum = i;
        p->handler = NULL;
    }
}


void _os_common_interrupt_handler(int32u_t flag)
{
    /* Gets irq number */
    int32u_t irq_num = hal_get_irq();
    if (irq_num == -1)
        return;
	
    /* Acknowledges the irq */
    hal_ack_irq(irq_num);
	
    /* Restores _eflags */
    hal_restore_interrupt(flag);

    /* Dispatches the handler and call it */
    _os_icb_t *p = &_os_icb_table[irq_num];
    if (p->handler != NULL) {
        //PRINT("entering irq handler 0x%x\n", (int32u_t)(p->handler));
        p->handler(irq_num, p->arg);
        //PRINT("exiting irq handler 0x%x\n", (int32u_t)(p->handler));
    }
}


int8s_t eos_set_interrupt_handler(int8s_t irqnum, eos_interrupt_handler_t handler, void *arg)
{
    PRINT("irqnum: %d, handler: 0x%x, arg: 0x%x\n", (int32u_t)irqnum, (int32u_t)handler, (int32u_t)arg);

    _os_icb_t *p = &_os_icb_table[irqnum];
    p->handler = handler;
    p->arg = arg;

    return 0;
}


eos_interrupt_handler_t eos_get_interrupt_handler(int8s_t irqnum)
{

    _os_icb_t *p = &_os_icb_table[irqnum];

    return p->handler;
}	
