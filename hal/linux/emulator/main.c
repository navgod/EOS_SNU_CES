/********************************************************
 * Filename: hal/linux/emulator/main.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/27/24
 *
 * Description: Entry point of eOS and dispatch interrupts
 ********************************************************/

#include "private.h"
#include "../emulator.h"
#include <core/eos.h>

int32u_t _eflags_saved;


/**
 * Entry point of eOS:
 * 	eOS starts as if it were reset -- jump to the reset handler
 */
int main(int argc, char **argv)
{
    PRINT("Processing reset\n");

    _eflags = 0;
    __asm__ __volatile__ (
        "jmp *%0		\n\t"
        :
        : "r"(_vector[0])
    );

    /* Never reaches here */
    return 0;
}


/**
 * Delivers an irq to CPU 
 * 	Emulation of CPU's reaction to the delivered irq
 */
void _deliver_irq()
{
    //PRINT("_eflags: %d, _irq_pending: 0x%x, _irq_mask: 0x%x\n", _eflags, _irq_pending, _irq_mask);
 
    if (_irq_pending & ~_irq_mask) {
        if (_eflags == 1) {
            // Hardware disables interrtupts and saves previous _eflags
            _eflags_saved = _eflags;
            _eflags = 0;

	    // Calls _os_irq_handler to execute the generic irq handler
            __asm__ __volatile__ (
                	"push $resume_eip	\n\t"
                	"jmp *%0		\n\t"
                "resume_eip:			\n\t"
			:
                	: "r"(_vector[3])
            );
        }
        // Else CPU silently ignores the interrupt
    }
    // Else CPU silently ignores the interrupt
}
