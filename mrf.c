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

typedef struct mrf_constant {
  const char* key;
  uint32_t value;
} mrf_constant;

mrf_constant power_constants[] = {
  {"-8",  MRF_TXPOWER_MINUS_8 },
  {"-5",  MRF_TXPOWER_MINUS_5 },
  {"-2",  MRF_TXPOWER_MINUS_2 },
  {"+1",  MRF_TXPOWER_PLUS_1  },
  {"+4",  MRF_TXPOWER_PLUS_4  },
  {"+7",  MRF_TXPOWER_PLUS_7  },
  {"+10", MRF_TXPOWER_PLUS_10 },
  {"+13", MRF_TXPOWER_PLUS_13 },
};

mrf_constant frequency_constants[] = {
  {"864.74",  MRF_FREQ_864_74 },
};

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
  int reinitialize_registers = 0;

  DEBUG_LOG("mrf_reset\n");

  md = luaL_checkudata(L, 1, MRF_MT);
  if (!(md->state & STATE_OPENED)) {
    luaL_error(L, "mrf device should be opened before reset");
  }

  if (lua_gettop(L) >= 2) {
    reinitialize_registers = lua_toboolean(L, 2);
  }

  status = ioctl(md->fd, MRF_IOC_RESET, reinitialize_registers);
  if (status) {
    luaL_error(L, "cannot reset mrf device(%d): %s", status, strerror(errno));
  }

  return 0;
}

static int mrf_set_addr(lua_State *L) {
  mrf_device *md;
  lua_Integer node_id;
  lua_Integer network_id;
  mrf_address addr;
  int status;

  DEBUG_LOG("mrf_set_addr\n");
  md = luaL_checkudata(L, 1, MRF_MT);

  node_id = luaL_checkinteger(L, 2);
  if ((node_id <= 0) || (node_id > 255)) luaL_argerror(L, 2, "Invalid node address (must be in range 0x01 .. 0xFF");
  network_id = luaL_checkinteger(L, 3);
  addr.node_id = (uint8_t) node_id;
  addr.network_id = (uint32_t) network_id;

  if (!(md->state & STATE_OPENED)) {
    luaL_error(L, "mrf device should be opened set addr");
  }

  status = ioctl(md->fd, MRF_IOC_SETADDR, &addr);
  if (status) {
    luaL_error(L, "cannot set address for mrf device(%d): %s", status, strerror(errno));
  }

  return 0;
}

static int mrf_set_power(lua_State *L) {
  mrf_device *md;
  const char* power_label;
  int i, status;
  int idx = -1;

  DEBUG_LOG("mrf_set_power\n");
  
  md = luaL_checkudata(L, 1, MRF_MT);
  if (!(md->state & STATE_OPENED)) {
    luaL_error(L, "mrf device should be opened before set power");
  }
  power_label = luaL_checklstring(L, 2, NULL);

  for (i = 0; i < ARRAY_LENGTH(power_constants); i++) {
    if (strcmp(power_constants[i].key, power_label) == 0) {
      idx = i;
    }
  }

  if (idx == -1) luaL_error(L, "unknown/impossible tx power value '%s'", power_label);

  status = ioctl(md->fd, MRF_IOC_SETPOWER, power_constants[idx].value);
  if (status) {
    luaL_error(L, "cannot set power %s on device(%d): %s", power_label, status, strerror(errno));
  }

  return 0;
}

static int mrf_set_freq(lua_State *L) {
  mrf_device *md;
  const char* freq_label;
  int i, status;
  int idx = -1;

  DEBUG_LOG("mrf_set_freq\n");

  md = luaL_checkudata(L, 1, MRF_MT);
  if (!(md->state & STATE_OPENED)) {
    luaL_error(L, "mrf device should be opened befor set frequency");
  }
  freq_label = luaL_checklstring(L, 2, NULL);

  for (i = 0; i < ARRAY_LENGTH(frequency_constants); i++) {
    if (strcmp(frequency_constants[i].key, freq_label) == 0) {
      idx = i;
    }
  }

  if (idx == -1) luaL_error(L, "unknown/impossible frequency value '%s'", freq_label);

  status = ioctl(md->fd, MRF_IOC_SETFREQ,  frequency_constants[idx].value);
  if (status) {
    luaL_error(L, "cannot set frequency %s on device(%d): %s", freq_label, status, strerror(errno));
  }

  return 0;
}

static luaL_Reg mrf_methods[] = {
  { "open",      mrf_open      },
  { "reset",     mrf_reset     },
  { "set_addr",  mrf_set_addr  },
  { "set_power", mrf_set_power },
  { "set_freq",  mrf_set_freq },
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
