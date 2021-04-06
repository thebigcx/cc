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

void asm_glob_sym(int id)
{
    _asm_glob_sym(id);
}

int asm_load_glob(int id)
{
    return _asm_load_glob(id);
}

int asm_load_locl(int id)
{
    return _asm_load_locl(id);
}

int asm_store_glob(int r, int id)
{
    return _asm_store_glob(r, id);
}

int asm_store_locl(int r, int id)
{
    return _asm_store_locl(r, id);
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

void asm_func_begin(int id)
{
    _asm_func_begin(id);
}

void asm_func_end(int func_id)
{
    _asm_func_end(func_id);
}

int asm_call(int r, int id)
{
    return _asm_call(r, id);
}

int asm_size(int datatype)
{
    return _asm_size(datatype);
}

void asm_return(int r, int func_id)
{
    _asm_return(r, func_id);
}

int asm_addrof(int id)
{
    return _asm_addrof(id);
}

int asm_deref(int r, int type)
{
    return _asm_deref(r, type);
}

int asm_store_deref(int r1, int r2, int type)
{
    return _asm_store_deref(r1, r2, type);
}

int asm_glob_str(char* str)
{
    return _asm_glob_str(str);
}

int asm_load_glob_str(int id)
{
    return _asm_load_glob_str(id);
}

int asm_get_stack_offset(int size, int is_param)
{
    return _asm_get_stack_offset(size, is_param);
}

int asm_reset_stack()
{
    return asm_reset_stack();
}

void asm_text_sect()
{
    _asm_text_sect();
}

void asm_rom_sect()
{
    _asm_rom_sect();
}