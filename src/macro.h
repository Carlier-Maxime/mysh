#ifndef MYSH_MACRO_H
#define MYSH_MACRO_H

#define pv this->private
#define COLOR_RESET "\033[0m"
#define COLOR(color_begin, str) color_begin str COLOR_RESET
#define RED_BEGIN "\033[31m"
#define RED(str) COLOR(RED_BEGIN, str)
#define GREEN_BEGIN "\033[32m"
#define GREEN(str) COLOR(GREEN_BEGIN, str)
#define BLUE_BEGIN "\033[34m"
#define BLUE(str) COLOR(BLUE_BEGIN, str)

#endif //MYSH_MACRO_H
