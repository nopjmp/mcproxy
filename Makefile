CC=gcc
CFLAGS=-g -Wall -I.
LDFLAGS=
CFILES := $(wildcard *.c)
OBJS := $(CFILES:.c=.o)
DEPS := $(wildcard *.h)
EXECUTABLE := mcproxy

# libev
CFLAGS += $(shell pkg-config --cflags libev)
LDFLAGS += $(shell pkg-config --libs libev)

all: $(OBJS) $(EXECUTABLE) 

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXECUTABLE): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJS) $(EXECUTABLE) *~ core
