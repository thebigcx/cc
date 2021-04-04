#include <stdlib.h>

#include "decl.h"
#include "data.h"

static int free_regs[4];
static const char* registers[] = { "%r8", "%r9", "%r10", "%r11" }; // 64-bit registers
static const char* dregisters[] = { "%r8d", "%r9d", "%r10d", "%r11d" }; // 32-bit registers
static const char* wregisters[] = { "%r8w", "%r9w", "%r10w", "%r11w" }; // 16-bit registers
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

    fprintf(out_file, "\tmovq\t$%d, %s\n", val, registers[reg]);
    return reg;
}

int _asm_add(int reg1, int reg2)
{
    fprintf(out_file, "\taddq\t%s, %s\n", registers[reg1], registers[reg2]);
    asm_free_register(reg1);
    return reg2;
}

int _asm_sub(int reg1, int reg2)
{
    fprintf(out_file, "\tsubq\t%s, %s\n", registers[reg2], registers[reg1]);
    asm_free_register(reg2);
    return reg1;
}

int _asm_mul(int reg1, int reg2)
{
    fprintf(out_file, "\timulq\t%s, %s\n", registers[reg1], registers[reg2]);
    asm_free_register(reg1);
    return reg2;
}

int _asm_div(int reg1, int reg2)
{
    fprintf(out_file, "\tmovq\t%s, %%rax\n", registers[reg1]);
    fprintf(out_file, "\tcqo\n");
    fprintf(out_file, "\tidivq\t%s\n", registers[reg2]);
    fprintf(out_file, "\tmovq\t%%rax, %s", registers[reg1]);
    asm_free_register(reg2);
    return reg1;
}

void _asm_glob_sym(int id)
{
    int size = asm_size(symbols[id].type);
    fprintf(out_file, "\t.comm\t%s, %d, %d\n", symbols[id].name, size, size);
}

int _asm_load_glob(int id)
{
    int r = _asm_alloc_register();
    char* name = symbols[id].name;

    switch (symbols[id].type)
    {
        case P_INT:
            fprintf(out_file, "\tmovslq\t%s(%%rip), %s\n", name, registers[r]);
            break;
        case P_CHAR:
            fprintf(out_file, "\tmovzbq\t%s(%%rip), %s\n", name, registers[r]);
            break;
        case P_SHORT:
            fprintf(out_file, "\tmovzwq\t%s(%%rip), %s\n", name, registers[r]);
        case P_LONG:
        case P_VOIDPTR:
        case P_CHARPTR:
        case P_INTPTR:
        case P_LONGPTR:
        case P_SHORTPTR:
            fprintf(out_file, "\tmovq\t%s(%%rip), %s\n", name, registers[r]);
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
            fprintf(out_file, "\tmovl\t%s, %s(%%rip)\n", dregisters[r], name);
            break;
        case P_CHAR:
            fprintf(out_file, "\tmovb\t%s, %s(%%rip)\n", bregisters[r], name);
            break;
        case P_SHORT:
            fprintf(out_file, "\tmovw\t%s, %s(%%rip)\n", wregisters[r], name);
        case P_LONG:
        case P_VOIDPTR:
        case P_CHARPTR:
        case P_INTPTR:
        case P_LONGPTR:
        case P_SHORTPTR:
            fprintf(out_file, "\tmovq\t%s, %s(%%rip)\n", registers[r], name);
            break;
    }

    return r;
}

int _asm_compare(int r1, int r2, char* how)
{
    fprintf(out_file, "\tcmpq\t%s, %s\n", registers[r1], registers[r2]);
    fprintf(out_file, "\t%s\t%s\n", how, bregisters[r2]);
    fprintf(out_file, "\tandq\t$255, %s\n", registers[r2]);
    return r2;
}

static char* cmp_list[] = { "sete", "setne", "setl", "setg", "setle", "setge" };

int _asm_compare_and_set(int type, int r1, int r2)
{
    fprintf(out_file, "\tcmpq\t%s, %s\n", registers[r2], registers[r1]);
    fprintf(out_file, "\t%s\t%s\n", cmp_list[type - N_EQ], bregisters[r2]);
    fprintf(out_file, "\tmovzbq\t%s, %s\n", bregisters[r2], registers[r2]);
    asm_free_register(r1);
    return r2;
}

static char* jmp_cmp_list[] = { "jne", "je", "jge", "jle", "jg", "jl" };

int _asm_compare_and_jump(int type, int r1, int r2, int label)
{
    fprintf(out_file, "\tcmpq\t%s, %s\n", registers[r2], registers[r1]);
    fprintf(out_file, "\t%s\tL%d\n", jmp_cmp_list[type - N_EQ], label);
    asm_free_all_registers();
    return NOREG;
}

void _asm_label(int label)
{
    fprintf(out_file, "L%d:\n", label);
}

void _asm_jump(int label)
{
    fprintf(out_file, "\tjmp\tL%d\n", label);
}

void _asm_func_begin(int id)
{
    fprintf(out_file, "%s:\n", symbols[id].name);
    fprintf(out_file, "\tpushq\t%%rbp\n");
    fprintf(out_file, "\tmovq\t%%rsp, %%rbp\n");
}

void _asm_func_end(int func_id)
{
    asm_label(symbols[func_id].endlabel);
    fprintf(out_file, "\tmovq\t%%rbp, %%rsp\n");
    fprintf(out_file, "\tpopq\t%%rbp\n");
    fprintf(out_file, "\tret\n\n");
}

int _asm_call(int r, int id)
{
    int out = asm_alloc_register();
    fprintf(out_file, "\tmovq\t%s, %%rdi\n", registers[r]);
    fprintf(out_file, "\tcall\t%s\n", symbols[id].name);
    fprintf(out_file, "\tmovq\t%%rax, %s\n", registers[out]);
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
        case P_SHORT:
            return 2;
        case P_INT:
            return 4;
        case P_LONG:
            return 8;
        case P_VOIDPTR:
        case P_CHARPTR:
        case P_INTPTR:
        case P_LONGPTR:
        case P_SHORTPTR:
            return 8;
    }
}

void _asm_return(int r, int func_id)
{
    switch (symbols[func_id].type)
    {
        case P_INT:
            fprintf(out_file, "\tmovl\t%s, %%eax\n", dregisters[r]);
            break;
        case P_CHAR:
            fprintf(out_file, "\tmovzbl\t%s, %%eax", bregisters[r]);
            break;
        case P_SHORT:
            fprintf(out_file, "\tmovzwl\t%s, %%eax\n", wregisters[r]);
        case P_LONG:
        case P_VOIDPTR:
        case P_CHARPTR:
        case P_LONGPTR:
        case P_SHORTPTR:
        case P_INTPTR:
            fprintf(out_file, "\tmovq\t%s, %%rax\n", registers[r]);
            break;

    }

    asm_jump(symbols[func_id].endlabel);
}

int _asm_addrof(int id)
{
    int r = asm_alloc_register();

    fprintf(out_file, "\tleaq\t%s(%%rip), %s\n", symbols[id].name, registers[r]);

    return r;   
}

int _asm_deref(int r, int type)
{
    switch (type)
    {
        case P_CHARPTR:
            fprintf(out_file, "\tmovzbq\t(%s), %s\n", registers[r], registers[r]);
        case P_SHORTPTR:
            fprintf(out_file, "\tmovslq\t(%s), %s\n", registers[r], registers[r]);
        case P_INTPTR:
        case P_LONGPTR:
            fprintf(out_file, "\tmovq\t(%s), %s\n", registers[r], registers[r]);
    }

    return r;
}

int _asm_store_deref(int r1, int r2, int type)
{
    switch (type)
    {
        case P_CHAR:
            fprintf(out_file, "\tmovb\t%s, (%s)\n", bregisters[r1], bregisters[r2]);
        case P_SHORT:
            fprintf(out_file, "\tmovw\t%s, (%s)\n", wregisters[r1], wregisters[r2]);
        case P_LONG:
        case P_INT:
            fprintf(out_file, "\tmovq\t%s, (%s)\n", registers[r1], registers[r2]);
    }
}