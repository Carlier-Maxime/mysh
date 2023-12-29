#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
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

unsigned long ps_size = 64, nb_proc = 0;
procInfo* ps = NULL;
const char* headers[] = {
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
unsigned int maxLen[11] = {4,3,4,4,3,3,3,4,5,4,7};

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
    for (i=0; i<11; i++) printf("%-*s ", maxLen[i], headers[i]);
    printf("\n");
}

void print_line(unsigned long i) {
    procInfo p = ps[i];
    printf("%-*s %-*lu %-*f %-*f %-*lu %-*lu, %-*s %-*s %-*lu %-*lu %-*s\n",
           maxLen[0], p.user, maxLen[0], p.pid, maxLen[0], p.cpu_percentage,
           maxLen[0], p.mem_percentage, maxLen[0], p.vsz, maxLen[0], p.rss, maxLen[0], p.tty, maxLen[0], p.stat,
           maxLen[0], p.start, maxLen[0], p.time, maxLen[0], p.command);
}

void print_ps() {
    print_header();
    for (unsigned long i=0; i<nb_proc; i++) print_line(i);
}

int main() {
    DIR* dir = NULL;
    struct dirent* entry;
    procInfo proc;
    if (!grow_ps()) goto end;
    if (!(dir=opendir("/proc"))) {
        Error_SetError(ERROR_OPEN_DIR);
        goto end;
    }
    while ((entry=readdir(dir))) {
        errno=0;
        long pid=strtol(entry->d_name, NULL, 10);
        if (entry->d_type!=DT_DIR || errno!=0 || pid==0) continue;
        proc.user = "?";
        proc.pid = pid;
        proc.cpu_percentage = 0;
        proc.mem_percentage = 0;
        proc.vsz = 0;
        proc.rss = 0;
        proc.tty = "?";
        proc.stat = "?";
        proc.start = 0;
        proc.time = 0;
        proc.command = "?";
        if (nb_proc>=ps_size) grow_ps();
        ps[nb_proc++]=proc;
    }
    closedir(dir);
    print_ps();
    end:
    if (Error_GetErrorStatus() != ERROR_NONE) Error_PrintErrorMsg("Error: ");
    closedir(dir);
    free(ps);
    return 0;
}