#ifndef MYSH_TOKEN_MAPPER_H
#define MYSH_TOKEN_MAPPER_H

#include <stdbool.h>

typedef enum {
    TOKEN_ERROR,
    TOKEN_NONE,
    TOKEN_CHAR,
    TOKEN_STR,
    TOKEN_EXECUTE,
    TOKEN_ESCAPE
} Token;

typedef struct TokenMapper {
    char last_char;
    Token last_token;
} TokenMapper;

TokenMapper* TokenMapper_create();
void TokenMapper_destroy(TokenMapper* this);
Token TokenMapper_processChar(TokenMapper* this, char c, bool backslash, unsigned int pos);

#endif //MYSH_TOKEN_MAPPER_H