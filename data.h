#pragma once
#ifndef extern_
    #define extern_ extern
#endif

#include <stdio.h>

extern_ int current_line;
extern_ int char_head_pos;
extern_ char* input;
extern_ char* input_lines[10000];
extern_ struct token current_tok;
extern_ int tok_head_pos;
extern_ struct token_array tok_array;
extern_ FILE* out_file;
extern_ struct sym symbols[1024];
extern_ int func_id;