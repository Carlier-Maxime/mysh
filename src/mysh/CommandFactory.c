#include <malloc.h>
#include <string.h>
#include "CommandFactory.h"
#include "../utils/Error.h"

bool CommandFactory_resizeIfFull(CommandFactory* this) {
    if (!this || !this->commands) {
        Error_SetError(ERROR_NULL_POINTER);
        return false;
    }
    if (this->nb_command==this->max_commands) {
        Command** tmp = realloc(this->commands, sizeof(Command*)*(this->max_commands<<=1));
        if (!tmp) {
            Error_SetError(ERROR_MEMORY_ALLOCATION);
            this->max_commands>>=1;
            return false;
        }
        this->commands=tmp;
        for (unsigned int i=this->nb_command; i<this->max_commands; i++) this->commands[i]=NULL;
    }
    Error_SetError(ERROR_NONE);
    return true;
}

const Command** CommandFactory_buildCommands(CommandFactory* this, const Token* tokens, char** args) {
    if (!this || !this->commands || !tokens || !args) {
        Error_SetError(ERROR_NULL_POINTER);
        return NULL;
    }
    unsigned int nb_args=0, start_args=0;
    char* tmp;
    this->nb_command=0;
    for (unsigned i=0; tokens[i]; i++) {
        if (!CommandFactory_resizeIfFull(this)) {
            this->nb_command=0;
            return NULL;
        }
        switch (tokens[i]) {
            case TOKEN_ERROR:
                this->nb_command=0;
                return NULL;
            case TOKEN_STR:
                nb_args++;
                break;
            case TOKEN_COMMAND:
                if (nb_args==0) {
                    this->nb_command=0;
                    return NULL;
                }
                tmp = args[start_args+nb_args];
                args[start_args+nb_args]=NULL;
                Command* command = Command_create(args[start_args], (const char**) (args+start_args));
                args[start_args+nb_args]=tmp;
                if (!command) {
                    this->nb_command=0;
                    return NULL;
                }
                Command_destroy(this->commands[this->nb_command]);
                this->commands[this->nb_command++]=command;
                start_args+=nb_args;
                nb_args=0;
                break;
            default:
                break;
        }
    }
    return (const Command**) this->commands;
}

CommandFactory *CommandFactory_create() {
    Error_SetError(ERROR_MEMORY_ALLOCATION);
    CommandFactory *this = malloc(sizeof(CommandFactory));
    if (!this) return NULL;
    this->max_commands=4;
    this->nb_command=0;
    if (!(this->commands=malloc(sizeof(Command*)*this->max_commands))) goto cleanup;
    for (unsigned int i=0; i<this->max_commands; i++) this->commands[i]=NULL;
    Error_SetError(ERROR_NONE);
    return this;
cleanup:
    CommandFactory_destroy(this);
    return NULL;
}

void CommandFactory_destroy(CommandFactory *this) {
    if (!this) return;
    if (this->commands) {
        for (unsigned int i=0; i<this->max_commands; i++) Command_destroy(this->commands[i]);
        free(this->commands);
    }
    free(this);
}