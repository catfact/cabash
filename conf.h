#include <stdbool.h>
#include <stdint.h>

#include "ca.h"

extern bool conf_load(const char* path);
extern const char* conf_get_addr();
extern const char* conf_get_port();
extern uint8_t conf_get_rule();
extern int conf_get_bound_l();
extern int conf_get_bound_r();
extern ca_bound_mode_t conf_get_bound_mode_l();
extern ca_bound_mode_t conf_get_bound_mode_r();
extern double conf_get_period();
