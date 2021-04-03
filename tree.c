#include "decl.h"
#include "data.h"

struct ast_node* make_tree_node(int type, struct ast_node* left, struct ast_node* mid, struct ast_node* right, int intvalue)
{
    struct ast_node* node;

    node = (struct ast_node*) malloc(sizeof(struct ast_node));

    node->type = type;
    node->left = left;
    node->mid = mid;
    node->right = right;
    node->v.intvalue = intvalue;

    return node;
}

struct ast_node* make_tree_node_leaf(int type, int intvalue)
{
    return make_tree_node(type, NULL, NULL, NULL, intvalue);
}

struct ast_node* make_tree_node_unary(int type, struct ast_node* left, int intvalue)
{
    return make_tree_node(type, left, NULL, NULL, intvalue);
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
        default:
            error("Syntax error.");
    }
}

static int op_prec[] = { 0, 10, 10, 20, 20, 30, 30, 40, 40, 40, 40 };

static int op_precedence(int tok_type)
{
    int prec = op_prec[tok_type];

    if (prec == 0)
    {
        syntax_error(line, "Unepected token: %s", token_strings[tok_type]);
    }
    return prec;
}

static struct ast_node* primary()
{
    struct ast_node* node;
    int id;

    switch (current_tok.type)
    {
        case T_INTLIT:
            node = make_tree_node_leaf(N_INTLIT, current_tok.v.int_value);
            break;
        case T_IDENT:
            id = find_glob(current_tok.v.str_value);
            if (id == -1)
            {
                syntax_error(line, "Undeclared variable.");
            }

            node = make_tree_node_leaf(N_IDENT, id);
            break;
        default:
            syntax_error(line, "Unexpected token '%s' in primary expression.", token_strings[current_tok.type]);
    }

    next_token();
    return node;
}

struct ast_node* binary_expr(int ptp)
{
    struct ast_node* left, *right;
    int tok_type;

    left = primary();

    tok_type = current_tok.type;
    if (tok_type == T_SEMI_COLON || tok_type == T_RPAREN)
    {
        return left;
    }

    while (op_precedence(tok_type) > ptp)
    {
        next_token();

        right = binary_expr(op_prec[tok_type]);
        left = make_tree_node(arithop(tok_type), left, NULL, right, 0);

        tok_type = current_tok.type;
        if (tok_type == T_SEMI_COLON || tok_type == T_RPAREN)
        {
            return left;
        }
    }

    return left;
}

struct ast_node* var_assign_statement()
{
    struct ast_node* node, *left, *right;
    int id;

    char* ident_name = current_tok.v.str_value;
    expect(T_IDENT);

    id = find_glob(current_tok.v.str_value); // Identifier name
    if (id == -1)
    {
        syntax_error(line, "Undeclared variable: %s", current_tok.v.str_value);
    }

    right = make_tree_node_leaf(N_LVIDENT, id);
    
    expect(T_ASSIGN);

    left = binary_expr(0);

    node = make_tree_node(N_ASSIGN, left, NULL, right, 0);

    expect(T_SEMI_COLON);

    return node;
}

void var_decl_statement()
{
    expect(T_INT);
    char* var_name = current_tok.v.str_value;
    expect(T_IDENT);

    add_glob(current_tok.v.str_value);
    asm_glob_sym(current_tok.v.str_value);

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

    return make_tree_node(N_IF, condition, true_case, else_case, 0);
}

struct ast_node* compound_statement()
{
    struct ast_node* left = NULL;
    struct ast_node* tree = NULL;

    expect(T_LBRACE);

    while (1)
    {
        switch (current_tok.type)
        {
            case T_INT:
                var_decl_statement();
                tree = NULL;
                break;
            case T_IDENT:
                tree = var_assign_statement();
                break;
            case T_NEWLINE:
                next_token();
                line++;
                break;
            case T_IF:
                tree = if_statement();
                break;
            case T_RBRACE:
                expect(T_RBRACE);
                return left;
            default:
                syntax_error(line, "Unexpected token '%s'", token_strings[current_tok.type]);
        }

        if (tree)
        {
            if (left == NULL)
            {
                left = tree;
            }
            else
            {
                left = make_tree_node(N_GLUE, left, NULL, tree, 0);
            }
        }

    }
}

void expect(int tok_type)
{
    if (current_tok.type != tok_type)
    {
        syntax_error(line, "Expected '%s'", token_strings[tok_type]);
    }
    next_token();
}