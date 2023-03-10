# =========================================================================
# 
# Cheap-8 Makefile
# 
#	TODO:
#		- Maybe add separate targets to compile with or without debugger
#		- Maybe add a separate target for cheap8 compiler after it's developed
#
# =========================================================================

# Program name
TARGET		= cheap8

# Compiler
CC			= gcc
CFLAGS		= -I -Wall -Werror

# Linker
LINKER 		= gcc
LFLAGS 		= -lsdl2 -lm -lpanel -lncurses

# Sources, objects and binaries directories
SRCDIR 		= src
OBJDIR 		= obj
BINDIR		= bin

SOURCES		:= $(wildcard $(SRCDIR)/*.c)
INCLUDES	:= $(wildcard $(SRCDIR)/*.h)
OBJECTS		:= $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Compiling objects file into an executable (or a binary)
$(BINDIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR) 
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking done"

# Making object files from all the source files
$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<

# Clean object files but leave a binary file in place
.PHONY: clean
clean:
	@rm -f $(OBJECTS)
	@echo "All object files successfully cleared"

# Delete both object files and a binary
.PHONY: remove
remove: clean
	@rm -f $(BINDIR)/$(TARGET)
	@echo "Executable successfully removed"
	
