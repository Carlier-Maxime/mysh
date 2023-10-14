#include <stdio.h>
#include "Error.h"

ErrorStatus status = ERROR_NONE;

const char* errorMsg[] = {
        "Success",
        "Allocation memory failed",
        "Null Pointer error"
};

void Error_SetError(ErrorStatus newStatus) {
    status=newStatus;
}

void Error_PrintErrorMsg(const char* prefix) {
    fprintf(stderr, "%s : %s", prefix, errorMsg[status]);
}

ErrorStatus Error_GetErrorStatus() {
    return status;
}