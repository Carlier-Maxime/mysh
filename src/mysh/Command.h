#ifndef MYSH_COMMAND_H
#define MYSH_COMMAND_H

#include <stdbool.h>

typedef struct Command {
    char* name;
    char** args;
} Command;

Command* Command_create(const char* name, const char** args);
void Command_destroy(Command* this);
bool Command_execute(const Command* this);

#endif //MYSH_COMMAND_H
