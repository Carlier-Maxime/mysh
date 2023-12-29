#include <stdlib.h>
#include "../utils/Error.h"

typedef struct {
    const char* user;
    unsigned long pid;
    float cpu_percentage;
    float mem_percentage;
    unsigned long vsz;
    unsigned long rss;
    char* tty;
    char* stat;
    unsigned long start;
    unsigned long time;
    char* command;
} procInfo;

unsigned long ps_size = 64;
procInfo* ps = NULL;

bool grow_ps() {
    procInfo* tmp;
    if (!ps) tmp = malloc(sizeof(procInfo)*ps_size<<1);
    else tmp = realloc(ps, sizeof(procInfo)*ps_size<<1);
    if (!tmp) {
        Error_SetError(ERROR_MEMORY_ALLOCATION);
        return false;
    }
    ps_size = ps_size<<1;
    ps = tmp;
    return true;
}

int main() {
    if (!grow_ps()) goto end;
    end:
    free(ps);
    return 0;
}