#include <malloc.h>
#include <string.h>
#include "CommandFactory.h"
#include "Error.h"
#include "macro.h"

bool CommandFactory_resizeIfFull(CommandFactory* this) {
    if (!this || !this->words) {
        Error_SetError(ERROR_NULL_POINTER);
        return false;
    }
    if (this->pos==this->size) {
        char** tmp = realloc(this->words, this->size<<=1);
        if (!tmp) {
            Error_SetError(ERROR_MEMORY_ALLOCATION);
            this->size>>=1;
            return false;
        }
        this->words=tmp;
    }
    Error_SetError(ERROR_NONE);
    return true;
}

CommandFactory* CommandFactory_addArgument(CommandFactory* this, const char* arg) {
    if (!this || !this->words) {
        Error_SetError(ERROR_NULL_POINTER);
        return NULL;
    }
    if (!CommandFactory_resizeIfFull(this)) return NULL;
    if (!(this->words[this->pos]=strdup(arg))) {
        Error_SetError(ERROR_MEMORY_ALLOCATION);
        return NULL;
    } else this->pos++;
    Error_SetError(ERROR_NONE);
    return this;
}

Command* CommandFactory_build(CommandFactory* this) {
    if (!this || !this->words) {
        Error_SetError(ERROR_NULL_POINTER);
        return NULL;
    }
    if (this->pos<1) {
        Error_SetError(ERROR_NO_ARGUMENT);
        return NULL;
    }
    if (!CommandFactory_resizeIfFull(this)) return NULL;
    this->words[this->pos]=NULL;
    Command* command = Command_create(this->words[0], (const char **) (this->words + 1));
    this->pos=0;
    return command;
}

int CommandFactory_getNbArgs(CommandFactory* this) {
    if (!this) {
        Error_SetError(ERROR_NULL_POINTER);
        return -1;
    }
    return this->pos;
}

CommandFactory *CommandFactory_create() {
    Error_SetError(ERROR_MEMORY_ALLOCATION);
    CommandFactory *this = malloc(sizeof(CommandFactory));
    if (!this) return NULL;
    this->size=8;
    this->pos=0;
    if (!(this->words=malloc(sizeof(char*)*this->size))) goto cleanup;
    Error_SetError(ERROR_NONE);
    return this;
cleanup:
    CommandFactory_destroy(this);
    return NULL;
}

void CommandFactory_destroy(CommandFactory *this) {
    if (!this) return;
    free(this->words);
    free(this);
}