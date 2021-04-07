#include <unistd.h>
#include <argp.h>
#include <time.h>

#include "defs.h"
#define extern_
#include "data.h"
#undef extern_
#include "decl.h"

// TODO: clean up this file

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

struct args_t
{
    char* input_files[64];
    int input_file_count;
    char* output_file;
    int output_assembly;
    char* defines[64];
    int define_count;
    char* include_dirs[16];
    int include_dir_count;
};

static char* change_suffix(char* str, char suffix)
{
    char* posn;
    char* newstr;

    if ((newstr = strdup(str)) == NULL)
    {
        return NULL;
    }

    if ((posn = strrchr(newstr, '.')) == NULL)
    {
        return NULL;
    }

    posn++;
    if (*posn == '\0')
    {
        return NULL;
    }

    *posn++ = suffix;
    *posn = '\0';

    return newstr;
}

char* compile_file(char* file_name)
{
    size_t size;

    current_file = file_name;

    char cmd[256];
    snprintf(cmd, 256, "python3 preproc.py %s", current_file);
    system(cmd);

    char* preproc_file = change_suffix(current_file, 'i');
    FILE* file = fopen(preproc_file, "rb");

    if (!file)
    {
        char msg[64];
        snprintf(msg, 64, "%s: No such file or directory", current_file);
        error(msg);
    }

    fseek(file, 0l, SEEK_END);
    size = ftell(file);
    rewind(file);

    input = calloc(1, size + 1);
    fread(input, size, 1, file);
    input[size] = EOF;
    fclose(file);

    make_file_lines(current_file);

    //timer = clock();

    current_line = 1;
    locls = 1023;
    tok_array = scan_file();

    tok_head_pos = -1;
    next_token();

    char* asm_file = change_suffix(current_file, 's');

    out_file = fopen(asm_file, "w");
    current_line = 1;
    gen_code();
    fclose(out_file);

    free(input);

    return asm_file;
}

int main(int argc, char* argv[])
{
    FILE* file;
    long size;
    int opt;
    clock_t timer;

    struct args_t args;
    args.output_file = NULL;
    args.input_file_count = 0;
    args.define_count = 0;
    args.include_dir_count = 0;

    while ((opt = getopt(argc, argv, "s:o:S")) != -1)
    {
        switch (opt)
        {
            case 's':
                args.input_files[args.input_file_count++] = optarg;
                break;
            case 'S':
                args.output_assembly = 1;
                break;
            case 'o':
                args.output_file = optarg;
                break;
            default:
                print_usage();
                exit(0);
        }
    }

    current_file = args.input_files[0];
    if (current_file == NULL)
    {
        error("No input file");
    }
    
    if (args.output_file == NULL)
    {
        args.output_file = malloc(6);
        strcpy(args.output_file, "a.out");
    }

    char* assemblies[32];
    int assembly_count = args.input_file_count;

    for (int i = 0; i < args.input_file_count; i++)
    {
        printf("cc %s\n", args.input_files[i]);
        assemblies[i] = compile_file(args.input_files[i]);
    }

    char assemble_cmd[1024];
    snprintf(assemble_cmd, 50, "gcc -no-pie %s -o %s", assemblies[0], args.output_file);

    system(assemble_cmd);

    return 0;
}