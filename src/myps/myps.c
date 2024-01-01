#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include <sys/sysinfo.h>
#include <time.h>
#include "../utils/Error.h"
#include "../utils/macro.h"

typedef struct {
    char* user;
    unsigned long pid;
    double cpu_percentage;
    double mem_percentage;
    unsigned long vsz;
    unsigned long rss;
    char* tty;
    char stat;
    time_t start;
    unsigned long time;
    char* command;
} procInfo;

char *timeFormat_HM = "%H:%M";
char *timeFormat_MDHM = "%b%e %H:%M";
char *timeFormat_YMDHM = "%Y %b%e %H:%M";
unsigned long ps_size = 64, nb_proc = 0;
procInfo* ps = NULL;
unsigned int maxLen[11] = {4,3,4,4,3,3,3,4,5,4,7};
unsigned long long totalRAM;
double current_time;
struct tm *actual_time;

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

int getSizeForStartTime(time_t start_time) {
    struct tm* start = localtime(&start_time);
    if (actual_time->tm_year != start->tm_year) return 20;
    else if (actual_time->tm_mon != start->tm_mon || actual_time->tm_mday != start->tm_mday) return 12;
    else return 6;
}

char* getTimeFormatForStartTime(time_t start_time) {
    struct tm* start = localtime(&start_time);
    if (actual_time->tm_year != start->tm_year) return timeFormat_YMDHM;
    else if (actual_time->tm_mon != start->tm_mon || actual_time->tm_mday != start->tm_mday) return timeFormat_MDHM;
    else return timeFormat_HM;
}

void print_header() {
    printf("%-*s %*s %*s %*s %*s %*s %-*s %-*s %-*s %-*s COMMAND\n",
           maxLen[0], "USER", maxLen[1], "PID", maxLen[2], "%CPU", maxLen[3], "%MEM", maxLen[4], "VSZ",
           maxLen[5], "RSS", maxLen[6], "TTY", maxLen[7], "STAT", maxLen[8], "START", maxLen[9], "TIME");
}

void print_line(unsigned long i) {
    procInfo p = ps[i];
    char dateStart[getSizeForStartTime(p.start)];
    strftime(dateStart, sizeof(dateStart), getTimeFormatForStartTime(p.start), localtime(&p.start));
    printf("%-*s %*lu %*.1f %*.1f %*lu %*lu %*s %*c %*s %*lu:%02lu %.*s\n",
           maxLen[0], p.user, maxLen[1], p.pid, maxLen[2], p.cpu_percentage,
           maxLen[3], p.mem_percentage, maxLen[4], p.vsz, maxLen[5], p.rss, maxLen[6], p.tty, maxLen[7], p.stat,
           maxLen[8], dateStart, maxLen[9]-3, p.time/60, p.time%60, maxLen[10], p.command);
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
        free(ps[i].user);
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
        username = malloc(sizeof(char)*9);
        strncpy(username, pw->pw_name, 8);
        if (strlen(pw->pw_name)>8) username[7]='+';
        username[8]='\0';
        break;
    }
    free(line);
    fclose(file);
    return username;
}

bool getStat(unsigned long pid, char *state, double *cpuPercentage, double *memPercentage, u_long *vsz, u_long *rss, time_t *start_time, u_long *run_time) {
    char path[numberOfDigits((1ULL << (sizeof(pid_t) * 8 - 1)) - 1)+16];
    FILE *file;
    char *line = NULL;
    size_t lineSize=0;
    snprintf(path, sizeof(path), "/proc/%lu/stat", pid);
    file = fopen(path, "r");
    if (!file) {
        Error_SetError(ERROR_OPEN_FILE);
        return false;
    }
    if (getline(&line, &lineSize, file) == -1) {
        Error_SetError(ERROR_READ);
        fclose(file);
        free(line);
        return false;
    }
    unsigned long utime, stime, starttime;
    sscanf(line, "%*d %*s %c %*d %*d %*d %*d %*d %*u %*u "
                 "%*u %*u %*u %lu %lu %*d %*d %*d %*d %*d "
                 "%*d %lu %lu %lu %*u %*d %*d %*d %*u",
           state, &utime, &stime, &starttime, vsz, rss);
    free(line);
    *vsz = *vsz/1024;
    *rss = (*rss) * getpagesize() / 1024;
    *run_time = utime + stime;
    *start_time = (time(NULL) - (time_t) (current_time / (double) sysconf(_SC_CLK_TCK))) + (time_t) (starttime / sysconf(_SC_CLK_TCK));
    *cpuPercentage = (double) *run_time / (current_time - (double) starttime) * 100.0;
    *memPercentage = (double) (*rss) / (double) totalRAM * 100;
    fclose(file);
    *run_time = *run_time / sysconf(_SC_CLK_TCK);
    return true;
}

int main() {
    DIR* dir = NULL;
    struct dirent* entry;
    procInfo proc;
    struct sysinfo sys;
    if (sysinfo(&sys) != 0) {
        Error_SetError(ERROR_ENVIRONMENT);
        goto end;
    }
    totalRAM = sys.totalram * sys.mem_unit / 1024;
    FILE* file = fopen("/proc/uptime", "r");
    if (!file) {
        Error_SetError(ERROR_OPEN_FILE);
        goto end;
    }
    char uptime_str[16];
    if (fscanf(file, "%15s", uptime_str) != 1) {
        Error_SetError(ERROR_READ);
        fclose(file);
        goto end;
    }
    fclose(file);
    if (!grow_ps()) goto end;
    if (!(dir=opendir("/proc"))) {
        Error_SetError(ERROR_OPEN_DIR);
        goto end;
    }
    current_time = strtod(uptime_str, NULL) * (double) sysconf(_SC_CLK_TCK);
    time_t current_time_sec = time(NULL);
    actual_time = localtime(&current_time_sec);
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
        getStat(pid, &proc.stat, &proc.cpu_percentage, &proc.mem_percentage, &proc.vsz, &proc.rss, &proc.start, &proc.time);
        len = numberOfDigits(proc.vsz);
        if (len>maxLen[4]) maxLen[4] = len;
        len = numberOfDigits(proc.rss);
        if (len>maxLen[5]) maxLen[5] = len;
        len = getSizeForStartTime(proc.start)-1;
        if (len>maxLen[8]) maxLen[8] = len;
        len = numberOfDigits(proc.time/60)+3;
        if (len>maxLen[9]) maxLen[9] = len;
        proc.tty = "?";
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