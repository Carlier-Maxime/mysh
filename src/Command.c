#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "Command.h"
#include "Error.h"
#include "macro.h"

typedef struct private_Command {
    char* name;
    char** args;
} private_Command;

void privateCommand_destroy(private_Command* this) {
    if (!this) return;
    if (this->args) {
        int nb=0;
        while (this->args[nb++]);
        for (size_t i=0; i<nb; i++) free(this->args[i]);
        free(this->args);
    }
    free(this->name);
    free(this);
}

private_Command* privateCommand_create(const char* name, const char** args) {
    Error_SetError(ERROR_MEMORY_ALLOCATION);
    private_Command *this = malloc(sizeof(private_Command));
    if (!this) return NULL;
    if  (!(this->name=strdup(name))) goto cleanup;
    int nb=0;
    while (args[nb]) nb++;
    if (!(this->args=malloc(sizeof(char*)*(nb+1)))) goto cleanup;
    for (size_t i=0; i<nb; i++) if (!(this->args[i] = strdup(args[i]))) goto cleanup;
    this->args[nb] = NULL;
    Error_SetError(ERROR_NONE);
    return this;
cleanup:
    privateCommand_destroy(this);
    return NULL;
}

bool execute(Command* this) {
    if (!this || !pv || !pv->name || !pv->args) {
        Error_SetError(ERROR_NULL_POINTER);
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
            printf("\nprocess [%d] finish with exit code : %d", pid, WEXITSTATUS(status));
        } else printf("\nprocess [%d] anormal finish", pid);
        printf("\n~> ");
    } else {
        execv(pv->name, pv->args);
        execvp(pv->name, pv->args);
        perror("exec failed");
        exit(1);
    }
    Error_SetError(ERROR_NONE);
    return true;
}

Command* Command_create(const char* name, const char** args) {
    Error_SetError(ERROR_MEMORY_ALLOCATION);
    Command* this = malloc(sizeof(Command));
    if (!this) return NULL;
    if (!(pv = privateCommand_create(name, args))) goto cleanup;
    this->execute=execute;
    Error_SetError(ERROR_NONE);
    return this;
cleanup:
    Command_destroy(this);
    return NULL;
}

void Command_destroy(Command* this) {
    if (!this) return;
    privateCommand_destroy(pv);
    free(this);
}