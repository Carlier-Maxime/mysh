#ifndef MYSH_TOKEN_MAPPER_H
#define MYSH_TOKEN_MAPPER_H

#include <stdbool.h>

typedef enum {
    TOKEN_ERROR,
    TOKEN_NONE,
    TOKEN_CHAR,
    TOKEN_STR,
    TOKEN_EXECUTE,
    TOKEN_NEW_LINE,
    TOKEN_PIPE,
    TOKEN_OR,
    TOKEN_COMMAND,
    TOKEN_BACKGROUND, // le &
    TOKEN_AND,
    TOKEN_REDIRECT_OUTPUT,
    TOKEN_REDIRECT_OUTPUT_APPEND,
    TOKEN_REDIRECT_INPUT,
} Token;

typedef struct TokenMapper {
    bool processCurrentChar, escapeChar, buildArg;
    char last_char, current_char;
    Token last_token, current_token;
} TokenMapper;

TokenMapper* TokenMapper_create();
void TokenMapper_destroy(TokenMapper* this);
bool TokenMapper_setCurrentChar(TokenMapper* this, char c);
Token TokenMapper_process(TokenMapper* this);

#endif //MYSH_TOKEN_MAPPER_H