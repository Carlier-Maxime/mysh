#ifndef MYSH_COMMAND_PARSER_H
#define MYSH_COMMAND_PARSER_H

#include <stdbool.h>

struct private_CommandParser;

typedef struct CommandParser {
    struct private_CommandParser *private;
    bool (*consumeChar)(struct CommandParser* this, char c);
} CommandParser;

CommandParser* CommandParser_create();
void CommandParser_destroy(CommandParser* this);

#endif //MYSH_COMMAND_PARSER_H
