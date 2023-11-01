#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include "../utils/Error.h"
#include "../utils/macro.h"

bool Test_ExecuteProgram(const char *name, char* const* args, int fd_stdin, int fd_stdout) {
    pid_t pid=fork();
    if (pid==-1) {
        Error_SetError(ERROR_FORK);
        return false;
    }
    if (pid) {
        int status;
        waitpid(pid, &status, 0);
    } else {
        if (dup2(fd_stdin, STDIN_FILENO) == -1) {
            perror(RED("dup2 for stdin, failed"));
            exit(1);
        }
        if (dup2(fd_stdout, STDOUT_FILENO) == -1) {
            perror(RED("dup2 for stdout, failed"));
            exit(1);
        }
        execv(name, args);
        execvp(name, args);
        perror(RED("exec failed"));
        exit(1);
    }
    Error_SetError(ERROR_NONE);
    return true;
}

char* Test_getProgramOutput(const char* name, char* const* args) {
    int pipe_fd_out[2] = {-1,-1};
    char buffer[1024];
    unsigned int len = sizeof(buffer), i=0;
    char* out = NULL;
    if (pipe(pipe_fd_out) == -1) {
        Error_SetError(ERROR_PIPE);
        goto exit;
    }
    if (!Test_ExecuteProgram(name, args, STDIN_FILENO, pipe_fd_out[1])) goto exit;
    if (!(out = malloc(sizeof(char)*len))) {
        Error_SetError(ERROR_MEMORY_ALLOCATION);
        goto exit;
    }
    ssize_t bytes_read;
    close(pipe_fd_out[1]);
    pipe_fd_out[1]=-1;
    while ((bytes_read = read(pipe_fd_out[0], buffer, sizeof(buffer))) > 0) {
        for (unsigned j=0; j<bytes_read; j++) {
            if (i==len) {
                char* tmp = realloc(out, len<<=1);
                if (!tmp) {
                    Error_SetError(ERROR_MEMORY_ALLOCATION);
                    free(out);
                    out=NULL;
                    goto exit;
                }
                out=tmp;
            }
            out[i++]=buffer[j];
        }
    }
    if (bytes_read < 0) {
        Error_SetError(ERROR_READ);
        free(out);
        out=NULL;
        goto exit;
    }
    Error_SetError(ERROR_NONE);
exit:
    close(pipe_fd_out[1]);
    close(pipe_fd_out[0]);
    return out;
}

int main() {
    char* const args[] = {"myls", "-Ra", "src", NULL};
    char* out;
    if (!(out=Test_getProgramOutput(args[0], args))) goto exit;
    printf("%s",out);
    free(out);
    Error_SetError(ERROR_NONE);
exit:
    if (Error_GetErrorStatus()!=ERROR_NONE) Error_PrintErrorMsg("Error : ");
    return Error_GetErrorStatus();
}