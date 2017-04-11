#include "mypthread.h"
#include "stdint.h"
#include "malloc.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "time.h"

#define STACK_SIZE (1024 * 256)
#define MAX_THREADS 5
#define TIME_QUANTUM 0.0002

typedef struct
{
    RNPNode_t * head; //* current;
    uint64_t time_quantum;
    uint32_t num_threads;
    ucontext_t mctx;
} ready_queue;

/** Global variables **/
bool scheduler_init = false;
ready_queue mypthread_scheduler;

// Type your own functions (void)
// e.g., free up sets of data structures created in your library

void mypthread_set_status(mypthread_t *thread, enum QUEUE_STATUS state) { thread->node.tstate = state; }

void mypthread_scheduler_init()
{

}

int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr, void *(*start_routine) (void *), void *arg)
{
    clock_t begin = clock();
    /** Check if the scheduler is running **/
    if (!scheduler_init)
    {
        mypthread_scheduler.head = NULL;
       // mypthread_scheduler.current = NULL;
        mypthread_scheduler.time_quantum = TIME_QUANTUM;
        scheduler_init = true;
    }

    int32_t ret, num_args;
   
   /** Get the context of the thread being created **/
    errno = 0;
    ret = getcontext(&thread->node.ucontext);
    if (ret) 
    { 
        printf("Cannot get context of thread: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    thread->node.ucontext.uc_link = 0;
    thread->node.ucontext.uc_stack.ss_sp = malloc(STACK_SIZE);
    thread->node.ucontext.uc_stack.ss_size = STACK_SIZE;
    thread->node.ucontext.uc_stack.ss_flags = 0;
    
    /** Determine if there are any arguments. Pthreads only take one argument.s**/
    if (arg != NULL) { num_args = 1; } else { num_args = 0; }

    printf("mypthread_create: creating thread with tid = %d\n", thread->tid);
    errno = 0;
    makecontext(&thread->node.ucontext, (void(*)(void))start_routine, num_args, arg);
    if (errno) 
    {
        mypthread_set_status(thread, FAILURE);
        printf("Cannot create context of thread: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    mypthread_set_status(thread, IDLE);

    /** Add thread to scheduler **/
    if (mypthread_scheduler.head == NULL) 
    {
        mypthread_scheduler.num_threads++;
        mypthread_scheduler.head = &thread->node;
        mypthread_scheduler.head->next = NULL;
    }
    else
    {
        mypthread_scheduler_push(thread);
    }

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("mypthread_create: thread creation time -> %f\n", time_spent);

    return 0;
}

void mypthread_scheduler_push(mypthread_t * thread)
{
    RNPNode_t * current = malloc(sizeof(RNPNode_t));
    current = mypthread_scheduler.head;
    while (current->next != NULL) { current = current->next; }
    current->next = &thread->node;
    current->next->next = NULL;
}

void mypthread_exit(void *retval)
{

}

int mypthread_yield(void)
{
    mypthread_t *current, next;
}

int mypthread_join(mypthread_t thread, void **retval)
{

}

void *helloworld(void *args)
{
    char *msg = (char*)args;
    printf("helloworld: (thread routine), with message: %s\n", msg);
}

int main()
{
    mypthread_t thread1, thread2, thread3, thread4, thread5, thread6;
    mypthread_attr_t thread1_attr;

    char *message = "fuck OS";
    thread1.tid = 1, thread2.tid = 2, thread3.tid = 3, thread4.tid = 4, thread5.tid = 5, thread6.tid = 6;
    mypthread_create(&thread1, &thread1_attr, helloworld, (void*)message);
    mypthread_create(&thread2, &thread1_attr, helloworld, (void*)message);
    mypthread_create(&thread3, &thread1_attr, helloworld, (void*)message);
    mypthread_create(&thread4, &thread1_attr, helloworld, (void*)message);
    mypthread_create(&thread5, &thread1_attr, helloworld, (void*)message);

}

/* Write whatever function is left here ....
  void mypthread_whatever (... ) ...*/