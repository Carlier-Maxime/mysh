#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
unsigned int maxLen[11] = {0};

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

void print_header() {
    unsigned int i;
    const char* words[] = {
            "USER",
            "PID",
            "%CPU",
            "%MEM",
            "VSZ",
            "RSS",
            "TTY",
            "STAT",
            "START",
            "TIME",
            "COMMAND"
    };
    for (i=0; i<11; i++) printf("%-*s ", (int) (maxLen[i]>strlen(words[i]) ? maxLen[i] : strlen(words[i])), words[i]);
    printf("\n");
}

int main() {
    if (!grow_ps()) goto end;
    print_header();
    end:
    free(ps);
    return 0;
}