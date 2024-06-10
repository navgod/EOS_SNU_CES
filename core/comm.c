/********************************************************
 * Filename: core/comm.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/30/24
 *
 * Description: Message queue management
 ********************************************************/

#include <core/eos.h>


void eos_init_mqueue(eos_mqueue_t *mq, void *queue_start, int16u_t queue_size, int8u_t msg_size, int8u_t queue_type)
{
    // To be filled by students: Project 4
    mq->queue_start = queue_start;
    mq->queue_size = queue_size;
    mq->msg_size = msg_size;
    mq->queue_type = queue_type;
    mq->front = queue_start;
    mq->rear = queue_start;
    mq->front_index = 0;
    mq->rear_index = 0;

    eos_init_semaphore(&mq->putsem, queue_size, queue_type);
    eos_init_semaphore(&mq->getsem, 0, queue_type);
}


int8u_t eos_send_message(eos_mqueue_t *mq, void *message, int32s_t timeout) 
{
    // To be filled by students: Project 4
    while(eos_acquire_semaphore(&mq->putsem, timeout) == 0){
        //PRINT("here\n");
        if (timeout == -1)
            return 0;
        else{
            eos_tcb_t *current_task = eos_get_current_task();
            if (&mq->putsem.queue_type == FIFO)
                _os_add_node_tail(&mq->putsem.wait_queue, &current_task->wait_queue);
            else
                _os_add_node_priority(&mq->putsem.wait_queue, &current_task->wait_queue);
            eos_sleep(timeout);
        }
    }
    for (int i = 0; i < mq->msg_size; i++) {
        mq->queue_start[mq->rear_index] = ((int8u_t *)message)[i];
        mq->rear_index = (mq->rear_index + 1) % (mq->queue_size * mq->msg_size);
    }

    eos_release_semaphore(&mq->getsem);
}


int8u_t eos_receive_message(eos_mqueue_t *mq, void *message, int32s_t timeout)
{
    // To be filled by students: Project 4
    eos_tcb_t *current_task = eos_get_current_task();
    while(eos_acquire_semaphore(&mq->getsem, timeout) == 0) {
        if (timeout == -1){
            return 0;
        }
        else{
            if (mq->getsem.queue_type == FIFO)
                _os_add_node_tail(&mq->getsem.wait_queue, &current_task->wait_queue);
            else
                _os_add_node_priority(&mq->getsem.wait_queue, &current_task->wait_queue);
            eos_sleep(timeout);
        }
    }
    for (int i = 0; i < mq->msg_size; i++) {
        ((int8u_t *)message)[i] = mq->queue_start[mq->front_index];
        mq->front_index = (mq->front_index + 1) % (mq->queue_size * mq->msg_size);
    }

    eos_release_semaphore(&mq->putsem);
}
