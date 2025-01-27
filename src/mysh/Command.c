#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "Command.h"
#include "../utils/Error.h"
#include "../utils/macro.h"

bool Command_execute(const Command* this) {
    if (!this || !this->name || !this->args) {
        Error_SetError(ERROR_NULL_POINTER);
        return false;
    }
    if (strcmp(this->name, "exit")==0) {
        Error_SetError(ERROR_NONE);
        return false;
    }
    pid_t pid=fork();
    if (pid==-1) {
        Error_SetError(ERROR_FORK);
        return false;
    }
    if (pid) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            fprintf(exit_code ? stderr : stdout,"%s\nprocess [%d] finish with exit code : %d%s\n", exit_code ? RED_BEGIN : GREEN_BEGIN, pid, exit_code, COLOR_RESET);
        } else fprintf(stderr,RED("\nprocess [%d] abnormally finished\n"), pid);
    } else {
        execv(this->name, this->args);
        execvp(this->name, this->args);
        perror(RED("exec failed"));
        exit(1);
    }
    Error_SetError(ERROR_NONE);
    return true;
}

Command* Command_create(const char* name, const char** args) {
    Error_SetError(ERROR_MEMORY_ALLOCATION);
    Command* this = malloc(sizeof(Command));
    if (!this) return NULL;
    if  (!(this->name=strdup(name))) goto cleanup;
    u_int nb=0;
    while (args[nb]) nb++;
    if (!(this->args=malloc(sizeof(char*)*(nb+2)))) goto cleanup;
    for (u_int i=0; i<nb; i++) if (!(this->args[i] = strdup(args[i]))) goto cleanup;
    this->args[nb+1] = NULL;
    Error_SetError(ERROR_NONE);
    return this;
cleanup:
    Command_destroy(this);
    return NULL;
}

void Command_destroy(Command* this) {
    if (!this) return;
    if (this->args) {
        u_int nb=0;
        while (this->args[nb++]);
        for (u_int i=0; i<nb; i++) free(this->args[i]);
        free(this->args);
    }
    free(this->name);
    free(this);
}