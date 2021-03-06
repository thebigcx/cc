#pragma once

#include "defs.h"

// Errors/warnings
void error(const char* msg);
void syntax_error(const char* format, ...);
void warning(const char* format, ...);

// Lexer
int scan(struct token* t);

// Tree methods
struct ast_node* make_tree_node(int type, int dtype, struct ast_node* left, struct ast_node* mid, struct ast_node* right, int intvalue);
struct ast_node* make_tree_node_leaf(int type, int dtype, int intvalue);
struct ast_node* make_tree_node_unary(int type, int dtype, struct ast_node* left, int intvalue);

// Expressions
int arithop(int tok);
struct ast_node* binary_expr(int ptp);
struct ast_node* paren_expression();
int parse_cast();

// Other parser stuff
void next_token();
void expect(int tok_type);
struct token peek_next_token();
void warn_narrow(int t1, int t2);

// Parser statements
struct ast_node* statement();
struct ast_node* compound_statement();
void var_decl_statement(int is_local);
struct ast_node* if_statement();
struct ast_node* while_statement();
struct ast_node* for_statement();
struct ast_node* func_decl_statement(int type);
struct ast_node* func_call();
struct ast_node* return_statement();
struct ast_node* prefix(); // Deref, addrof
struct ast_node* switch_statement();
struct ast_node* break_statement();
struct ast_node* continue_statement();
struct ast_node* array_index();
void multi_var_decl(int type, int is_local);

// Symbol table
int find_sym(char* s);
int add_glob(char* name, int type, int stype, int endlabel, int arr_elements);
int add_locl(char* name, int type, int stype, int endlabel, int arr_elements);
void update_sym(int slot, char* name, int type, int stype, int endlabel, int arr_elements, int class, int stack_offset);

// Generation
int gen_ast(struct ast_node* node, int reg, int parent);
void gen_code();

// ASM interface
int label();
void asm_free_all_registers();
int asm_alloc_register();
void asm_free_register(int reg);
void asm_preamble();
int asm_load(int value);
int asm_add(int reg1, int reg2);
int asm_sub(int reg1, int reg2);
int asm_mul(int reg1, int reg2);
int asm_div(int reg1, int reg2);
void asm_glob_sym(int id);
int asm_load_glob(int id);
int asm_load_locl(int id);
int asm_store_glob(int r, int id);
int asm_store_locl(int r, int id);
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
void asm_func_begin(int id);
void asm_func_end(int func_id);
int asm_call(int r, int id);
int asm_size(int datatype);
void asm_return(int r, int func_id);
int asm_addrof(int id);
int asm_deref(int r, int type);
int asm_store_deref(int r1, int r2, int type);
int asm_glob_str(char* str);
int asm_load_glob_str(int id);
int asm_get_stack_offset(int size, int is_param);
int asm_reset_stack();
void asm_text_sect();
void asm_rom_sect();

// ASM Implementations (asm gen varies based on os)
void _asm_free_all_registers();
int _asm_alloc_register();
void _asm_free_register(int reg);
void _asm_preamble();
int _asm_load(int value);
int _asm_add(int reg1, int reg2);
int _asm_sub(int reg1, int reg2);
int _asm_mul(int reg1, int reg2);
int _asm_div(int reg1, int reg2);
void _asm_glob_sym(int id);
int _asm_load_locl(int id);
int _asm_load_glob(int id);
int _asm_store_glob(int r, int id);
int _asm_store_locl(int r, int id);
int _asm_compare(int r1, int r2, char* how);
int _asm_compare_and_set(int type, int r1, int r2);
int _asm_compare_and_jump(int type, int r1, int r2, int label);
void _asm_label(int label);
void _asm_jump(int label);
void _asm_func_begin(int id);
void _asm_func_end(int func_id);
int _asm_call(int r, int id);
int _asm_size(int datatype);
void _asm_return(int r, int func_id);
int _asm_addrof(int id);
int _asm_deref(int r, int type);
int _asm_store_deref(int r1, int r2, int type);
int _asm_glob_str(char* str);
int _asm_load_glob_str(int id);
int _asm_get_stack_offset(int size, int is_param);
int _asm_reset_stack();
void _asm_text_sect();
void _asm_rom_sect();

// Types
int integral_type(int type);
int pointer_type(int type);
int compatible_types(int t1, int t2);
int is_type(int tok);
int parse_type();
int make_pointer(int base_type);
int pointed_to_type(int ptr_type);

// Preprocessor
char* preprocess(char* src);