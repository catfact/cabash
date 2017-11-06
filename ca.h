#pragma once

#include <stdint.h>
#include <stdbool.h>

enum { CA_NUM_CELLS = 128 };

typedef enum {
  CA_BOUND_MODE_WRAPPED,  // wrap boundaries when looking at neighbors
  CA_BOUND_MODE_FIXED_HIGH, // consider boundaries to be always on
  CA_BOUND_MODE_FIXED_LOW // consider boundaries to be always off
} ca_bound_mode_t;

// allocate and initialize a CA structure
extern void* ca_new();
// update and return the next 16-bit "frame" of binary states
extern uint16_t ca_next_frame(void* ca);
// get a pointer to all the cells (array of bools)
extern bool* ca_get_cells(void* ca);
// set the offset of the window into the frame
extern int ca_set_offset(void* ca, int off);
// arbitraily set the left hand boundary index
extern int ca_set_bound_l(void* ca, int idx);
// arbitrarily set the right hand boundary index
extern int ca_set_bound_r(void* ca, int idx);
// set the boundary mode
extern int ca_set_bound_mode_l(void* ca, ca_bound_mode_t mode);
// set the boundary mode
extern int ca_set_bound_mode_r(void* ca, ca_bound_mode_t mode);
// set state for a given cell
extern void ca_set_cell(void* ca, unsigned int idx, bool state);
// set the rule
extern void ca_set_rule (void* ca, uint8_t val);
// get the rule
extern uint8_t ca_get_rule (void* ca);
// get bound position
extern int ca_get_bound_l(void *ca);
extern int ca_get_bound_r(void *ca);
// get bound mode
extern int ca_get_bound_mode_l(void *ca);
extern int ca_get_bound_mode_r(void *ca);
