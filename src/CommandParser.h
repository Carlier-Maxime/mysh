#ifndef MYSH_COMMANDPARSER_H
#define MYSH_COMMANDPARSER_H

#include <stdbool.h>

struct private_CommandParser;

typedef struct CommandParser {
    struct private_CommandParser *private;
    bool (*consumeChar)(struct CommandParser* this, char c);
    bool (*isExecutionReady)(struct CommandParser* this);
    void (*execute)(struct CommandParser* this);
} CommandParser;

CommandParser* CommandParser_create();
void CommandParser_destroy(CommandParser* this);

#endif //MYSH_COMMANDPARSER_H
