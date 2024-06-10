/********************************************************
 * Filename: hal/linux/context.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/31/24
 *
 * Description: Context management (define, create, switch)
 ********************************************************/

#include <core/eos.h>
#include "emulator_asm.h"

typedef struct _os_context {
    // To be filled by students: Project 2
    // Emulate the stack of the interrupted task right after pushal instruciton
    /* low address */
    /* high address */	
    int32u_t edi;
    int32u_t esi;
    int32u_t ebp;
    int32u_t esp;
    int32u_t ebx;
    int32u_t edx;
    int32u_t ecx;
    int32u_t eax;
    int32u_t eflags;
    int32u_t eip;
} _os_context_t;


void print_context(addr_t context)
{
    if(context == NULL)
        return;

    _os_context_t *ctx = (_os_context_t *)context;

    // To be filled by students if needed: Project 2
    
    PRINT("edi  =0x%x\n", ctx->edi);
    PRINT("esi  =0x%x\n", ctx->esi);
    PRINT("ebp  =0x%x\n", ctx->ebp);
    PRINT("esp  =0x%x\n", ctx->esp);
    PRINT("ebx  =0x%x\n", ctx->ebx);
    PRINT("edx  =0x%x\n", ctx->edx);
    PRINT("ecx  =0x%x\n", ctx->ecx);
    PRINT("eax  =0x%x\n", ctx->eax);
    PRINT("eflags  =0x%x\n", ctx->eflags);
    PRINT("eip  =0x%x\n", ctx->eip);
    //...
}


addr_t _os_create_context(addr_t stack_base, size_t stack_size, void (*entry)(void *), void *arg)
{
    // To be filled by students: Project 2
    int32u_t *stack_ptr = (int32u_t *)(stack_base + stack_size);

    if((addr_t)stack_ptr <= stack_base){
        return NULL; // stack overflow
    }
    *(--stack_ptr) = (int32u_t)arg; // push arg
    *(--stack_ptr) = (int32u_t)NULL; // dummy
    *(--stack_ptr) = (int32u_t)entry; // push entry

    // pusha
    *(--stack_ptr) = (int32u_t)1; // eflags
    for (int i = 0; i < 8; i++) {
        *(--stack_ptr) = NULL;
    }

    return (addr_t)stack_ptr;
}


void _os_restore_context(addr_t sp)
{
    // To be filled by students: Project 2
    __asm__ __volatile__ (
        "movl %0, %%esp\n"
        "popa\n"
        "popf\n"
        "ret\n"
        :
        : "m"(sp)
    );
}


addr_t _os_save_context()
{
    // To be filled by students: Project 2
    __asm__ __volatile__ (
        "pushl %%ebp\n"
        "movl %%esp, %%ebp\n"
        "pushl $resume_point\n"
        "pushf\n"
        "movl $0, %%eax\n"
        "pusha\n"
        "movl %%esp, %%eax\n"
        "pushl 4(%%ebp)\n"
        "pushl (%%ebp)\n"
        "movl %%esp, %%ebp\n"
        "resume_point:\n"
        "leave\n"
        "ret\n"
        :
    );
    return NULL;
}