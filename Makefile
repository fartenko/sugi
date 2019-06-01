# Reference: 
# https://stackoverflow.com/questions/7004702

TARGET = exec

SRCDIR = src
OBJDIR = obj
BINDIR = bin
INCDIR = include

CC     = gcc
LINKER = gcc

CFLAGS = -w -I./$(INCDIR)/ 
LFLAGS = -lGL -lGLU -lGLEW -lSDL2 -lSDL2main -lm

# Linker flags:
# * -lGL -lGLU -lGLEW: GLEW libs
# * -lSDL2 -lSDL2main: SDL2 libs
# * -lm: math lib

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

