CC = gcc
LD = gcc
CFLAGS = -Wall -Werror -pedantic
LD_FLAGS =
SRC_DIR = src
TEST_SRC_DIR = $(SRC_DIR)/test
OBJ_DIR = obj
TEST_OBJ_DIR = $(OBJ_DIR)/test
C_FILES = $(wildcard $(SRC_DIR)/*.c)
TEST_C_FILES = $(wildcard $(TEST_SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_FILES))
TEST_OBJS = $(patsubst $(TEST_SRC_DIR)/%.c,$(TEST_OBJ_DIR)/%.o,$(TEST_C_FILES))
ALL_OBJS = $(OBJS) $(TEST_OBJS)
DEPS = $(ALL_OBJS:.o=.d)

all : $(OBJ_DIR) $(TEST_OBJ_DIR) mysh test

$(OBJ_DIR):
	@mkdir $@

$(TEST_OBJ_DIR):
	@mkdir $@

test : $(TEST_OBJS)
	$(LD) $^ $(LD_FLAGS) -o $@

mysh : $(OBJS)
	$(LD) $^ $(LD_FLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_OBJ_DIR)/%.d: $(TEST_SRC_DIR)/%.c
	@mkdir -p $(TEST_OBJ_DIR)
	@set -e; rm -f $@; \
	$(CC) -MM $< | sed 's/\($*\)\.o[ :]*/$(OBJ_DIR)\/\1.o : /g' > $@

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@set -e; rm -f $@; \
	$(CC) -MM $< | sed 's/\($*\)\.o[ :]*/$(OBJ_DIR)\/\1.o : /g' > $@

clean :
	rm -f  $(TEST_OBJS) $(OBJS) $(DEPS) $(EXEC)
	rmdir $(TEST_OBJ_DIR) $(OBJ_DIR)

.PHONY: clean all

-include $(DEPS)