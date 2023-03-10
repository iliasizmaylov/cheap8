# =========================================================================
# 
# Cheap-8 Makefile
# 
# =========================================================================

TARGET		= cheap8

CC			= gcc
CFLAGS		= -I -Wall -Werror

LINKER 		= gcc
LFLAGS 		= -lsdl2 -lm -lpanel -lncurses

SRCDIR 		= src
OBJDIR 		= obj
BINDIR		= bin

SOURCES		:= $(wildcard $(SRCDIR)/*.c)
INCLUDES	:= $(wildcard $(SRCDIR)/*.h)
OBJECTS		:= $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

$(BINDIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR) 
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking done"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<

.PHONY: clean
clean:
	@rm -f $(OBJECTS)
	@echo "All object files successfully cleared"

.PHONY: remove
remove: clean
	@rm -f $(BINDIR)/$(TARGET)
	@echo "Executable successfully removed"
	
