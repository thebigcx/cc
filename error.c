#include <stdarg.h>

#include "decl.h"
#include "data.h"

void error(const char* msg)
{
    char full_msg[256];
    snprintf(full_msg, 256, "\033[1m\033[37m%s:%d: \033[1m\033[31merror: \033[0m\033[37m%s", current_file, current_line, msg);
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
    snprintf(full_msg, 256, "\033[1m\033[37m%s:\033[0m\033[37m In function '\033[1m\033[37m%s\033[0m\033[37m':\n\033[1m\033[37m%s:%d: \033[1m\033[31merror: \033[0m\033[37mSyntax error: %s\n  %d |\t%s", current_file, symbols[func_id].name, current_file, current_line, usr_msg, current_line, input_lines[current_line]);
    fprintf(stderr, "%s\n", full_msg);
    exit(-1);
}

void warning(const char* format, ...)
{
    va_list list;

    va_start(list, format);

    // Format
    char usr_msg[128];
    vsprintf(usr_msg, format, list);
    va_end(list);

    char full_msg[256];
    snprintf(full_msg, 256, "\033[1m\033[37m%s:\033[0m\033[37m In function '\033[1m\033[37m%s\033[0m\033[37m':\n\033[1m\033[37m%s:%d: \033[1m\033[35mwarning\033[0m\033[37m: %s\n  %d |\t%s", current_file, symbols[func_id].name, current_file, current_line, usr_msg, current_line, input_lines[current_line]);

    fprintf(stdout, "%s\n", full_msg);
}