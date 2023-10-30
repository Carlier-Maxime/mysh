#ifndef MYSH_ENVIRONMENT_H
#define MYSH_ENVIRONMENT_H

#include <stdbool.h>

bool Environment_init();
const char* Environment_getCwd();
void Environment_free();

#endif //MYSH_ENVIRONMENT_H
