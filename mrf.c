#include "mrf.h"

static char* obj_registry = "mrf{registry}";

#if 0
static int create_registry(lua_State *L) {
  /* create registry to store values */
  lua_pushlightuserdata(L, &obj_registry);
  lua_newtable(L);

  lua_createtable(L, 0, 1);
  lua_setmetatable(L, -2);

  lua_settable(L, LUA_REGISTRYINDEX); /* registry[obj_registry] = table */
  lua_pop(L, 1); /* remove obj_registry from stack */
  return 0;
}
#endif

static int mrf_open(lua_State *L) {
  mrf_device *md;
  int status;
  
  DEBUG_LOG("mrf_open\n");

  md = luaL_checkudata(L, 1, MRF_MT);
  if (md->state & STATE_OPENED) {
    luaL_error(L, "mrf device is already open");
  }
  status = open(DEVICE_FILE, O_RDWR);
  if (status == -1) {
    luaL_error(L, "error opening mrf device %s: %s", DEVICE_FILE, strerror(errno));
  }
  md->fd = status;
  md->state = (md->state | STATE_OPENED);
  
  return 0;
}

static int mrf_reset(lua_State *L) {
  mrf_device *md;
  int status;
  
  DEBUG_LOG("mrf_reset\n");

  md = luaL_checkudata(L, 1, MRF_MT);
  if (!(md->state & STATE_OPENED)) {
    luaL_error(L, "mrf device should be opened before reset");
  }
  status = ioctl(md->fd, MRF_IOCRESET, NULL);
  if (status) {
    luaL_error(L, "cannot reset mrf device(%d): %s", status, strerror(errno));
  }
  
  return 0;
}

static luaL_Reg mrf_methods[] = {
  { "open",  mrf_open },
  { "reset", mrf_reset },
  { NULL, NULL }
};

LUALIB_API int luaopen_mrf(lua_State *L) {
  mrf_device *md;
  DEBUG_LOG("luaopen_mrf\n");
  //create_registry(L);

  md = (mrf_device*) lua_newuserdata(L, sizeof(mrf_device));
  memset(md, 0, sizeof(mrf_device));
  md->state = STATE_UNDEFINED;

  /* create metatable */
  luaL_newmetatable(L, MRF_MT);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  luaL_setfuncs(L, mrf_methods, 0);

  /* assign metatable to mrf_device */
  lua_setmetatable(L, -2);
  
  return 1; /* mrf_device */
}
