CC      = $(CROSS_COMPILE)gcc

OBJS = main.o

CFLAGS += -Wall -I$(TEEC_EXPORT)/include

BINARY = hello

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) -o $@ $<

clean:
	rm -f $(OBJS) $(BINARY)
