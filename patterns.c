#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "patterns.h"
#include "ca.h"


void pat_fill(bool *p, const int n, pat_t pat) {
  switch (pat) {

  case PAT_CLEAR:
    memset(p, true, n * sizeof(bool) );
    break;
  case PAT_ONE:
    memset(p, true, n * sizeof(bool) );
    p[n/2] = true;
    break;
  case PAT_TWO:
    memset(p, true, n * sizeof(bool) );
    p[n/2] = true;
    p[n/2+1] = true;
    break;
  case PAT_ALT1:
    for(int i=0; i<n; ++i) { p[i] = (i%2 == 0); }
    break;
  case PAT_ALT2:
    for(int i=0; i<n; ++i) { p[i] = (i%3 == 0); }
    break;
  case PAT_ALT3:
    for(int i=0; i<n; ++i) { p[i] = (i%4 == 0); }
    break;
  case PAT_ALT12:
    for(int i=0; i<n; ++i) {
      switch(i%5) {
      case 0: case 2: p[i] = true; break;
      default: p[i] = false;
      }
    }
    break;
  case PAT_ALT123:
    for(int i=0; i<n; ++i) {
      switch(i%9) {
      case 0: case 2: case 5: p[i] = true; break;
      default: p[i] = false;
      }
    }    
    break;
  case PAT_ALT1234:
    for(int i=0; i<n; ++i) {
      switch(i%15) {
      case 0: case 2: case 5: case 9: p[i] = true; break;
      default: p[i] = false;
      }
    }    
    break;    
  case PAT_RAND:
    srand(time(NULL));
    for(int i=0; i<n; ++i) { p[i] = rand() > (RAND_MAX/2); }
  }
}
