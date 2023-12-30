#ifndef MYSH_COMMAND_FACTORY_H
#define MYSH_COMMAND_FACTORY_H

#include <stdbool.h>
#include "Command.h"
#include "TokenMapper.h"

typedef struct CommandFactory {
    unsigned int nb_command, max_commands;
    Command** commands;
} CommandFactory;

typedef struct _subcommand{
	char** args;
	Token* tokens;
} subcommand;

CommandFactory *CommandFactory_create();
void CommandFactory_destroy(CommandFactory *this);
/*subcommand**/ void create_subcommand(subcommand* _subcommand, char** args, int args_length, Token* tokens, int token_length);
Token* simplify_token_tab(const Token* tokens);

void execute_subcommand_list(subcommand* subcommand_list, int idx_subcommand_list, Token* token_list, int idx_token_list);
const Command** CommandFactory_buildCommands(CommandFactory* this, const Token *tokens, char** args);

#endif //MYSH_COMMAND_FACTORY_H
