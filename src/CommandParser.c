#include <malloc.h>
#include "CommandParser.h"
#include "Error.h"
#include "macro.h"
#include "CommandFactory.h"

#define IS_WHITE_SPACE(c) ((c)=='\n' || (c)==' ' || (c)=='\t' || (c)=='\v' || (c)=='\r')
#define IS_SPECIAL_CHAR(c) ((c)=='\\' || (c)=='\n')

typedef struct private_CommandParser {
    unsigned int size, pos;
    bool backslash;
    char *chars;
    CommandFactory* factory;
    bool (*executeCommandQueue)(CommandParser* this);
    bool (*resizeIfFull)(CommandParser* this);
} private_CommandParser;

bool CommandParser_executeCommandQueue(CommandParser* this) {
    if (!this) {
        Error_SetError(ERROR_NULL_POINTER);
        return false;
    }
    Command *command = pv->factory->build(pv->factory);
    if (!command) return false;
    command->execute(command);
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
    bool isWhiteSpace = IS_WHITE_SPACE(c);
    if ((!IS_SPECIAL_CHAR(c) || pv->backslash || c=='\n') && ((pv->pos && pv->chars[pv->pos-1]) || !isWhiteSpace)) {
        pv->chars[pv->pos++]=(char)(isWhiteSpace ? '\0' : c);
        if (isWhiteSpace) {
            if (!pv->factory->addArgument(pv->factory, pv->chars)) return false;
            pv->pos=0;
        }
    }
    if (!pv->backslash && c=='\n') pv->executeCommandQueue(this);
    if (pv->backslash) pv->backslash=false;
    else if (c=='\\') pv->backslash=true;
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