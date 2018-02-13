CC      = $(CROSS_COMPILE)gcc

OBJS = main.o ../common/helper.o

CFLAGS += -Wall -I../../ta/include -I$(TEEC_EXPORT)/include
CFLAGS += -I$(TA_DEV_KIT_DIR)/host_include
CFLAGS += -I../common
LDADD += -lteec -L$(TEEC_EXPORT)/lib
LDADD += ../common/helper.o

BINARY = perObj

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) -o $@ $< $(LDADD)

clean:
	rm -f $(OBJS) $(BINARY)
