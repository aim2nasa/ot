CC      = $(CROSS_COMPILE)gcc

OBJS = main.o enumHelper.o

CFLAGS += -Wall -I../../ta/include -I$(TEEC_EXPORT)/include
LDADD += -lteec -L$(TEEC_EXPORT)/lib
LDADD += ./enumHelper.o

BINARY = keyenum

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) -o $@ $< $(LDADD)

clean:
	rm -f $(OBJS) $(BINARY)
