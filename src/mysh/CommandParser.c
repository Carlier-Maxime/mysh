#include <malloc.h>
#include "CommandParser.h"
#include "../utils/Error.h"
#include "../utils/macro.h"
#include "Environment.h"

#define COMMAND_PARSER_WORD_DEFAULT_LEN 16

bool CommandParser_resizeIfFull(CommandParser* this) {
    if (!this || !this->args || !this->len_args) {
        Error_SetError(ERROR_NULL_POINTER);
        return false;
    }
    Error_SetError(ERROR_MEMORY_ALLOCATION);
    if (this->nb_arg == this->max_args) {
        char** tmp=realloc(this->args, sizeof(char*) * (this->max_args<<=1));
        if (!tmp) {
            this->max_args>>=1;
            return false;
        }
        unsigned int* tmp2 = realloc(this->len_args, sizeof(unsigned int) * this->max_args);
        if (!tmp2) {
            this->max_args>>=1;
            return false;
        }
        for (unsigned int i=this->nb_arg; i < this->max_args; i++) {
            tmp2[i] = COMMAND_PARSER_WORD_DEFAULT_LEN;
            if (!(tmp[i]=malloc(sizeof(char)*tmp2[i]))) {
                this->max_args>>=1;
                return false;
            }
        }
        this->args=tmp;
        this->len_args=tmp2;
    }
    if (this->arg_pos == this->len_args[this->nb_arg]) {
        char* tmp = realloc(this->args[this->nb_arg], this->len_args[this->nb_arg]<<=1);
        if (!tmp) {
            this->len_args[this->nb_arg]>>=1;
            return false;
        }
        this->args[this->nb_arg]=tmp;
    }
    if (this->nb_token == this->max_tokens) {
        Token* tmp = realloc(this->tokens, this->max_tokens<<=1);
        if (!tmp) {
            this->max_tokens>>=1;
            return false;
        }
        this->tokens=tmp;
    }
    Error_SetError(ERROR_NONE);
    return true;
}

bool CommandParser_consumeChar(struct CommandParser* this, char c) {
    if (c==EOF) return false;
    if (!this || !this->args || !this->len_args) {
        Error_SetError(ERROR_NULL_POINTER);
        return false;
    }
    if(!TokenMapper_setCurrentChar(this->tokenMapper, c)) return false;
    Token token;
    const Command** commands;
    unsigned int i;
    while ((token=TokenMapper_process(this->tokenMapper))!=TOKEN_NONE) {
        if (!CommandParser_resizeIfFull(this)) return false;
        if (token!=TOKEN_CHAR) this->tokens[this->nb_token++]=token;
        switch (token) {
            case TOKEN_ERROR:
                return false;
            case TOKEN_CHAR:
                this->args[this->nb_arg][this->arg_pos++]=c;
                break;
            case TOKEN_STR:
                if (!this->arg_pos) break;
                this->args[this->nb_arg][this->arg_pos++]='\0';
                this->arg_pos=0;
                this->nb_arg++;
                break;
            case TOKEN_EXECUTE:
                this->args[this->nb_arg]=NULL;
                if (!(commands=CommandFactory_buildCommands(this->factory, this->tokens, this->args))) return false;
                for (i=0; commands[i]; i++) Command_execute(commands[i]);
                printf("%s%s%s> ", BLUE_BEGIN, Environment_getCwd(), COLOR_RESET);
                this->arg_pos=0;
                this->nb_arg=0;
                this->nb_token=0;
                break;
            case TOKEN_NEW_LINE:
                printf("> ");
                break;
            default:
                break;
        }
    }
    Error_SetError(ERROR_NONE);
    return true;
}

CommandParser* CommandParser_create() {
    Error_SetError(ERROR_MEMORY_ALLOCATION);
    CommandParser *this = malloc(sizeof(CommandParser));
    if (!this) return NULL;
    this->max_args=16;
    this->nb_arg=0;
    this->arg_pos=0;
    this->max_tokens=32;
    this->nb_token=0;
    this->tokens=NULL;
    this->args=NULL;
    this->len_args=NULL;
    this->factory=NULL;
    this->tokenMapper=NULL;
    if (!(this->args = malloc(sizeof(char*) * this->max_args))) goto cleanup;
    for (unsigned int i=0; i<this->max_args; i++) this->args[i]=NULL;
    if (!(this->len_args = malloc(sizeof(unsigned int) * this->max_args))) goto cleanup;
    for (unsigned int i=0; i<this->max_args; i++) {
        this->len_args[i]=COMMAND_PARSER_WORD_DEFAULT_LEN;
        if(!(this->args[i] = malloc(sizeof(char) * this->len_args[i]))) goto cleanup;
    }
    if (!(this->tokens = malloc(sizeof(Token) * this->max_tokens))) goto cleanup;
    if (!(this->factory=CommandFactory_create())) goto cleanup;
    if (!(this->tokenMapper=TokenMapper_create())) goto cleanup;
    Error_SetError(ERROR_NONE);
    return this;
cleanup:
    CommandParser_destroy(this);
    return NULL;
}

void CommandParser_destroy(CommandParser* this) {
    if (!this) return;
    TokenMapper_destroy(this->tokenMapper);
    CommandFactory_destroy(this->factory);
    if (this->args) for (unsigned int i=0; i < this->max_args; i++) free(this->args[i]);
    free(this->args);
    free(this->len_args);
    free(this->tokens);
    free(this);
}