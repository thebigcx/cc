#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "decl.h"

char* concat(const char* s1, const char* s2)
{
    char* result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void error(const char* msg)
{
    char* full_msg = concat("\033[1m\033[37mcompiler: \033[1m\033[31merror: \033[0m\033[37m", msg);
    fprintf(stderr, "%s\n", full_msg);
    free(full_msg);
    exit(-1);
}

void syntax_error(int line, const char* format, ...)
{
    va_list list;

    va_start(list, 10);

    // Format
    char usr_msg[128];
    vsprintf(usr_msg, format, list);
    va_end(list);

    char full_msg[256];
    snprintf(full_msg, 256, "Syntax error on line %d: %s", line, usr_msg);
    error(full_msg);
}