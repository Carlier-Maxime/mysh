#include <stdio.h>
#include <stdlib.h>

unsigned int line_size=64, line_pos=0;

void execute_line(char *line) {
    printf("%s\n~> ", line);
    line_pos=0;
}

int main()
{
    int status=EXIT_SUCCESS, c;
    char *line=malloc(sizeof(char)*line_size), *tmp;
    if (!line) {
        status=EXIT_FAILURE;
        goto exit;
    }
    printf("~> ");
    while ((c=getchar())!=EOF) {
        if (line_pos==line_size) {
            tmp=realloc(line, line_size=line_size<1);
            if (!tmp) {
                status=EXIT_FAILURE;
                break;
            }
            line=tmp;
        }
        line[line_pos++]=(char) (c=='\n' ? '\0' : c);
        if (c=='\n') execute_line(line);
    }
exit:
    free(line);
    printf("\nexit mysh with status : %d\n", status);
    return status;
}
