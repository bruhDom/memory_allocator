# --- Configuration ---------------------------------------------------------
BIN     := myalloc
CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -O0 -g -D_GNU_SOURCE
LDFLAGS :=

# --- Auto-discovered files -------------------------------------------------
SRCS := $(wildcard *.c)
HDRS := $(wildcard *.h)
OBJS := $(SRCS:.c=.o)

# --- Targets ---------------------------------------------------------------
.PHONY: all run clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN) $(LDFLAGS)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(OBJS) $(BIN)