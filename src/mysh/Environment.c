#include <unistd.h>
#include <linux/limits.h>
#include "Environment.h"
#include "../utils/Error.h"

char current_dir[PATH_MAX];

bool Environment_init() {
    if (!getcwd((char *) &current_dir, PATH_MAX)) {
        Error_SetError(ERROR_ENVIRONMENT);
        return false;
    }
    return true;
}

const char* Environment_getCwd() {
    return current_dir;
}

void Environment_free() {

}
