CC = gcc
CFLAGS = -Wall -pedantic
SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.c)

OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

EXEC = mysh

all : $(EXEC)

mysh : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/main.o : $(SRC_DIR)/main.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c $(SRC_DIR)/%.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	rm -f  $(OBJ) $(EXEC)

.PHONY: clean all
