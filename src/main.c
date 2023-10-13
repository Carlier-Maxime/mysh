#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Error.h"

#define IS_WHITE_SPACE(c) (c=='\n' || c==' ' || c=='\t' || c=='\v' || c=='\r')

typedef struct {
    unsigned int size, pos;
    char *chars;
} Line;

Line *createLine() {
    Line *line=malloc(sizeof(Line));
    if (!line) {
        Error_SetError(ERROR_MEMORY_ALLOCATION);
        return NULL;
    }
    line->pos=0;
    line->size=64;
    line->chars=malloc(sizeof(char)*line->size);
    if (!line->chars) {
        free(line);
        Error_SetError(ERROR_MEMORY_ALLOCATION);
        return NULL;
    }
    Error_SetError(ERROR_NONE);
    return line;
}

void destroyLine(Line *line) {
    if (!line) return;
    free(line->chars);
    free(line);
}

bool Line_addChar(Line *line, char c) {
    if (line->pos==line->size) {
        char* tmp;
        tmp=realloc(line->chars, line->size<<=1);
        if (!tmp) {
            Error_SetError(ERROR_MEMORY_ALLOCATION);
            return false;
        }
        line->chars=tmp;
    }
    bool isWhiteSpace = IS_WHITE_SPACE(c);
    if ((line->pos && line->chars[line->pos-1]) || !isWhiteSpace) line->chars[line->pos++]=(char)(isWhiteSpace ? '\0' : c);
    Error_SetError(ERROR_NONE);
    return true;
}

void execute_line(Line* line) {
    for (unsigned int i=0; i<line->pos; i++) putchar(line->chars[i] ? line->chars[i] : ' ');
    printf("\n~> ");
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
    destroyLine(line);
    if (Error_GetErrorStatus()) Error_PrintErrorMsg("A Error is occurred");
    printf("\nexit mysh with status : %d\n", Error_GetErrorStatus());
    return Error_GetErrorStatus();
}
