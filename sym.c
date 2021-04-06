#include "decl.h"
#include "data.h"

static int globs = 0;

static int new_glob()
{
    if (globs >= 1024)
    {
        error("Too many symbols!");
    }
    return globs++;
}

int find_glob(char* name)
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

int add_glob(char* name, int type, int stype, int endlabel, int arr_elements)
{
    int slot = new_glob();
    symbols[slot].name = name;
    symbols[slot].type = type;
    symbols[slot].stype = stype;
    symbols[slot].endlabel = endlabel;
    symbols[slot].arr_elements = arr_elements;
    return slot;
}