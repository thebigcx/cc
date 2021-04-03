#pragma once

#include "defs.h"

int scan(struct token* t);
char* concat(const char* s1, const char* s2);
void error(const char* msg);
void syntax_error(int line, const char* format, ...);
struct ast_node* make_tree_node(int type, struct ast_node* left, struct ast_node* mid, struct ast_node* right, int intvalue);
struct ast_node* make_tree_node_leaf(int type, int intvalue);
struct ast_node* make_tree_node_unary(int type, struct ast_node* left, int intvalue);
int arithop(int tok);
struct ast_node* binary_expr(int ptp);
void next_token();

void expect(int tok_type);
struct ast_node* compound_statement();
void var_decl_statement();
struct ast_node* var_assign_statement();
struct ast_node* if_statement();

// Symbol table
int find_glob(char* s);
int add_glob(char* name);

int gen_ast(struct ast_node* node, int reg, int parent);
void gen_code();

void asm_free_all_registers();
int asm_alloc_register();
void asm_free_register(int reg);

void asm_preamble();
void asm_postamble();
int asm_load(int value);
int asm_add(int reg1, int reg2);
int asm_sub(int reg1, int reg2);
int asm_mul(int reg1, int reg2);
int asm_div(int reg1, int reg2);
void asm_glob_sym(char* sym);
int asm_load_glob(char* identifier);
int asm_store_glob(int r, char* identifier);
int asm_equal(int r1, int r2);
int asm_not_equal(int r1, int r2);
int asm_less_than(int r1, int r2);
int asm_greater_than(int r1, int r2);
int asm_less_equal(int r1, int r2);
int asm_greater_equal(int r1, int r2);
int asm_compare_and_set(int type, int r1, int r2);
int asm_compare_and_jump(int type, int r1, int r2, int label);
void asm_label(int label);
void asm_jump(int label);

// Implementations (asm gen varies based on os)
void _asm_free_all_registers();
int _asm_alloc_register();
void _asm_free_register(int reg);
void _asm_preamble();
void _asm_postamble();
int _asm_load(int value);
int _asm_add(int reg1, int reg2);
int _asm_sub(int reg1, int reg2);
int _asm_mul(int reg1, int reg2);
int _asm_div(int reg1, int reg2);
void _asm_glob_sym(char* sym);
int _asm_load_glob(char* identifier);
int _asm_store_glob(int r, char* identifier);
int _asm_compare(int r1, int r2, char* how);
int _asm_compare_and_set(int type, int r1, int r2);
int _asm_compare_and_jump(int type, int r1, int r2, int label);
void _asm_label(int label);
void _asm_jump(int label);