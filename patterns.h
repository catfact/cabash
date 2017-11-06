#pragma once

#include <stdbool.h>

typedef enum {
  PAT_CLEAR,
  PAT_ONE,
  PAT_TWO,
  PAT_ALT1,
  PAT_ALT2,
  PAT_ALT3,
  PAT_ALT12,
  PAT_ALT123,
  PAT_ALT1234,
  PAT_RAND
} pat_t;

void pat_fill(bool *arr, int num, pat_t pat);
