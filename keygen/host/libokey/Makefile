CC      = $(CROSS_COMPILE)gcc

CFLAGS += -Wall -I../../ta/include -I$(TEEC_EXPORT)/include
LDADD += -lteec -L$(TEEC_EXPORT)/lib

LIBNAME = libokey
BINARY = $(LIBNAME).so.1 
OBJS = okey.o

all: $(BINARY)

$(BINARY): $(OBJS)
	@echo $@:$(OBJS)...
	$(CC) -shared -Wl,-soname,$@ -o $@ $< $(LDADD)

okey.o : okey.c
	@echo $@...
	$(CC) -fPIC -c okey.c $(CFLAGS)
clean:
	rm -f $(OBJS) $(BINARY)
