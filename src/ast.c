#include <stdio.h>
#include "ast.h"

static ast_t bottom = {
    .kind = AST_KIND_VAR,
    .rc = 10000,
    .value.var = 0
};

void free_ast_list(ast_list_t * al) {
  while(al != nullptr) {
    ast_list_t * ne = al->next;
    free_ast(al->value);
    al = ne;
  }
}

void free_ast(ast_t * a) {
  if(a->rc > 1) {
    a->rc--;
    return;
  }
  switch(a->kind) {
  case AST_KIND_APP:
    free_ast(a->value.app.M);
    free_ast(a->value.app.N);
    break;
  case AST_KIND_LAMBDA:
  case AST_KIND_PAI:
    free_ast(a->value.lambda.M);
    free_ast(a->value.lambda.N);
    break;
  case AST_KIND_CONST:
    free_ast_list(a->value.cont.args);
    free(a->value.cont.c);
    break;
  }
  free(a);
}

void print_ast(ast_t * a) {
  if(a == nullptr) { printf("nullptr"); return; }
  switch(a->kind) {
  case AST_KIND_VAR: printf(a->value.var == 0 ? "_" : "%d", a->value.var); return;
  case AST_KIND_STAR: putc('*', stdout); return;
  case AST_KIND_SORT: printf("@"); return;
  case AST_KIND_APP:
    putc('(', stdout);
    print_ast(a->value.app.M);
    putc(')', stdout);
    putc('(', stdout);
    print_ast(a->value.app.N);
    putc(')', stdout);
    return;
  case AST_KIND_LAMBDA:
  case AST_KIND_PAI:
    printf(a->kind == AST_KIND_LAMBDA ? "$" : "?");
    putc(a->value.lambda.x, stdout);
    putc(':', stdout);
    putc('(', stdout);
    print_ast(a->value.lambda.M);
    putc(')', stdout);
    putc('.', stdout);
    putc('(', stdout);
    print_ast(a->value.lambda.N);
    putc(')', stdout);
    return;
  case AST_KIND_CONST:
    printf("%s", a->value.cont.c);
    putc('[', stdout);
    ast_list_t * al = a->value.cont.args;
    while(al != nullptr) {
      putc('(', stdout);
      print_ast(al->value);
      putc(')', stdout);
      al = al->next;
      if(al == nullptr) break;
      putc(',', stdout);
    }
    putc(']', stdout);
    return;
  }
}
