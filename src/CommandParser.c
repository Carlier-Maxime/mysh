#include <malloc.h>
#include "CommandParser.h"
#include "Error.h"

#define IS_WHITE_SPACE(c) (c=='\n' || c==' ' || c=='\t' || c=='\v' || c=='\r')
#define pv this->private

typedef struct private_CommandParser {
    unsigned int size, pos;
    char *chars;
    bool (*executeCommandQueue)(CommandParser* this);
} private_CommandParser;

bool executeCommandQueue(CommandParser* this) {
    if (!this) return false;
    for (unsigned int i=0; i<pv->pos; i++) putchar(pv->chars[i] ? pv->chars[i] : ' ');
    printf("\n~> ");
    pv->pos=0;
    return true;
}

void privateCommandParser_destroy(private_CommandParser *this) {
    if (!this) return;
    free(this->chars);
    free(this);
}

private_CommandParser* privateCommandParser_create() {
    Error_SetError(ERROR_MEMORY_ALLOCATION);
    private_CommandParser *this = malloc(sizeof(private_CommandParser));
    if (!this) return NULL;
    this->size=64;
    this->pos=0;
    this->executeCommandQueue=executeCommandQueue;
    if (!(this->chars = malloc(sizeof(char)*this->size))) goto cleanup;
    Error_SetError(ERROR_NONE);
    return this;
cleanup:
    privateCommandParser_destroy(this);
    return NULL;
}

bool consumeChar(struct CommandParser* this, char c) {
    if (c==EOF) return false;
    if (pv->pos==pv->size) {
        char* tmp;
        tmp=realloc(pv->chars, pv->size<<=1);
        if (!tmp) {
            Error_SetError(ERROR_MEMORY_ALLOCATION);
            return false;
        }
        pv->chars=tmp;
    }
    bool isWhiteSpace = IS_WHITE_SPACE(c);
    if ((pv->pos && pv->chars[pv->pos-1]) || !isWhiteSpace) pv->chars[pv->pos++]=(char)(isWhiteSpace ? '\0' : c);
    if (c=='\n') pv->executeCommandQueue(this);
    Error_SetError(ERROR_NONE);
    return true;
}

CommandParser* CommandParser_create() {
    Error_SetError(ERROR_MEMORY_ALLOCATION);
    CommandParser *this = malloc(sizeof(CommandParser));
    if (!this) return NULL;
    if (!(pv=privateCommandParser_create())) goto cleanup;
    this->consumeChar=consumeChar;
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