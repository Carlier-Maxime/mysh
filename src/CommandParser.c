#include <malloc.h>
#include "CommandParser.h"
#include "Error.h"
#include "macro.h"
#include "Environment.h"

bool CommandParser_executeCommandQueue(CommandParser* this) {
    if (!this) {
        Error_SetError(ERROR_NULL_POINTER);
        return false;
    }
    Command *command = CommandFactory_build(this->factory);
    if (!command) return false;
    if (!Command_execute(command)) return false;
    this->pos=0;
    Error_SetError(ERROR_NONE);
    return true;
}

bool CommandParser_resizeIfFull(CommandParser* this) {
    if (!this || !this->chars) {
        Error_SetError(ERROR_NULL_POINTER);
        return false;
    }
    if (this->pos==this->size) {
        char* tmp;
        tmp=realloc(this->chars, this->size<<=1);
        if (!tmp) {
            Error_SetError(ERROR_MEMORY_ALLOCATION);
            this->size>>=1;
            return false;
        }
        this->chars=tmp;
    }
    Error_SetError(ERROR_NONE);
    return true;
}

bool CommandParser_consumeChar(struct CommandParser* this, char c) {
    if (c==EOF) return false;
    if (!this || !this->chars) {
        Error_SetError(ERROR_NULL_POINTER);
        return false;
    }
    if (!CommandParser_resizeIfFull(this)) return false;
    Token token = TokenMapper_processChar(this->tokenMapper, c);
    switch (token) {
        case TOKEN_ERROR:
            return false;
        case TOKEN_CHAR:
            this->chars[this->pos++]=c;
            break;
        case TOKEN_STR:
            if (!this->pos) break;
            this->chars[this->pos++]='\0';
            if (!CommandFactory_addArgument(this->factory, this->chars)) return false;
            this->pos=0;
            break;
        case TOKEN_EXECUTE:
            if (this->pos) {
                this->chars[this->pos]='\0';
                if (!CommandFactory_addArgument(this->factory, this->chars)) return false;
            }
            int nbArgs=CommandFactory_getNbArgs(this->factory);
            if (nbArgs==-1) return false;
            if (nbArgs && !CommandParser_executeCommandQueue(this)) return false;
            printf("%s%s%s> ", BLUE_BEGIN, Environment_getCwd(), COLOR_RESET);
            this->pos=0;
            break;
        case TOKEN_NEW_LINE:
            printf("> ");
            break;
        default:
            break;
    }
    Error_SetError(ERROR_NONE);
    return true;
}

CommandParser* CommandParser_create() {
    Error_SetError(ERROR_MEMORY_ALLOCATION);
    CommandParser *this = malloc(sizeof(CommandParser));
    if (!this) return NULL;
    this->size=16;
    this->pos=0;
    if (!(this->chars = malloc(sizeof(char)*this->size))) goto cleanup;
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
    free(this->chars);
    free(this);
}