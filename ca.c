#include <stdlib.h>
#include "ca.h"

struct ca_state {
  // doing this the big, easy way
  // replace this with bit operations on uint32 array
  bool cell[CA_NUM_CELLS];
  uint8_t rule;
  int bound_l;
  int bound_r;
  int offset;
  ca_bound_mode_t bound_mode_l;  
  ca_bound_mode_t bound_mode_r;
};

//------------------------
//--- static functions

// get neighbor code for given index
// assume the index is within bounds (inclusive)
static uint8_t get_neighbor_code(struct ca_state *ca, int idx) {
  uint8_t code = 0;

  //--- left neighbor
  if(idx <= ca->bound_l) {    
    switch(ca->bound_mode_l) {
    case CA_BOUND_MODE_WRAPPED :
      if(ca->cell[ca->bound_r]) {
	code |= 0b100;
      }
      break;
    case CA_BOUND_MODE_FIXED_HIGH :
      code |= 0b100;
      break;      
    case CA_BOUND_MODE_FIXED_LOW :
    default: ;;// nothing to do
    }            
  } else { // above lower bound
    if(ca->cell[idx-1]) { code |= 0b100; }
  }
  
  //-- middle (self)
  if(ca->cell[idx]) { code |= 0b010; }
  
  //--- right neighbor
  if (idx >= ca->bound_r) {
    switch(ca->bound_mode_r) {
    case CA_BOUND_MODE_WRAPPED :
      if(ca->cell[ca->bound_l]) {
	code |= 0b001;
      }
      break;
    case CA_BOUND_MODE_FIXED_HIGH :
      code |= 0b001;
      break;      
    case CA_BOUND_MODE_FIXED_LOW :
    default: ;;// nothing to do
    }            
  } else { // below upper bound
    if(ca->cell[idx+1]) { code |= 0b001; }
  }
  return code;
}

// update all cell states
static void update(struct ca_state *ca) {
  uint8_t code;
  for(int i=ca->bound_l; i<=ca->bound_r; ++i) {
    code = get_neighbor_code(ca, i);
    ca->cell[i] = (ca->rule & (1 << code)) > 0;
  }
}

static void ca_init(struct ca_state *ca) {
    for (int i=0; i<CA_NUM_CELLS; ++i) {
      ca->cell[i] = false;
  }
    ca->rule = 0;
    ca->bound_l = 0;
    ca->bound_r = CA_NUM_CELLS - 1;

}
 
//---------------------
//--- extern functions

// allocate and initialize
void* ca_new() {
  struct ca_state *ca = calloc(1, sizeof(struct ca_state));
  ca_init(ca);
  return ca;
}
 
uint16_t ca_next_frame(void *p) {
  struct ca_state *ca = (struct ca_state*)p;
  update(ca);
  uint8_t ret = 0;
  for(int i=0; i<16; ++i) {
    if(ca->cell[i + ca->offset]) {
      ret |= (1 << i);
    }
  }
  return ret;
}

int ca_set_offset(void *p, int idx) {
  struct ca_state *ca = (struct ca_state*)p;
  if(idx < 0) { ca->offset = 0; }
  else if (idx > (CA_NUM_CELLS - 16)) { ca->offset = CA_NUM_CELLS - 16; }
  else { ca->offset = idx; }
  return ca->offset;
}

// NB: just allowing the caller to do pathological things like let the bounds cross
// i don't know what will happen
int ca_set_bound_l(void *p, int idx) {
  struct ca_state *ca = (struct ca_state*)p;
  if(idx < 0) { ca->bound_l = 0; }
  else if (idx > (CA_NUM_CELLS - 1)) { ca->bound_l = CA_NUM_CELLS - 1; }
  else { ca->bound_l = idx; }
  return ca->bound_l;
}

int ca_set_bound_r(void *p, int idx) {
  struct ca_state *ca = (struct ca_state*)p;
  if(idx < 0) { ca->bound_r = 0; }
  else if (idx > (CA_NUM_CELLS - 1)) { ca->bound_r = CA_NUM_CELLS - 1; }
  else { ca->bound_r = idx; }
  return ca->bound_r;
}

int ca_set_bound_mode_l(void *p, ca_bound_mode_t mode) {
  struct ca_state *ca = (struct ca_state*)p;
  ca->bound_mode_l = mode;
  return mode;
}

int ca_set_bound_mode_r(void *p, ca_bound_mode_t mode) {
  struct ca_state *ca = (struct ca_state*)p;
  ca->bound_mode_r = mode;
  return mode;
}

void ca_set_cell(void *p, unsigned int idx, bool state) {
  struct ca_state *ca = (struct ca_state*)p;
  if(idx < CA_NUM_CELLS) {  ca->cell[idx] = state; }
}

void ca_set_rule (void *p, uint8_t val) {
  struct ca_state *ca = (struct ca_state*)p;
    ca->rule = val;
}

bool* ca_get_cells(void* p) {
  struct ca_state *ca = (struct ca_state*)p;
  return ca->cell;
}

uint8_t ca_get_rule (void* ca) {
  return ((struct ca_state*)ca)->rule;
}

int ca_get_bound_l(void *ca) {
  return ((struct ca_state*)ca)->bound_l;
}

int ca_get_bound_r(void *ca) {
  return ((struct ca_state*)ca)->bound_r;
}

int ca_get_bound_mode_l(void *ca) { 
  return ((struct ca_state*)ca)->bound_mode_l;
}
int ca_get_bound_mode_r(void *ca) { 
  return ((struct ca_state*)ca)->bound_mode_r;
}
