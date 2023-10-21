#include <malloc.h>
#include "CommandParser.h"
#include "Error.h"
#include "macro.h"
#include "CommandFactory.h"
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

typedef struct private_CommandParser {
    unsigned int size, pos;
    bool backslash;
    char *chars;
    CommandFactory* factory;
    bool (*executeCommandQueue)(CommandParser* this);
    bool (*resizeIfFull)(CommandParser* this);
    Token (*processToken)(CommandParser* this, char c);
} private_CommandParser;

bool CommandParser_executeCommandQueue(CommandParser* this) {
    if (!this) {
        Error_SetError(ERROR_NULL_POINTER);
        return false;
    }
    Command *command = pv->factory->build(pv->factory);
    if (!command) return false;
    if (!command->execute(command)) return false;
    pv->pos=0;
    Error_SetError(ERROR_NONE);
    return true;
}

bool CommandParser_resizeIfFull(CommandParser* this) {
    if (!this || !pv || !pv->chars) {
        Error_SetError(ERROR_NULL_POINTER);
        return false;
    }
    if (pv->pos==pv->size) {
        char* tmp;
        tmp=realloc(pv->chars, pv->size<<=1);
        if (!tmp) {
            Error_SetError(ERROR_MEMORY_ALLOCATION);
            pv->size>>=1;
            return false;
        }
        pv->chars=tmp;
    }
    Error_SetError(ERROR_NONE);
    return true;
}

Token CommandParser_processToken(CommandParser* this, char c) {
    if (!this || !pv) {
        Error_SetError(ERROR_NULL_POINTER);
        return TOKEN_ERROR;
    }
    Error_SetError(ERROR_NONE);
    if (c=='\\' || pv->backslash) return TOKEN_ESCAPE;
    if (c=='\n') return TOKEN_EXECUTE;
    if (IS_WHITE_SPACE(c)) return pv->pos ? TOKEN_STR : TOKEN_NONE;
    return TOKEN_CHAR;
}

void privateCommandParser_destroy(private_CommandParser *this) {
    if (!this) return;
    CommandFactory_destroy(this->factory);
    free(this->chars);
    free(this);
}

private_CommandParser* privateCommandParser_create() {
    Error_SetError(ERROR_MEMORY_ALLOCATION);
    private_CommandParser *this = malloc(sizeof(private_CommandParser));
    if (!this) return NULL;
    this->size=16;
    this->pos=0;
    this->backslash=false;
    this->executeCommandQueue=CommandParser_executeCommandQueue;
    this->resizeIfFull=CommandParser_resizeIfFull;
    this->processToken=CommandParser_processToken;
    if (!(this->chars = malloc(sizeof(char)*this->size))) goto cleanup;
    if (!(this->factory=CommandFactory_create())) goto cleanup;
    Error_SetError(ERROR_NONE);
    return this;
cleanup:
    privateCommandParser_destroy(this);
    return NULL;
}

bool CommandParser_consumeChar(struct CommandParser* this, char c) {
    if (c==EOF) return false;
    if (!this || !pv || !pv->chars) {
        Error_SetError(ERROR_NULL_POINTER);
        return false;
    }
    if (!pv->resizeIfFull(this)) return false;
    Token token = pv->processToken(this, c);
    switch (token) {
        case TOKEN_ERROR:
            return false;
        case TOKEN_CHAR:
            pv->chars[pv->pos++]=c;
            break;
        case TOKEN_STR:
            pv->chars[pv->pos++]='\0';
            if (!pv->factory->addArgument(pv->factory, pv->chars)) return false;
            pv->pos=0;
            break;
        case TOKEN_ESCAPE:
            if (pv->backslash) {
                pv->backslash=false;
                if (c=='\n') printf("> ");
                else pv->chars[pv->pos++]=c;
            } else pv->backslash=true;
            break;
        case TOKEN_EXECUTE:
            if (pv->pos) {
                pv->chars[pv->pos]='\0';
                if (!pv->factory->addArgument(pv->factory, pv->chars)) return false;
            }
            int nbArgs=pv->factory->getNbArgs(pv->factory);
            if (nbArgs==-1) return false;
            if (nbArgs && !pv->executeCommandQueue(this)) return false;
            printf("%s%s%s> ", BLUE_BEGIN, Environment_getCwd(), COLOR_RESET);
            pv->pos=0;
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
    if (!(pv=privateCommandParser_create())) goto cleanup;
    this->consumeChar=CommandParser_consumeChar;
    Error_SetError(ERROR_NONE);
    return this;
cleanup:
    CommandParser_destroy(this);
    return NULL;
}

void CommandParser_destroy(CommandParser* this) {
    if (!this) return;
    privateCommandParser_destroy(pv);
    free(this);
}