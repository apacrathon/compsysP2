#include "mypthread.h"
#include "stdint.h"
#include "malloc.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "signal.h"
#include "sys/time.h"
#include "time.h"

#define STACK_SIZE (1024 * 256)
#define TIME_QUANTUM 1

/** Global variables **/

typedef struct
{
    struct mypthread_t * head; //* current;
    uint64_t time_quantum;
    uint32_t num_threads, terminated_threads;
} ready_queue;

static ucontext_t mctx;
bool scheduler_init = false;
ready_queue mypthread_scheduler;

int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr, void *(*start_routine) (void *), void *arg)
{
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

    //printf("mypthread_create: creating thread with external tid = %d\n", thread->tid);
    errno = 0;
    makecontext(&thread->node.ucontext, (void(*)(void))start_routine, num_args, arg);
    if (errno) 
    {
        mypthread_set_status(thread, FAILURE);
        printf("Cannot create context of thread: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /** Add thread to scheduler **/
    if (!scheduler_init)
    {
        /** Allocate memory to keep track of the main context thread **/
        mypthread_scheduler.head = malloc(sizeof(mypthread_t));
        mypthread_scheduler.head->node.next = mypthread_scheduler.head;
        mypthread_scheduler.head->node.internal_tid = 0;
        mypthread_scheduler.head->node.tstate = RUNNING;
        mypthread_scheduler.head->node.ucontext = mctx;
        
        /** The scheduler is now aware of one thread running **/
        mypthread_scheduler.num_threads = 1;
        mypthread_scheduler.terminated_threads = 0;

        /** NULLED Round Robin scheduling **/
        //mypthread_scheduler.time_quantum = TIME_QUANTUM;
        //myptrhead_scheduler_init;

        /** Make thread ready to execute and add to scheduling queue **/
        mypthread_set_status(thread, IDLE);
        mypthread_scheduler_push(thread);
        scheduler_init = true;
    }
    else //if (mypthread_scheduler.num_threads >= 2)
    {
        /** Make thread ready to execute and add to scheduling queue **/
        mypthread_scheduler_push(thread);
        mypthread_set_status(thread, IDLE);
    }
    // Remove for Round Robin algo.
    mypthread_yield();
    return 0;
}

void mypthread_exit(void *retval)
{
    mypthread_scheduler.head->retval = retval;
    mypthread_scheduler.head->node.tstate = TERMINATED;
    mypthread_scheduler.terminated_threads++;
    mypthread_yield();
}

int mypthread_yield(void)
{
    mypthread_t * current = mypthread_scheduler.head;
    mypthread_t * next = mypthread_scheduler.head->node.next;

    /** Get the next thread in the 'queue' that isn't terminated **/
    while (next->node.tstate == TERMINATED) 
    {
        //if (next->node.internal_tid == current->node.internal_tid) { printf("true"); return TERMINATED;}
        next = next->node.next;
    }
    
    if (next->node.internal_tid == current->node.internal_tid) 
    {
        current->node.tstate = TERMINATED; 
        return TERMINATED;
    }

    switch (next->node.tstate)
    {
        case RUNNING:
            /** shouldn't come to this point, but now we'll know during debugging **/
            return RUNNING;
        case TERMINATED:
            /* cleanup thread */
            return TERMINATED;
        case BLOCKED:
            /* Blocked and Idle threads just continue execution on yield */
        case IDLE:
            /* Move 'next' thread to the first position, & insert the previous one in the back of the scheduler */
            mypthread_scheduler.head = next;
            mypthread_scheduler.head->node.tstate = RUNNING;
            if (current->node.tstate != TERMINATED) 
            {
                if (current->node.tstate == RUNNING) { current->node.tstate = BLOCKED; }
                else { current->node.tstate = IDLE; } 
            }
            break;
    }

    mypthread_t * curr = mypthread_scheduler.head;
    return swapcontext(&current->node.ucontext, &next->node.ucontext);    
}

int mypthread_join(mypthread_t thread, void **retval)
{
    while (thread.node.tstate != TERMINATED && mypthread_yield() == TERMINATED);
    thread.retval = retval;
    return 0;
}

void mypthread_scheduler_push(mypthread_t * thread)
{
    int i;
    mypthread_t * tail = mypthread_scheduler.head;
    for (i = 1; i < mypthread_scheduler.num_threads; i++)
    {
        tail = tail->node.next;
    }
    thread->node.next = tail->node.next;
    tail->node.next = thread;
    tail->node.next->node.internal_tid = i;
    mypthread_scheduler.num_threads++;

    mypthread_t * curr = mypthread_scheduler.head;
        for (int i = 0; i < mypthread_scheduler.num_threads; i++)
    {
        //printf("swap thread_list: tid = %d, state = %d\n", curr->node.internal_tid, curr->node.tstate);
        curr = curr->node.next;
    }
}

void mypthread_set_status(mypthread_t *thread, enum QUEUE_STATUS state) { thread->node.tstate = state; }

/** The following functions were for my own implementation of a Round Robin algorithm,
  * however, the mtsort driver has it's own yielding implemented, therefore,
  * I nulled the functionality of my custom scheduler. Explanation in report **/
void timer(int signum) { mypthread_yield(); }

void mypthread_scheduler_init()
{
    struct sigaction sa;
    struct itimerval thread_timer;

    memset(&sa, 0, sizeof (sa));
    sa.sa_handler = &timer;
    sigaction(SIGVTALRM, &sa, NULL);

    thread_timer.it_value.tv_sec = TIME_QUANTUM;
    thread_timer.it_value.tv_usec = 0;
    thread_timer.it_interval.tv_sec = TIME_QUANTUM;
    thread_timer.it_interval.tv_usec = 0;

    setitimer(ITIMER_VIRTUAL, &thread_timer, NULL);
    for(;;);
}
