#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ncurses.h>
#include <pthread.h>

#include "ca.h"
#include "metro.h"
#include "patterns.h"
#include "ui.h"

// call (most) ncurses functions with hard error checking
#define CHECK(fn, ...)                      \
    do {if (fn(__VA_ARGS__) == ERR) {       \
            fail_exit(# fn "() failed"); }} \
    while (false)

pthread_mutex_t exit_lock;

static WINDOW *win_rule;
static WINDOW *win_bounds;
static WINDOW *win_metro;
static WINDOW *win_cells;

static bool visual_mode = false;
static bool should_exit = false;

static void resize(void);
static noreturn void fail_exit(const char *msg);

// offset for the middle 8 cells
static int midcell = CA_NUM_CELLS / 2 + 3;

// print a CA rule in the rule window
static void print_ca_rule(void* ca) {
  int r = ca_get_rule(ca);
  wclear(win_rule);  
  wprintw(win_rule, "R: %d \t 0x%02X \t %d%d%d%d%d%d%d%d",
	  r, r, 
	  (r&128)>>7, (r&64)>>6, (r&32)>>5, (r&16)>>4,
	  (r&8)>>3, (r&4)>>2, (r&2)>>1, r&1);
  wrefresh(win_rule);
}

// toggle a bit in the rule (and print it)
static void toggle_ca_rule(void* ca, int bit) {
  uint8_t rule = ca_get_rule(ca);
  rule ^= (1 << bit);
  ca_set_rule(ca, rule);
  print_ca_rule(ca);
}


/*
static void print_last_cell_row(void *ca) {
  //// FIXME:
  /// would like to just overwrite the last line of the scrolling window.
  /// this is actually a massive drag
  // becuase metro ticking is on separate thread from key handling
  /// i don't feel like adding an event loop r/n
  ;;
}
*/

static void print_bounds(void* ca) {
  wclear(win_bounds);
  int l = ca_get_bound_l(ca);
  int r = ca_get_bound_r(ca);
  mvwprintw(win_bounds, 0, 0,
	    "BOUNDS: [%d, %d], MODES: [%d, %d]",
	    l, r, ca_get_bound_mode_l(ca), ca_get_bound_mode_r(ca)); 
  wrefresh(win_bounds);
}

// increment left bound by some amount
static void inc_bound_l(void* ca, int amt) {
  ca_set_bound_l(ca, ca_get_bound_l(ca) + amt);
  print_bounds(ca);
}
  
// increment right bound by some amount
static void inc_bound_r(void* ca, int amt) {
  ca_set_bound_r(ca, ca_get_bound_r(ca) + amt);
  print_bounds(ca);
}

// set a cell state
static void set_cell(void* ca, int x, bool z) {
  ca_set_cell(ca, x, z);
  //  print_last_cell_row(ca);
  // FIXME:
  // this just prints an extra row, looks funny.
  // but i think better than no feedback at all
  ui_update(ca); 
}

// toggle the bounds mode
static void toggle_bounds_mode(void* ca, int lr) {
  ca_bound_mode_t m = lr > 0 ? ca_get_bound_mode_r(ca) : ca_get_bound_mode_r(ca);
  ca_bound_mode_t m1;
  switch(m) {
  case CA_BOUND_MODE_WRAPPED: m1 = CA_BOUND_MODE_FIXED_HIGH; break;
  case CA_BOUND_MODE_FIXED_HIGH: m1 = CA_BOUND_MODE_FIXED_LOW; break;
  case CA_BOUND_MODE_FIXED_LOW: default: m1 = CA_BOUND_MODE_WRAPPED;    
  }
  if(lr) { 
    ca_set_bound_mode_r(ca, m1);
  } else {
    ca_set_bound_mode_l(ca, m1);
  }
}

// change the metro's speed
static void inc_metro(void *metro, double amt) {
  wclear(win_metro);
  double t = metro_get_time(metro);
  t += amt;
  if(t < (1.0/24)) { t = (1.0/24); }
  metro_stop(metro);
  metro_set_time(metro, t);
  metro_start(metro, 0, -1);
  mvwprintw(win_metro, 0, 0, "METRO PERIOD: %f", t);
  wrefresh(win_metro);
}

//------------------------------
//---- extern functions

void ui_loop(void *ca, void* metro) {
  CHECK(clearok, curscr, TRUE);
  resize();

  print_ca_rule(ca);
  
    while (1) {
        if(should_exit) {
            break;
        }
        int k = getch();
	
        switch(k) {
	  // number row: toggle rule bits
	case '1': toggle_ca_rule(ca, 7); continue;
	case '2': toggle_ca_rule(ca, 6); continue; 
	case '3': toggle_ca_rule(ca, 5); continue; 
	case '4': toggle_ca_rule(ca, 4); continue; 
	case '5': toggle_ca_rule(ca, 3); continue; 
	case '6': toggle_ca_rule(ca, 2); continue; 
	case '7': toggle_ca_rule(ca, 1); continue; 
	case '8': toggle_ca_rule(ca, 0); continue;
	  // first two letter rows: toggle middle cell states
	case 'q': set_cell(ca, midcell, true); continue;
	case 'w': set_cell(ca, midcell+1, true); continue;
	case 'e': set_cell(ca, midcell+2, true); continue;
	case 'r': set_cell(ca, midcell+3, true); continue;
	case 't': set_cell(ca, midcell+4, true); continue;
	case 'y': set_cell(ca, midcell+5, true); continue;
	case 'u': set_cell(ca, midcell+6, true); continue;
	case 'i': set_cell(ca, midcell+7, true); continue;	  
	case 'a': set_cell(ca, midcell, false); continue;
	case 's': set_cell(ca, midcell+1, false); continue;
	case 'd': set_cell(ca, midcell+2, false); continue;
	case 'f': set_cell(ca, midcell+3, false); continue;
	case 'g': set_cell(ca, midcell+4, false); continue;
	case 'h': set_cell(ca, midcell+5, false); continue;
	case 'j': set_cell(ca, midcell+6, false); continue;
	case 'k': set_cell(ca, midcell+7, false); continue;
		  
	  // brackets: move the bounds
	case '[': inc_bound_l(ca, -1); continue; 
	case ']': inc_bound_l(ca, 1); continue; 
	case '{': inc_bound_r(ca, -1); continue; 
	case '}': inc_bound_r(ca, 1); continue;
	  // o, p: toggle l/r bounds modes
	case 'o': toggle_bounds_mode(ca, 0); continue;
	case 'p': toggle_bounds_mode(ca, 1); continue;
	  // space: pause/play 
	case ' ':	  	  
	  if(metro_is_running(metro)) { metro_stop(metro); }
	  else { metro_start(metro, 0, -1); }
	  continue;
	  // +/- : faster/slower (shift for more change)
	case '-': inc_metro(metro, (1.0/60.0)); continue;
	case '=': inc_metro(metro, (-1.0/60.0)); continue;	  
	case '_': inc_metro(metro, (1.0/15.0)); continue;
	case '+': inc_metro(metro, (-1.0/15.0)); continue;
	  // z,x,c,v,b,n,m,,,.,/ : patterns!
	case 'z': pat_fill(ca_get_cells(ca), CA_NUM_CELLS, PAT_CLEAR); continue;
	case 'x': pat_fill(ca_get_cells(ca), CA_NUM_CELLS, PAT_ONE); continue;
	case 'c': pat_fill(ca_get_cells(ca), CA_NUM_CELLS, PAT_TWO); continue;
	case 'v': pat_fill(ca_get_cells(ca), CA_NUM_CELLS, PAT_ALT1); continue;
	case 'b': pat_fill(ca_get_cells(ca), CA_NUM_CELLS, PAT_ALT2); continue;
	case 'n': pat_fill(ca_get_cells(ca), CA_NUM_CELLS, PAT_ALT3); continue;
	case 'm': pat_fill(ca_get_cells(ca), CA_NUM_CELLS, PAT_ALT12); continue;
	case ',': pat_fill(ca_get_cells(ca), CA_NUM_CELLS, PAT_ALT123); continue;
	case '.': pat_fill(ca_get_cells(ca), CA_NUM_CELLS, PAT_ALT1234); continue;
	case '/': pat_fill(ca_get_cells(ca), CA_NUM_CELLS, PAT_RAND); continue;
	}
	// if we got here, do more expensive string comparisons for fns
	const char *kname = keyname(k);
	if(strcmp(kname, "^X") == 0) { should_exit = 1; }
    }

    ui_deinit();
}


void ui_init(void) {
  
  if (initscr() == NULL) {
        fail_exit("failed to initialize ncurses");
    }
    visual_mode = true;
    
    CHECK(cbreak);
    CHECK(noecho);
    CHECK(curs_set, 0);
    
    int nrows, ncols;
    getmaxyx(stdscr, nrows, ncols);
    
    win_rule = newwin(1, ncols, 0, 0);
    win_bounds = newwin(1, ncols, 1, 0);
    win_metro = newwin(1, ncols, 2, 0);
    win_cells = newwin(nrows-3, ncols, 3, 0);

    scrollok(win_cells, TRUE);
}

static char bound_mode_symbol(ca_bound_mode_t mode) {
  switch(mode) {
  case CA_BOUND_MODE_FIXED_HIGH: return '=';
  case CA_BOUND_MODE_FIXED_LOW: return '-';
  case CA_BOUND_MODE_WRAPPED: default: return '~';
  }
}  

void ui_update(void *ca) {

  // fixme: can't seem to get these to print at top of ui_loop
  /// shouldn't need them every update
  print_ca_rule(ca);
  print_bounds(ca);
    
  bool* cells = ca_get_cells(ca);
  int l = ca_get_bound_l(ca);
  int r = ca_get_bound_r(ca);
  
  char ch;
  char choob_l = bound_mode_symbol(ca_get_bound_mode_l(ca));
  char choob_r = bound_mode_symbol(ca_get_bound_mode_r(ca));

  for(int i=0; i<CA_NUM_CELLS; ++i) {
    if(i < l) { ch = choob_l; }
    else if(i > r) { ch = choob_r; }
    else { ch = cells[i] ? 'O' : '.'; }
    wprintw(win_cells, "%c", ch);
  }
  wprintw(win_cells, "\n");
  wrefresh(win_cells);
}

void ui_deinit(void) {
    pthread_mutex_lock(&exit_lock);
    CHECK(delwin, win_rule);
    CHECK(delwin, win_bounds);
    CHECK(delwin, win_cells);
    CHECK(endwin);
    visual_mode = false;
    pthread_mutex_unlock(&exit_lock);    
}

void resize(void)
{
    if (LINES >= 4) {
        CHECK(wresize, win_rule, 1, COLS);
        CHECK(wresize, win_bounds, 1, COLS);
	CHECK(wresize, win_metro, 1, COLS);
	CHECK(wresize, win_cells, LINES-2, COLS);
    }

    // batch refreshes and commit them with doupdate()
    CHECK(wnoutrefresh, win_rule);
    CHECK(wnoutrefresh, win_bounds);
    CHECK(wnoutrefresh, win_metro);
    CHECK(wnoutrefresh, win_cells);
    
    CHECK(doupdate);
}

noreturn void fail_exit(const char *msg)
{
    if (visual_mode) {
        endwin();
    }
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}
