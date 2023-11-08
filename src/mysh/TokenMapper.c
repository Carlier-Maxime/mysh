#include "TokenMapper.h"
#include <stdlib.h>
#include <stdio.h>
#include "../utils/Error.h"

#define IS_WHITE_SPACE(c) ((c)==EOF || (c)=='\n' || (c)==' ' || (c)=='\t' || (c)=='\v' || (c)=='\r')

TokenMapper* TokenMapper_create() {
    TokenMapper* this = malloc(sizeof(TokenMapper));
    if (!this) return NULL;
    this->last_char=EOF;
    this->last_token=TOKEN_NONE;
    this->current_char=EOF;
    this->current_token=TOKEN_NONE;
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

bool TokenMapper_processDoubleChar(TokenMapper* this, char c, Token token_double, Token token_single) {
    if (this->last_char!=c) return false;
    if (this->last_token==TOKEN_NONE) this->current_token = this->current_char==c ? token_double : token_single;
    else if (this->current_char==c) this->current_token = TOKEN_NONE;
    else return false;
    return true;
}

Token TokenMapper_process(TokenMapper* this) {
    if (!this) {
        Error_SetError(ERROR_NULL_POINTER);
        return TOKEN_ERROR;
    }
    if (!this->processCurrentChar) this->current_token=TOKEN_NONE;
    else if (this->last_char=='\\' && this->escapeChar) {
        this->current_token = this->current_char=='\n' ? TOKEN_NEW_LINE : TOKEN_CHAR;
        this->escapeChar = false;
        this->processCurrentChar=false;
        this->current_char=EOF;
    }
    else if (TokenMapper_processDoubleChar(this, '&', TOKEN_AND, TOKEN_BACKGROUND) ||
            TokenMapper_processDoubleChar(this, '|', TOKEN_OR, TOKEN_PIPE) ||
            TokenMapper_processDoubleChar(this, '>', TOKEN_REDIRECT_OUTPUT_APPEND, TOKEN_REDIRECT_OUTPUT)
    ) goto end;
    else if (this->current_char=='\\') {
        this->escapeChar = true;
        this->current_token=TOKEN_NONE;
    }
    else if (this->current_char=='\n') {
        if (this->buildArg) this->current_token=TOKEN_STR;
        else if (this->last_token==TOKEN_NONE || this->last_token==TOKEN_STR) this->current_token=TOKEN_COMMAND;
        else if (this->last_token==TOKEN_COMMAND) this->current_token=TOKEN_EXECUTE;
        else this->current_token=TOKEN_NONE;
    }
    else if (IS_WHITE_SPACE(this->current_char)) {
        this->processCurrentChar = false;
        this->current_token = IS_WHITE_SPACE(this->last_char) ? TOKEN_NONE : TOKEN_STR;
    }
    else if(this->current_char==';') {
        if (this->buildArg) this->current_token=TOKEN_STR;
        else {
            this->current_token = TOKEN_COMMAND;
            this->processCurrentChar=false;
        }
    }
    else if (this->current_char=='&' || this->current_char=='|' || this->current_char=='>') {
        if (this->buildArg) this->current_token=TOKEN_STR;
        else this->current_token = TOKEN_NONE;
    }
    else if (this->current_char=='<') {
        if (this->buildArg) this->current_token=TOKEN_STR;
        else {
            this->current_token = TOKEN_REDIRECT_INPUT;
            this->processCurrentChar=false;
        }
    }
    else {
        this->current_token=TOKEN_CHAR;
        this->processCurrentChar=false;
    }
    end:
    if (this->current_token==TOKEN_NONE) this->processCurrentChar=false;
    else if (this->current_token==TOKEN_CHAR) this->buildArg=true;
    else if (this->current_token==TOKEN_STR) this->buildArg=false;
    this->last_token=this->current_token;
    Error_SetError(ERROR_NONE);
    return this->current_token;
}