
// std
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
// posix
#include <errno.h>
#include <pthread.h>
#include <time.h>

#include "metro.h"

enum {
    METRO_STATUS_RUNNING,
    METRO_STATUS_STOPPED
};

struct metro {
    int status;                  // running/stopped status
    double seconds;              // period in seconds
    uint64_t count;              // total iterations ( <=0 -> infinite )
    uint64_t stage;              // current count of iterations
    uint64_t time;               // current time (in nsec)
    uint64_t delta;              // current delta (in nsec)
    pthread_t tid;               // thread id
    pthread_mutex_t status_lock; // mutex protecting status
  pthread_mutex_t stage_lock;  // mutex protecting stage number
  void* data; // arbitrary user data, pass to callback
  metro_cb_t cb; // callback
};

//-----------------------
//--- static functions

static void metro_handle_error(int code, const char *msg) {
    printf("error code: %d ; message: \"%s\"", code, msg); fflush(stdout);
}

// set the current time
static void metro_set_current_time(struct metro *m) {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    m->time =
        (uint64_t) ( (1000000000 *
                      (int64_t)time.tv_sec) + (int64_t)time.tv_nsec );
}

// accurate sleep function
static void metro_sleep(struct metro *m) {
    struct timespec ts;
    m->time += m->delta;
    ts.tv_sec = (time_t) m->time / 1000000000;
    ts.tv_nsec = (long) m->time % 1000000000;
    clock_nanosleep (CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, NULL);
}

// reset the stage number of possibly running metro
static void metro_reset(struct metro *m, int stage) {
    pthread_mutex_lock( &(m->stage_lock) );
    if(stage > 0) { m->stage = stage; }
    else { m->stage = 0; }
    pthread_mutex_unlock( &(m->stage_lock) );
}

// stop a metro's thread
void metro_cancel(struct metro *m) {
    int ret = pthread_cancel(m->tid);
    if(ret) {
        printf("metro_stop(): pthread_cancel() failed; error: 0x%08x\r\n", ret);
        fflush(stdout);
    } else {
        m->status = METRO_STATUS_STOPPED;
    }
}

// function for pthread
static void *metro_thread_loop(void *p) {
    struct metro *m = (struct metro *) p;
    int stop = 0;

    metro_set_current_time(m);

    while(!stop) {
        pthread_mutex_lock( &(m->stage_lock) );
        if( ( m->stage >= m->count) && ( m->count > 0) ) {
            stop = 1;
        }
        pthread_mutex_unlock( &(m->stage_lock) );
        if(stop) { break; }
        pthread_testcancel(); // important! creates a cancellation point
        pthread_mutex_lock( &(m->stage_lock) );
	(*m->cb)(m->data);
        m->stage += 1;
        pthread_mutex_unlock( &(m->stage_lock) );
        metro_sleep(m);
    }
    return NULL;
}

// do the actual low-level thread creation stuff
static void metro_init(struct metro *m, uint64_t nsec, int count) {
    int res;
    pthread_attr_t attr;

    res = pthread_attr_init(&attr);
    if(res != 0) {
        metro_handle_error(res, "pthread_attr_init");
        return;
    }
    // set other thread attributes here...
    
    m->delta = nsec;
    m->count = count;
    res = pthread_create(&(m->tid), &attr, &metro_thread_loop, (void *)m);
    if(res != 0) {
        metro_handle_error(res, "pthread_create");
        return;
    }
    else {
        // set thread priority to realtime
        // struct sched_param param;
        // param.sched_priority = sched_get_priority_max (SCHED_RR);
        // res = pthread_setschedparam (m->tid, SCHED_RR, &param);
        m->status = METRO_STATUS_RUNNING;
        if(res != 0) {
            metro_handle_error(res, "pthread_setschedparam");
            printf("\n");
            switch(res) {
            case ESRCH:
                printf("specified thread does not exist\n");
                break;
            case EINVAL:
                printf("invalid thread policy value or associated parameter\n");
                break;
            case EPERM:
	      printf("failed to set scheduling priority. "
		     "caller should be in the realtime group, or root. "
		     "(or just don't worry about this.) \n");
	      fflush(stdout);
                break;
            default:
                printf("unknown error code \n");
            }
            return;
        }
    }
}

//-----------------------
//--- extern functions

void* metro_new(metro_cb_t cb, void* data, int count) {
  struct metro *m = calloc(1, sizeof(struct metro));
  m->status = METRO_STATUS_STOPPED;
  m->seconds = 1;
  m->data = data;
  m->cb = cb;
  m->count = count;
  return m;
}

void metro_destroy(void* t) {
  metro_stop((struct metro*)t);
  free(t);
}

void metro_set_time(void* p, double sec) {
  struct metro* m = (struct metro*)p;
  if(m->seconds > 0.0) {
    m->seconds = sec;
  } 
}

void metro_start(void* p, int stage, int count) {
    struct metro* m = (struct metro*)p;
  uint64_t nsec;
  pthread_mutex_lock(&m->status_lock);
  if(m->status == METRO_STATUS_RUNNING) {
    metro_cancel(m);
  }
  pthread_mutex_unlock(&m->status_lock);
  nsec = (uint64_t)(m->seconds * 1000000000.0);
  metro_reset(m, stage);
  metro_init(m, nsec, count);
}


void metro_stop(void* p) {
    struct metro* m = (struct metro*)p;
    int ret = pthread_cancel(m->tid);
    if(ret) {
      printf("metro_stop(): pthread_cancel() failed; error: 0x%08x\r\n", ret);
      fflush(stdout);
    } else {
      m->status = METRO_STATUS_STOPPED;
    }

}

void metro_wait(void *m) {
  pthread_join(((struct metro*)m)->tid, NULL);
}

bool metro_is_running(void *m) {
  return ((struct metro*)m)->status == METRO_STATUS_RUNNING;
}

double metro_get_time(void *m) {
  return ((struct metro*)m)->seconds;
}

