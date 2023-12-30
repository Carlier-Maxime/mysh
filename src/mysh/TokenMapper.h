#ifndef MYSH_TOKEN_MAPPER_H
#define MYSH_TOKEN_MAPPER_H

#include <stdbool.h>

typedef enum {
    TOKEN_NONE,     // 0
    TOKEN_ERROR,    // 1
    TOKEN_START,
    TOKEN_CHAR,     // 2
    TOKEN_STR,      // 3
    TOKEN_EXECUTE,  // 4
    TOKEN_NEW_LINE, // 5
    TOKEN_PIPE,     // 6
    TOKEN_OR,       // 7
    TOKEN_COMMAND,  // 8
    TOKEN_BACKGROUND, // 9 // le &
    TOKEN_AND,                      // 10
    TOKEN_REDIRECT_OUTPUT,          // 11
    TOKEN_REDIRECT_OUTPUT_APPEND,   // 12
    TOKEN_REDIRECT_INPUT,           // 13
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