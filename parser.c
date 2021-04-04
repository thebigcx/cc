#include "decl.h"
#include "data.h"

const char* type_strings[] = { "none", "void", "int", "char", "long", "short", "void*", "int*", "char*", "long*", "short*" };

static void print_current_token()
{
    printf("%s\n", token_strings[current_tok.type]);
}

struct ast_node* make_tree_node(int type, int dtype, struct ast_node* left, struct ast_node* mid, struct ast_node* right, int intvalue)
{
    struct ast_node* node;

    node = (struct ast_node*) malloc(sizeof(struct ast_node));

    node->type = type;
    node->dtype = dtype;
    node->left = left;
    node->mid = mid;
    node->right = right;
    node->v.intvalue = intvalue;

    return node;
}

struct ast_node* make_tree_node_leaf(int type, int dtype, int intvalue)
{
    return make_tree_node(type, dtype, NULL, NULL, NULL, intvalue);
}

struct ast_node* make_tree_node_unary(int type, int dtype, struct ast_node* left, int intvalue)
{
    return make_tree_node(type, dtype, left, NULL, NULL, intvalue);
}

void next_token()
{
    tok_head_pos++;
    current_tok = tok_array.data[tok_head_pos];

    while (current_tok.type == T_NEWLINE)
    {
        current_line++;
        tok_head_pos++;
        current_tok = tok_array.data[tok_head_pos];
    }
}

int arithop(int tok)
{
    switch (tok)
    {
        case T_PLUS:
            return N_ADD;
        case T_MINUS:
            return N_SUB;
        case T_SLASH:
            return N_DIV;
        case T_STAR:
            return N_MULT;
        case T_EQ:
            return N_EQ;
        case T_NE:
            return N_NE;
        case T_LT:
            return N_LT;
        case T_LE:
            return N_LE;
        case T_GT:
            return N_GT;
        case T_GE:
            return N_GE;
        case T_ASSIGN:
            return N_ASSIGN;
        default:
            syntax_error("Expected arithmetic operator, got '%s'", token_strings[tok]);
    }
}

static int op_prec[] = { 0, 10, 20, 20, 30, 30, 40, 40, 50, 50, 50, 50 };

static int op_precedence(int tok_type)
{
    int prec = op_prec[tok_type];

    if (prec == 0)
    {
        syntax_error("Unepected token: '%s'", token_strings[tok_type]);
    }
    return prec;
}

static int rightassoc(int tok_type)
{
    if (tok_type == T_ASSIGN)
    {
        return 1;
    }
    return 0;
}

static struct ast_node* primary()
{
    struct ast_node* node;
    int id;

    switch (current_tok.type)
    {
        case T_INTLIT:
            if (current_tok.v.int_value < 256)
            {
                node = make_tree_node_leaf(N_INTLIT, P_CHAR, current_tok.v.int_value);
            }
            else if (current_tok.v.int_value < 65536)
            {
                node = make_tree_node_leaf(N_INTLIT, P_SHORT, current_tok.v.int_value);
            }
            else if (current_tok.v.int_value < 4294967296)
            {
                node = make_tree_node_leaf(N_INTLIT, P_INT, current_tok.v.int_value);
            }
            else
            {
                node = make_tree_node_leaf(N_INTLIT, P_LONG, current_tok.v.int_value);
            }
            
            break;
        case T_IDENT:
            if (tok_array.data[tok_head_pos + 1].type == T_LPAREN)
            {
                return func_call();
            }

            id = find_glob(current_tok.v.str_value);
            if (id == -1)
            {
                syntax_error("Undeclared variable '%s'", current_tok.v.str_value);
            }

            node = make_tree_node_leaf(N_IDENT, symbols[id].type, id);
            break;
        default:
            syntax_error("Unexpected token '%s' in primary expression.", token_strings[current_tok.type]);
    }

    next_token();
    return node;
}

struct ast_node* binary_expr(int ptp)
{
    struct ast_node* left, *right, *ltemp, *rtemp;
    int ast_op;
    int tok_type;

    left = prefix(); // Calls primary as well

    tok_type = current_tok.type;
    if (tok_type == T_SEMI_COLON || tok_type == T_RPAREN)
    {
        left->rvalue = 1;
        return left;
    }

    while (op_precedence(tok_type) > ptp || (rightassoc(tok_type) && op_precedence(tok_type) == ptp))
    {
        next_token();

        right = binary_expr(op_prec[tok_type]);
        ast_op = arithop(tok_type);

        if (ast_op == N_ASSIGN)
        {
            right->rvalue = 1;

            if (!compatible_types(right->dtype, left->dtype))
            {
                syntax_error("Incompatible types '%s' and '%s' in assignment.", type_strings[right->dtype], type_strings[left->dtype]);
            }

            ltemp = left;
            left = right;
            right = ltemp;
        }
        else
        {
            left->rvalue = 1;
            right->rvalue = 1;
        }

        left = make_tree_node(arithop(tok_type), left->dtype, left, NULL, right, 0);

        tok_type = current_tok.type;
        if (tok_type == T_SEMI_COLON || tok_type == T_RPAREN)
        {
            left->rvalue = 1;
            return left;
        }
    }

    left->rvalue = 1;
    return left;
}

void var_decl_statement()
{
    int type = parse_type(current_tok.type);
    next_token();
    char* var_name = current_tok.v.str_value;
    expect(T_IDENT);

    int id = add_glob(current_tok.v.str_value, type, 0);
    asm_glob_sym(id);

    expect(T_SEMI_COLON);
}

struct ast_node* if_statement()
{
    struct ast_node* condition, *true_case, *else_case = NULL;

    expect(T_IF);
    expect(T_LPAREN);

    condition = binary_expr(0);

    expect(T_RPAREN);

    true_case = compound_statement();

    if (current_tok.type == T_ELSE)
    {
        expect(T_ELSE);
        else_case = compound_statement();
    }

    return make_tree_node(N_IF, P_NONE, condition, true_case, else_case, 0);
}

struct ast_node* while_statement()
{
    struct ast_node* condition, *body;

    expect(T_WHILE);
    expect(T_LPAREN);

    condition = binary_expr(0);

    expect(T_RPAREN);

    body = compound_statement();

    return make_tree_node(N_WHILE, P_NONE, condition, NULL, body, 0);
}

struct ast_node* for_statement()
{
    struct ast_node* pre, *cond, *update, *tree, *body;

    expect(T_FOR);
    expect(T_LPAREN);

    pre = statement();

    expect(T_SEMI_COLON);

    cond = binary_expr(0);

    expect(T_SEMI_COLON);

    update = statement();

    expect(T_RPAREN);

    body = compound_statement();

    tree = make_tree_node(N_GLUE, P_NONE, body, NULL, update, 0);
    tree = make_tree_node(N_WHILE, P_NONE, cond, NULL, tree, 0);

    return make_tree_node(N_GLUE, P_NONE, pre, NULL, tree, 0);
}

struct ast_node* func_decl_statement()
{
    struct ast_node* tree;
    int nameslot;

    int type = parse_type(current_tok.type);
    next_token();
    char* name = current_tok.v.str_value;
    nameslot = add_glob(name, type, label());
    func_id = nameslot;
    expect(T_IDENT);
    expect(T_LPAREN);
    expect(T_RPAREN);

    tree = compound_statement();

    return make_tree_node_unary(N_FUNCTION, P_VOID, tree, nameslot);
}

struct ast_node* return_statement()
{
    struct ast_node* tree;
    int return_type, func_type;

    if (symbols[func_id].type == P_VOID)
    {
        syntax_error("Attempted to return from void function");
    }

    expect(T_RETURN);
    
    tree = binary_expr(0);

    return_type = tree->dtype;
    func_type = symbols[func_id].type;
    
    if (!compatible_types(return_type, func_type))
    {
        syntax_error("Incompatible return type '%s' in function returning '%s'", type_strings[return_type], type_strings[func_type]);
    }

    tree = make_tree_node_unary(N_RETURN, P_NONE, tree, 0);

    return tree;
}

struct ast_node* prefix()
{
    struct ast_node* tree;

    switch (current_tok.type)
    {
        case T_AMPER:
            next_token();
            tree = prefix();

            tree->type = N_ADDROF;
            tree->dtype = make_pointer(tree->dtype);
            break;
        case T_STAR:
            next_token();
            tree = prefix();

            tree = make_tree_node_unary(N_DEREF, pointed_to_type(tree->dtype), tree, 0);
            break;
        default:
            tree = primary();
            break;
    }

    return tree;
}

struct ast_node* statement()
{
    switch (current_tok.type)
    {
        case T_CHAR:
        case T_INT:
        case T_LONG:
        case T_SHORT:
            var_decl_statement();
            return NULL;
        case T_NEWLINE:
            next_token();
            current_line++;
            return NULL;
        case T_IF:
            return if_statement();
        case T_WHILE:
            return while_statement();
        case T_FOR:
            return for_statement();
        case T_RETURN:
            return return_statement();
        default:
            return binary_expr(0);
    }

    return NULL;
}

struct ast_node* compound_statement()
{
    struct ast_node* left = NULL;
    struct ast_node* tree = NULL;

    expect(T_LBRACE);

    while (1)
    {
        if (current_tok.type == T_RBRACE)
        {
            expect(T_RBRACE);
            return left;
        }

        tree = statement();

        if (tree != NULL && (tree->type == N_ASSIGN || tree->type == N_FUNCCALL || tree->type == N_RETURN))
        {
            expect(T_SEMI_COLON);
        }

        if (tree)
        {
            if (left == NULL)
            {
                left = tree;
            }
            else
            {
                left = make_tree_node(N_GLUE, P_NONE, left, NULL, tree, 0);
            }
        }
    }
}

struct ast_node* func_call()
{
    struct ast_node* tree;
    int id;

    id = find_glob(current_tok.v.str_value);
    if (id == -1)
    {
        syntax_error("Undeclared function '%s'", current_tok.v.str_value);
    }

    next_token();
    expect(T_LPAREN);

    tree = binary_expr(0);

    tree = make_tree_node_unary(N_FUNCCALL, symbols[id].type, tree, id);

    expect(T_RPAREN);
    return tree;
}

void expect(int tok_type)
{
    while (current_tok.type == T_NEWLINE)
    {
        next_token();
    }

    if (current_tok.type != tok_type)
    {
        syntax_error("Expected '%s', got '%s'", token_strings[tok_type], token_strings[current_tok.type]);
    }
    next_token();
}

int parse_type(int tok)
{
    int type;
    switch (tok)
    {
        case T_CHAR:
            type = P_CHAR;
            break;
        case T_INT:
            type = P_INT;
            break;
        case T_VOID:
            type = P_VOID;
            break;
        case T_LONG:
            type = P_LONG;
            break;
        case T_SHORT:
            type = P_SHORT;
    }

    if (tok_array.data[tok_head_pos + 1].type == T_STAR)
    {
        next_token();
        return make_pointer(type);
    }

    return type;
}

int pointer_type(int type)
{
    return type == P_VOIDPTR || type == P_INTPTR || type == P_SHORTPTR || type == P_CHARPTR || type == P_LONGPTR;
}

int integral_type(int type)
{
    return type == P_CHAR || type == P_INT || type == P_SHORT || type == P_LONG || pointer_type(type);
}

int compatible_types(int t1, int t2)
{
    if (t1 == t2)
    {
        return 1;
    }
    else if (integral_type(t1) && integral_type(t2))
    {
        return 1;
    }

    return 0;
}

int make_pointer(int base_type)
{
    switch (base_type)
    {
        case P_VOID:
            return P_VOIDPTR;
        case P_INT:
            return P_INTPTR;
        case P_CHAR:
            return P_CHARPTR;
        case P_LONG:
            return P_LONGPTR;
        case P_SHORT:
            return P_SHORTPTR;
        default:
            syntax_error("Invalid base pointer type '%s'", type_strings[base_type]);
    }
}

int pointed_to_type(int ptr_type)
{
    switch (ptr_type)
    {
        case P_VOIDPTR:
            return P_VOID;
        case P_INTPTR:
            return P_INT;
        case P_CHARPTR:
            return P_CHAR;
        case P_LONGPTR:
            return P_LONG;
        case P_SHORTPTR:
            return P_SHORT;
        default:
            syntax_error("Invalid pointer type '%s'", type_strings[ptr_type]);
    }
}