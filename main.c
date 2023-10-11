#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    NO_ERROR=0,
    ALLOCATION_FAILED=1
} Status;
Status status=NO_ERROR;

typedef struct {
    unsigned int size, pos;
    char *chars;
} Line;

Line *createLine() {
    Line *line=malloc(sizeof(Line));
    if (!line) {
        status=ALLOCATION_FAILED;
        return NULL;
    }
    line->pos=0;
    line->size=64;
    line->chars=malloc(sizeof(char)*line->size);
    if (!line->chars) {
        free(line);
        status=ALLOCATION_FAILED;
        return NULL;
    }
    status=NO_ERROR;
    return line;
}

bool Line_addChar(Line *line, char c) {
    if (line->pos==line->size) {
        char* tmp;
        tmp=realloc(line->chars, line->size<<=1);
        if (!tmp) {
            status = ALLOCATION_FAILED;
            return false;
        }
        line->chars=tmp;
    }
    line->chars[line->pos++]=(char)(c=='\n' ? '\0' : c);
    status=NO_ERROR;
    return true;
}

void execute_line(Line* line) {
    printf("%s\n~> ", line->chars);
    line->pos=0;
}

int main()
{
    int c;
    Line* line = createLine();
    if (!line) goto exit;
    printf("~> ");
    while ((c=getchar())!=EOF) {
        if (!Line_addChar(line, (char) c)) goto exit;
        if (c=='\n') execute_line(line);
    }
exit:
    free(line);
    printf("\nexit mysh with status : %d\n", status);
    return status;
}
