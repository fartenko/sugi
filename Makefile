# Reference: 
# https://stackoverflow.com/questions/7004702

TARGET = exec

SRCDIR = src
OBJDIR = obj
BINDIR = bin
INCDIR = include

CC 		 = gcc
LINKER = gcc

CFLAGS = -w -I./$(INCDIR)/ 
LFLAGS = -lGL -lGLU -lGLEW -lSDL2 -lSDL2main

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(INCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

rm = rm -r -f

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "LINKER\t$(@)"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "CC\t"$<

run:
	@$(BINDIR)/$(TARGET)

clean: 
	@$(rm) $(OBJECTS)
	@echo "$(rm) $(OBJECTS)"

remove:
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "$(rm) $(BINDIR)/$(TARGET)"


# Old version
# Reference:
# Lazy Foo's SDL2 Tutorials.
# ---------------------------------------
# OBJS = main.c  
# CC = gcc
#
# COMPILER_FLAGS = -w
# LINKER_FLAGS = -lSDL2 -lSDL2main
#
# OBJ_NAME = bin\exec 
# 
# INCLUDE_PATHS = -I./include/
# 
# all : $(OBJS)
# 	$(CC) $(OBJS) $(INCLUDE_PATHS)    \
# 	$(COMPILER_FLAGS) $(LINKER_FLAGS) \
# 	-o $(OBJ_NAME)
# ---------------------------------------
