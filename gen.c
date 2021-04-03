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

static int gen_while_tree(struct ast_node* node)
{
    int l_top, l_end;
    
    l_top = label();
    l_end = label();

    asm_label(l_top);

    gen_ast(node->left, l_end, node->type);
    asm_free_all_registers();

    gen_ast(node->right, NOREG, node->type);
    asm_free_all_registers();

    asm_jump(l_top);
    asm_label(l_end);

    return NOREG;
}

int gen_ast(struct ast_node* node, int reg, int parent)
{
    int left = -1, right = -1;

    if (!node)
    {
        return -1;
    }

    switch (node->type)
    {
        case N_IF:
            return gen_if_tree(node);
        case N_WHILE:
            return gen_while_tree(node);
        case N_FUNCTION:
            asm_func_begin(symbols[node->v.id].name);
            gen_ast(node->left, NOREG, node->type);
            asm_func_end();
            return NOREG;
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
            if (parent == N_IF || parent == N_WHILE)
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
    while (1)
    {
        struct ast_node* tree = func_decl_statement();
        gen_ast(tree, NOREG, 0);
        if (current_tok.type == T_EOF)
        {
            break;
        }
    }
}