#include "defs.h"
#include "data.h"
#include "decl.h"

const char* token_strings[] = {
    "EOF", "=", "+", "-", "*", "/", "==", "!=", "<", ">", "<=", ">=", "intlit", ";", 
    "identifier", "{", "}", "(", ")", "int", "if", "else", "while", "for", "void", 
    "char", "long", "return", "&", "short", ",", "sizeof", "switch", "continue", "break", 
    "[", "]", "strlit", "\\n"
};

static char identifier_buf[32];

static int next()
{
    int c;

    c = input[char_head_pos];
    char_head_pos++;

    if (c == '\n')
    {
        current_line++;
    }

    return c;
}

static int peek_next()
{
    return input[char_head_pos]; // head_pos is one after current char
}

static int skip()
{
    int c;

    c = next();
    while (c == ' ' || c == '\t' || c == '\r' || c == '\f')
    {
        c = next();
    }
    return c;
}

static int chrpos(char* s, int c)
{
    char* p;

    p = strchr(s, c);
    return p ? p - s : -1;
}

static int scanint(int c)
{
    int k, val = 0;

    while ((k = chrpos("0123456789", c)) >= 0)
    {
        val = val * 10 + k;
        c = next();
    }

    char_head_pos--;
    return val;
}

static int scanchar(int c)
{
    if (c == '\\')
    {
        int esc = next();
        switch (esc)
        {
            case 'n':
                return '\n';
            case 't':
                return '\t';
            case 'r':
                return '\r';
            case 'b':
                return '\b';
            case '"':
                return '"';
            case '\\':
                return '\\';
            case 'a':
                return '\a';
            case '\'':
                return '\'';
            case 'v':
                return '\v';
            case 'f':
                return '\f';
            default:
                error("Invalid escape sequence.");
        }
    }

    return c;
}

static char* scanstr(int c)
{
    char* str = malloc(1024);

    int i = 0;
    while (c != '"')
    {
        str[i] = scanchar(c);
        c = next();
        i++;
    }

    str[i] = '\0';

    return str;
}

static int scan_identifier(int c, char* buf, int lim)
{
    int i = 0;

    while (isalpha(c) || isdigit(c) || c == '_')
    {
        if (i == lim - 1)
        {
            syntax_error("Identifier too long.");
        }
        else
        {
            buf[i++] = c;
        }

        c = next();
    }

    char_head_pos--;
    buf[i] = '\0';
    return i;
}

static int keyword(char* str)
{
    if (!strcmp(str, "int"))
    {
        return T_INT;
    }
    else if (!strcmp(str, "if"))
    {
        return T_IF;
    }
    else if (!strcmp(str, "else"))
    {
        return T_ELSE;
    }
    else if (!strcmp(str, "while"))
    {
        return T_WHILE;
    }
    else if (!strcmp(str, "for"))
    {
        return T_FOR;
    }
    else if (!strcmp(str, "void"))
    {
        return T_VOID;
    }
    else if (!strcmp(str, "char"))
    {
        return T_CHAR;
    }
    else if (!strcmp(str, "long"))
    {
        return T_LONG;
    }
    else if (!strcmp(str, "return"))
    {
        return T_RETURN;
    }
    else if (!strcmp(str, "short"))
    {
        return T_SHORT;
    }
    else if (!strcmp(str, "sizeof"))
    {
        return T_SIZEOF;
    }
    else if (!strcmp(str, "switch"))
    {
        return T_SWITCH;
    }
    else if (!strcmp(str, "break"))
    {
        return T_BREAK;
    }
    else if (!strcmp(str, "continue"))
    {
        return T_CONTINUE;
    }

    return 0;
}

int scan(struct token* t)
{
    int c;

    c = skip();

    switch (c)
    {
        case '\n':
            t->type = T_NEWLINE;
            break;
        case EOF:
            t->type = T_EOF;
            return 0;
        case '\0':
            return 0;
        case '+':
            t->type = T_PLUS;
            break;
        case '-':
            t->type = T_MINUS;
            break;
        case '*':
            t->type = T_STAR;
            break;
        case '/':
            t->type = T_SLASH;
            break;
        case ';':
            t->type = T_SEMI_COLON;
            break;
        case ',':
            t->type = T_COMMA;
            break;
        case '=':
            if (peek_next() == '=')
            {
                next();
                t->type = T_EQ;
            }
            else
            {
                t->type = T_ASSIGN;
            }
            break;
        case '!':
            if (peek_next() == '=')
            {
                next();
                t->type = T_NE;
            }
            break;
        case '<':
            if (peek_next() == '=')
            {
                next();
                t->type = T_LE;
            }
            else
            {
                t->type = T_LT;
            }
            break;
        case '>':
            if (peek_next() == '=')
            {
                next();
                t->type = T_GE;
            }
            else
            {
                t->type = T_GT;
            }
            break;
        case '{':
            t->type = T_LBRACE;
            break;
        case '}':
            t->type = T_RBRACE;
            break;
        case '(':
            t->type = T_LPAREN;
            break;
        case ')':
            t->type = T_RPAREN;
            break;
        case '&':
            t->type = T_AMPER;
            break;
        case '[':
            t->type = T_LBRACKET;
            break;
        case ']':
            t->type = T_RBRACKET;
            break;
        case '"':
            c = next();
            t->type = T_STRLIT;
            t->v.str_value = scanstr(c);
            break;
        case '\'':
            next();
            t->type = T_INTLIT;
            t->v.int_value = scanchar(c);
            next();
            break;

        default:
            if (isdigit(c))
            {
                t->v.int_value = scanint(c);
                t->type = T_INTLIT;
                break;
            }
            else if (isalpha(c) || c == '_')
            {
                scan_identifier(c, identifier_buf, 32);

                t->v.str_value = malloc(strlen(identifier_buf) + 1);
                strcpy(t->v.str_value, identifier_buf);
                
                int tok_type;
                if (tok_type = keyword(identifier_buf))
                {
                    t->type = tok_type;
                    break;
                }

                t->type = T_IDENT;
                break;
            }

            syntax_error("Unrecognised character: '%c'", c);
    }

    return 1;
}