#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h> 

#include "conf.h"

lua_State *lvm;

bool conf_load(const char* filename) {
  char path[PATH_MAX + 128];
  lvm = luaL_newstate();
  luaL_openlibs(lvm);
  lua_pcall(lvm, 0, 0, 0);

  getcwd(path, sizeof(path));
  snprintf(&(path[strlen(path)]),  sizeof(path), "/%s", filename);

  printf("conf path: %s\n", path);
  
  if (luaL_dofile(lvm, path)) {
    return false;
  }
  return true;
}

const char* conf_get_addr() {
  lua_getglobal(lvm, "conf");
  lua_getfield(lvm, -1, "addr");
   const char* res = lua_tostring(lvm, -1);
  lua_settop(lvm, 0);
  return res;
}

const char* conf_get_port() {
  lua_getglobal(lvm, "conf");
  lua_getfield(lvm, -1, "port");
  const char* res = lua_tostring(lvm, -1);
  lua_settop(lvm, 0);
  return res;
}

 uint8_t conf_get_rule() {
  lua_getglobal(lvm, "conf");
  lua_getfield(lvm, -1, "rule");
  uint8_t res = lua_tonumber(lvm, -1);
  lua_settop(lvm, 0);
  return res;
}

 int conf_get_bound_l() {
  lua_getglobal(lvm, "conf");
  lua_getfield(lvm, -1, "bound_l");
  int res = lua_tonumber(lvm, -1);
  lua_settop(lvm, 0);
  return res;
}

 int conf_get_bound_r() {
  lua_getglobal(lvm, "conf");
  lua_getfield(lvm, -1, "bound_r");
   int res = lua_tonumber(lvm, -1);
  lua_settop(lvm, 0);
  return res;
}

 ca_bound_mode_t conf_get_bound_mode_l() {
  lua_getglobal(lvm, "conf");
  lua_getfield(lvm, -1, "bound_mode_l");
   int res = lua_tonumber(lvm, -1);
  lua_settop(lvm, 0);
  return res;
}

 ca_bound_mode_t conf_get_bound_mode_r() {
  lua_getglobal(lvm, "conf");
  lua_getfield(lvm, -1, "bound_mode_r");
   int res = lua_tonumber(lvm, -1);
  lua_settop(lvm, 0);
  return res;
}

double conf_get_period() {
  lua_getglobal(lvm, "conf");
  lua_getfield(lvm, -1, "period");
   double res = lua_tonumber(lvm, -1);
  lua_settop(lvm, 0);
  return res;
}
