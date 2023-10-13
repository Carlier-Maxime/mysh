#include <stdio.h>
#include <stdbool.h>
#include "Error.h"
#include "CommandParser.h"

int main()
{
    int c=0;
    CommandParser* commandParser = CommandParser_create();
    if (!commandParser) goto exit;
    printf("~> ");
    bool ready=false;
    while (c!=EOF) {
        while (commandParser->consumeChar(commandParser, (char) (c=getchar())) && !(ready=commandParser->isExecutionReady(commandParser))) if (c==EOF) break;
        if (ready) commandParser->execute(commandParser);
        else break;
    }
exit:
    CommandParser_destroy(commandParser);
    if (Error_GetErrorStatus()) Error_PrintErrorMsg("A Error is occurred");
    printf("\nexit mysh with status : %d\n", Error_GetErrorStatus());
    return Error_GetErrorStatus();
}
