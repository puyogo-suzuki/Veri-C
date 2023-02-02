#pragma once
#include "ast.h"

typedef struct var_def_t {
  char var;
  ast_t * lambda;
} var_def_t;

typedef struct context_t {
  var_def_t * data;
  int length;
} context_t;

typedef struct definition_t {
	context_t cont;
	char * name;
	ast_t * l1;
	ast_t * l2;

} definition_t;

struct judgement_t;

typedef struct judgement_l_t {
  struct definition_t * defs;
  int defs_length;
  context_t context;
} judgement_l_t;

typedef struct judgement_t {
  judgement_l_t left;
  ast_t * l1;
  ast_t * l2;
} judgement_t;


judgement_t * check_sort(void);
judgement_t * check_var(judgement_t * m, char v);
judgement_t * check_weak(judgement_t * m, judgement_t * n, char v);
judgement_t * check_form(judgement_t * m, judgement_t * n);
judgement_t * check_appl(judgement_t * m, judgement_t * n);
judgement_t * check_abst(judgement_t * m, judgement_t * n);
judgement_t * check_conv(judgement_t * m, judgement_t * n);
judgement_t * check_def(judgement_t * m, judgement_t * n, char v);
judgement_t * check_def_prim(judgement_t * m, judgement_t * n, char v);
judgement_t * check_inst(judgement_t * m, judgement_t ** n, int v);
void print_judgement(judgement_t * judge);