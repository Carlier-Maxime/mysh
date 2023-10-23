#ifndef MYSH_COMMAND_FACTORY_H
#define MYSH_COMMAND_FACTORY_H

#include <stdbool.h>
#include "Command.h"

typedef struct CommandFactory {
    unsigned int size, pos;
    char **words;
} CommandFactory;

CommandFactory *CommandFactory_create();
void CommandFactory_destroy(CommandFactory *this);
CommandFactory* CommandFactory_addArgument(CommandFactory* this, const char* arg);
Command* CommandFactory_build(CommandFactory* this);
int CommandFactory_getNbArgs(CommandFactory* this);

#endif //MYSH_COMMAND_FACTORY_H
