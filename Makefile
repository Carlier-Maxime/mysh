CC = gcc
LD = gcc
CFLAGS = -Wall -Werror -pedantic
LD_FLAGS =
SRC_DIR = src
OBJ_DIR = obj
OBJS_GLOBAL = $(patsubst $(SRC_DIR)/utils/%.c,$(OBJ_DIR)/utils/%.o,$(wildcard $(SRC_DIR)/utils/*.c))
EXE = mysh test myls
OBJ_DIRS := $(addprefix $(OBJ_DIR)/, $(EXE) utils)

all : $(OBJ_DIRS) $(EXE)
$(OBJ_DIRS):
	@mkdir -p $@

define build_exe
$(1) : $(OBJS_GLOBAL) $(patsubst $(SRC_DIR)/$(1)/%.c,$(OBJ_DIR)/$(1)/%.o,$(wildcard $(SRC_DIR)/$(1)/*.c))
	$(LD) $$^ $(LD_FLAGS) -o $$@
endef


$(foreach exe, $(EXE), $(eval $(call build_exe,$(exe))))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	rm -rf  $(OBJ_DIR) $(EXE)

define build_dep
$(OBJ_DIR)/$(1)/%.d: $(SRC_DIR)/$(1)/%.c $(OBJ_DIR)/$(1)
	@$(CC) -MM $$< | sed 's/\($$*\)\.o[ :]*/$(OBJ_DIR)\/$(1)\/\1.o : /g' > $$@
endef

$(foreach exe, $(EXE) utils, $(eval $(call build_dep,$(exe))))

DEPS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.d,$(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c))


.PHONY: clean all

-include $(DEPS)