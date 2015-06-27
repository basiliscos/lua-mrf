OBJS=mrf.o

LUA_INCLUDE = -I $(LUA_HOME)/include
LUA_LIBS := -L $(LUA_HOME)/lib -l lua

CFLAGS += -fPIC -g -Wall $(LUA_INCLUDE)
LDFLAGS += -g -fPIC -lm -ldl

all: mrf.so

mrf.so: $(OBJS)
	$(CC) -shared -Wl,-soname,$@ -o $@ $^ $(LDFLAGS)

test: calibri.so
        prove -r t
clean:
	@rm -rf *.o calibri calibri.so
