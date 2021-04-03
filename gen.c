#include "decl.h"
#include "data.h"

static int label()
{
    static int id = 1;
    return id++;
}

static int gen_if_tree(struct ast_node* node)
{
    int l_false, l_end;

    l_false = label();
    if (node->right)
    {
        l_end = label();
    }

    gen_ast(node->left, l_false, node->type);
    asm_free_all_registers();

    gen_ast(node->mid, NOREG, node->type);
    asm_free_all_registers();

    if (node->right)
    {
        asm_jump(l_end);
    }

    asm_label(l_false);

    if (node->right)
    {
        gen_ast(node->right, NOREG, node->type);
        asm_free_all_registers();
        asm_label(l_end);
    }
}

int gen_ast(struct ast_node* node, int reg, int parent)
{
    int left, right;

    switch (node->type)
    {
        case N_IF:
            return gen_if_tree(node);
        case N_GLUE:
            gen_ast(node->left, NOREG, node->type);
            asm_free_all_registers();
            gen_ast(node->right, NOREG, node->type);
            asm_free_all_registers();
            return NOREG;
    }

    if (node->left)
    {
        left = gen_ast(node->left, NOREG, node->type);
    }
    if (node->right)
    {
        right = gen_ast(node->right, left, node->type);
    }

    switch (node->type)
    {
        case N_ADD:
            return asm_add(left, right);
        case N_SUB:
            return asm_sub(left, right);
        case N_MULT:
            return asm_mul(left, right);
        case N_DIV:
            return asm_div(left, right);
        case N_EQ:
        case N_NE:
        case N_GT:
        case N_LT:
        case N_GE:
        case N_LE:
            if (parent == N_IF)
            {
                return asm_compare_and_jump(node->type, left, right, reg);
            }
            else
            {
                return asm_compare_and_set(node->type, left, right);
            }
        case N_INTLIT:
            return asm_load(node->v.intvalue);
        case N_IDENT:
            return asm_load_glob(symbols[node->v.id].name);
        case N_LVIDENT:
            return asm_store_glob(reg, symbols[node->v.id].name);
        case N_ASSIGN:
            return right;
    }
}

void gen_code()
{
    asm_preamble();
    struct ast_node* tree = compound_statement();
    gen_ast(tree, NOREG, 0);
    asm_postamble();
}