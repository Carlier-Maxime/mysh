#ifndef MYSH_ERROR_H
#define MYSH_ERROR_H

#include <stdbool.h>

typedef enum {
    ERROR_NONE=0,
    ERROR_MEMORY_ALLOCATION=1,
    ERROR_NULL_POINTER=2
} ErrorStatus;

void Error_SetError(ErrorStatus newStatus);
void Error_PrintErrorMsg(const char* prefix);
ErrorStatus Error_GetErrorStatus();

#endif //MYSH_ERROR_H
