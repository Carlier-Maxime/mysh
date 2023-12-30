#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include "../utils/Error.h"
#include "../utils/macro.h"

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
unsigned int maxLen[11] = {4,3,4,4,3,3,3,4,5,4,7};

int getTerminalWidth() {
    if (!isatty(STDOUT_FILENO)) return -1;
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

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
            "TIME"
    };
    for (unsigned int i=0; i<10; i++) printf("%*s ", maxLen[i], headers[i]);
    printf("COMMAND\n");
}

void print_line(unsigned long i) {
    procInfo p = ps[i];
    printf("%*s %*lu %*.1f %*.1f %*lu %*lu %*s %*s %*lu %*lu %.*s\n",
           maxLen[0], p.user, maxLen[1], p.pid, maxLen[2], p.cpu_percentage,
           maxLen[3], p.mem_percentage, maxLen[4], p.vsz, maxLen[5], p.rss, maxLen[6], p.tty, maxLen[7], p.stat,
           maxLen[8], p.start, maxLen[9], p.time, maxLen[10], p.command);
}

bool goodWidth() {
    int w = getTerminalWidth();
    if (w==-1) return true;
    unsigned int len=0;
    for (unsigned long i=0; i<10; i++) len+=maxLen[i]+1;
    if ((u_int) w<len+7) return false;
    maxLen[10]=w-len;
    return true;
}

void print_ps() {
    if (!goodWidth()) {
        fprintf(stderr, RED("Terminal width is too small, please resize or redirect to file"));
        return;
    }
    print_header();
    for (unsigned long i=0; i<nb_proc; i++) print_line(i);
}

int numberOfDigits(unsigned long n) {
    int cpt = 0;
    if (n == 0) return 1;
    while (n > 0) {
        n /= 10;
        cpt++;
    }
    return cpt;
}

char* readCmdLine(unsigned long pid) {
    char path[numberOfDigits((1ULL << (sizeof(pid_t) * 8 - 1)) - 1)+16];
    sprintf(path, "/proc/%lu/cmdline", pid);
    FILE *file;
    if (!(file=fopen(path, "r"))) {
        Error_SetError(ERROR_OPEN_FILE);
        return NULL;
    }
    unsigned long fileSize=0;
    while (fgetc(file) != EOF) fileSize++;
    fseek(file, 0, SEEK_SET);
    char *cmdline = (char*) malloc(fileSize + 1);
    if (!cmdline) {
        Error_SetError(ERROR_MEMORY_ALLOCATION);
        fclose(file);
        return NULL;
    }
    size_t read = fread(cmdline, 1, fileSize, file);
    cmdline[read] = '\0';
    for (size_t i=0; i<read; i++) if (cmdline[i]=='\0') cmdline[i]=' ';
    fclose(file);
    return cmdline;
}

void free_ps() {
    for (unsigned long i=0; i<nb_proc; i++) {
        free(ps[i].command);
    }
    free(ps);
}

char *getUsernameFromPid(unsigned long pid) {
    char path[numberOfDigits((1ULL << (sizeof(pid_t) * 8 - 1)) - 1)+16];
    FILE *file;
    char *line = NULL;
    size_t lineSize = 0;
    char *username = NULL;
    sprintf(path, "/proc/%lu/status", pid);
    file = fopen(path, "r");
    if (!file) {
        Error_SetError(ERROR_OPEN_FILE);
        return NULL;
    }
    while (getline(&line, &lineSize, file) != -1) {
        if (strncmp(line, "Uid:", 4) != 0) {
            free(line);
            line=NULL;
            lineSize=0;
            continue;
        }
        char *uidString = strtok(line + 4, "\t ");
        if (!uidString) {
            Error_SetError(ERROR_NO_ARGUMENT);
            break;
        }
        char* endPtr;
        long uid = strtol(uidString, &endPtr, 10);
        if (*endPtr!='\0') {
            Error_SetError(ERROR_CONVERSION);
            break;
        }
        struct passwd *pw = getpwuid(uid);
        if (!pw) {
            Error_SetError(ERROR_PWUID);
            break;
        }
        username = strdup(pw->pw_name);
        break;
    }
    free(line);
    fclose(file);
    return username;
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
        unsigned long len;
        if (entry->d_type!=DT_DIR || errno!=0 || pid==0) continue;
        proc.user = getUsernameFromPid(pid);
        len = strlen(proc.user);
        if (len>maxLen[0]) maxLen[0] = len;
        proc.pid = pid;
        len = numberOfDigits(pid);
        if (len>maxLen[1]) maxLen[1] = len;
        proc.cpu_percentage = 0;
        proc.mem_percentage = 0;
        proc.vsz = 0;
        proc.rss = 0;
        proc.tty = "?";
        proc.stat = "?";
        proc.start = 0;
        proc.time = 0;
        proc.command = readCmdLine(pid);
        len = strlen(proc.command);
        if (len>maxLen[10]) maxLen[10] = len;
        if (nb_proc>=ps_size && !grow_ps()) goto end;
        ps[nb_proc++]=proc;
    }
    print_ps();
    end:
    if (Error_GetErrorStatus() != ERROR_NONE) Error_PrintErrorMsg("Error: ");
    closedir(dir);
    free_ps();
    return 0;
}