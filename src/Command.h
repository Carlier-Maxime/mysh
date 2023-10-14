#ifndef MYSH_COMMAND_H
#define MYSH_COMMAND_H

#include <stdbool.h>

struct private_Command;

typedef struct Command {
    struct private_Command *private;
    bool (*execute)(struct Command* this);
} Command;

Command* Command_create(const char* name, const char** args);
void Command_destroy(Command* this);

#endif //MYSH_COMMAND_H
