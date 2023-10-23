#ifndef MYSH_COMMAND_PARSER_H
#define MYSH_COMMAND_PARSER_H

#include <stdbool.h>
#include "CommandFactory.h"
#include "TokenMapper.h"

typedef struct CommandParser {
    unsigned int size, pos;
    char *chars;
    CommandFactory* factory;
    TokenMapper* tokenMapper;
} CommandParser;

CommandParser* CommandParser_create();
void CommandParser_destroy(CommandParser* this);

bool CommandParser_consumeChar(CommandParser* this, char c);
#endif //MYSH_COMMAND_PARSER_H
