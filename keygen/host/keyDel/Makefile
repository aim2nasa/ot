CC      = $(CROSS_COMPILE)gcc

OBJS = main.o

CFLAGS += -Wall -I../../ta/include -I$(TEEC_EXPORT)/include -I$(OKEY_ROOT)
CFLAGS += -I$(TA_DEV_KIT_DIR)/host_include
LDADD += -lteec -L$(TEEC_EXPORT)/lib -lokey -L$(OKEY_ROOT)

BINARY = keydel

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) -o $@ $< $(LDADD)

clean:
	rm -f $(OBJS) $(BINARY)
