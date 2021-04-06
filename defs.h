#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum TOKENS
{
    T_EOF,
    T_ASSIGN,
    T_PLUS, T_MINUS, T_STAR, T_SLASH,
    T_EQ, T_NE,
    T_LT, T_GT, T_LE, T_GE,
    T_INTLIT, T_SEMI_COLON, T_IDENT,
    T_LBRACE, T_RBRACE, T_LPAREN, T_RPAREN,
    T_INT, T_IF, T_ELSE, T_WHILE, T_FOR, T_VOID, T_CHAR, T_LONG, T_RETURN, T_AMPER, T_SHORT,
    T_COMMA, T_SIZEOF, T_SWITCH, T_CONTINUE, T_BREAK, T_LBRACKET, T_RBRACKET, T_STRLIT,

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
    N_INTLIT, N_IDENT, N_ASSIGN, N_GLUE, N_IF, N_WHILE, N_FOR, N_FUNCTION, N_FUNCCALL, N_RETURN, N_ADDROF, N_DEREF, N_CAST,
    N_BREAK, N_CONTINUE, N_SCALE, N_STRLIT
};

struct ast_node
{
    int type;
    struct ast_node* left;
    struct ast_node* mid;
    struct ast_node* right;
    int dtype; // Primitive type
    int rvalue;
    union
    {
        int intvalue; // N_INTLIT
        int id; // N_IDENT
    } v;
};

#define NOREG -1

enum st_type // Structural type
{
    S_FUNC, S_VAR, S_ARR
};

struct sym
{
    char* name;
    int type;
    int stype;
    int endlabel;
    int arr_elements;
};

enum TYPES
{
    P_NONE, P_VOID, P_INT, P_CHAR, P_LONG, P_SHORT,
    P_VOIDPTR, P_INTPTR, P_CHARPTR, P_LONGPTR, P_SHORTPTR
};

extern const char* type_strings[];