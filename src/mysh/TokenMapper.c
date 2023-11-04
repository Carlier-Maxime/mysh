#include "TokenMapper.h"
#include <stdlib.h>
#include <stdio.h>
#include "../utils/Error.h"

#define IS_WHITE_SPACE(c) ((c)=='\n' || (c)==' ' || (c)=='\t' || (c)=='\v' || (c)=='\r')

TokenMapper* TokenMapper_create() {
    TokenMapper* this = malloc(sizeof(TokenMapper));
    if (!this) return NULL;
    this->last_char=EOF;
    this->last_token=TOKEN_NONE;
    this->current_char=EOF;
    this->processCurrentChar=false;
    this->escapeChar=false;
    this->buildArg=false;
    return this;
}

void TokenMapper_destroy(TokenMapper* this) {
    free(this);
}

bool TokenMapper_setCurrentChar(TokenMapper* this, char c) {
    if (!this) {
        Error_SetError(ERROR_NULL_POINTER);
        return false;
    }
    this->last_char=this->current_char;
    this->current_char=c;
    this->processCurrentChar=true;
    return true;
}

Token TokenMapper_process(TokenMapper* this) {
    if (!this) {
        Error_SetError(ERROR_NULL_POINTER);
        return TOKEN_ERROR;
    }
    Token token;
    start_if:
    if (!this->processCurrentChar) token=TOKEN_NONE;
    else if (this->last_char=='\\' && this->escapeChar) {
        token = this->current_char=='\n' ? TOKEN_NEW_LINE : TOKEN_CHAR;
        this->escapeChar = false;
        this->processCurrentChar=false;
        this->current_char=EOF;
    }
    else if (this->last_char=='&') {
        if (this->last_token==TOKEN_NONE) token = this->current_char=='&' ? TOKEN_AND : TOKEN_BACKGROUND;
        else if (this->current_char=='&') token = TOKEN_NONE;
        else {
            this->last_char=EOF;
            goto start_if;
        }
    }
    else if (this->last_char=='|') {
        if (this->last_token==TOKEN_NONE) token = this->current_char=='|' ? TOKEN_OR : TOKEN_PIPE;
        else if (this->current_char=='|') token = TOKEN_NONE;
        else {
            this->last_char=EOF;
            goto start_if;
        }
    }
    else if (this->last_char=='>') {
        if (this->last_token==TOKEN_NONE) token = this->current_char=='>' ? TOKEN_REDIRECT_OUTPUT_APPEND : TOKEN_REDIRECT_OUTPUT;
        else if (this->current_char=='>') token = TOKEN_NONE;
        else {
            this->last_char=EOF;
            goto start_if;
        }
    }
    else if (this->current_char=='\\') {
        this->escapeChar = true;
        token=TOKEN_NONE;
    }
    else if (this->current_char=='\n') {
        if (this->buildArg) token=TOKEN_STR;
        else if (this->last_token==TOKEN_NONE || this->last_token==TOKEN_STR) token=TOKEN_COMMAND;
        else if (this->last_token==TOKEN_COMMAND) token=TOKEN_EXECUTE;
        else token=TOKEN_NONE;
    }
    else if (IS_WHITE_SPACE(this->current_char)) {
        this->processCurrentChar = false;
        token = IS_WHITE_SPACE(this->last_char) ? TOKEN_NONE : TOKEN_STR;
    }
    else if(this->current_char==';') {
        if (this->buildArg) token=TOKEN_STR;
        else {
            token = TOKEN_COMMAND;
            this->processCurrentChar=false;
        }
    }
    else if (this->current_char=='&' || this->current_char=='|' || this->current_char=='>') {
        if (this->buildArg) token=TOKEN_STR;
        else token = TOKEN_NONE;
    }
    else if (this->current_char=='<') {
        if (this->buildArg) token=TOKEN_STR;
        else {
            token = TOKEN_REDIRECT_INPUT;
            this->processCurrentChar=false;
        }
    }
    else {
        token=TOKEN_CHAR;
        this->processCurrentChar=false;
    }
    if (token==TOKEN_NONE) this->processCurrentChar=false;
    else if (token==TOKEN_CHAR) this->buildArg=true;
    else if (token==TOKEN_STR) this->buildArg=false;
    this->last_token=token;
    Error_SetError(ERROR_NONE);
    return token;
}