#include <stdarg.h>

#include "decl.h"
#include "data.h"

void error(const char* msg)
{
    char full_msg[256];
    snprintf(full_msg, 256, "\033[1m\033[37mcompiler: \033[1m\033[31merror: \033[0m\033[37m%s", msg);
    fprintf(stderr, "%s\n", full_msg);
    exit(-1);
}

void syntax_error(const char* format, ...)
{
    va_list list;

    va_start(list, format);

    // Format
    char usr_msg[128];
    vsprintf(usr_msg, format, list);
    va_end(list);

    char full_msg[256];
    snprintf(full_msg, 256, "Syntax error on line %d: %s\n\t%s", current_line, usr_msg, input_lines[current_line]);
    error(full_msg);
}