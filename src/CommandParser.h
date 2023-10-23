#ifndef MYSH_COMMAND_PARSER_H
#define MYSH_COMMAND_PARSER_H

#include <stdbool.h>
#include "CommandFactory.h"

typedef struct CommandParser {
    unsigned int size, pos;
    bool backslash;
    char *chars;
    CommandFactory* factory;
} CommandParser;

CommandParser* CommandParser_create();
void CommandParser_destroy(CommandParser* this);

bool CommandParser_consumeChar(CommandParser* this, char c);
#endif //MYSH_COMMAND_PARSER_H
