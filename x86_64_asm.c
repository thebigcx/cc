#include <stdlib.h>

#include "decl.h"
#include "data.h"

static int free_regs[4];
static const char* registers[] = { "%r8", "%r9", "%r10", "%r11" }; // 64-bit registers
static const char* bregisters[] = { "%r8b", "%r9b", "%r10b", "%r11b" }; // 8-bit registers

void _asm_free_all_registers()
{
    free_regs[0] = free_regs[1] = free_regs[2] = free_regs[3] = 1;
}

int _asm_alloc_register()
{
    for (int i = 0; i < 4; i++)
    {
        if (free_regs[i])
        {
            free_regs[i] = 0;
            return i;
        }
    }

    printf("Out of registers.\n");
    exit(1);
}

void _asm_free_register(int reg)
{
    free_regs[reg] = 1;
}

void _asm_preamble()
{
    asm_free_all_registers();

    fputs(

        "\t.text\n"
        "\t.global main\n"
        "main:\n"
        "\tpushq %rbp\n"
        "\tmovq %rsp, %rbp\n"

    , out_file);
}

void _asm_postamble()
{
    fputs(

        "\tmovq %rbp, %rsp\n"
        "\tpopq %rbp\n"
        "\n"

    , out_file);
}

int _asm_load(int val)
{
    int reg = asm_alloc_register();

    fprintf(out_file, "\tmovq $%d, %s\n", val, registers[reg]);
    return reg;
}

int _asm_add(int reg1, int reg2)
{
    fprintf(out_file, "\taddq %s, %s\n", registers[reg1], registers[reg2]);
    asm_free_register(reg1);
    return reg2;
}

int _asm_sub(int reg1, int reg2)
{
    fprintf(out_file, "\tsubq %s, %s\n", registers[reg1], registers[reg2]);
    asm_free_register(reg2);
    return reg1;
}

int _asm_mul(int reg1, int reg2)
{
    fprintf(out_file, "\timulq %s, %s\n", registers[reg1], registers[reg2]);
    asm_free_register(reg1);
    return reg2;
}

int _asm_div(int reg1, int reg2)
{
    fprintf(out_file, "\tmovq %s, %%rax\n", registers[reg1]);
    fprintf(out_file, "\tcqo\n");
    fprintf(out_file, "\tidivq %s\n", registers[reg2]);
    fprintf(out_file, "\tmovq %%rax, %s", registers[reg1]);
    asm_free_register(reg2);
    return reg1;
}

void _asm_glob_sym(char* sym)
{
    fprintf(out_file, "\t.comm %s, 8, 8\n", sym);
}

int _asm_load_glob(char* identifier)
{
    int r = _asm_alloc_register();

    fprintf(out_file, "\tmovq %s(%%rip), %s\n", identifier, registers[r]);
    return r;
}

int _asm_store_glob(int r, char* identifier)
{
    fprintf(out_file, "\tmovq %s, %s(%%rip)\n", registers[r], identifier);
}

int _asm_compare(int r1, int r2, char* how)
{
    fprintf(out_file, "\tcmpq %s, %s\n", registers[r1], registers[r2]);
    fprintf(out_file, "\t%s %s\n", how, bregisters[r2]);
    fprintf(out_file, "\tandq $255, %s\n", registers[r2]);
    return r2;
}

static char* cmp_list[] = { "sete", "setne", "setl", "setg", "setle", "setge" };

int _asm_compare_and_set(int type, int r1, int r2)
{
    fprintf(out_file, "\tcmpq %s, %s\n", registers[r2], registers[r1]);
    fprintf(out_file, "\t%s %s\n", cmp_list[type - N_EQ], bregisters[r2]);
    fprintf(out_file, "\tmovzbq %s, %s\n", bregisters[r2], registers[r2]);
    asm_free_register(r1);
    return r2;
}

static char* jmp_cmp_list[] = { "jne", "je", "jge", "jle", "jg", "jl" };

int _asm_compare_and_jump(int type, int r1, int r2, int label)
{
    fprintf(out_file, "\tcmpq %s, %s\n", registers[r2], registers[r1]);
    fprintf(out_file, "\t%s L%d\n", jmp_cmp_list[type - N_EQ], label);
    asm_free_all_registers();
    return NOREG;
}

void _asm_label(int label)
{
    fprintf(out_file, "L%d:\n", label);
}

void _asm_jump(int label)
{
    fprintf(out_file, "\tjmp L%d\n", label);
}