OBJS=mrf.o

LUA_INCLUDE = -I $(LUA_HOME)/include
LUA_LIBS := -L $(LUA_HOME)/lib -l lua

KERNEL_MRF_INCLUDE := -I ../c-mrf89xa/

CFLAGS += -g -Wall $(LUA_INCLUDE) $(KERNEL_MRF_INCLUDE)
LDFLAGS += -g -lm -ldl

all: mrf.so

mrf.so: $(OBJS)
	$(CC) -shared -Wl,-soname,$@ -o $@ $^ $(LDFLAGS)

test: mrf.so
	prove -r t

clean:
	@rm -rf *.o calibri calibri.so
