CC      = $(CROSS_COMPILE)gcc

OBJS = main.o helper.o

CFLAGS += -Wall -I../ta/include -I$(TEEC_EXPORT)/include
CFLAGS += -I$(TA_DEV_KIT_DIR)/host_include
LDADD += -lteec -L$(TEEC_EXPORT)/lib

BINARY = persistentObj

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) -o $@ $< $(LDADD) helper.o

clean:
	rm -f $(OBJS) $(BINARY)
