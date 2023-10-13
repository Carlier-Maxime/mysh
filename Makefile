CC = gcc
CFLAGS = -Wall
SRCDIR = src
OBJDIR = obj

SRC = $(wildcard $(SRCDIR)/*.c)

OBJ = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))

EXEC = mysh

all : $(EXEC)

mysh : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

obj/main.o : src/main.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o : $(SRCDIR)/%.c $(SRCDIR)/%.h
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	rm -f  $(OBJ) $(EXEC)

.PHONY: clean all
