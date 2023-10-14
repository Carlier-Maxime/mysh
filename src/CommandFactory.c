#include <malloc.h>
#include <string.h>
#include "CommandFactory.h"
#include "Error.h"
#include "macro.h"

typedef struct private_CommandFactory {
    unsigned int size, pos;
    char **words;
    bool isMaking;
    bool (*resizeIfFull)(CommandFactory *this);
} private_CommandFactory;

void privateCommandFactory_destroy(private_CommandFactory *this) {
    if (!this) return;
    free(this->words);
    free(this);
}

bool resizeIfFull(CommandFactory* this) {
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
    this->resizeIfFull=resizeIfFull;
    this->isMaking=false;
    if (!(this->words=malloc(sizeof(char*)*this->size))) goto cleanup;
    Error_SetError(ERROR_NONE);
    return this;
cleanup:
    privateCommandFactory_destroy(this);
    return NULL;
}

CommandFactory* create(CommandFactory* this, const char* name) {
    if (!this || !pv) {
        Error_SetError(ERROR_NULL_POINTER);
        return NULL;
    }
    if (pv->isMaking) return NULL;
    pv->isMaking=true;
    this->addArgument(this,name);
    return this;
}

CommandFactory* addArgument(CommandFactory* this, const char* arg) {
    if (!this || !pv || !pv->words) {
        Error_SetError(ERROR_NULL_POINTER);
        return NULL;
    }
    if (!pv->isMaking) return NULL;
    if (!pv->resizeIfFull(this)) return NULL;
    if (!(pv->words[pv->pos]=strdup(arg))) {
        Error_SetError(ERROR_MEMORY_ALLOCATION);
        return NULL;
    } else pv->pos++;
    Error_SetError(ERROR_NONE);
    return this;
}

bool isMaking(CommandFactory *this) {
    if (!this || !pv) {
        Error_SetError(ERROR_NULL_POINTER);
        return false;
    }
    return pv->isMaking;
}

Command* build(CommandFactory* this) {
    if (!this || !pv || !pv->words) {
        Error_SetError(ERROR_NULL_POINTER);
        return NULL;
    }
    if (!pv->isMaking || pv->pos<1) return NULL;
    if (!pv->resizeIfFull(this)) return NULL;
    pv->words[pv->pos]=NULL;
    Command* command = Command_create(pv->words[0], (const char **) (pv->words + 1));
    pv->pos=0;
    pv->isMaking=false;
    return command;
}

CommandFactory *CommandFactory_create() {
    Error_SetError(ERROR_MEMORY_ALLOCATION);
    CommandFactory *this = malloc(sizeof(CommandFactory));
    if (!this) return NULL;
    if (!(pv=privateCommandFactory_create())) goto cleanup;
    this->create=create;
    this->addArgument=addArgument;
    this->build=build;
    this->isMaking=isMaking;
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