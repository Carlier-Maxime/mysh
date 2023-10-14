#ifndef MYSH_COMMAND_FACTORY_H
#define MYSH_COMMAND_FACTORY_H

#include <stdbool.h>
#include "Command.h"

struct private_CommandFactory;

typedef struct CommandFactory {
    struct private_CommandFactory *private;
    struct CommandFactory* (*create)(struct CommandFactory* this, const char* name);
    struct CommandFactory* (*addArgument)(struct CommandFactory* this, const char* arg);
    Command* (*build)(struct CommandFactory* this);
    bool (*isMaking)(struct CommandFactory* this);
} CommandFactory;

CommandFactory *CommandFactory_create();
void CommandFactory_destroy(CommandFactory *this);

#endif //MYSH_COMMAND_FACTORY_H
