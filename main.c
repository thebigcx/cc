#include "defs.h"
#define extern_
#include "data.h"
#undef extern_
#include "decl.h"

static void print_tokens(struct token_array array)
{
    printf("Tokens: \n");

    for (int i = 0; i < array.size; i++)
    {
        printf("Token: %s", token_strings[array.data[i].type]);
        if (array.data[i].type == T_INTLIT)
        {
            printf(", value %d", array.data[i].v.int_value);
        }
        printf("\n");
    }
}

static struct ast_node* make_ast()
{
    return binary_expr(0);
}

static struct token_array scan_file()
{
    struct token_array array;
    array.data = NULL;

    struct token tok;
    int i = 0;
    while (1)
    {
        int should_continue = scan(&tok);

        array.data = realloc(array.data, sizeof(struct token) * (i + 1));
        array.data[i] = tok;
        i++;

        if (!should_continue)
        {
            break;
        }
    }

    array.size = i;

    return array;
}

void next_token()
{
    tok_head_pos++;
    current_tok = tok_array.data[tok_head_pos];

    while (current_tok.type == T_NEWLINE)
    {
        line++;
        tok_head_pos++;
        current_tok = tok_array.data[tok_head_pos];
    }
}

int main(int argc, char** argv)
{
    FILE* file;
    long size;

    if (argc < 2)
    {
        printf("usage: compiler <input_file>\n");
        exit(-1);
    }

    char* file_name = argv[1];
    file = fopen(file_name, "rb");

    if (!file)
    {
        char* msg = concat(file_name, ": No such file or directory");
        error(msg);
        free(msg);
    }

    fseek(file, 0l, SEEK_END);
    size = ftell(file);
    rewind(file);

    input = calloc(1, size + 1);
    fread(input, size, 1, file);
    input[size] = EOF;

    line = 1;
    tok_array = scan_file();
    //print_tokens(tok_array);

    tok_head_pos = -1;
    next_token();

    out_file = fopen("out.s", "w");
    line = 1;
    gen_code();
    fclose(out_file);

    system("gcc -no-pie out.s");

    fclose(file);
    free(input);

    return 0;
}