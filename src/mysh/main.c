#include <stdio.h>
#include "../utils/Error.h"
#include "CommandParser.h"
#include "../utils/macro.h"
#include "Environment.h"

int main()
{
    Environment_init();
    CommandParser* commandParser = CommandParser_create();
    if (!commandParser) goto exit;
    printf("%s%s%s> ", BLUE_BEGIN, Environment_getCwd(), COLOR_RESET);
    while (CommandParser_consumeChar(commandParser, (char) (getchar())));
exit:
    CommandParser_destroy(commandParser);
    Environment_free();
    int exit_code = Error_GetErrorStatus();
    if (exit_code) Error_PrintErrorMsg("A Error is occurred");
    fprintf(exit_code ? stderr : stdout, "%s\nexit mysh with status : %d%s\n", exit_code ? RED_BEGIN : GREEN_BEGIN, exit_code, COLOR_RESET);
    return Error_GetErrorStatus();
}
