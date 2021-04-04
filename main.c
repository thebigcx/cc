#include <unistd.h>

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

void make_file_lines(char* file_name)
{
    FILE* file = fopen(file_name, "r");
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    int i = 0;
    while ((read = getline(&line, &len, file)) != -1)
    {
        input_lines[i + 1] = malloc(128);
        strcpy(input_lines[i + 1], line);
        i++;
    }

    fclose(file);
}

void print_usage()
{
    printf("usage: compiler <input_file>\n");
}

int main(int argc, char* argv[])
{
    char* file_name = NULL;
    char* output_name = NULL;
    FILE* file;
    long size;
    int opt;
    int output_assembly = 0;

    while ((opt = getopt(argc, argv, "s:o:S")) != -1)
    {
        switch (opt)
        {
            case 's':
                file_name = optarg;
                break;
            case 'S':
                output_assembly = 1;
                break;
            case 'o':
                output_name = optarg;
                break;
            default:
                print_usage();
                exit(0);
        }
    }

    if (file_name == NULL)
    {
        error("No input file");
    }

    if (output_name == NULL)
    {
        output_name = malloc(6);
        strcpy(output_name, "a.out");
    }

    file = fopen(file_name, "rb");

    if (!file)
    {
        char msg[64];
        snprintf(msg, 64, "%s: No such file or directory", file_name);
        error(msg);
    }

    fseek(file, 0l, SEEK_END);
    size = ftell(file);
    rewind(file);

    input = calloc(1, size + 1);
    fread(input, size, 1, file);
    input[size] = EOF;
    fclose(file);

    make_file_lines(file_name);

    current_line = 1;
    tok_array = scan_file();
    //print_tokens(tok_array);

    tok_head_pos = -1;
    next_token();

    char assembly_output_name[128];
    snprintf(assembly_output_name, 128, "%s.s", output_name);

    out_file = fopen(assembly_output_name, "w");
    current_line = 1;
    gen_code();
    fclose(out_file);

    char assemble_cmd[256];
    snprintf(assemble_cmd, 256, "gcc -no-pie %s -o %s", assembly_output_name, output_name);

    system(assemble_cmd);

    if (!output_assembly)
    {
        remove(assembly_output_name);
    }

    free(input);

    return 0;
}