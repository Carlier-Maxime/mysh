CC = gcc
LD = gcc
CFLAGS = -Wall -Werror -pedantic
LD_FLAGS =
SRC_DIR = src
OBJ_DIR = obj
HEADERS_FILES = $(wildcard $(SRC_DIR)/*.h)
C_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_FILES))
DEPS = $(OBJS:.o=.d)
EXEC = mysh

$(EXEC) : $(OBJS)
	$(LD) $^ $(LD_FLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.c
	$(CC) -MM $< | sed 's/\($*\)\.o[ :]*/$(OBJ_DIR)\/\1.o : /g' > $@

clean :
	rm -f  $(OBJS) $(DEPS) $(EXEC)

.PHONY: clean all

-include $(DEPS)