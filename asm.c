#include <stdlib.h>

#include "decl.h"
#include "data.h"

void asm_free_all_registers()
{
    _asm_free_all_registers();
}

int asm_alloc_register()
{
    return _asm_alloc_register();
}

void asm_free_register(int reg)
{
    _asm_free_register(reg);
}

void asm_preamble()
{
    _asm_preamble();
}

int asm_load(int val)
{
    return _asm_load(val);
}

int asm_add(int reg1, int reg2)
{
    return _asm_add(reg1, reg2);
}

int asm_sub(int reg1, int reg2)
{
    return _asm_sub(reg1, reg2);
}

int asm_mul(int reg1, int reg2)
{
    return _asm_mul(reg1, reg2);
}

int asm_div(int reg1, int reg2)
{
    return _asm_div(reg1, reg2);
}

void asm_glob_sym(char* sym)
{
    _asm_glob_sym(sym);
}

int asm_load_glob(char* identifier)
{
    return _asm_load_glob(identifier);
}

int asm_store_glob(int r, char* identifier)
{
    return _asm_store_glob(r, identifier);
}

int asm_equal(int r1, int r2)
{
    return _asm_compare(r1, r2, "sete");
}

int asm_not_equal(int r1, int r2)
{
    return _asm_compare(r1, r2, "setne");
}

int asm_less_than(int r1, int r2)
{
    return _asm_compare(r1, r2, "setl");
}

int asm_greater_than(int r1, int r2)
{
    return _asm_compare(r1, r2, "setg");
}

int asm_less_equal(int r1, int r2)
{
    return _asm_compare(r1, r2, "setle");
}

int asm_greater_equal(int r1, int r2)
{
    return _asm_compare(r1, r2, "setge");
}

int asm_compare_and_set(int type, int r1, int r2)
{
    return _asm_compare_and_set(type, r1, r2);
}

int asm_compare_and_jump(int type, int r1, int r2, int label)
{
    return _asm_compare_and_jump(type, r1, r2, label);
}

void asm_label(int label)
{
    _asm_label(label);
}

void asm_jump(int label)
{
    _asm_jump(label);
}

void asm_func_begin(char* name)
{
    _asm_func_begin(name);
}

void asm_func_end()
{
    _asm_func_end();
}