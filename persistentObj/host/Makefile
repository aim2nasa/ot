CC      = $(CROSS_COMPILE)gcc

OBJS = main.o

CFLAGS += -Wall

BINARY = persitentObj

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) -o $@ $<

clean:
	rm -f $(OBJS) $(BINARY)
