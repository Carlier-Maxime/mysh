#include <stdio.h>
#include "Error.h"
#include "CommandParser.h"

int main()
{
    CommandParser* commandParser = CommandParser_create();
    if (!commandParser) goto exit;
    printf("~> ");
    while (commandParser->consumeChar(commandParser, (char) (getchar())));
exit:
    CommandParser_destroy(commandParser);
    if (Error_GetErrorStatus()) Error_PrintErrorMsg("A Error is occurred");
    printf("\nexit mysh with status : %d\n", Error_GetErrorStatus());
    return Error_GetErrorStatus();
}
