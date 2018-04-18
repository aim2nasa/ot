CC      = $(CROSS_COMPILE)gcc

OBJS = main.o

CFLAGS += -Wall -I$(TEEC_EXPORT)/include -I$(OKEY_ROOT)
LDADD += -lteec -L$(TEEC_EXPORT)/lib -lokey -L$(OKEY_ROOT)

BINARY = keyinj

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) -o $@ $< $(LDADD)

clean:
	rm -f $(OBJS) $(BINARY)
