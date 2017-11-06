#include <stdio.h>
#include <stdlib.h>

#include <lo/lo.h>
#include <ncurses.h>

#include "ca.h"
#include "metro.h"
#include "ui.h"

void* ca;
lo_address osc_addr;

// lazy, just send blob of bytes
// yes, we could pack to bit array..
// but then might as well make bit-array routines and use throughout
void send_osc(const bool* p, const int n) {
  if(osc_addr == NULL) { return; }
  // assuming bool == byte
  lo_blob b = lo_blob_new(n, p);
  lo_send(osc_addr, "/ca/cells", "b", b);
  free(b);
}

void metro_bang(void* data) {
  (void)data;
  ca_next_frame(ca);
  send_osc(ca_get_cells(ca), CA_NUM_CELLS);
  ui_update(ca);
}

int main(int argc, const char **argv) {
  (void)argc;
  (void)argv;
  const char* port = "57120";
  const char* addr = "127.0.0.1";
  osc_addr = lo_address_new(addr, port);  

  
  int n = CA_NUM_CELLS;
  ca = ca_new();
  
  ui_init();

  printf("ui_init() done (main.c) \r\n");

  ca_set_rule(ca, 90);
  ca_set_cell(ca, n/2, true);
  //  ca_set_cell(ca, n/2 + 1, true);

  void* m = metro_new(&metro_bang, NULL, -1);
  metro_set_time(m, 0.0625);
  metro_start(m, 0, -1);

  ui_loop(ca, m);
  
  metro_stop(m);
  free(m);
  free(ca);

}
