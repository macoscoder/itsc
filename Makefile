CFLAGS = -Wall -g
LDFLAGS =
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))
PROG = a.out

.PHONY: all
all: $(PROG)

$(PROG): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) $(PROG) $(OBJS)
