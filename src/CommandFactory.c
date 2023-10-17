#include <malloc.h>
#include <string.h>
#include "CommandFactory.h"
#include "Error.h"
#include "macro.h"

typedef struct private_CommandFactory {
    unsigned int size, pos;
    char **words;
    bool (*resizeIfFull)(CommandFactory *this);
} private_CommandFactory;

void privateCommandFactory_destroy(private_CommandFactory *this) {
    if (!this) return;
    free(this->words);
    free(this);
}

bool CommandFactory_resizeIfFull(CommandFactory* this) {
    if (!this || !pv || !pv->words) {
        Error_SetError(ERROR_NULL_POINTER);
        return false;
    }
    if (pv->pos==pv->size) {
        char** tmp = realloc(pv->words, pv->size<<=1);
        if (!tmp) {
            Error_SetError(ERROR_MEMORY_ALLOCATION);
            pv->size>>=1;
            return false;
        }
        pv->words=tmp;
    }
    Error_SetError(ERROR_NONE);
    return true;
}

private_CommandFactory *privateCommandFactory_create() {
    Error_SetError(ERROR_MEMORY_ALLOCATION);
    private_CommandFactory *this = malloc(sizeof(private_CommandFactory));
    if (!this) return NULL;
    this->size=8;
    this->pos=0;
    this->resizeIfFull=CommandFactory_resizeIfFull;
    if (!(this->words=malloc(sizeof(char*)*this->size))) goto cleanup;
    Error_SetError(ERROR_NONE);
    return this;
cleanup:
    privateCommandFactory_destroy(this);
    return NULL;
}

CommandFactory* addArgument(CommandFactory* this, const char* arg) {
    if (!this || !pv || !pv->words) {
        Error_SetError(ERROR_NULL_POINTER);
        return NULL;
    }
    if (!pv->resizeIfFull(this)) return NULL;
    if (!(pv->words[pv->pos]=strdup(arg))) {
        Error_SetError(ERROR_MEMORY_ALLOCATION);
        return NULL;
    } else pv->pos++;
    Error_SetError(ERROR_NONE);
    return this;
}

Command* build(CommandFactory* this) {
    if (!this || !pv || !pv->words) {
        Error_SetError(ERROR_NULL_POINTER);
        return NULL;
    }
    if (pv->pos<1) {
        Error_SetError(ERROR_NO_ARGUMENT);
        return NULL;
    }
    if (!pv->resizeIfFull(this)) return NULL;
    pv->words[pv->pos]=NULL;
    Command* command = Command_create(pv->words[0], (const char **) (pv->words + 1));
    pv->pos=0;
    return command;
}

CommandFactory *CommandFactory_create() {
    Error_SetError(ERROR_MEMORY_ALLOCATION);
    CommandFactory *this = malloc(sizeof(CommandFactory));
    if (!this) return NULL;
    if (!(pv=privateCommandFactory_create())) goto cleanup;
    this->addArgument=addArgument;
    this->build=build;
    Error_SetError(ERROR_NONE);
    return this;
cleanup:
    CommandFactory_destroy(this);
    return NULL;
}

void CommandFactory_destroy(CommandFactory *this) {
    if (!this) return;
    privateCommandFactory_destroy(pv);
    free(this);
}