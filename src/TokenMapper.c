#include "TokenMapper.h"
#include <stdlib.h>
#include <stdio.h>
#include "Error.h"

#define IS_WHITE_SPACE(c) ((c)=='\n' || (c)==' ' || (c)=='\t' || (c)=='\v' || (c)=='\r')

TokenMapper* TokenMapper_create() {
    TokenMapper* this = malloc(sizeof(TokenMapper));
    if (!this) return NULL;
    this->last_char=EOF;
    this->last_token=TOKEN_NONE;
    return this;
}

void TokenMapper_destroy(TokenMapper* this) {
    free(this);
}

Token TokenMapper_processChar(TokenMapper* this, char c) {
    if (!this) {
        Error_SetError(ERROR_NULL_POINTER);
        return TOKEN_ERROR;
    }
    Token token;
    if (this->last_char=='\\' && this->last_token==TOKEN_NONE && c=='\n') token=TOKEN_NEW_LINE;
    else if (c=='\\') token=TOKEN_NONE;
    else if (c=='\n') token=TOKEN_EXECUTE;
    else if (IS_WHITE_SPACE(c)) return TOKEN_STR;
    else token=TOKEN_CHAR;
    this->last_char=c;
    this->last_token=token;
    Error_SetError(ERROR_NONE);
    return token;
}