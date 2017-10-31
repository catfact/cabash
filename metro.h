#pragma once

#include <stdbool.h>

// a simple, high-resolution timer class with callback
// calling this a 'metro' to keep with computer music tradition,
// and to avoid colliision with <time.h> 


// callback type
typedef void (*metro_cb_t)(void* data);

// allocate and return a new metro objecta
// @param cb : callback function
// @param data: pointer to user data, passed to callback (or NULL)
// @param count: number of callbacks (-1 for infinite)
// @return: pointer to newly allocated metro
extern void* metro_new(metro_cb_t cb, void* data, int count);

// set the period of a metro
extern void metro_set_time(void* metro, double sec);

// start a metro
extern void metro_start(void* metro, int stage, int count);

// stop a metro
extern void metro_stop(void* metro);

// sleep calling thread until metro is finished
extern void metro_wait(void *m);

// get the current state
extern bool metro_is_running(void *m);

// get the current period in seconds
extern double metro_get_time(void *m);
