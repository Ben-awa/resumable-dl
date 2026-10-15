CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -Iinclude
TARGET  = dl
SRCS    = src/main.c src/create.c src/write.c src/status.c src/progress.c
OBJS    = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c include/dl.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) .progress

.PHONY: all clean
