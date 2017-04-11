#ifndef H_MYPTHREAD
#define H_MYPTHREAD

#include <ucontext.h>
#include <stdio.h>      
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

//struct threadNode; //defined in .c class
enum QUEUE_STATUS { RUNNING, IDLE, TERMINATED, BLOCKED, FAILURE, FINISHED };

typedef struct RNPNode_t
{
    /** Ready queue ID **/
    uint16_t ready_id;
    uint64_t exec_time;
    ucontext_t ucontext;
    struct RNPNode_t * next;
    enum QUEUE_STATUS tstate;
} RNPNode_t;

typedef struct mypthread_t 
{
    //External thread ID
    uint16_t tid;
    RNPNode_t node;
} mypthread_t;

typedef struct {
    //Not specified to implement in project instructions but decided to do it anyway.
} mypthread_attr_t;

// Functions
int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr, void *(*start_routine) (void *), void *arg);

void mypthread_exit(void *retval);

int mypthread_yield(void);

int mypthread_join(mypthread_t thread, void **retval);

void mypthread_set_status(mypthread_t *thread, enum QUEUE_STATUS state);

void mypthread_scheduler_init();

void mypthread_scheduler_push(mypthread_t *thread);



/* Don't touch anything after this line.
 *
 * This is included just to make the mtsort.c program compatible
 * with both your ULT implementation as well as the system pthreads
 * implementation. The key idea is that mutexes are essentially
 * useless in a cooperative implementation, but are necessary in
 * a preemptive implementation.
 */

typedef int mypthread_mutex_t;
typedef int mypthread_mutexattr_t;

static inline int mypthread_mutex_init(mypthread_mutex_t *mutex,
        	const mypthread_mutexattr_t *attr) { return 0; }

static inline int mypthread_mutex_destroy(mypthread_mutex_t *mutex) { return 0; }

static inline int mypthread_mutex_lock(mypthread_mutex_t *mutex) { return 0; }

static inline int mypthread_mutex_trylock(mypthread_mutex_t *mutex) { return 0; }

static inline int mypthread_mutex_unlock(mypthread_mutex_t *mutex) { return 0; }

#endif
