#include <stdlib.h>

#include "decl.h"
#include "data.h"

static int free_regs[4];
static const char* registers[] = { "%r8", "%r9", "%r10", "%r11" }; // 64-bit registers
static const char* dregisters[] = { "%r8d", "%r9d", "%r10d", "%r11d" };
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
    fprintf(out_file, "\tsubq %s, %s\n", registers[reg2], registers[reg1]);
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

void _asm_glob_sym(int id)
{
    int size = asm_size(symbols[id].type);
    fprintf(out_file, "\t.comm %s, %d, %d\n", symbols[id].name, size, size);
}

int _asm_load_glob(int id)
{
    int r = _asm_alloc_register();
    char* name = symbols[id].name;

    switch (symbols[id].type)
    {
        case P_INT:
            fprintf(out_file, "\tmovzbl %s(%%rip), %s\n", name, registers[r]);
            break;
        case P_CHAR:
            fprintf(out_file, "\tmovzbq %s(%%rip), %s\n", name, registers[r]);
            break;
        case P_LONG:
            fprintf(out_file, "\tmovq %s(%%rip), %s\n", name, registers[r]);
            break;
    }

    return r;
}

int _asm_store_glob(int r, int id)
{
    char* name = symbols[id].name;
    switch (symbols[id].type)
    {
        case P_INT:
            fprintf(out_file, "\tmovl %s, %s(%%rip)\n", dregisters[r], name);
            break;
        case P_CHAR:
            fprintf(out_file, "\tmovb %s, %s(%%rip)\n", bregisters[r], name);
            break;
        case P_LONG:
            fprintf(out_file, "\tmovq %s, %s(%%rip)\n", registers[r], name);
            break;
    }

    return r;
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

void _asm_func_begin(int id)
{
    fprintf(out_file, "%s:\n", symbols[id].name);
    fprintf(out_file, "\tpushq %%rbp\n");
    fprintf(out_file, "\tmovq %%rsp, %%rbp\n");
}

void _asm_func_end(int func_id)
{
    asm_label(symbols[func_id].endlabel);
    fprintf(out_file, "\tmovq %%rbp, %%rsp\n");
    fprintf(out_file, "\tpopq %%rbp\n");
    fprintf(out_file, "\tret\n\n");
}

int _asm_call(int r, int id)
{
    int out = asm_alloc_register();
    fprintf(out_file, "\tmovq %s, %%rdi\n", registers[r]);
    fprintf(out_file, "\tcall %s\n", symbols[id].name);
    fprintf(out_file, "\tmovq %%rax, %s\n", registers[out]);
    asm_free_register(r);
    return out;
}

int _asm_size(int datatype)
{
    switch (datatype)
    {
        case P_NONE:
            return 0;
        case P_VOID:
            return 0;
        case P_CHAR:
            return 1;
        case P_INT:
            return 4;
        case P_LONG:
            return 8;
    }
}

void _asm_return(int r, int func_id)
{
    switch (symbols[func_id].type)
    {
        case P_INT:
            fprintf(out_file, "\tmovl %s, %%eax\n", dregisters[r]);
            break;
        case P_CHAR:
            fprintf(out_file, "\tmovzbl %s, %%eax", bregisters[r]);
            break;
        case P_LONG:
            fprintf(out_file, "\tmovq %s, %%rax\n", registers[r]);
            break;
    }

    asm_jump(symbols[func_id].endlabel);
}