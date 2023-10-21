#include <stdio.h>
#include "Error.h"
#include "CommandParser.h"
#include "macro.h"
#include "Environment.h"

int main()
{
    Environment_init();
    CommandParser* commandParser = CommandParser_create();
    if (!commandParser) goto exit;
    printf("%s%s%s> ", BLUE_BEGIN, Environment_getCwd(), COLOR_RESET);
    while (commandParser->consumeChar(commandParser, (char) (getchar())));
exit:
    CommandParser_destroy(commandParser);
    Environment_free();
    int exit_code = Error_GetErrorStatus();
    if (exit_code) Error_PrintErrorMsg("A Error is occurred");
    fprintf(exit_code ? stderr : stdout, "%s\nexit mysh with status : %d\n", exit_code ? RED_BEGIN : GREEN_BEGIN, exit_code);
    return Error_GetErrorStatus();
}
