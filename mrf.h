#ifndef MRF_H
#define MRF_H

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "mrf89xa.h"

#define DEBUG_LOG(format, ...) fprintf(stderr, format, ##__VA_ARGS__)
#define DEVICE_FILE "/dev/mrf"

#define MRF_MT "mrf{mt}"

#define STATE_UNDEFINED 0
#define STATE_OPENED 1


typedef struct mrf_device {
  int fd;
  int state;
} mrf_device;

#endif /* MRF_H */
