CC      = $(CROSS_COMPILE)gcc

OBJS = main.o util.o

CFLAGS += -Wall -I$(TEEC_EXPORT)/include -I$(OKEY_ROOT) -I../common
LDADD += -lteec -L$(TEEC_EXPORT)/lib -lokey -L$(OKEY_ROOT)

BINARY = kgen

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDADD)

util.o: ../common/util.c ../common/util.h
	$(CC) -c ../common/util.c 

clean:
	rm -f $(OBJS) $(BINARY)
