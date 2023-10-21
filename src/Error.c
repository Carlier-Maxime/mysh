#include <stdio.h>
#include "Error.h"

ErrorStatus status = ERROR_NONE;
const char* file;
unsigned int line;

const char* errorMsg[] = {
        "Success",
        "Allocation memory failed",
        "Null Pointer error",
        "No Argument provided",
        "Fork failed",
        "A manipulation environment error"
};

void Error_SetError_(ErrorStatus newStatus, const char* newFile, unsigned int newLine) {
    status=newStatus;
    file=newFile;
    line=newLine;
}

void Error_PrintErrorMsg(const char* prefix) {
    if (file && line) fprintf(stderr, "%s : %s in %s at line %d\n", prefix, errorMsg[status], file, line);
    else fprintf(stderr, "%s : %s\n", prefix, errorMsg[status]);
}

ErrorStatus Error_GetErrorStatus() {
    return status;
}