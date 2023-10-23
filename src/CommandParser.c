#include <malloc.h>
#include "CommandParser.h"
#include "Error.h"
#include "macro.h"
#include "Environment.h"

#define IS_WHITE_SPACE(c) ((c)=='\n' || (c)==' ' || (c)=='\t' || (c)=='\v' || (c)=='\r')

typedef enum {
    TOKEN_ERROR,
    TOKEN_NONE,
    TOKEN_CHAR,
    TOKEN_STR,
    TOKEN_EXECUTE,
    TOKEN_ESCAPE
} Token;

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

Token CommandParser_processToken(CommandParser* this, char c) {
    if (!this) {
        Error_SetError(ERROR_NULL_POINTER);
        return TOKEN_ERROR;
    }
    Error_SetError(ERROR_NONE);
    if (c=='\\' || this->backslash) return TOKEN_ESCAPE;
    if (c=='\n') return TOKEN_EXECUTE;
    if (IS_WHITE_SPACE(c)) return this->pos ? TOKEN_STR : TOKEN_NONE;
    return TOKEN_CHAR;
}

bool CommandParser_consumeChar(struct CommandParser* this, char c) {
    if (c==EOF) return false;
    if (!this || !this->chars) {
        Error_SetError(ERROR_NULL_POINTER);
        return false;
    }
    if (!CommandParser_resizeIfFull(this)) return false;
    Token token = CommandParser_processToken(this, c);
    switch (token) {
        case TOKEN_ERROR:
            return false;
        case TOKEN_CHAR:
            this->chars[this->pos++]=c;
            break;
        case TOKEN_STR:
            this->chars[this->pos++]='\0';
            if (!CommandFactory_addArgument(this->factory, this->chars)) return false;
            this->pos=0;
            break;
        case TOKEN_ESCAPE:
            if (this->backslash) {
                this->backslash=false;
                if (c=='\n') printf("> ");
                else this->chars[this->pos++]=c;
            } else this->backslash=true;
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
    this->backslash=false;
    if (!(this->chars = malloc(sizeof(char)*this->size))) goto cleanup;
    if (!(this->factory=CommandFactory_create())) goto cleanup;
    Error_SetError(ERROR_NONE);
    return this;
cleanup:
    CommandParser_destroy(this);
    return NULL;
}

void CommandParser_destroy(CommandParser* this) {
    if (!this) return;
    CommandFactory_destroy(this->factory);
    free(this->chars);
    free(this);
}