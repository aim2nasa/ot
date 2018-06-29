CC      = $(CROSS_COMPILE)gcc

CFLAGS += -Wall -I../../ta/include -I$(TEEC_EXPORT)/include
LDADD += -lteec -L$(TEEC_EXPORT)/lib
LDADD += ./enumHelper.o

LIBNAME = libokey
BINARY = $(LIBNAME).so.1 
OBJS = okey.o enumHelper.o

all: $(BINARY)

$(BINARY): $(OBJS)
	@echo $@:$(OBJS)...
	$(CC) -shared -Wl,-soname,$@ -o $@ $< $(LDADD)
	mkdir lib
	mkdir include
	cp okey.h include
	cp libokey.so.1 lib
	ln -s lib/libokey.so.1 lib/libokey.so
	ln -s libokey.so.1 libokey.so

okey.o : okey.c
	@echo $@...
	$(CC) -fPIC -c okey.c $(CFLAGS)

enumHelper.o : enumHelper.c
	@echo $@...
	$(CC) -fPIC -c enumHelper.c $(CFLAGS)

clean:
	rm -f $(OBJS) $(BINARY)
	rm -rf libokey*
	rm -rf lib
	rm -rf include
