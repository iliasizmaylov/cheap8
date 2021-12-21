CC = gcc

CFLAGS = -I -g -Wall
LDFLAGS = -lsdl2 -lm

DEPS = vm.h opcodes.h c8core.h input.h types.h
SOURCES = main.c vm.c opcodes.c c8core.c input.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = cheap8

%.o : %.c $(DEPS)
	$(CC) -c $< -o $@ $(CFLAGS)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	@rm -f $(TARGET) $(OBJECTS)
	
