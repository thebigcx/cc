#include "decl.h"
#include "data.h"

static int new_glob()
{
    if (globs >= locls)
    {
        error("Too many global symbols!");
    }
    return globs++;
}

static int new_locl()
{
    if (locls <= globs)
    {
        error("Too many local symbols!");
    }
    return locls--;
}

static int find_glob(char* name)
{
    for (int i = 0; i < globs; i++)
    {
        if (!strcmp(name, symbols[i].name))
        {
            return i;
        }
    }

    return -1;
}

static int find_locl(char* name)
{
    for (int i = 1023; i > locls; i--)
    {
        if (!strcmp(name, symbols[i].name))
        {
            return i;
        }
    }

    return -1;
}

int find_sym(char* name)
{
    int slot;

    slot = find_locl(name);
    if (slot == -1)
    {
        slot = find_glob(name);
    }

    return slot;
}

int add_glob(char* name, int type, int stype, int endlabel, int arr_elements)
{
    int slot = new_glob();
    update_sym(slot, name, type, stype, endlabel, arr_elements, C_GLOBAL, 0);
    if (stype == S_VAR)
    {
        asm_glob_sym(slot);
    }
    return slot;
}

int add_locl(char* name, int type, int stype, int endlabel, int arr_elements)
{
    int slot, stack_offset;
    slot = new_locl();
    if (arr_elements != 0)
    {
        stack_offset = asm_get_stack_offset(asm_size(pointed_to_type(type)) * arr_elements, 0);
    }
    else
    {
        stack_offset = asm_get_stack_offset(asm_size(type), 0);
    }
    update_sym(slot, name, type, stype, endlabel, arr_elements, C_LOCAL, stack_offset);
    return slot;
}

void update_sym(int slot, char* name, int type, int stype, int endlabel, int arr_elements, int class, int stack_offset)
{
    symbols[slot].name = name;
    symbols[slot].type = type;
    symbols[slot].stype = stype;
    symbols[slot].endlabel = endlabel;
    symbols[slot].arr_elements = arr_elements;
    symbols[slot].class = class;
    symbols[slot].stack_offset = stack_offset;
}