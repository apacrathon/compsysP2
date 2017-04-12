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
#define MAX_THREADS 5
#define TIME_QUANTUM 0.0002

typedef struct
{
    struct mypthread_t * head; //* current;
    uint64_t time_quantum;
    uint32_t num_threads, terminated_threads;
    ucontext_t mctx;
} ready_queue;

/** Global variables **/
bool scheduler_init = false;
ready_queue mypthread_scheduler;

// Type your own functions (void)
// e.g., free up sets of data structures created in your library

void mypthread_set_status(mypthread_t *thread, enum QUEUE_STATUS state) { thread->node.tstate = state; }

void timer_handler (int signum)
{
    mypthread_yield();
}

void mypthread_scheduler_init()
{
    struct sigaction sa;
    struct itimerval timer;

    /* Install timer_handler as the signal handler for SIGVTALRM. */
    memset(&sa, 0, sizeof (sa));
    sa.sa_handler = &timer_handler;
    sigaction(SIGVTALRM, &sa, NULL);

    /* Configure the timer to expire after 250 msec... */
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;
    /* ... and every 250 msec after that. */
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;
    /* Start a virtual timer. It counts down whenever this process is
    executing. */
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
    for(;;);
}

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
    
    thread->node.ucontext.uc_link = &mypthread_scheduler.mctx;
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

    /** Add thread to scheduler **/
     /** Check if the scheduler is running **/
    if (!scheduler_init)
    {
        getcontext(&mypthread_scheduler.mctx);
        if (scheduler_init) { return 0; }
        mypthread_scheduler.head = malloc(sizeof(mypthread_t));
        mypthread_scheduler.head->node.next = mypthread_scheduler.head;
        mypthread_scheduler.head->node.internal_tid = 0;
        mypthread_scheduler.head->node.tstate = RUNNING;
        mypthread_scheduler.head->node.ucontext = mypthread_scheduler.mctx;
        mypthread_scheduler.num_threads++;
        mypthread_scheduler.terminated_threads = 0;

        mypthread_scheduler.time_quantum = TIME_QUANTUM;
        mypthread_set_status(thread, IDLE);
        mypthread_scheduler_push(thread);
        scheduler_init = true;
        mypthread_scheduler_init();
    }
    else
    {
        mypthread_scheduler_push(thread);
        mypthread_set_status(thread, IDLE);
        mypthread_scheduler_init();
    }
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
}

void mypthread_exit(void *retval)
{
    mypthread_scheduler.head->retval = retval;
    mypthread_scheduler.head->node.tstate = TERMINATED;
    mypthread_scheduler.terminated_threads++;
    //printf("mypthread_exit: thread (%d), state(%d)\n\n", mypthread_scheduler.head->node.internal_tid, mypthread_scheduler.head->node.tstate);
}

int mypthread_yield(void)
{
    mypthread_t * current = mypthread_scheduler.head;
    mypthread_t * next = mypthread_scheduler.head->node.next;

    if (mypthread_scheduler.num_threads - mypthread_scheduler.terminated_threads < 1) { return 0; }

    /** Get the next thread in the 'queue' that isn't terminated **/
    while (next->node.tstate == TERMINATED) { next = next->node.next; }

    switch (next->node.tstate)
    {
        case RUNNING:
            /* error */
            return;
        case TERMINATED:
            /* cleanup thread */
            printf("I am thread %d, and i'm terminating\n", next->node.internal_tid);
            return;
        case BLOCKED:
            /* new thread */
        case IDLE:
            /* middle of execution */
            mypthread_scheduler.head = next;
            mypthread_scheduler.head->node.tstate = RUNNING;
            //next->node.tstate = RUNNING;
            if (current->node.tstate != TERMINATED) { current->node.tstate = IDLE; }
            break;
    }
    
    mypthread_t * curr = mypthread_scheduler.head;
    for (int i = 0; i < mypthread_scheduler.num_threads; i++)
    {
        printf("swap thread_list: tid = %d, state = %d\n", curr->node.internal_tid, curr->node.tstate);
        curr = curr->node.next;
    }
    printf("mypthread_yield: swapping from thread %d to thread %d\n", current->node.internal_tid, next->node.internal_tid);
    
    return swapcontext(&current->node.ucontext, &next->node.ucontext);    
}

int mypthread_join(mypthread_t thread, void **retval)
{

}

void *helloworld(void *args)
{
    char *msg = (char*)args;
    printf("helloworld: (thread routine), with message: %s\n", msg);
    mypthread_exit(0);
}

/*int main()
{
    mypthread_t thread;
    mypthread_attr_t thread1_attr;

    printf("RUNNING = %d, IDLE = %d, TERMINATED = %d, BLOCKED = %d, FAILURE = %d\n\n", RUNNING, IDLE, TERMINATED, BLOCKED, FAILURE);


    char *message = "fuck OS";


    for( int i = 0; i < 4; i++ )
    {
        thread.tid = i;
        if ( mypthread_create(&thread, 0, helloworld, (void*)message) != 0 )
        {
            printf( "[FATAL] Could not create thread: %d\n", i );
            exit( 1 );
        }
    }
}*/

/* Write whatever function is left here ....
  void mypthread_whatever (... ) ...*/