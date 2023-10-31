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
        if (dup2(fd_stdin, 0) == -1) {
            perror(RED("dup2 for stdin, failed"));
            exit(1);
        }
        if (dup2(fd_stdout, 1) == -1) {
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

int main() {
    int pipe_fd_in[2] = {-1,-1};
    int pipe_fd_out[2] = {-1,-1};
    if (pipe(pipe_fd_in) == -1) {
        Error_SetError(ERROR_PIPE);
        goto exit;
    }
    if (pipe(pipe_fd_out) == -1) {
        Error_SetError(ERROR_PIPE);
        goto exit;
    }
    char* const args[] = {"myls", "-Ra", "src", NULL};
    Test_ExecuteProgram(args[0], args, pipe_fd_in[0], pipe_fd_out[1]);
exit:
    close(pipe_fd_in[0]);
    close(pipe_fd_in[1]);
    close(pipe_fd_out[0]);
    close(pipe_fd_out[1]);
    if (Error_GetErrorStatus()!=ERROR_NONE) Error_PrintErrorMsg("Error : ");
    return Error_GetErrorStatus();
}