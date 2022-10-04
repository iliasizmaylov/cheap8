CC = gcc

CFLAGS = -I -Wall -Werror
LDFLAGS = -lsdl2 -lm -lpanel -lncurses

DEPS = vm.h opcodes.h c8core.h input.h types.h c8debug.h
SOURCES = main.c vm.c opcodes.c c8core.c input.c c8debug.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = cheap8

%.o : %.c $(DEPS)
	$(CC) -c -g $(CFLAGS) $< -o $@

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	@rm -f $(TARGET) $(OBJECTS)
