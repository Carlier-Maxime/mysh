#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

u_int Test_countLines(const char* str) {
    u_int lines = 0;
    for (u_int i = 0; str[i] != '\0'; i++) if (str[i] == '\n') lines++;
    return lines;
}

bool Test_AssertString(const char* expected, const char* actual) {
    bool same = strcmp(expected,actual)==0;
    if (!same) {
        const char indent[] = "    ";
        fprintf(stderr, "Assertion String, failed :\n");
        size_t expected_len = strlen(expected);
        size_t actual_len = strlen(actual);
        if (expected_len != actual_len) {
            fprintf(stderr, RED("%snot same length")" : diff: "RED("%zu")", expected: "BLUE("%zu")", actual: "BLUE("%zu")"\n",
                    indent, expected_len > actual_len ? expected_len - actual_len : actual_len - expected_len, expected_len, actual_len);
            u_int expected_nbLines = Test_countLines(expected);
            u_int actual_nbLines = Test_countLines(actual);
            if (expected_nbLines!=actual_nbLines) {
                fprintf(stderr, RED("%snot same number lines")" : diff: "RED("%u")", expected: "BLUE("%u")", actual: "BLUE("%u")"\n",
                        indent, expected_nbLines > actual_nbLines ? expected_nbLines - actual_nbLines : actual_nbLines - expected_nbLines, expected_nbLines, actual_nbLines);
            }
        }
        size_t diff = actual_len < expected_len ? expected_len - actual_len : actual_len - expected_len;
        size_t min_len = actual_len < expected_len ? actual_len : expected_len;
        u_int i;
        for (i = 0; i < min_len; i++) if (expected[i] != actual[i]) diff++;
        fprintf(stderr,RED("%snot same chars")" : %zu different(s) char(s) (the number of difference may be not exact)\n", indent, diff);
        fprintf(stderr, "%s""expected:\n%s%s", indent, indent, indent);
        for (i=0; expected[i]!='\0'; i++) {
            fprintf(stderr, "%c", expected[i]);
            if (expected[i]=='\n') fprintf(stderr, "%s%s", indent, indent);
        }
        fprintf(stderr, "\r%s""actual:\n%s%s", indent, indent, indent);
        for (i=0; actual[i]!='\0'; i++) {
            fprintf(stderr, "%c", actual[i]);
            if (actual[i]=='\n') fprintf(stderr, "%s%s", indent, indent);
        }
        fprintf(stderr,"\r");
    }
    return same;
}

bool Test_AssertProgramsOutput(const char* program1, char* const* args_program1, const char* program2, char* const* args_program2) {
    bool same=false;
    char* out=NULL, *out2=NULL;
    if (!(out=Test_getProgramOutput(program1, args_program1))) goto exit;
    if (!(out2=Test_getProgramOutput(program2, args_program2))) goto exit;
    if (!Test_AssertString(out,out2)) goto exit;
    same=true;
exit:
    free(out);
    free(out2);
    return same;
}

bool Test_test_myls() {
#define TEST_TEST_MYLS_MAX_ARGS 6
#define TEST_TEST_MYLS_MYLS_ARGS_BASE 2
#define TEST_TEST_MYLS_LS_ARGS_BASE 3
    char* myls[TEST_TEST_MYLS_MAX_ARGS+TEST_TEST_MYLS_MYLS_ARGS_BASE] = {"myls", "--no-color"};
    char* ls[TEST_TEST_MYLS_MAX_ARGS+TEST_TEST_MYLS_LS_ARGS_BASE] = {"ls", "--color=never", "-l"};
    char* end_args[][TEST_TEST_MYLS_MAX_ARGS] = {
            {"src/myls/myls.c", NULL},
            {"src", NULL},
            {"-a", "src", NULL},
            {"-Ra", "src", NULL},
            {"src", "-a", "-R", NULL},
            {NULL}
    };
    u_int j;
    for (u_int i=0; end_args[i][0]; i++) {
        for (j=0; end_args[i][j]; j++) {
            myls[TEST_TEST_MYLS_MYLS_ARGS_BASE+j]=end_args[i][j];
            ls[TEST_TEST_MYLS_LS_ARGS_BASE+j]=end_args[i][j];
        }
        if (!Test_AssertProgramsOutput(ls[0], ls, myls[0], myls)) return false;
    }
    return true;
}

int main() {
    if (!Test_test_myls()) goto exit;
    Error_SetError(ERROR_NONE);
exit:
    if (Error_GetErrorStatus()!=ERROR_NONE) Error_PrintErrorMsg("Error : ");
    return Error_GetErrorStatus();
}