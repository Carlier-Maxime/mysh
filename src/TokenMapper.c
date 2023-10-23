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

Token TokenMapper_processChar(TokenMapper* this, char c, bool backslash, unsigned int pos) {
    if (!this) {
        Error_SetError(ERROR_NULL_POINTER);
        return TOKEN_ERROR;
    }
    Error_SetError(ERROR_NONE);
    if (c=='\\' || backslash) return TOKEN_ESCAPE;
    if (c=='\n') return TOKEN_EXECUTE;
    if (IS_WHITE_SPACE(c)) return pos ? TOKEN_STR : TOKEN_NONE;
    return TOKEN_CHAR;
}