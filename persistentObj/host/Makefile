CC      = $(CROSS_COMPILE)gcc

OBJS = main.o

CFLAGS += -Wall -I$(TEEC_EXPORT)/include
LDADD += -lteec -L$(TEEC_EXPORT)/lib

BINARY = persitentObj

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) -o $@ $< $(LDADD)

clean:
	rm -f $(OBJS) $(BINARY)
