#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#include "ca.h"
#include "metro.h"
#include "ui.h"

void* ca;

void metro_bang(void* data) {
  (void)data;
  ca_next_frame(ca);
  ui_update(ca);
}

int main(void) {
  int n = CA_NUM_CELLS;
  ca = ca_new();
  
  ui_init();

  printf("ui_init() done (main.c) \r\n");

  ca_set_rule(ca, 110);
  ca_set_cell(ca, n/2, true);
  ca_set_cell(ca, n/2 + 1, true);

  void* m = metro_new(&metro_bang, NULL, -1);
  metro_set_time(m, 0.0625);
  metro_start(m, 0, -1);

  ui_loop(ca, m);
  
  metro_stop(m);
  free(m);
  free(ca);

}
