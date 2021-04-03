#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum TOKENS
{
    T_EOF,
    T_PLUS, T_MINUS, T_STAR, T_SLASH,
    T_EQ, T_NE,
    T_LT, T_GT, T_LE, T_GE,
    T_INTLIT, T_SEMI_COLON, T_ASSIGN, T_IDENT,
    T_LBRACE, T_RBRACE, T_LPAREN, T_RPAREN,
    T_INT, T_IF, T_ELSE, T_WHILE, T_FOR, T_VOID, T_CHAR,

    T_NEWLINE
};

extern const char* token_strings[];

struct token
{
    int type;
    union
    {
        int int_value;
        char* str_value;
    } v;
};

struct token_array
{
    int size;
    struct token* data;
};

enum NODES
{
    N_ADD, N_SUB, N_MULT, N_DIV,
    N_EQ, N_NE, N_LT, N_GT, N_LE, N_GE,
    N_INTLIT, N_IDENT, N_ASSIGN, N_LVIDENT, N_GLUE, N_IF, N_WHILE, N_FOR, N_FUNCTION
};

struct ast_node
{
    int type;
    struct ast_node* left;
    struct ast_node* mid;
    struct ast_node* right;
    int dtype; // Primitive type
    union
    {
        int intvalue; // N_INTLIT
        int id; // N_IDENT
    } v;
};

#define NOREG -1

struct sym
{
    char* name;
    int type;
};

enum TYPES
{
    P_NONE, P_VOID, P_INT, P_CHAR
};

extern const char* type_strings[];