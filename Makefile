# Reference:
# https://stackoverflow.com/questions/7004702

TARGET = exec

SRCDIR = src
OBJDIR = obj
BINDIR = bin
INCDIR = include

CC     = gcc
LINKER = gcc

CFLAGS = -w -g -I./$(INCDIR)/ -I/usr/include/lua5.3
LFLAGS = -lGL -lGLU -lGLEW -lSDL2 -lSDL2main -lm -llua5.3

# Linker flags:
# * -lGL -lGLU -lGLEW : GLEW libs
# * -lSDL2 -lSDL2main : SDL2 libs
# * -lm : math lib
# * -llualib5.2
# * -lluar5.2
# Include Flags
# * -I/usr/include/lua5.2 : lua

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


setup:
	@mkdir $(BINDIR)
	@mkdir $(OBJDIR)

run:
	@$(BINDIR)/$(TARGET)

clean:
	@$(rm) $(OBJECTS)
	@echo "$(rm) $(OBJECTS)"

remove:
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "$(rm) $(BINDIR)/$(TARGET)"
