#ifndef MYSH_ERROR_H
#define MYSH_ERROR_H

#include <stdbool.h>

typedef enum {
    ERROR_NONE=0,
    ERROR_MEMORY_ALLOCATION=1,
    ERROR_NULL_POINTER=2,
    ERROR_NO_ARGUMENT=3,
    ERROR_FORK=4,
    ERROR_ENVIRONMENT=5
} ErrorStatus;

#ifdef DEBUG
#define Error_SetError(status) Error_SetError_(status, __FILE__, __LINE__)
#else
#define Error_SetError(status) Error_SetError_(status, NULL, 0)
#endif
void Error_SetError_(ErrorStatus newStatus, const char* file, unsigned int line);
void Error_PrintErrorMsg(const char* prefix);
ErrorStatus Error_GetErrorStatus();

#endif //MYSH_ERROR_H
